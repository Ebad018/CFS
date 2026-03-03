#include "WorldGen.h"
#include "ECS.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <set>
#include <string>

// Simple LCG random for deterministic noise based on coordinates
float WorldGen::Noise(int x, int y, int seed) {
    int n = x + y * 57 + seed * 131;
    n = (n << 13) ^ n;
    return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
}

// Map layer generation
void WorldGen::GenerateLayer(MapLayer& layer, const Config& config) {
    for (int y = 0; y < layer.height; ++y) {
        for (int x = 0; x < layer.width; ++x) {
            float noiseVal = PerlinNoise((float)x, (float)y, config);
            
            // Normalize to 0-1
            noiseVal = (noiseVal + 1.0f) * 0.5f;

            // Apply Contrast Stretch to force extremes (More Water/Mountains/Snow)
            // Center is 0.5. Stretch factor 2.0.
            noiseVal = (noiseVal - 0.5f) * 2.5f + 0.5f;
            noiseVal = std::clamp(noiseVal, 0.0f, 1.0f);
            
            Tile& tile = layer.tiles[y * layer.width + x];
            tile.x = x;
            tile.y = y;
            tile.z = layer.z_index;
            
            // Enhanced Biome Logic (More variety - Adjusted Thresholds)
            // Noise is 0.0 to 1.0 (clamped)
            if (noiseVal < 0.15f) {
                tile.type = "DEEP_WATER";
                tile.is_walkable = false;
                tile.qi_density = 15.0f;
            } else if (noiseVal < 0.25f) {
                tile.type = "WATER";
                tile.is_walkable = false;
                tile.qi_density = 10.0f;
            } else if (noiseVal < 0.30f) {
                tile.type = "SAND"; // Beach
                tile.is_walkable = true;
                tile.qi_density = 5.0f;
            } else if (noiseVal < 0.45f) {
                tile.type = "SWAMP"; 
                tile.is_walkable = true;
                tile.qi_density = 8.0f; 
            } else if (noiseVal < 0.60f) {
                tile.type = "GRASS";
                tile.is_walkable = true;
                tile.qi_density = 12.0f;
            } else if (noiseVal < 0.75f) {
                tile.type = "FOREST";
                tile.is_walkable = true;
                tile.qi_density = 30.0f;
            } else if (noiseVal < 0.85f) {
                tile.type = "MOUNTAIN";
                tile.is_walkable = true; 
                tile.qi_density = 60.0f;
            } else if (noiseVal < 0.92f) {
                tile.type = "VOLCANO"; 
                tile.is_walkable = true;
                tile.qi_density = 80.0f; 
            } else {
                tile.type = "SNOW";
                tile.is_walkable = true;
                tile.qi_density = 40.0f;
            }

            // Simple Random POI Generation (1% chance)
            if (tile.is_walkable && (rand() % 1000) < 5) { // 0.5% chance
                 if (tile.type == "MOUNTAIN") tile.poi_type = "SECT_RUINS";
                 else if (tile.type == "FOREST") tile.poi_type = "DEMON_BEAST_LAIR";
                 else if (tile.type == "GRASS") tile.poi_type = "VILLAGE";
            }
        }
    }
}

// Adding Resource Populator Logic
void WorldGen::PopulateResources(Registry& registry, MapLayer& layer) {
    for (int y = 0; y < layer.height; ++y) {
        for (int x = 0; x < layer.width; ++x) {
            const auto& tile = layer.tiles[y * layer.width + x];
            if (!tile.is_walkable) continue;
            
            // 2% chance for resource
            if ((rand() % 100) < 2) {
                int entity = registry.CreateEntity();
                registry.transforms[entity] = { x, y, layer.z_index };
                
                std::string resType = "Herb";
                int amount = 10;
                uint32_t color = 0x00FF00FF; // Green
                char sym = '*';
                
                if (tile.type == "MOUNTAIN" || tile.type == "VOLCANO") {
                    resType = "Spirit Stone Mine";
                    amount = 50 + (rand() % 50);
                    color = 0x00FFFFFF; // Cyan
                    sym = '#';
                } else if (tile.type == "FOREST" || tile.type == "SWAMP") {
                    resType = "Spirit Herb";
                    amount = 5 + (rand() % 10);
                    color = 0xFF0000FF; // Red
                    sym = '%';
                } else {
                    resType = "Common Ore";
                    amount = 20;
                    color = 0x808080FF; // Grey
                    sym = 'o';
                }
                
                registry.resources[entity] = { "RES_" + std::to_string(entity), resType, amount, true };
                registry.renderables[entity] = { sym, color, resType };
            }
        }
    }
}

