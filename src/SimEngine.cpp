#include "SimEngine.h"
#include "WorldMap.h"
#include "HistoryGenerator.h"
#include "GameData.h" 
#include "EventManager.h"
#include <iostream>

SimEngine::SimEngine() {
}

SimEngine::~SimEngine() {
}

SimEngine& SimEngine::Get() {
    static SimEngine instance;
    return instance;
}

void SimEngine::InitializeWorld() {
    std::cout << "[SimEngine] Initializing World..." << std::endl;
    
    // Create Player Entity
    player_id = registry.CreateEntity();
    
    TransformComponent tc;
    tc.x = 50; // Center of initial map
    tc.y = 50;
    tc.z = 0;
    registry.transforms[player_id] = tc;
    
    StatsComponent sc;
    sc.hp = 100;
    sc.max_hp = 100;
    registry.stats[player_id] = sc;
    
    CultivationComponent cc;
    cc.qi = 0.0f;
    cc.max_qi = 100.0f;
    cc.realm_major = 0; // Mortal
    cc.talent = 1.0f;
    registry.cultivations[player_id] = cc;
    
    RenderComponent rc;
    rc.name = "Ye Chen";
    rc.symbol = '@';
    rc.color = 0xFFFFFFFF;
    registry.renderables[player_id] = rc;
    registry.renderables[player_id] = rc;
}

// --- Loot Helpers ---
void SimEngine::CreateLootBag(int x, int y, const InventoryComponent& inv) {
    if (inv.items.empty() && inv.spirit_stones == 0) return; // Empty

    int bagID = registry.CreateEntity();
    registry.transforms[bagID] = {x, y, 0};
    registry.renderables[bagID] = {'$', 0xFFD700FF, "Loot Bag", true};
    registry.inventories[bagID] = inv; // Copy inventory
    registry.locations[bagID].is_inside = false; // Always on ground
}

void SimEngine::HandleDeath(int entity, const std::string& cause) {
    auto* stats = registry.GetStats(entity);
    if (stats) {
        stats->is_dead = true;
        stats->cause_of_death = cause;
        stats->hp = 0;
        // Year approx
        stats->death_year = (int)(current_tick / 8640) + 1; 
    }

    auto* tc = registry.GetTransform(entity);
    auto* inv = registry.GetInventory(entity);
    auto* render = registry.GetRenderable(entity);
    
    // Position
    if (tc && stats) {
        stats->death_x = tc->x;
        stats->death_y = tc->y;
    }

    // Drop Loot
    if (tc && inv) {
        CreateLootBag(tc->x, tc->y, *inv);
        // Clear inventory after drop to prevent duping if revived (future proofing)
        inv->items.clear();
        inv->spirit_stones = 0;
    }
    
    // Disable Render & AI
    if (render) {
        render->visible_on_map = false;
        render->symbol = 'X'; // Marker for debug
        render->color = 0x888888FF; // Grey
    }
    auto* ai = registry.GetAI(entity);
    if (ai) {
        ai->behavior = "Dead";
        ai->target_entity_id = -1;
    }
    
    std::cout << "[SimEngine] Entity " << entity << " Died. Cause: " << cause << std::endl;
}

bool SimEngine::AdvanceTime(int ticks) {
    current_tick += ticks;
    
    SystemCultivation(ticks);
    // SystemAI(ticks); // Replaced by SystemDailyUpdate called in SimulateTime loop? 
    // Actually, AdvanceTime is called by Player Moves too. 
    // If player moves, time passes. Should we run DailyUpdate? 
    // Only if accumulated ticks >= 1 Day.
    // For now, let's keep AdvanceTime simple and let SimulateTime handle the "Macro" simulation.
    
    return true; 
}

void SimEngine::SimulateTime(int amount, TimeUnit unit) {
    // Deprecated synch method, delegating to async but waiting (blocking)
    StartAsyncSimulation(amount, unit);
    WaitSimulation();
}

void SimEngine::StartAsyncSimulation(int amount, TimeUnit unit) {
    if (is_simulating.load()) return; // Already running

    int totalDays = amount;
    if (unit == TimeUnit::Months) totalDays = amount * 30;
    else if (unit == TimeUnit::Years) totalDays = amount * 360;

    is_simulating.store(true);
    current_progress.store(0.0f);

    std::cout << "[SimEngine] Starting Async Simulation of " << totalDays << " Days..." << std::endl;

    if (simulation_thread.joinable()) simulation_thread.join();

    simulation_thread = std::thread([this, amount, unit, totalDays]() {
        if (unit == TimeUnit::Years) {
            // Use Abstract Simulation for long durations
            SimulateHistory(amount);
        } else {
            // Use Detailed Simulation for short durations
            for (int day = 0; day < totalDays; ++day) {
                AdvanceTime(24); // 24 Ticks per day
                SystemDailyUpdate();
                current_progress.store((float)(day + 1) / (float)totalDays);
            }
        }
        is_simulating.store(false);
        std::cout << "[SimEngine] Async Simulation Complete." << std::endl;
    });
}


void SimEngine::WaitSimulation() {
    if (simulation_thread.joinable()) {
        simulation_thread.join();
    }
}

#include <filesystem>
#include <fstream>
namespace fs = std::filesystem;

void SimEngine::SaveGame(const std::string& slotName, WorldMap& map) {
    std::string basePath = "SaveData/" + slotName;
    if (!fs::exists(basePath)) {
        fs::create_directories(basePath);
    }
    
    std::cout << "[SimEngine] Saving Game to " << basePath << "..." << std::endl;
    
    // 1. Save Metadata
    json meta;
    meta["current_tick"] = current_tick;
    meta["player_id"] = player_id;
    // Add timestamp?
    std::ofstream oMeta(basePath + "/metadata.json");
    oMeta << meta << std::endl;
    
    // 2. Save Registry
    json regData = registry.Serialize();
    std::ofstream oReg(basePath + "/registry.json");
    oReg << regData << std::endl;
    
    // 3. Save Map
    map.SaveToJSON(basePath + "/map.json");
    
    std::cout << "[SimEngine] Game Saved Successfully." << std::endl;
}