float WorldGen::SmoothedNoise(int x, int y, int seed) {
    float corners = (Noise(x - 1, y - 1, seed) + Noise(x + 1, y - 1, seed) + Noise(x - 1, y + 1, seed) + Noise(x + 1, y + 1, seed)) / 16.0f;
    float sides = (Noise(x - 1, y, seed) + Noise(x + 1, y, seed) + Noise(x, y - 1, seed) + Noise(x, y + 1, seed)) / 8.0f;
    float center = Noise(x, y, seed) / 4.0f;
    return corners + sides + center;
}

float Interpolate(float a, float b, float x) {
    float ft = x * 3.1415927f;
    float f = (1.0f - cos(ft)) * 0.5f;
    return a * (1.0f - f) + b * f;
}

float WorldGen::InterpolatedNoise(float x, float y, int seed) {
    int integer_X = (int)floor(x);
    float fractional_X = x - integer_X;
    int integer_Y = (int)floor(y);
    float fractional_Y = y - integer_Y;

    float v1 = SmoothedNoise(integer_X, integer_Y, seed);
    float v2 = SmoothedNoise(integer_X + 1, integer_Y, seed);
    float v3 = SmoothedNoise(integer_X, integer_Y + 1, seed);
    float v4 = SmoothedNoise(integer_X + 1, integer_Y + 1, seed);

    float i1 = Interpolate(v1, v2, fractional_X);
    float i2 = Interpolate(v3, v4, fractional_X);

    return Interpolate(i1, i2, fractional_Y);
}

float WorldGen::PerlinNoise(float x, float y, const Config& config) {
    float total = 0;
    float frequency = config.scale;
    float amplitude = 1.0f;
    float maxValue = 0;  // Used for normalizing result to 0.0 - 1.0

    for (int i = 0; i < config.octaves; i++) {
        total += InterpolatedNoise(x * frequency, y * frequency, config.seed) * amplitude;

        maxValue += amplitude;
        amplitude *= config.persistence;
        frequency *= config.lacunarity;
    }

    return total / maxValue;
}

#include "GameData.h"
#include "ECS.h"
#include "NamingGenerator.h"
#include "NPCGenerator.h"

void WorldGen::PopulateWorld(class Registry& registry, MapLayer& layer) {
    auto& beasts = GameData::GetBeasts();
    auto& resources = GameData::GetResources();

    for (const auto& tile : layer.tiles) {
        if (!tile.is_walkable) continue;

        // Resource Spawning (10% chance)
        if ((rand() % 100) < 10) {
            std::vector<ResourceDefinition> candidates;
            for (const auto& res : resources) {
                if (res.biome == tile.type) {
                    candidates.push_back(res);
                }
            }

            if (!candidates.empty()) {
                const auto& res = candidates[rand() % candidates.size()];
                int entity = registry.CreateEntity();
                
                registry.transforms[entity] = { tile.x, tile.y, layer.z_index };
                registry.renderables[entity] = { res.symbol, res.color, res.name };
                registry.resources[entity] = { res.id, res.type, 1, true };
            }
        }

        // Beast Spawning (2% chance)
        if ((rand() % 100) < 2) {
            std::vector<BeastDefinition> candidates;
            for (const auto& beast : beasts) {
                if (beast.biome == tile.type) {
                    candidates.push_back(beast);
                }
            }

            if (!candidates.empty()) {
                const auto& beast = candidates[rand() % candidates.size()];
                int entity = registry.CreateEntity();
                
                registry.transforms[entity] = { tile.x, tile.y, layer.z_index };
                registry.renderables[entity] = { beast.symbol, beast.color, beast.name };
                registry.renderables[entity].visible_on_map = false; // Invisible by default per user request
                registry.stats[entity] = { beast.tier * 50, beast.tier * 50, beast.tier * 10, beast.tier * 2 };
                if (beast.aggressive) {
                    registry.ai[entity] = { "Aggressive", -1, 10 };
                } else {
                    registry.ai[entity] = { "Wander", -1, 10 };
                }
            }
        }
    }
}