void SimEngine::LoadGame(const std::string& slotName, WorldMap& map) {
    std::string basePath = "SaveData/" + slotName;
    if (!fs::exists(basePath)) {
        std::cerr << "[SimEngine] Save slot not found: " << slotName << std::endl;
        return;
    }
    
    std::cout << "[SimEngine] Loading Game from " << basePath << "..." << std::endl;
    
    // 1. Load Metadata
    std::ifstream iMeta(basePath + "/metadata.json");
    if (iMeta.is_open()) {
        json meta;
        iMeta >> meta;
        if(meta.contains("current_tick")) current_tick = meta["current_tick"];
        if(meta.contains("player_id")) player_id = meta["player_id"];
    }
    
    // 2. Load Registry
    std::ifstream iReg(basePath + "/registry.json");
    if (iReg.is_open()) {
        json regData;
        iReg >> regData;
        registry.Deserialize(regData);
    }
    
    // 3. Load Map
    map.LoadFromJSON(basePath + "/map.json");
    
    std::cout << "[SimEngine] Game Loaded Successfully." << std::endl;
}

void SimEngine::StartAsyncSave(const std::string& slotName, WorldMap& map) {
    if (is_saving || is_loading) return; // Busy
    
    if (persistence_thread.joinable()) persistence_thread.join();
    
    is_saving = true;
    persistence_thread = std::thread([this, slotName, &map]() {
        SaveGame(slotName, map);
        is_saving = false;
    });
}

void SimEngine::StartAsyncLoad(const std::string& slotName, WorldMap& map) {
    if (is_saving || is_loading) return; // Busy

    if (persistence_thread.joinable()) persistence_thread.join();

    is_loading = true;
    persistence_thread = std::thread([this, slotName, &map]() {
        LoadGame(slotName, map);
        is_loading = false;
    });
}

// Helper to get random item based on Challenge Tier
InventoryComponent::ItemInstance GetRandomItem(int lootTier) {
    auto& items = GameData::GetItems();
    std::vector<InventoryComponent::ItemInstance> pool;
    for(const auto& def : items) {
        // Strict Tier matching for High Risk, loose for Low Risk
        if (def.tier == lootTier || (lootTier == 1 && def.tier == 1)) {
            pool.push_back({def.id, 1, 100, false});
        }
    }
    // Fallback if empty
    if (pool.empty()) return {"ORE_005", 10 * lootTier, 100, false}; 
    return pool[rand() % pool.size()];
}

// Helper to stack items
static void AddItem(InventoryComponent& inv, InventoryComponent::ItemInstance item) {
    if (item.item_id == "SS") { // Special case handling if needed
        inv.spirit_stones += item.count;
        return;
    }
    
    // Check for existing stack
    for (auto& existing : inv.items) {
        if (existing.item_id == item.item_id) {
            existing.count += item.count;
            return;
        }
    }
    // New item
    inv.items.push_back(item);
}


// --- Quest System ---
void SimEngine::SystemQuestGeneration() {
    // 10% Chance per day for each Sect to generate a mission
    for(auto& [id, sc] : registry.sects) {
        if(sc.rank != 5) continue; // Only process Leaders (Sect Resource Holder)
        
        SectResourceComponent* res = registry.GetSectResource(id);
        if(!res) continue;

        if (res->mission_queue.size() >= 5) continue; // Max 5 quests

        if (rand() % 100 < 10) { 
            // Generate Quest
            int typeRoll = rand() % 3;
            SectResourceComponent::Mission newMission;
            newMission.active = true;
            newMission.target_location_id = -1;
            newMission.required_strength = 0;

            if (typeRoll == 0) {
                // Herb Gathering
                newMission.type = "Gather Herbs";
                newMission.required_item_id = "HERB_001"; // Generic Spirit Grass
                // Randomize herb type based on tiers later? For now basic.
                if (rand()%2==0) newMission.required_item_id = "HERB_002"; // Blood Ginseng
                
                newMission.required_count = 3 + rand() % 5;
                newMission.reward_contribution = newMission.required_count * 10 + 20; // 10 per herb + base
            } else if (typeRoll == 1) {
                // Ore Mining
                newMission.type = "Mine Ores";
                newMission.required_item_id = "ORE_001"; // Iron
                if (rand()%2==0) newMission.required_item_id = "ORE_005"; // Cold Iron
                
                newMission.required_count = 2 + rand() % 4;
                newMission.reward_contribution = newMission.required_count * 15 + 20;
            } else {
                 // Monetary Donation
                 newMission.type = "Fund Sect";
                 newMission.required_item_id = "SS"; // Spirit Stones
                 newMission.required_count = 100 + (rand() % 5) * 50;
                 newMission.reward_contribution = newMission.required_count / 2; // 0.5 ratio
            }
            
            res->mission_queue.push_back(newMission);
            // std::cout << "[Quest] Generated " << newMission.type << " for Sect " << sc.name << std::endl;
        }
    }
}

bool SimEngine::CompleteMission(int entityID, int sectLeaderID, int missionIndex) {
    auto* inv = registry.GetInventory(entityID);
    auto* sectRes = registry.GetSectResource(sectLeaderID);
    
    if (!inv || !sectRes) return false;
    if (missionIndex < 0 || missionIndex >= sectRes->mission_queue.size()) return false;
    
    // Copy mission to avoid invalidation if we modify vector (though we erase at end)
    auto& mission = sectRes->mission_queue[missionIndex];
    
    // Check Requirements
    if (mission.required_item_id == "SS") {
        if (inv->spirit_stones >= mission.required_count) {
             inv->spirit_stones -= mission.required_count;
             sectRes->spirit_stones += mission.required_count;
             inv->contribution_points += mission.reward_contribution;
             
             // Remove Mission
             sectRes->mission_queue.erase(sectRes->mission_queue.begin() + missionIndex);
             
             std::cout << "[Quest] Entity " << entityID << " completed Fund Sect mission." << std::endl;
             return true;
        }
    } else {
        // Find Item
        auto it = std::find_if(inv->items.begin(), inv->items.end(), [&](const InventoryComponent::ItemInstance& i){ 
            return i.item_id == mission.required_item_id; 
        });
        
        if (it != inv->items.end() && it->count >= mission.required_count) {
            // Deduct
            it->count -= mission.required_count;
            if (it->count <= 0) inv->items.erase(it);
            
            // Add to Stash
            bool stacked = false;
            for(auto& sit : sectRes->stash) {
                if(sit.item_id == mission.required_item_id) { sit.count += mission.required_count; stacked = true; break; }
            }
            if(!stacked) sectRes->stash.push_back({mission.required_item_id, mission.required_count, 100, false});
            
            // Reward
            inv->contribution_points += mission.reward_contribution;
            
            // Remove Mission
            sectRes->mission_queue.erase(sectRes->mission_queue.begin() + missionIndex);
            
            std::cout << "[Quest] Entity " << entityID << " completed " << mission.type << "." << std::endl;
            return true;
        }
    }
    
    return false;
}