// --- Multiverse Generators ---

void WorldGen::GenerateSky(MapLayer& layer, const Config& config) {
    // Top-tier floating islands. 
    // High threshold for noise -> islands.
    for (int y = 0; y < layer.height; ++y) {
        for (int x = 0; x < layer.width; ++x) {
            float val = PerlinNoise((float)x, (float)y, config);
            val = (val + 1.0f) * 0.5f;

            Tile& tile = layer.tiles[y * layer.width + x];
            tile.x = x; tile.y = y; tile.z = layer.z_index;
            tile.hazard = "Thin Air"; // Standard Sky Hazard

            if (val > 0.70f) {
                tile.type = "SKY_ISLAND_GRASS";
                tile.is_walkable = true;
                tile.qi_density = 50.0f;
            } else if (val > 0.65f) {
                 tile.type = "SKY_ISLAND_ROCK";
                 tile.is_walkable = true;
                 tile.qi_density = 40.0f;
            } else {
                tile.type = "SKY_VOID"; // Fall to death
                tile.is_walkable = false; // Need flight
                tile.qi_density = 100.0f; // Pure Qi in air
            }
        }
    }
}

void WorldGen::GenerateVoid(MapLayer& layer, const Config& config) {
    // Chaotic. Random spatial tears.
    for (int y = 0; y < layer.height; ++y) {
        for (int x = 0; x < layer.width; ++x) {
            Tile& tile = layer.tiles[y * layer.width + x];
            tile.x = x; tile.y = y; tile.z = layer.z_index;
            tile.hazard = "Spatial Shear";
            
            float val = PerlinNoise((float)x * 5.0f, (float)y * 5.0f, config); // High frequency chaos
             val = (val + 1.0f) * 0.5f;
             
             if (val > 0.8f) {
                 tile.type = "VOID_CRYSTAL";
                 tile.is_walkable = true;
                 tile.qi_density = 500.0f;
             } else if (val < 0.2f) {
                 tile.type = "SPATIAL_TEAR";
                 tile.is_walkable = false;
                 tile.qi_density = -100.0f;
             } else {
                 tile.type = "VOID_EMPTY";
                 tile.is_walkable = false; // Need specialized movement
                 tile.qi_density = 10.0f;
             }
        }
    }
}

void WorldGen::GenerateUnderdark(MapLayer& layer, const Config& config) {
    // Caves. 
    for (int y = 0; y < layer.height; ++y) {
        for (int x = 0; x < layer.width; ++x) {
             Tile& tile = layer.tiles[y * layer.width + x];
            tile.x = x; tile.y = y; tile.z = layer.z_index;
            tile.hazard = "Darkness";
            
            // Simple noise for caves for now (Simplex/Ridged is better but sticking to Perlin)
            float val = PerlinNoise((float)x * 2.0f, (float)y * 2.0f, config);
            val = (val + 1.0f) * 0.5f;
            
            if (val > 0.45f && val < 0.55f) {
                // Tunnels
                tile.type = "CAVE_FLOOR";
                tile.is_walkable = true;
                tile.qi_density = 20.0f;
            } else {
                tile.type = "CAVE_WALL";
                tile.is_walkable = false;
                tile.qi_density = 0.0f;
            }
        }
    }
}