void SimEngine::SystemDailyUpdate() {
    SystemQuestGeneration();

    // 1. NPC Logic (Missions, Movement)
    std::vector<int> allEntities;
    for (auto& [e, _] : registry.transforms) allEntities.push_back(e);

    // Grid for fast interaction lookup (Simple mapping for now)
    std::map<std::pair<int,int>, std::vector<int>> tileMap;

    for (int e : allEntities) {
        auto* ai = registry.GetAI(e);
        auto* loc = registry.GetLocation(e);
        auto* tc = registry.GetTransform(e);
        auto* sect = registry.GetSect(e);
        auto* inv = registry.GetInventory(e);
        auto* render = registry.GetRenderable(e);
        
        if (!tc) continue;
        
        // Add to map for interaction check later
        tileMap[{tc->x, tc->y}].push_back(e);

        if (!ai) continue; // Beasts might not have complex AI but we handle them

        // A. BEAST LOGIC (Invisible Wandering)
        if (sect == nullptr) { 
             if (rand() % 10 < 3) {
                 tc->x += (rand() % 3) - 1;
                 tc->y += (rand() % 3) - 1;
                 // Clamp
                 if (tc->x < 0) tc->x = 0; else if (tc->x >= 200) tc->x = 199;
                 if (tc->y < 0) tc->y = 0; else if (tc->y >= 200) tc->y = 199;
             }
             continue;
        }

        // B. CULTIVATOR LOGIC
        if (loc && loc->is_inside) {
            if (ai->behavior == "Idle" || ai->behavior == "Patrol") {
                if (rand() % 100 < 5) { // 5% chance daily to go out
                     ai->behavior = "Adventure";
                     loc->is_inside = false; 
                     if (render) render->visible_on_map = true;
                     ai->target_entity_id = -999; 
                }
            }
        } else {
            // "Outside" logic (Movement)
            if (ai->behavior == "Adventure") {
                 // Move
                 int dx = (rand() % 3) - 1;
                 int dy = (rand() % 3) - 1;
                 tc->x += dx; tc->y += dy;
                 
                 // --- RISK/REWARD ENCOUNTER ---
                 if (rand() % 100 < 10) { // 10% Chance of POI
                     // 1. Roll Danger (1-10)
                     int danger = 1 + rand() % 10;
                     int lootTier = 1;
                     if (danger > 7) lootTier = 3;      // High Risk (Realm 3-4 items)
                     else if (danger > 4) lootTier = 2; // Med Risk (Realm 2 items)
                     
                     // 2. Decide (Bravery Check)
                     auto* pers = registry.GetPersonality(e);
                     int bravery = pers ? pers->bravery : 50;
                     
                     bool takeRisk = (bravery + (rand()%20)) >= (danger * 10);
                     
                     if (takeRisk) {
                         // 3. Resolve (Success depends on Stats vs Danger)
                         int survivalChance = 90 - (danger * 5); // Danger 10 = 40% survival, Danger 1 = 85%
                         
                         if (rand() % 100 < survivalChance) {
                             // SUCCESS
                             if (inv) {
                                 auto item = GetRandomItem(lootTier);
                                 AddItem(*inv, item); // Use Stack Helper
                                 
                                 std::string iName = item.item_id;
                                 for(auto& def : GameData::GetItems()) if(def.id == item.item_id) iName = def.name;
                                 
                                 std::string riskDesc = (danger > 7) ? "DEADLY" : (danger > 4) ? "Dangerous" : "Safe";
                                 HistoryGenerator::AddEvent(render->name + " survived a " + riskDesc + " encounter and found " + iName + "!", e);
                                 
                                 if (pers) pers->bravery++;
                             }
                         } else {
                             // FAILURE
                             HistoryGenerator::AddEvent(render->name + " failed a dangerous encounter!", e);
                             // High danger = Death Chance
                             if (danger > 7 && rand()%100 < 30) {
                                HandleDeath(e, "Qi Deviation (Breakthrough Failure)"); 
                                 continue; 
                             } else {
                                  auto* inj = registry.GetInjury(e);
                                  if (inj) { 
                                      inj->severity = 2; 
                                      inj->duration = 5 * danger; 
                                      inj->description = "Adventure Wounds";
                                      inj->source = "Unknown Danger (Tier " + std::to_string(danger) + ")";
                                      inj->year_sustained = (int)(current_tick / 360); // Approx Year
                                  }
                                  ai->behavior = "Return"; 
                             }
                         }
                     } else {
                         // Too scary
                         if (danger > 5) ai->behavior = "Return"; 
                     }
                 }
                 
                 // Return home random trigger
                 if (rand() % 100 < 10) ai->behavior = "Return";
            } else if (ai->behavior == "Return") {
                if (loc && loc->location_entity_id != -1) {
                    TransformComponent* baseTC = registry.GetTransform(loc->location_entity_id);
                    if (baseTC) {
                        int dx = (baseTC->x > tc->x) ? 1 : (baseTC->x < tc->x) ? -1 : 0;
                        int dy = (baseTC->y > tc->y) ? 1 : (baseTC->y < tc->y) ? -1 : 0;
                        tc->x += dx; tc->y += dy;
                        
                        // Scavenging Logic
                        for(auto& [bagID, bagTC] : registry.transforms) {
                            if (bagID != e && bagTC.x == tc->x && bagTC.y == tc->y) {
                                auto* bagR = registry.GetRenderable(bagID);
                                if (bagR && bagR->symbol == '$' && bagR->visible_on_map) {
                                    auto* bagInv = registry.GetInventory(bagID);
                                    if (bagInv && inv) {
                                        // Take All (Stacking)
                                        inv->spirit_stones += bagInv->spirit_stones;
                                        for(auto& it : bagInv->items) {
                                            AddItem(*inv, it);
                                        }
                                        HistoryGenerator::AddEvent(render->name + " scavenged a loot bag.", e);
                                        bagR->visible_on_map = false; 
                                        bagR->symbol = ' '; 
                                        bagInv->items.clear(); // Empty the bag
                                        bagInv->spirit_stones = 0;
                                    }
                                }
                            }
                        }

                        if (tc->x == baseTC->x && tc->y == baseTC->y) {
                            loc->is_inside = true;
                            if (render) render->visible_on_map = false;
                            ai->behavior = "Idle";
                            HistoryGenerator::AddEvent(render->name + " returned to sect.", e);
                        }
                    }
                }
            }
        }
    }

    // 2. Interaction Logic (Updated Filter)
    for (auto& [pos, entities] : tileMap) {
        if (entities.size() < 2) continue;
        
        // Filter: Only Living Cultivators/NPCs interact
        std::vector<int> interactors;
        for (int e : entities) {
            // Must have Race or Sect AND be alive (visible)
            if (registry.GetRace(e) && registry.GetRenderable(e)->visible_on_map && registry.GetRenderable(e)->symbol != '$') {
                interactors.push_back(e);
            }
        }

        if (interactors.size() < 2) continue;
        
        // Pick random pair
        int idxA = rand() % interactors.size();
        int idxB = rand() % interactors.size();
        if (idxA == idxB) continue;
        
        int a = interactors[idxA];
        int b = interactors[idxB];
        
        auto* relA = registry.GetRelation(a);
        auto* renderA = registry.GetRenderable(a);
        auto* renderB = registry.GetRenderable(b);
        if (relA && renderA && renderB) {
            // Check existing affinity
            int aff = relA->network[b].affinity;
            
            // Interaction Roll
            int roll = rand() % 100;
            if (roll < 20) {
                // CHAT (+Affinity)
                relA->network[b].affinity += 5;
                if(relA->network[b].history.empty() || relA->network[b].history.back() != "Chatted") 
                    relA->network[b].history.push_back("Chatted");

                if (registry.GetRelation(b)) {
                    registry.GetRelation(b)->network[a].affinity += 5;
                     if(registry.GetRelation(b)->network[a].history.empty() || registry.GetRelation(b)->network[a].history.back() != "Chatted")
                        registry.GetRelation(b)->network[a].history.push_back("Chatted");
                }
                if (aff > 20) HistoryGenerator::AddEvent(renderA->name + " chatted with friend " + renderB->name + ".", {a, b});
            } else if (roll < 25) {
                // ARGUE (-Affinity)
                relA->network[b].affinity -= 5;
                 relA->network[b].history.push_back("Argued");
                if (registry.GetRelation(b)) {
                    registry.GetRelation(b)->network[a].affinity -= 5;
                    registry.GetRelation(b)->network[a].history.push_back("Argued");
                }
            } else if (roll < 30 && aff < -50) {
                // FIGHT
                 HistoryGenerator::AddEvent(renderA->name + " FOUGHT rival " + renderB->name + "!", {a, b});
                 relA->network[b].history.push_back("Fought");
                 
                 // Simple dmg
                 auto* statsB = registry.GetStats(b);
                 if (statsB) {
                     statsB->hp -= registry.GetStats(a)->attack;
                     if (statsB->hp <= 0) HandleDeath(b, "Combat with " + renderA->name);
                 }
            }
        }
    }
}


// Function to calculate stats based on Realm
void UpdateStats(Registry& registry, int entity) {
    auto* stats = registry.GetStats(entity);
    auto* cult = registry.GetCultivation(entity);
    if (!stats || !cult) return;

    // Exponential Scaling: 100 -> 1000 -> 10000 -> 100000
    long long multiplier = (long long)std::pow(10.0, cult->realm_major);
    
    stats->max_hp = 100 * multiplier + (cult->realm_minor * 10 * multiplier / 10);
    if (stats->hp > stats->max_hp) stats->hp = stats->max_hp; // Clamp but don't auto-heal
    
    stats->attack = 10 * multiplier + (cult->realm_minor * multiplier / 10);
    stats->defense = 2 * multiplier + (cult->realm_minor * multiplier / 20);
    
    // Lifespan increases with realm
    cult->lifespan = 100 + (cult->realm_major * 100); 
}

std::string SimEngine::GetDateString() const {
    const int TICKS_PER_DAY = 24;
    const int DAYS_PER_MONTH = 30;
    const int MONTHS_PER_YEAR = 12;
    const int TICKS_PER_YEAR = TICKS_PER_DAY * DAYS_PER_MONTH * MONTHS_PER_YEAR; // 8640

    long long ticks = current_tick;
    long long year = 1 + (ticks / TICKS_PER_YEAR);
    long long remainder = ticks % TICKS_PER_YEAR;
    long long month = 1 + (remainder / (TICKS_PER_DAY * DAYS_PER_MONTH));
    long long day = 1 + ((remainder % (TICKS_PER_DAY * DAYS_PER_MONTH)) / TICKS_PER_DAY);
    
    return "Year " + std::to_string(year) + " Month " + std::to_string(month) + " Day " + std::to_string(day);
}


void SimEngine::SystemCultivation(int ticks) {
    if (ticks <= 0) return;

    // Use a copy of keys to avoid iterator invalidation if death removes components (though we just flag dead)
    std::vector<int> entities;
    for (auto& [e, _] : registry.cultivations) entities.push_back(e);

    const int TICKS_PER_YEAR = 8640; // 360 Days * 24 Hours/Ticks

    for (int e : entities) {
        auto* cult = registry.GetCultivation(e);
        auto* stats = registry.GetStats(e);
        
        if (!cult || !stats) continue;
        if (stats->is_dead) continue; // Skip dead

        // 1. Age Progression
        cult->age_ticks += ticks;
        if (cult->age_ticks >= TICKS_PER_YEAR) {
            cult->age++;
            cult->age_ticks -= TICKS_PER_YEAR;

            // Log milestone
            if (cult->age % 10 == 0) {
                 auto* render = registry.GetRenderable(e);
                 if (render) /* Optional: Log generic age milestone? Too spammy. */ {}
            }

            // Death Check (Old Age)
            if (cult->age >= cult->lifespan) {
                auto* render = registry.GetRenderable(e);
                std::string name = render ? render->name : "Unknown";
                HistoryGenerator::AddEvent("[DEATH] " + name + " has died of old age at " + std::to_string(cult->age) + ".", e);
                HandleDeath(e);
                continue;
            }
        }

        // 2. Passive Qi Regeneration (if not injured)
        auto* injury = registry.GetInjury(e);
        if (!injury || injury->severity == 0) {
            float gain = 0.1f * cult->talent * ticks; // Slow passive gain per tick
            cult->qi += gain;
            
            // Cap at Max
            if (cult->qi >= cult->max_qi) cult->qi = cult->max_qi;
            
            // Note: Active breakthroughs happen in specialized events or active cultivation, 
            // but we could add a passive breakthrough chance here too if desired.
        }
    }
}

// --- Sect Economy ---
void SimEngine::DonateToSect(int entityID, const std::string& itemID, int count) {
    auto* inv = registry.GetInventory(entityID);
    auto* sectC = registry.GetSect(entityID);
    if (!inv || !sectC) return;
    
    // Find Leader
    SectResourceComponent* treasury = nullptr;
    for(auto& [bid, sc] : registry.sects) {
        if(sc.sect_id == sectC->sect_id && sc.rank == 5) {
            treasury = registry.GetSectResource(bid);
            break;
        }
    }
    if (!treasury) return;

    // Check Player Item
    auto it = std::find_if(inv->items.begin(), inv->items.end(), [&](const InventoryComponent::ItemInstance& i){ return i.item_id == itemID; });
    if (it != inv->items.end() && it->count >= count) {
        it->count -= count;
        if (it->count <= 0) inv->items.erase(it);
        
        bool stacked = false;
        for(auto& sit : treasury->stash) {
            if(sit.item_id == itemID) { sit.count += count; stacked = true; break; }
        }
        if(!stacked) treasury->stash.push_back({itemID, count, 100, false});
        
        int value = 1;
        for(const auto& def : GameData::GetItems()) {
            if(def.id == itemID) { value = def.value; break; }
        }
        inv->contribution_points += (value * count);
        std::cout << "[Economy] Entity " << entityID << " donated " << count << "x " << itemID << ". CP: " << inv->contribution_points << std::endl;
    }
}

void SimEngine::ExchangeFromSect(int entityID, const std::string& itemID) {
    auto* inv = registry.GetInventory(entityID);
    auto* sectC = registry.GetSect(entityID);
    if (!inv || !sectC) return;
    
    SectResourceComponent* treasury = nullptr;
    for(auto& [bid, sc] : registry.sects) {
        if(sc.sect_id == sectC->sect_id && sc.rank == 5) {
            treasury = registry.GetSectResource(bid);
            break;
        }
    }
    if (!treasury) return;

    auto it = std::find_if(treasury->stash.begin(), treasury->stash.end(), [&](const InventoryComponent::ItemInstance& i){ return i.item_id == itemID; });
    if (it != treasury->stash.end()) {
        int cost = 99999;
        for(const auto& def : GameData::GetItems()) {
            if(def.id == itemID) { cost = def.value; break; }
        }
        
        if (inv->contribution_points >= cost) {
            inv->contribution_points -= cost;
            AddItem(*inv, {itemID, 1, 100, false});
            it->count--;
            if (it->count <= 0) treasury->stash.erase(it);
             std::cout << "[Economy] Entity " << entityID << " exchanged CP for " << itemID << std::endl;
        }
    }
}