void WorldGen::GenerateAbyss(MapLayer& layer, const Config& config) {
    // Hellscape
     for (int y = 0; y < layer.height; ++y) {
        for (int x = 0; x < layer.width; ++x) {
             Tile& tile = layer.tiles[y * layer.width + x];
            tile.x = x; tile.y = y; tile.z = layer.z_index;
            tile.hazard = "Miasma";
            
            float val = PerlinNoise((float)x, (float)y, config);
            val = (val + 1.0f) * 0.5f;
            
            if (val < 0.4f) {
                tile.type = "ABYSS_LAVA";
                tile.is_walkable = false;
            } else {
                tile.type = "ABYSS_ROCK";
                tile.is_walkable = true;
            }
            tile.qi_density = 0.0f; // Anti-Qi
        }
    }
}

// Helper for English Sect Names
std::string GenerateEnglishSectName() {
    std::vector<std::string> prefixes = { "Mystic", "Heavenly", "Blood", "Violet", "Azure", "Golden", "Frozen", "Raging", "Nether", "Spirit", "Grand", "Scarlet", "White", "Shadow" };
    std::vector<std::string> cores = { "Sword", "Saber", "Pill", "Vessel", "Beast", "Dragon", "Phoenix", "Tiger", "Ghost", "Star", "Moon", "Sun", "Cloud", "Thunder", "Ocean", "Mountain", "Bone", "Soul" };
    std::vector<std::string> suffixes = { "Sect", "Gate", "Valley", "Pavilion", "Palace", "Hall", "Grotto", "Mountain", "Courtyard", "Cult", "Tower" };

    std::string p = prefixes[rand() % prefixes.size()];
    std::string c = cores[rand() % cores.size()];
    std::string s = suffixes[rand() % suffixes.size()];
    
    return p + " " + c + " " + s;
}