void SimEngine::PlayerMove(int dx, int dy) {
    if (player_id == -1) return;
    auto* tc = registry.GetTransform(player_id);
    if (tc) {
        tc->x += dx; tc->y += dy;
        
        // Random Encounter Check
        EventManager::Get().CheckRandomEncounter(player_id, 
            tc->x, tc->y, 
            "Plains"); // Placeholder Biome: In future, get from WorldMap::Get().GetTile(x,y).biome
            
        AdvanceTime(1);
    }
}

// ... (skipping PlayerCultivate/UpdateRelations/GetRelation/SimulateHistory start) ...
// NOTE: I cannot use ReplaceFileContent well for disjoint edits.
// I will split this into two calls.
// Call 1: Insert CP Methods.
// Call 2: Update Breakthrough Logic.


void SimEngine::PlayerCultivate(int ticks) {
    if (player_id == -1) return;
    auto* cc = registry.GetCultivation(player_id);
    if (cc) {
        cc->qi += ticks * 5.0f;
        AdvanceTime(ticks);
    }
}

// Helper for Relations
void SimEngine::UpdateRelations(int entityA, int entityB, int change) {
    if (entityA == entityB) return;
    
    // Check if components exist
    auto* relA = registry.GetRelation(entityA);
    auto* relB = registry.GetRelation(entityB);
    
    // Force Create if missing (lazy initialization)
    if (!relA && registry.GetStats(entityA)) { // Only if alive
         registry.relations[entityA] = RelationComponent{};
         relA = &registry.relations[entityA];
         relA->gender = (rand() % 2 == 0) ? "Male" : "Female"; // Assign gender on init
    }
    if (!relB && registry.GetStats(entityB)) {
         registry.relations[entityB] = RelationComponent{};
         relB = &registry.relations[entityB];
         relB->gender = (rand() % 2 == 0) ? "Male" : "Female";
    }
    
    if (relA) {
        // Initialize entry if new
        if (relA->network.find(entityB) == relA->network.end()) {
            relA->network[entityB] = {entityB, 0, 0, "None", {}};
        }
        relA->network[entityB].affinity += change;
        if (relA->network[entityB].affinity > 100) relA->network[entityB].affinity = 100;
        if (relA->network[entityB].affinity < -100) relA->network[entityB].affinity = -100;
        
        // Log Significant Changes
        if (std::abs(change) > 10) {
             std::string log = "Year " + std::to_string(current_tick / 8640) + ": Affinity changed by " + std::to_string(change);
             relA->network[entityB].history.push_back(log);
        }
        
        // MARRIAGE CHECK (Simplistic for now)
        if (relA->network[entityB].affinity >= 90 && relA->spouse_id == -1) {
             if (relB && relB->spouse_id == -1 && relA->gender != relB->gender) {
                 // Proposal Event (10% Chance per update if criteria met)
                 if (rand() % 100 < 10) {
                     relA->spouse_id = entityB;
                     relB->spouse_id = entityA;
                     relA->network[entityB].type = "Spouse";
                     relB->network[entityA].type = "Spouse";
                     HistoryGenerator::AddEvent(registry.GetRenderable(entityA)->name + " and " + registry.GetRenderable(entityB)->name + " became Taoist Companions!", {entityA, entityB});
                 }
             }
        }
    }
    
    // Symmetric update for now (or make it asymmetric? Design says Graph, so A likes B doesn't mean B likes A?)
    // Game Design implies "Relationship" is mutual usually, but "Nemesis" can be one-sided.
    // Let's mirror it for simplicity unless specified.
    if (relB) {
         if (relB->network.find(entityA) == relB->network.end()) {
            relB->network[entityA] = {entityA, 0, 0, "None", {}};
        }
        relB->network[entityA].affinity += change;
        if (relB->network[entityA].affinity > 100) relB->network[entityA].affinity = 100;
        if (relB->network[entityA].affinity < -100) relB->network[entityA].affinity = -100;
        
        if (relA) relB->network[entityA].type = relA->network[entityB].type; // Sync types
    }
}

int SimEngine::GetRelation(int sectA, int sectB) {
    if (sectRelations[sectA].find(sectB) == sectRelations[sectA].end()) return 0; // Neutral
    return sectRelations[sectA][sectB];
}

void SimEngine::SimulateHistory(int years) {
    std::cout << "[SimEngine] Deep Simulation of " << years << " years..." << std::endl;

    const int TICKS_PER_YEAR = 8640; 
    const int TICKS_PER_MONTH = TICKS_PER_YEAR / 12;

    for (int year = 1; year <= years; ++year) {
        
        // Use a vector of IDs to avoid iterator invalidation when creating/destroying
        std::vector<int> allEntities;
        for (auto& [e, _] : registry.transforms) allEntities.push_back(e);

        EventManager::Get().UpdateGlobalEvents(year);

        // MONTHLY LOOP
        for (int month = 1; month <= 12; ++month) {
            current_tick += TICKS_PER_MONTH;
            
            // --- 1. Pass: Aging & Cultivation (Resource Based) ---
            for (int e : allEntities) {
                 auto* cult = registry.GetCultivation(e);
                 auto* stats = registry.GetStats(e);
                 auto* injury = registry.GetInjury(e);
                 
                 if (!cult || !stats) continue;

                 // Accumulate Age Ticks
                 cult->age_ticks += TICKS_PER_MONTH;
                 if (cult->age_ticks >= TICKS_PER_YEAR) {
                     cult->age++;
                     cult->age_ticks -= TICKS_PER_YEAR;

                     // Birthday Logic (Optional: Log specific birthday events?)
                     
                     // Death Check (Yearly check on birthday)
                     if (cult->age > cult->lifespan) {
                         HistoryGenerator::AddEvent("[DEATH] " + (registry.GetRenderable(e) ? registry.GetRenderable(e)->name : "Unknown") + " died of old age at " + std::to_string(cult->age) + ".", e);
                         HandleDeath(e, "Old Age");
                         continue; 
                     }
                 }
                 
                 // Heal Injury
                 if (injury && injury->severity > 0) {
                     injury->duration--; // Duration is in months? Or ticks? Assuming simulation Abstract units.
                     // Let's assume duration is decremented monthly here.
                     if (injury->duration <= 0) {
                         injury->severity = 0;
                         injury->description = "Healthy";
                     } else {
                         continue; // Cannot cultivate while injured (or reduced)
                     }
                 }

                 // Base Gain (Monthly)
                 float gain = (50.0f * cult->talent) / 12.0f; // Yearly rate divided by 12
                 
                 auto* pers = registry.GetPersonality(e);
                 if (pers && pers->diligence > 60) gain *= 1.2f;

                 cult->qi += gain;
                 
                 // Breakthrough
                 if (cult->qi >= cult->max_qi) {
                     // Reduced chance per month to balance frequency
                     if ((rand() % 100) < 5) { 
                         cult->realm_minor++;
                         if (cult->realm_minor > 9) {
                             cult->realm_minor = 1;
                             cult->realm_major++;
                             HistoryGenerator::AddEvent(registry.GetRenderable(e)->name + " broke through to Realm " + std::to_string(cult->realm_major) + "!", e);
                         }
                         cult->qi = 0;
                         cult->max_qi *= 1.5f;
                         UpdateStats(registry, e);
                     } else {
                     // FAILED BREAKTHROUGH
                     bool died = false;
                     // Risk Check: Bottleneck Death
                     if (cult->realm_major >= 3) { // Core Formation+
                         int deathChance = (cult->realm_major - 2) * 5; // R3=5%, R4=10%, R5=15%, R6=20%
                         if (rand() % 100 < deathChance) {
                             died = true;
                             HistoryGenerator::AddEvent("[DEATH] " + registry.GetRenderable(e)->name + " DIED attempting to break through to Realm " + std::to_string(cult->realm_major+1) + "!", e);
                             HandleDeath(e);
                         }
                     }
                     
                     if (!died) {
                         if (injury) {
                             injury->severity = 1; 
                             injury->duration = 3;
                             injury->description = "Qi Deviation (Minor)";
                             HistoryGenerator::AddEvent(registry.GetRenderable(e)->name + " suffered Qi Deviation!", e);
                         }
                         cult->qi = cult->max_qi * 0.8f;
                     }
                     }
                 }
             }
        } // Close Monthly Loop
        
        // --- 2. Pass: Sect Logic (Diplomacy, Collapse, Raids) ---
        // Group by Sect
        std::map<int, std::vector<int>> sectMembers;
        for (auto& [e, s] : registry.sects) {
            sectMembers[s.sect_id].push_back(e);
        }
        
        std::vector<int> collapsedSects;
        
        for (auto& [sectID, members] : sectMembers) {
            if (members.empty()) continue;
            
            // Ignored Special Sects (Bandits)
            if (sectID == 999) continue;
            
            // Find Leader
            int leaderID = -1;
            for (int mid : members) {
                if (registry.GetSect(mid)->rank == 5) { leaderID = mid; break; }
            }
            if (leaderID == -1) continue; 
            
            auto* pers = registry.GetPersonality(leaderID);
            auto* sectRes = registry.GetSectResource(leaderID);
            auto* sectInfo = registry.GetSect(leaderID);
            
            if (!pers || !sectRes) continue;
            
            // A. Consumption & Scarcity
            sectRes->spirit_stones -= 100; // Consumption
            
             if (sectRes->spirit_stones < 0) {
                sectRes->starving = true;
                sectRes->starvation_years++;
                sectRes->spirit_stones = 0;
            } else {
                sectRes->starving = false;
                sectRes->starvation_years = 0;
            }
            
            // B. COLLAPSE CHECK
            if (sectRes->starvation_years >= 5) {
                HistoryGenerator::AddEvent("[COLLAPSE] " + sectInfo->name + " has collapsed due to starvation!", leaderID);
                collapsedSects.push_back(sectID);
                continue; // Stop processing this sect
            }
            
            // C. DIPLOMACY (Ask for Help)
            if (sectRes->starving) {
                bool gotHelp = false;
                for (auto& [otherID, otherRes] : registry.sect_resources) {
                    if (otherID == leaderID) continue;
                    
                    if (registry.GetSect(otherID) == nullptr) continue;
                    int otherSectID = registry.GetSect(otherID)->sect_id;
                    
                    int relation = GetRelation(sectID, otherSectID);
                    
                    // Simple "Good Neighbor" check if relation is positive
                    if (relation >= 0 && otherRes.spirit_stones > 2000) {
                        // Aid Granted
                        int aid = 500;
                        otherRes.spirit_stones -= aid;
                        sectRes->spirit_stones += aid;
                        UpdateRelations(sectID, otherSectID, 10);
                        HistoryGenerator::AddEvent(sectInfo->name + " requested aid. " + registry.GetSect(otherID)->name + " sent " + std::to_string(aid) + " stones.", {leaderID, otherID});
                        gotHelp = true;
                        break;
                    }
                }
            }
            
            // D. RAID
            bool tryRaid = (sectRes->starving || (pers->ambition > 60 && rand() % 100 < 20));
            
            if (tryRaid) {
                int targetSectID = -1;
                 for (auto& [otherID, _] : sectMembers) {
                    if (otherID != sectID && otherID != 999) { targetSectID = otherID; break; } 
                }
                
                if (targetSectID != -1) {
                    long long myPower = 0;
                    for (int mid : members) { UpdateStats(registry, mid); myPower += registry.GetStats(mid)->attack; }
                    
                    long long enemyPower = 0;
                    auto& enemies = sectMembers[targetSectID];
                    int defenderID = -1;
                    for (int eid : enemies) {
                        UpdateStats(registry, eid);
                        enemyPower += registry.GetStats(eid)->defense + registry.GetStats(eid)->hp / 10; 
                        if (registry.GetSect(eid)->rank == 5) defenderID = eid;
                    }
                    
                    long long attackRoll = (long long)(myPower * (pers->wisdom / 50.0f) * (0.8f + (rand()%40)/100.0f));
                    
                    std::string sectName = sectInfo->name;
                    std::string enemyName = (defenderID != -1) ? registry.GetSect(defenderID)->name : "Unknown Sect";
                    
                    if (attackRoll > enemyPower) {
                        // VICTORY
                        int stolen = 1000;
                        sectRes->spirit_stones += stolen;
                         if (defenderID != -1 && registry.GetSectResource(defenderID)) {
                            registry.GetSectResource(defenderID)->spirit_stones = std::max(0, registry.GetSectResource(defenderID)->spirit_stones - stolen);
                            UpdateRelations(sectID, targetSectID, -50); // War!
                        }
                        std::vector<int> participants = {leaderID};
                        if (defenderID != -1) participants.push_back(defenderID);
                        HistoryGenerator::AddEvent(sectName + " RAIDED " + enemyName + "! Stole " + std::to_string(stolen) + " stones.", participants);
                    } else {
                         std::vector<int> participants = {leaderID};
                         if (defenderID != -1) participants.push_back(defenderID);
                         HistoryGenerator::AddEvent(sectName + " FAILED to raid " + enemyName + ".", participants);
                         UpdateRelations(sectID, targetSectID, -30);
                    }
                }
            }
            
             // Passive Income
            sectRes->spirit_stones += 60 + (pers->diligence);
        }
        
        // --- 3. Pass: Handle Collapsed Sects ---
        for (int collapsedID : collapsedSects) {
             std::vector<int>& members = sectMembers[collapsedID];
             for (int mid : members) {
                 // 50% Bandit, 50% Refugee
                 SectComponent* sc = registry.GetSect(mid);
                 RenderComponent* rc = registry.GetRenderable(mid);
                 PersonalityComponent* pc = registry.GetPersonality(mid);
                 
                  // Remove Sect resources from leader maps if leader
                 if (sc && sc->rank == 5) {
                     registry.sect_resources.erase(mid);
                 }
                 
                 if (rand() % 100 < 50) {
                     // BECOME BANDIT
                     if (rc) rc->name = "Bandit " + rc->name;
                     if (sc) {
                         sc->name = "Bandits"; 
                         sc->sect_id = 999; 
                         sc->rank = 1; 
                     }
                     if (pc) {
                         pc->ruthlessness += 20;
                         pc->current_goal = "Robbery";
                     }
                     registry.ai[mid].behavior = "Aggressive";
                 } else {
                     // SEEK REFUGE (Join random other sect)
                      int targetSectID = -1;
                      std::string targetSectName = "";
                      for (auto& [sid, m] : sectMembers) {
                          if (sid != collapsedID && sid != 999) {
                              targetSectID = sid;
                              if (!m.empty()) targetSectName = registry.GetSect(m[0])->name;
                              break; 
                          }
                      }
                      
                      if (targetSectID != -1) {
                          if (sc) {
                              sc->sect_id = targetSectID;
                              sc->name = targetSectName;
                              sc->rank = 1; 
                          }
                          if (rc) rc->name = rc->name + " (Refugee)";
                          if (pc) pc->current_goal = "Recover";
                      }
                 }
             }
        }
        
        // --- 4. Pass: Adventures (Random Encounters) ---
        for (int e : allEntities) {
            auto* ai = registry.GetAI(e);
            auto* pers = registry.GetPersonality(e);
            if (!ai || !pers) continue;
            
            // Adventure Chance (Bravery/Curiosity)
            if (pers->bravery > 60 && (rand() % 100 < 5)) { // 5% chance per year
                // Event Roll
                int roll = rand() % 100;
                std::string npcName = registry.GetRenderable(e)->name;
                
                if (roll < 20) {
                    // Resource Find
                    HistoryGenerator::AddEvent(npcName + " went adventuring and found a Spirit Vein! (Qi +500)");
                    registry.GetCultivation(e)->qi += 500;
                } else if (roll < 40) {
                    // Beast Attack
                    HistoryGenerator::AddEvent(npcName + " was attacked by a Demon Beast!");
                    long long myAtk = registry.GetStats(e)->attack;
                    long long beastAtk = (long long)(myAtk * 0.8f + (rand() % (int)(myAtk * 0.5f + 1))); 
                    
                    if (myAtk > beastAtk) {
                        HistoryGenerator::AddEvent("...and slew the beast! (Bravery +1)");
                        pers->bravery++;
                    } else {
                        HistoryGenerator::AddEvent("...and was heavily injured!");
                        auto* inj = registry.GetInjury(e);
                        if (inj) { inj->severity = 3; inj->duration = 10; inj->description = "Beast Mauling"; }
                    }
                } 
                // else nothing interesting
            }
        }
    }
    std::cout << "[SimEngine] Deep Simulation Complete." << std::endl;
}