void WorldGen::PopulateSects(class Registry& registry, MapLayer& layer) {
    // Only Populate Sects on Mortal Plane for now
    if (layer.z_index != 0) return;
    
    // Generate 3-5 Sects
    int sectCount = 3 + (rand() % 3);

    std::set<std::string> localUsedNames;

    for (int i = 0; i < sectCount; ++i) {
        int cx = rand() % layer.width;
        int cy = rand() % layer.height;
        
        // Ensure sect is on walkable land, preferably Mountain
        std::string biomeType = "MOUNTAIN";
        
        int attempts = 0;
        while (attempts < 100) {
             if (layer.GetTile(cx, cy).type == biomeType && layer.GetTile(cx, cy).is_walkable) break;
             cx = rand() % layer.width;
             cy = rand() % layer.height;
             attempts++;
        }
        
        std::string sectName = GenerateEnglishSectName();
        int attemptsName = 0;
        while (localUsedNames.count(sectName) && attemptsName < 50) {
             sectName = GenerateEnglishSectName();
             attemptsName++;
        }
        localUsedNames.insert(sectName);
        int sectUniqueId = i + 1; // 1-based ID

        // Create Base First so members can be "Inside" it
        int baseEntityID = registry.CreateEntity();
        registry.transforms[baseEntityID] = { cx, cy, layer.z_index };
        registry.renderables[baseEntityID] = { '^', 0xFF00FFFF, sectName + " Base" };
        registry.renderables[baseEntityID].visible_on_map = true;
        // Mark it as a location? Actually we don't have a component for "IsLocation", but we can infer or add one.
        // For now, we just know its ID.
        
        
        // --- Mass Population (Mixed Hierarchy) ---
        struct RankConfig { int count; int rank; std::string title_suffix; uint32_t color; };
        std::vector<RankConfig> ranks = {
            {1, 6, " Grd. Eld", 0xFF4500FF}, // Grand Elder (Rank 6)
            {1, 5, " Leader", 0xFFD700FF}, 
            {2, 4, " Elder", 0xC0C0C0FF},  
            {5, 3, " Core Dis.", 0x800080FF}, 
            {10, 2, " Inner Dis.", 0x0000FFFF}, 
            {32, 1, " Outer Dis.", 0xFFFFFFFF}  
        };

        for (const auto& r : ranks) {
            for (int k = 0; k < r.count; ++k) {
                int entity = registry.CreateEntity();
                
                // Scatter them slightly around the base
                int ox = (rand() % 5) - 2;
                int oy = (rand() % 5) - 2;
                int nx = std::clamp(cx + ox, 0, layer.width - 1);
                int ny = std::clamp(cy + oy, 0, layer.height - 1);

                // --- NEW MODULAR GENERATION ---
                NPCGenContext ctx;
                ctx.sect_id = sectName;
                ctx.role = (r.rank == 6) ? "GRAND_ELDER" : 
                           (r.rank == 5) ? "LEADER" : 
                           (r.rank == 4) ? "ELDER" : 
                           (r.rank == 3) ? "CORE_DISCIPLE" : 
                           (r.rank == 2) ? "INNER_DISCIPLE" : "DISCIPLE";
                
                // Map Rank to Realm Index for Generator
                // 0=Mortal, 1=Qi, 2=Found, 3=Core, 4=Nascent, 5=Soul, 6=Emp
                int minRealm = 0;
                if (r.rank == 6) minRealm = 6;
                else if (r.rank == 5) minRealm = 4;
                else if (r.rank == 4) minRealm = 3;
                else if (r.rank == 3) minRealm = 2;
                else if (r.rank == 2) minRealm = 1;
                
                ctx.min_realm_idx = minRealm;

                // Generate Unique NPC Data
                NPC npc = NPCGenerator::Generate(ctx);

                // Persist to Disk
                std::string saveDir = "SaveData/World_01/Sects/" + sectName + "/Members";
                NPCGenerator::SaveNPC(npc, saveDir);

                // --- MAP TO ECS (Runtime) ---
                // Identity
                registry.transforms[entity] = { nx, ny, layer.z_index };
                registry.renderables[entity] = { ' ', r.color, npc.identity.given_name + " " + npc.identity.surname + r.title_suffix }; // Symbol will be set below
                registry.renderables[entity].symbol = (r.rank >= 4) ? '@' : 'a';
                
                // Sect
                registry.sects[entity] = { sectName, r.rank, sectUniqueId };
                
                // Race
                registry.races[entity] = { npc.biology.race_id, "Humanoid", npc.biology.strength_mod, 1.0f }; // Simplification

                // Cultivation
                // Map "Golden Core" string to int level? 
                // Simple mapping for now: Mortal=0, Qi=1, Found=2, Core=3
                int realmInt = 0;
                if (npc.cultivation.realm.major == "Qi Condensation") realmInt = 1;
                else if (npc.cultivation.realm.major == "Foundation") realmInt = 2;
                else if (npc.cultivation.realm.major == "Golden Core") realmInt = 3;
                else if (npc.cultivation.realm.major == "Nascent Soul") realmInt = 4;
                else if (npc.cultivation.realm.major == "Soul Formation") realmInt = 5;
                else if (npc.cultivation.realm.major == "Emperor") realmInt = 6;
                
                registry.cultivations[entity] = { 
                    (float)npc.cultivation.qi_current, 
                    (float)npc.cultivation.qi_max, 
                    realmInt, 
                    npc.cultivation.realm.minor, 
                    npc.cultivation.attribute, 
                    npc.cultivation.realm.purity, 
                    npc.identity.age, 
                    npc.identity.lifespan 
                };

                // Stats (Recalculate based on Gen)
                long long multiplier = (long long)std::pow(10.0, realmInt);
                long long maxHP = 100 * multiplier * npc.biology.strength_mod;
                long long atk = 10 * multiplier;
                long long def = 2 * multiplier;
                
                registry.stats[entity] = { maxHP, maxHP, atk, def, 1.0f };
                registry.ai[entity] = { "Patrol", sectUniqueId, 10 };
                registry.relations[entity].gender = npc.identity.gender;

                // Inventory & Skills (Basic transfer)
                // TODO: Parse npc.cultivation.known_methods into registry.skills
                
                registry.locations[entity].is_inside = true;
                registry.locations[entity].location_entity_id = baseEntityID;

                if (r.rank == 5) {
                    std::vector<InventoryComponent::ItemInstance> starterStash;
                    starterStash.push_back({"WPN_SWD_01", 5, 100, false}); // 5 Basic Swords
                    starterStash.push_back({"CSM_001", 20, 100, false});   // 20 Pills
                    
                    registry.sect_resources[entity] = {
                        1000 + (rand() % 5000), 
                        500 + (rand() % 2000), 
                        500 + (rand() % 500),
                        0, // Prestige
                        false, 0, // Starvation
                        {}, // Missions
                        starterStash
                    };
                    
                    // Init Diplomacy
                    auto& diplo = registry.sect_diplomacy[entity];
                    for(int otherSect=1; otherSect<=sectCount; ++otherSect) {
                        if(otherSect == sectUniqueId) continue;
                        int aff = (rand() % 100) - 50; // -50 to 50
                        std::string status = "Neutral";
                        if (aff > 30) status = "Ally";
                        if (aff < -30) status = "Hostile";
                        diplo.relations[otherSect] = {otherSect, aff, status};
                    }
                }

                // 4. Initial Relations (Stub)
                registry.relations[entity].network[baseEntityID].affinity = 50;
                registry.relations[entity].network[baseEntityID].type = "Sect";

                registry.personalities[entity] = { 
                    npc.personality.courage, npc.personality.greed, npc.personality.honor, npc.personality.loyalty, 
                    npc.personality.curiosity, npc.personality.temper, npc.personality.compassion, npc.personality.ambition, 
                    npc.personality.diligence, npc.personality.pride, 
                    npc.personality.current_goal 
                };

                registry.injuries[entity] = { 0, 0, "Healthy" };
                registry.inventories[entity].spirit_stones = (r.rank * 10); // Starter pocket money

                if (r.rank == 5) {
                    std::vector<InventoryComponent::ItemInstance> starterStash;
                    starterStash.push_back({"WPN_SWD_01", 5, 100, false}); // 5 Basic Swords
                    starterStash.push_back({"CSM_001", 20, 100, false});   // 20 Pills
                    
                    registry.sect_resources[entity] = {
                        1000 + (rand() % 5000), 
                        500 + (rand() % 2000), 
                        500 + (rand() % 500),
                        0, // Prestige
                        false, 0, // Starvation
                        {}, // Missions
                        starterStash
                    };
                    
                    // Init Diplomacy
                    auto& diplo = registry.sect_diplomacy[entity];
                    for(int otherSect=1; otherSect<=sectCount; ++otherSect) {
                        if(otherSect == sectUniqueId) continue;
                        int aff = (rand() % 100) - 50; // -50 to 50
                        std::string status = "Neutral";
                        if (aff > 30) status = "Ally";
                        if (aff < -30) status = "Hostile";
                        diplo.relations[otherSect] = {otherSect, aff, status};
                    }
                } 


                // --- NEW: Skills & Inventory (Phase 2) ---
                auto& items = GameData::GetItems();
                auto& skills = GameData::GetSkills();
                auto& methods = GameData::GetCultivationMethods();
                
                // 1. Assign Method
                if (!methods.empty()) {
                    // Leader gets better method
                    int idx = (r.rank >= 4) ? (rand() % methods.size()) : 0; 
                    registry.skills[entity].active_method_id = methods[idx].id;
                }
                
                // 2. Assign Skill
                if (!skills.empty()) {
                     int idx = rand() % skills.size();
                     registry.skills[entity].known_skills.push_back(skills[idx].id);
                }

                // 3. Assign Weapon (Matching Tier roughly)
                if (!items.empty()) {
                    for (const auto& it : items) {
                        if (it.type == "Weapon" && it.tier <= (realmInt + 1)) {
                             registry.inventories[entity].items.push_back({it.id, 1, 100, true});
                             break; // Just one for now
                        }
                    }
                }

                // 4. Initial Relations (Stub)
                // Everyone likes the Sect Leader (simplified)
                registry.relations[entity].network[baseEntityID].affinity = 50;
                registry.relations[entity].network[baseEntityID].type = "Sect";
            }
        }
        
    }
}
