#include "SimEngine.h"
#include "HistoryGenerator.h"
#include "GameDataLoader.h"
#include "NamingGenerator.h"
#include "Skills.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <random>
#include <set>
#include <sstream>

namespace fs = std::filesystem;

namespace {
    int RandInt(int min, int max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        if (max < min) std::swap(min, max);
        std::uniform_int_distribution<> dis(min, max);
        return dis(gen);
    }
    bool Chance(int percent) { return RandInt(1, 100) <= percent; }

    std::string Lower(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        return s;
    }

    std::string RealmName(int major) {
        switch (major) {
            case 0: return "Mortal";
            case 1: return "Qi Condensation";
            case 2: return "Foundation";
            case 3: return "Golden Core";
            case 4: return "Nascent Soul";
            case 5: return "Soul Formation";
            case 6: return "Void Refining";
            case 7: return "Body Integration";
            case 8: return "Mahayana";
            case 9: return "Tribulation";
            default: return "God";
        }
    }

    const std::vector<std::string>& TownSkills() {
        static std::vector<std::string> skills = {
            "Forging", "Construction", "Cultivation", "Alchemy", "Healing",
            "Warfare", "Stealth", "Intellect", "Weapons Mastery", "Formations",
            "Farming", "Food Gathering", "Hunting", "Scribing", "Scouting"
        };
        return skills;
    }

    const std::vector<std::string>& JobList() {
        static std::vector<std::string> jobs = {
            "Farmer", "Forager", "Woodcutter", "Miner", "Builder",
            "Guard", "Healer", "Cultivator", "Idle"
        };
        return jobs;
    }
}

SimEngine& SimEngine::Get() {
    static SimEngine instance;
    return instance;
}

void SimEngine::Log(const std::string& line) {
    recent_log.push_back(line);
    HistoryGenerator::AddStructuredEvent(CurrentYear(), "Event", -1, -1, town_id, line);
}

std::string SimEngine::GetDateString() const {
    long long day = current_tick;
    int year = (int)(day / 360) + 1;
    int month = (int)((day % 360) / 30) + 1;
    int dom = (int)(day % 30) + 1;
    return "Year " + std::to_string(year) + ", Month " + std::to_string(month) + ", Day " + std::to_string(dom);
}

// ---------------------------------------------------------------------------
// Data loading & person generation
// ---------------------------------------------------------------------------

void SimEngine::LoadDataFiles() {
    if (data_loaded) return;
    GameDataLoader::LoadAll();

    // Fallbacks if the markdown databases are missing or unparsable
    bool has_weapon = false;
    for (const auto& d : GameData::items) if (d.category == "Weapon") { has_weapon = true; break; }
    if (!has_weapon) {
        ItemDefinition sword{"WPN_BASIC_01", "Iron Sword", "Weapon", "SWD", 1, 100, "A plain but reliable blade.", 15, 0, "", ""};
        ItemDefinition spear{"WPN_BASIC_02", "Ash Spear", "Weapon", "SPR", 1, 80, "Long reach, simple point.", 12, 0, "", ""};
        ItemDefinition club{"WPN_BASIC_03", "Heavy Cudgel", "Weapon", "CLB", 1, 40, "Crushes what it lands on.", 10, 0, "", ""};
        GameData::items.push_back(sword);
        GameData::items.push_back(spear);
        GameData::items.push_back(club);
    }
    if (GameData::beasts.empty()) {
        GameData::beasts.push_back({"SD_W1", "Grey Wolf", "None", "Forest", 1, true, 'w', 0xFFFFFFFF});
        GameData::beasts.push_back({"SD_B1", "Wild Boar", "None", "Forest", 1, true, 'b', 0xFFFFFFFF});
        GameData::beasts.push_back({"SD_T1", "Mountain Tiger", "None", "Mountain", 3, true, 'T', 0xFFFFFFFF});
    }
    data_loaded = true;
}

int SimEngine::CreatePerson(int age_min, int age_max) {
    int eid = registry.CreateEntity();

    bool male = Chance(50);
    int age = RandInt(age_min, age_max);

    RenderComponent rc;
    rc.symbol = '@';
    rc.name = NamingGenerator::GenerateNPCName(male);
    registry.renderables[eid] = rc;

    StatsComponent sc;
    sc.hp = sc.max_hp = 100;
    registry.stats[eid] = sc;

    CultivationComponent cc;
    cc.age = age;
    cc.lifespan = RandInt(75, 110);
    cc.talent = 0.5f + (float)RandInt(0, 150) / 100.0f;
    cc.max_qi = 100.0f;
    cc.birth_year = CurrentYear() - age;
    registry.cultivations[eid] = cc;

    RelationComponent rel;
    rel.gender = male ? "Male" : "Female";
    registry.relations[eid] = rel;

    PersonalityComponent pc;
    pc.ambition = RandInt(10, 95); pc.greed = RandInt(10, 95); pc.bravery = RandInt(10, 95);
    pc.loyalty = RandInt(10, 95); pc.ruthlessness = RandInt(10, 95); pc.caution = RandInt(10, 95);
    pc.wisdom = RandInt(10, 95); pc.social = RandInt(10, 95); pc.vengeance = RandInt(10, 95);
    pc.diligence = RandInt(10, 95);
    registry.personalities[eid] = pc;

    SkillSetComponent ssc;
    for (const auto& s : TownSkills()) ssc.skills[s] = RandInt(0, 5);
    if (Chance(35)) {
        // Specialist: one strong talent
        const auto& list = TownSkills();
        ssc.skills[list[RandInt(0, (int)list.size() - 1)]] = RandInt(6, 10);
    }
    registry.skill_sets[eid] = ssc;

    SkillsComponent skc;
    skc.active_method_id = "MTH_GEN_01";
    registry.skills[eid] = skc;

    InventoryComponent inv;
    inv.spirit_stones = RandInt(0, 50);
    registry.inventories[eid] = inv;

    // Full anatomy: the whole point of being alive here
    float scale = (age < 14) ? 0.5f : 0.9f + (float)RandInt(0, 25) / 100.0f;
    registry.bodies[eid] = Body::CreateHumanoid(scale);

    registry.jobs[eid] = JobComponent{};
    registry.needs[eid] = NeedsComponent{};

    return eid;
}

void SimEngine::NewGame() {
    registry.Clear();
    HistoryGenerator::Clear();
    recent_log.clear();
    rivals.clear();
    missions.clear();
    mission_reports.clear();
    merchant_stock.clear();
    merchant_days = 0;
    candidates.clear();
    current_tick = 0;
    town_id = -1;
    game_started = false;

    // Default labor priorities (player adjusts in the Town panel)
    labor_priority = {{"Build", 5}, {"Heal", 5}, {"Farm", 4}, {"Forage", 3},
                      {"Woodcut", 3}, {"Mine", 2}, {"Guard", 2}};

    LoadDataFiles();
    RerollCandidates();
}

void SimEngine::RerollCandidates() {
    candidates.clear();
    for (int i = 0; i < 10; ++i) candidates.push_back(CreatePerson(16, 40));
}

void SimEngine::RerollCandidate(int slot) {
    if (slot < 0 || slot >= (int)candidates.size()) return;
    candidates[slot] = CreatePerson(16, 40);
}

void SimEngine::StartGame() {
    if (game_started) return;
    game_started = true;

    // 0. The world itself
    world_map.Generate((unsigned)RandInt(1, 0x7FFFFFFF));

    // 1. The settlement
    town_id = registry.CreateEntity();
    TownComponent tc;
    tc.name = "Hope's Peak";
    tc.population = (int)candidates.size();
    tc.wealth = 100;
    tc.food_reserves = 450;  // Basic starter resources
    tc.wood = 150;
    tc.stone = 60;
    tc.defense_rating = 5;
    registry.towns[town_id] = tc;

    SectResourceComponent src;
    src.spirit_stones = 500;
    src.herbs = 50;
    src.ores = 30;
    registry.sect_resources[town_id] = src;

    // 2. Starter armory: a handful of real weapons from the database
    std::vector<const ItemDefinition*> weapon_pool;
    for (const auto& d : GameData::items)
        if (d.category == "Weapon" && d.tier <= 2) weapon_pool.push_back(&d);
    if (weapon_pool.empty())
        for (const auto& d : GameData::items)
            if (d.category == "Weapon") weapon_pool.push_back(&d);
    for (int i = 0; i < 5 && !weapon_pool.empty(); ++i) {
        InventoryComponent::ItemInstance it;
        it.item_id = weapon_pool[RandInt(0, (int)weapon_pool.size() - 1)]->id;
        it.count = 1;
        registry.sect_resources[town_id].stash.push_back(it);
    }

    // 3. Rival settlements out in the wilds, placed on the overworld
    int n_rivals = RandInt(3, 5);
    for (int i = 0; i < n_rivals; ++i) {
        RivalSettlement r;
        r.id = i;
        r.name = NamingGenerator::GenerateSectName();
        r.power = RandInt(30, 90);
        r.wealth = RandInt(100, 600);
        r.relation = RandInt(-30, 30);

        // Find land far enough from the town and other rivals
        for (int attempt = 0; attempt < 200; ++attempt) {
            int x = RandInt(5, world_map.Width() - 6);
            int y = RandInt(5, world_map.Height() - 6);
            if (!world_map.IsLand(x, y)) continue;
            int dx = x - world_map.town_x, dy = y - world_map.town_y;
            float dist = std::sqrt((float)(dx * dx + dy * dy));
            if (dist < 30.0f) continue;
            bool too_close = false;
            for (const auto& other : rivals) {
                int ox = x - other.x, oy = y - other.y;
                if (ox * ox + oy * oy < 20 * 20) { too_close = true; break; }
            }
            if (too_close) continue;
            world_map.FindSettleSpot(x, y, r.x, r.y);
            r.distance_days = std::max(2, (int)(dist / 18.0f) + 1);
            break;
        }
        if (r.x == 0 && r.y == 0) { // Placement failed; keep it abstract but on land
            world_map.FindSettleSpot(RandInt(10, world_map.Width() - 10), RandInt(10, world_map.Height() - 10), r.x, r.y);
            r.distance_days = RandInt(2, 6);
        }
        rivals.push_back(r);
    }

    Log("The town of " + tc.name + " was founded by " + std::to_string(candidates.size()) + " brave souls.");
    for (int eid : candidates) {
        HistoryGenerator::AddStructuredEvent(CurrentYear(), "Foundation", eid, -1, town_id,
            registry.renderables[eid].name + " is among the founders.");
    }
    candidates.clear();
}

// ---------------------------------------------------------------------------
// Queries
// ---------------------------------------------------------------------------

std::vector<int> SimEngine::LivingPeople() const {
    std::vector<int> out;
    for (const auto& [id, n] : registry.needs) {
        auto it = registry.stats.find(id);
        if (it != registry.stats.end() && !it->second.is_dead) out.push_back(id);
    }
    return out;
}

int SimEngine::FindPersonByName(const std::string& query) const {
    std::string q = Lower(query);
    // "#3" addresses the 3rd person in the roster (as shown by 'people')
    if (!q.empty() && q[0] == '#') {
        int n = std::atoi(q.c_str() + 1);
        auto people = LivingPeople();
        if (n >= 1 && n <= (int)people.size()) return people[n - 1];
        return -1;
    }
    int exact = -1, partial = -1;
    for (const auto& [id, n] : registry.needs) {
        auto st = registry.stats.find(id);
        if (st == registry.stats.end() || st->second.is_dead) continue;
        auto rc = registry.renderables.find(id);
        if (rc == registry.renderables.end()) continue;
        std::string name = Lower(rc->second.name);
        if (name == q) { exact = id; break; }
        if (partial == -1 && name.find(q) != std::string::npos) partial = id;
    }
    return exact != -1 ? exact : partial;
}

bool SimEngine::IsOnMission(int entity) const {
    for (const auto& m : missions)
        for (int e : m.party)
            if (e == entity) return true;
    return false;
}

int SimEngine::CountBuildings(const std::string& type, bool finished_only) const {
    int n = 0;
    for (const auto& [id, b] : registry.buildings)
        if (b.type == type && (!finished_only || !b.under_construction)) n++;
    return n;
}

int SimEngine::CountJob(const std::string& job) const {
    int n = 0;
    for (const auto& [id, j] : registry.jobs) {
        auto st = registry.stats.find(id);
        if (st == registry.stats.end() || st->second.is_dead) continue;
        if (IsOnMission(id)) continue;
        if (j.job == job) n++;
    }
    return n;
}

static int SkillOf(const Registry& reg, int entity, const std::string& skill) {
    auto it = reg.skill_sets.find(entity);
    if (it == reg.skill_sets.end()) return 0;
    auto s = it->second.skills.find(skill);
    return s != it->second.skills.end() ? s->second : 0;
}

// Adds XP toward a skill; returns the new level if a level-up happened, else -1.
static int TrainSkill(Registry& reg, int entity, const std::string& skill, int xp_amount) {
    auto it = reg.skill_sets.find(entity);
    if (it == reg.skill_sets.end()) return -1;
    int& lvl = it->second.skills[skill];
    if (lvl >= SKILL_CAP) return -1;
    int& xp = it->second.xp[skill];
    xp += xp_amount;
    int leveled = -1;
    while (lvl < SKILL_CAP && xp >= SkillXpToNext(lvl)) {
        xp -= SkillXpToNext(lvl);
        lvl++;
        leveled = lvl;
    }
    return leveled;
}

int SimEngine::GetSkill(int entity, const std::string& skill) const {
    return SkillOf(registry, entity, skill);
}

void SimEngine::SetLaborPriority(const std::string& category, int priority) {
    if (LaborSkillMap().count(category) == 0) return;
    labor_priority[category] = std::clamp(priority, 0, 5);
}

std::string SimEngine::EquippedWeaponName(int entity) const {
    auto it = registry.inventories.find(entity);
    if (it == registry.inventories.end()) return "unarmed";
    for (const auto& item : it->second.items) {
        if (!item.equipped) continue;
        const ItemDefinition* def = GameData::FindItem(item.item_id);
        return def ? def->name : item.item_id;
    }
    return "unarmed";
}

int SimEngine::EffectiveAttack(int entity) const {
    int atk = 6;
    auto cc = registry.cultivations.find(entity);
    if (cc != registry.cultivations.end())
        atk += cc->second.realm_major * 15 + cc->second.realm_minor * 2;
    atk += SkillOf(registry, entity, "Weapons Mastery") * 2;
    auto inv = registry.inventories.find(entity);
    if (inv != registry.inventories.end()) {
        for (const auto& item : inv->second.items) {
            if (!item.equipped) continue;
            const ItemDefinition* def = GameData::FindItem(item.item_id);
            if (def) atk += def->attack;
        }
    }
    return atk;
}

int SimEngine::EffectiveDefense(int entity) const {
    int def = 2;
    auto cc = registry.cultivations.find(entity);
    if (cc != registry.cultivations.end()) def += cc->second.realm_major * 5;
    def += SkillOf(registry, entity, "Warfare");
    return def;
}

Combatant SimEngine::MakeCitizenCombatant(int entity) {
    Combatant c;
    c.entity_id = entity;
    auto rc = registry.renderables.find(entity);
    c.name = rc != registry.renderables.end() ? rc->second.name : "Citizen";
    if (!registry.bodies.count(entity)) registry.bodies[entity] = Body::CreateHumanoid();
    c.body = &registry.bodies[entity];
    c.attack = EffectiveAttack(entity);
    c.defense = EffectiveDefense(entity);
    c.skill = 0.8f + 0.04f * SkillOf(registry, entity, "Weapons Mastery") + 0.02f * SkillOf(registry, entity, "Warfare");

    // Damage type follows the equipped weapon class
    c.damage_type = "blunt"; // bare fists
    auto inv = registry.inventories.find(entity);
    if (inv != registry.inventories.end()) {
        for (const auto& item : inv->second.items) {
            if (!item.equipped) continue;
            const ItemDefinition* def = GameData::FindItem(item.item_id);
            if (!def) continue;
            std::string sub = Lower(def->sub_category);
            if (sub.find("spr") != std::string::npos || sub.find("spear") != std::string::npos) c.damage_type = "pierce";
            else if (sub.find("clb") != std::string::npos || sub.find("hammer") != std::string::npos) c.damage_type = "blunt";
            else c.damage_type = "slash";
        }
    }
    return c;
}

Combatant SimEngine::MakeBeastCombatant(const BeastDefinition& def, std::vector<BodyComponent>& storage) {
    storage.push_back(Body::CreateForBeast(def.name, def.tier));
    Combatant c;
    c.entity_id = -1;
    c.name = def.name;
    c.body = &storage.back();
    c.attack = 4 + def.tier * 6;
    c.defense = def.tier * 2;
    c.skill = 0.7f + 0.04f * def.tier;
    c.damage_type = Chance(50) ? "slash" : "pierce"; // claws or fangs
    return c;
}

// ---------------------------------------------------------------------------
// Player decisions
// ---------------------------------------------------------------------------

const std::vector<BuildingSpec>& SimEngine::BuildingCatalog() {
    static std::vector<BuildingSpec> catalog = {
        {"Farm",       20,  0, 4,  "Grows food. Farmers work here (up to 4 each)."},
        {"Cottage",    30,  0, 4,  "Houses 4 people. Crowding hurts morale."},
        {"Granary",    40,  0, 5,  "Protects the food stores from rot and rats."},
        {"Barracks",   50, 20, 8,  "Guards train faster and defend better."},
        {"Wall",        0, 50, 10, "Greatly improves town defense."},
        {"Infirmary",  40,  0, 6,  "The wounded heal much faster here."},
        {"Blacksmith", 30, 30, 8,  "Forges a new weapon every 10 days while ore lasts."},
        {"Shrine",      0, 30, 6,  "Lifts the spirits of the whole town."},
    };
    return catalog;
}

std::string SimEngine::Build(const std::string& type) {
    const BuildingSpec* spec = nullptr;
    for (const auto& s : BuildingCatalog())
        if (Lower(s.type) == Lower(type)) { spec = &s; break; }
    if (!spec) return "Unknown building type: " + type;

    TownComponent& town = registry.towns[town_id];
    if (town.wood < spec->wood_cost) return "Not enough wood (" + std::to_string(spec->wood_cost) + " needed, have " + std::to_string(town.wood) + ").";
    if (town.stone < spec->stone_cost) return "Not enough stone (" + std::to_string(spec->stone_cost) + " needed, have " + std::to_string(town.stone) + ").";

    town.wood -= spec->wood_cost;
    town.stone -= spec->stone_cost;

    int bid = registry.CreateEntity();
    BuildingComponent b;
    b.type = spec->type;
    b.under_construction = true;
    b.build_days_left = spec->build_days;
    registry.buildings[bid] = b;

    Log("Construction of a " + spec->type + " has begun (" + std::to_string(spec->build_days) + " builder-days).");
    return "Construction of a " + spec->type + " has begun. Assign Builders to speed it up.";
}

std::string SimEngine::AssignJob(int entity, const std::string& job) {
    std::string match;
    for (const auto& j : JobList())
        if (Lower(j) == Lower(job)) { match = j; break; }
    if (match.empty()) {
        std::string opts;
        for (const auto& j : JobList()) opts += (opts.empty() ? "" : ", ") + j;
        return "Unknown job. Options: " + opts;
    }
    auto st = registry.stats.find(entity);
    if (st == registry.stats.end() || st->second.is_dead) return "That person is dead.";
    if (IsOnMission(entity)) return "They are away on a mission.";
    auto cc = registry.cultivations.find(entity);
    if (cc != registry.cultivations.end() && cc->second.age < 14 && match != "Idle")
        return registry.renderables[entity].name + " is only " + std::to_string(cc->second.age) + " - too young to work.";

    registry.jobs[entity].job = match;
    return registry.renderables[entity].name + " is now a " + match + ".";
}

std::string SimEngine::AssignWeapon(int entity, const std::string& item_query) {
    auto st = registry.stats.find(entity);
    if (st == registry.stats.end() || st->second.is_dead) return "That person is dead.";
    auto body = registry.bodies.find(entity);
    if (body != registry.bodies.end() && Body::CountGraspingHands(body->second) == 0)
        return registry.renderables[entity].name + " has no working hands to hold a weapon!";

    auto& stash = registry.sect_resources[town_id].stash;
    std::string q = Lower(item_query);
    int found = -1;
    for (int i = 0; i < (int)stash.size(); ++i) {
        const ItemDefinition* def = GameData::FindItem(stash[i].item_id);
        std::string name = def ? Lower(def->name) : Lower(stash[i].item_id);
        if (name.find(q) != std::string::npos || Lower(stash[i].item_id) == q) { found = i; break; }
    }
    if (found == -1) return "No such weapon in the armory. Try 'armory' to list it.";

    // Return any currently equipped weapon to the stash first
    UnassignWeapon(entity);

    InventoryComponent::ItemInstance item = stash[found];
    item.count = 1;
    item.equipped = true;
    if (stash[found].count > 1) stash[found].count--;
    else stash.erase(stash.begin() + found);

    registry.inventories[entity].items.push_back(item);
    const ItemDefinition* def = GameData::FindItem(item.item_id);
    std::string wname = def ? def->name : item.item_id;
    Log(registry.renderables[entity].name + " takes up the " + wname + ".");
    return registry.renderables[entity].name + " now wields the " + wname + " (attack " + std::to_string(EffectiveAttack(entity)) + ").";
}

std::string SimEngine::UnassignWeapon(int entity) {
    auto inv = registry.inventories.find(entity);
    if (inv == registry.inventories.end()) return "Nothing equipped.";
    auto& items = inv->second.items;
    for (int i = 0; i < (int)items.size(); ++i) {
        if (!items[i].equipped) continue;
        InventoryComponent::ItemInstance back = items[i];
        back.equipped = false;
        registry.sect_resources[town_id].stash.push_back(back);
        items.erase(items.begin() + i);
        return "Weapon returned to the armory.";
    }
    return "Nothing equipped.";
}

std::string SimEngine::StartMission(const std::string& type, const std::vector<int>& party, int target_rival) {
    static const std::vector<std::string> types = {"Hunt", "Gather", "Adventure", "Raid", "Invade"};
    std::string match;
    for (const auto& t : types) if (Lower(t) == Lower(type)) { match = t; break; }
    if (match.empty()) return "Unknown mission type. Options: Hunt, Gather, Adventure, Raid, Invade.";
    if (party.empty()) return "The party is empty.";

    for (int e : party) {
        auto st = registry.stats.find(e);
        if (st == registry.stats.end() || st->second.is_dead) return "A party member is dead.";
        if (IsOnMission(e)) return registry.renderables[e].name + " is already on a mission.";
        auto cc = registry.cultivations.find(e);
        if (cc != registry.cultivations.end() && cc->second.age < 14)
            return registry.renderables[e].name + " is too young for missions.";
    }

    ActiveMission m;
    m.type = match;
    m.party = party;
    if (match == "Hunt") m.total_days = RandInt(3, 5);
    else if (match == "Gather") m.total_days = RandInt(3, 5);
    else if (match == "Adventure") m.total_days = RandInt(8, 14);
    else {
        if (target_rival < 0 || target_rival >= (int)rivals.size()) return "Pick a target settlement (see 'rivals').";
        if (rivals[target_rival].destroyed) return "That settlement is already a ruin.";
        m.target_rival = target_rival;
        m.total_days = rivals[target_rival].distance_days * 2 + 1;
    }
    m.days_left = m.total_days;
    missions.push_back(m);

    std::string names;
    for (int e : party) names += (names.empty() ? "" : ", ") + registry.renderables[e].name;
    std::string target = (m.target_rival >= 0) ? " against " + rivals[m.target_rival].name : "";
    Log("A " + match + " party set out" + target + ": " + names + " (back in ~" + std::to_string(m.total_days) + " days).");
    return "The party has departed.";
}

std::string SimEngine::HoldElection() {
    auto people = LivingPeople();
    std::vector<int> adults;
    for (int e : people) {
        auto cc = registry.cultivations.find(e);
        if (cc != registry.cultivations.end() && cc->second.age >= 16) adults.push_back(e);
    }
    if (adults.size() < 2) return "Not enough adults for an election.";

    std::map<int, int> votes;
    for (int voter : adults) {
        int best = -1, best_score = -1000000;
        auto rel = registry.relations.find(voter);
        for (int cand : adults) {
            if (cand == voter) continue;
            int score = 0;
            if (rel != registry.relations.end()) {
                auto r = rel->second.network.find(cand);
                if (r != rel->second.network.end()) score += r->second.affinity * 2;
            }
            auto pc = registry.personalities.find(cand);
            if (pc != registry.personalities.end()) score += pc->second.wisdom + pc->second.social / 2;
            score += RandInt(0, 20); // a little electoral chaos
            if (score > best_score) { best_score = score; best = cand; }
        }
        if (best != -1) votes[best]++;
    }

    int winner = -1, most = -1;
    for (const auto& [cand, n] : votes)
        if (n > most) { most = n; winner = cand; }
    if (winner == -1) return "The election was inconclusive.";

    TownComponent& town = registry.towns[town_id];
    town.mayor_id = winner;
    town.morale = std::min(100, town.morale + 8);
    std::string name = registry.renderables[winner].name;
    Log("ELECTION: " + name + " has been elected Mayor with " + std::to_string(most) + " of " + std::to_string(adults.size()) + " votes!");
    return name + " is the new Mayor (" + std::to_string(most) + "/" + std::to_string(adults.size()) + " votes).";
}

// ---------------------------------------------------------------------------
// The simulation itself: daily systems
// ---------------------------------------------------------------------------

void SimEngine::AdvanceDays(int days) {
    if (!game_started) return;
    for (int d = 0; d < days; ++d) {
        current_tick++;

        AssignLabor();
        TickConstruction();
        TickProduction();
        TickNeeds();
        TickBodies();
        TickMissions();
        TickRandomEvents();
        TickWorldEvents();
        TickRivals();
        TickSocial();
        if (current_tick % 30 == 15) TickTournament();
        if (current_tick % 360 == 0) TickYearly();
        RecomputeDefense();

        if (LivingPeople().empty()) {
            Log("=== Every last soul of " + registry.towns[town_id].name + " has perished. The fortress has fallen. ===");
            return;
        }
    }
}

// ONI-style task system: the player sets per-category priorities; every day
// the best-skilled available people are assigned to the highest-priority work.
// The unassigned follow their personality: the diligent train, the rest lounge.
void SimEngine::AssignLabor() {
    std::vector<int> pool;
    for (int eid : LivingPeople()) {
        auto cc = registry.cultivations.find(eid);
        if (cc == registry.cultivations.end() || cc->second.age < 14) continue;
        if (IsOnMission(eid)) continue;
        pool.push_back(eid);
    }
    for (int eid : pool) registry.jobs[eid].job = "Idle";
    if (pool.empty()) return;

    // How many workers each category can actually use today
    int constructions = 0;
    for (auto& [bid, b] : registry.buildings)
        if (b.under_construction) constructions++;
    int wounded = 0;
    for (int eid : LivingPeople()) {
        auto body = registry.bodies.find(eid);
        if (body == registry.bodies.end()) continue;
        bool hurt = body->second.blood < body->second.max_blood * 0.95f;
        for (const auto& p : body->second.parts)
            if (p.wound != (int)WoundLevel::Fine && !p.IsGone()) { hurt = true; break; }
        if (hurt) wounded++;
    }
    std::map<std::string, int> demand;
    demand["Build"] = constructions > 0 ? std::min(6, 2 + constructions) : 0;
    demand["Farm"] = CountBuildings("Farm") * 4;
    demand["Heal"] = wounded > 0 ? 1 + wounded / 3 : 0;
    demand["Forage"] = (int)pool.size();
    demand["Woodcut"] = (int)pool.size();
    demand["Mine"] = (int)pool.size();
    demand["Guard"] = (int)pool.size();

    struct Cat { std::string name; int pri; int demand; };
    std::vector<Cat> cats;
    int total_pri = 0;
    for (const auto& [cat, skill] : LaborSkillMap()) {
        int pri = labor_priority.count(cat) ? labor_priority.at(cat) : 3;
        if (pri <= 0 || demand[cat] <= 0) continue;
        cats.push_back({cat, pri, demand[cat]});
        total_pri += pri;
    }
    if (cats.empty() || total_pri == 0) total_pri = 1;
    std::sort(cats.begin(), cats.end(),
              [](const Cat& a, const Cat& b) { return a.pri != b.pri ? a.pri > b.pri : a.name < b.name; });

    static const std::map<std::string, std::string> job_name = {
        {"Farm", "Farmer"}, {"Forage", "Forager"}, {"Woodcut", "Woodcutter"}, {"Mine", "Miner"},
        {"Build", "Builder"}, {"Guard", "Guard"}, {"Heal", "Healer"}};

    int initial_pool = (int)pool.size();
    std::set<int> taken;
    for (const auto& c : cats) {
        // Proportional share of the workforce, demand-capped; high priority claims first
        int quota = std::max(1, (initial_pool * c.pri + total_pri - 1) / total_pri);
        quota = std::min({quota, c.demand, initial_pool - (int)taken.size()});
        if (quota <= 0) continue;

        const std::string& skill = LaborSkillMap().at(c.name);
        std::vector<int> avail;
        for (int eid : pool)
            if (!taken.count(eid)) avail.push_back(eid);
        std::sort(avail.begin(), avail.end(),
                  [&](int a, int b) { return SkillOf(registry, a, skill) > SkillOf(registry, b, skill); });
        for (int i = 0; i < quota && i < (int)avail.size(); ++i) {
            registry.jobs[avail[i]].job = job_name.at(c.name);
            taken.insert(avail[i]);
        }
    }

    // The idle follow their nature: the driven cultivate, the rest lounge about
    for (int eid : pool) {
        if (taken.count(eid)) continue;
        auto pc = registry.personalities.find(eid);
        int diligence = pc != registry.personalities.end() ? pc->second.diligence : 50;
        int ambition = pc != registry.personalities.end() ? pc->second.ambition : 50;
        registry.jobs[eid].job = (diligence >= 55 || ambition >= 70) ? "Cultivator" : "Idle";
    }
}

void SimEngine::TickConstruction() {
    // Total build power: skilled builders work much faster
    float power = 0.0f;
    std::vector<int> builders;
    for (const auto& [eid, j] : registry.jobs) {
        auto st = registry.stats.find(eid);
        if (st == registry.stats.end() || st->second.is_dead || IsOnMission(eid)) continue;
        if (j.job == "Builder") {
            builders.push_back(eid);
            power += 1.0f + 0.25f * (float)SkillOf(registry, eid, "Construction");
        }
    }
    if (builders.empty()) return;
    for (auto& [bid, b] : registry.buildings) {
        if (!b.under_construction) continue;
        int progress = (int)power;
        if (Chance((int)((power - (float)progress) * 100.0f))) progress++;
        b.build_days_left -= progress;
        for (int eid : builders) TrainSkill(registry, eid, "Construction", 6);
        if (b.build_days_left <= 0) {
            b.under_construction = false;
            b.build_days_left = 0;
            Log("The " + b.type + " is complete!");
        }
        break; // One project at a time
    }
}

void SimEngine::TickProduction() {
    TownComponent& town = registry.towns[town_id];
    SectResourceComponent& res = registry.sect_resources[town_id];

    int farms = CountBuildings("Farm");
    int farm_capacity = farms * 4;
    int farmers_used = 0;
    std::vector<int> tribulation_deaths; // Deferred: can't HandleDeath while iterating jobs

    // Mayor wisdom gives a small production bonus to everyone
    float mayor_bonus = 1.0f;
    if (town.mayor_id != -1) {
        auto pc = registry.personalities.find(town.mayor_id);
        auto st = registry.stats.find(town.mayor_id);
        if (pc != registry.personalities.end() && st != registry.stats.end() && !st->second.is_dead)
            mayor_bonus = 1.0f + (float)pc->second.wisdom / 400.0f;
    }

    for (const auto& [eid, j] : registry.jobs) {
        auto st = registry.stats.find(eid);
        if (st == registry.stats.end() || st->second.is_dead) continue;
        if (IsOnMission(eid)) continue;

        // Crippled workers work at reduced output
        float ability = 1.0f;
        auto body = registry.bodies.find(eid);
        if (body != registry.bodies.end()) {
            float grasp = Body::Capacity(body->second, BodyFunc::Grasp);
            float stand = Body::Capacity(body->second, BodyFunc::Stand);
            ability = 0.25f + 0.45f * grasp + 0.30f * stand;
        }
        ability *= mayor_bonus;

        const std::string& job = j.job;
        if (job == "Farmer") {
            if (farmers_used < farm_capacity) {
                farmers_used++;
                town.food_reserves += (int)((3 + SkillOf(registry, eid, "Farming") / 2) * ability);
                TrainSkill(registry, eid, "Farming", 8);
            } else {
                town.food_reserves += (int)(1 * ability); // No farm: scratching at dirt
            }
        } else if (job == "Forager") {
            town.food_reserves += (int)((1 + SkillOf(registry, eid, "Food Gathering") / 3) * ability);
            if (Chance(20)) res.herbs += 1;
            TrainSkill(registry, eid, "Food Gathering", 8);
        } else if (job == "Woodcutter") {
            town.wood += (int)((2 + SkillOf(registry, eid, "Construction") / 4) * ability);
        } else if (job == "Miner") {
            town.stone += (int)((1 + SkillOf(registry, eid, "Forging") / 4) * ability);
            if (Chance(50)) res.ores += 1;
            if (Chance(2)) {
                int find = RandInt(5, 20);
                res.spirit_stones += find;
                Log(registry.renderables[eid].name + " struck a spirit stone seam while mining! (+" + std::to_string(find) + " spirit stones)");
            }
        } else if (job == "Guard") {
            TrainSkill(registry, eid, "Warfare", CountBuildings("Barracks") > 0 ? 15 : 6);
            TrainSkill(registry, eid, "Weapons Mastery", CountBuildings("Barracks") > 0 ? 12 : 5);
        } else if (job == "Cultivator") {
            auto cc = registry.cultivations.find(eid);
            auto pc = registry.personalities.find(eid);
            if (cc != registry.cultivations.end()) {
                float diligence = pc != registry.personalities.end() ? (float)pc->second.diligence : 50.0f;
                float skill_mult = 1.0f + 0.05f * (float)SkillOf(registry, eid, "Cultivation");
                float gain = cc->second.talent * (2.0f + diligence / 25.0f) * skill_mult;

                // Dual cultivation: spouses who train together advance 50% faster
                auto rel = registry.relations.find(eid);
                if (rel != registry.relations.end() && rel->second.spouse_id != -1) {
                    auto sp_job = registry.jobs.find(rel->second.spouse_id);
                    auto sp_st = registry.stats.find(rel->second.spouse_id);
                    if (sp_job != registry.jobs.end() && sp_job->second.job == "Cultivator" &&
                        sp_st != registry.stats.end() && !sp_st->second.is_dead)
                        gain *= 1.5f;
                }
                cc->second.qi += gain;
                TrainSkill(registry, eid, "Cultivation", 10);
                if (cc->second.qi >= cc->second.max_qi) {
                    cc->second.qi = 0;
                    cc->second.realm_minor++;
                    if (cc->second.realm_minor > 9) {
                        cc->second.realm_minor = 1;
                        cc->second.realm_major++;

                        // Heavenly Tribulation: the heavens test every major breakthrough
                        std::string name = registry.renderables[eid].name;
                        auto body = registry.bodies.find(eid);
                        if (body != registry.bodies.end() && Chance(30)) {
                            Log("The skies darken - HEAVENLY TRIBULATION descends upon " + name + "!");
                            int strikes = RandInt(1, 3);
                            for (int s = 0; s < strikes; ++s) {
                                int part = Body::PickHitPart(body->second);
                                if (part < 0) break;
                                WoundResult w = Body::ApplyDamage(body->second, part, RandInt(12, 28), "blunt", name);
                                Log("  Lightning strikes! " + w.text);
                            }
                            if (Body::IsDead(body->second)) {
                                tribulation_deaths.push_back(eid);
                                continue;
                            }
                            Log("  " + name + " endures the lightning, soul tempered by the heavens!");
                        }

                        cc->second.lifespan += 30;
                        st->second.max_hp += 50;
                        st->second.hp = st->second.max_hp;
                        if (cc->second.realm_major >= 3 && registry.renderables[eid].title.empty())
                            registry.renderables[eid].title = "Elder";
                        Log("BREAKTHROUGH! " + name + " has reached the " +
                            RealmName(cc->second.realm_major) + " realm!");
                    }
                    cc->second.max_qi *= 1.25f;
                }
            }
        }
    }

    for (int eid : tribulation_deaths)
        HandleDeath(eid, "heavenly tribulation lightning");

    // Blacksmith forges a weapon every 10 days if there is ore
    if (current_tick % 10 == 0 && CountBuildings("Blacksmith") > 0 && res.ores >= 10) {
        std::vector<const ItemDefinition*> pool;
        for (const auto& d : GameData::items)
            if (d.category == "Weapon" && d.tier <= 3) pool.push_back(&d);
        if (!pool.empty()) {
            res.ores -= 10;
            const ItemDefinition* pick = pool[RandInt(0, (int)pool.size() - 1)];
            InventoryComponent::ItemInstance it;
            it.item_id = pick->id;
            it.count = 1;
            res.stash.push_back(it);
            Log("The blacksmith finished forging a " + pick->name + ". It waits in the armory.");
        }
    }
}

void SimEngine::TickNeeds() {
    TownComponent& town = registry.towns[town_id];
    int shrine = CountBuildings("Shrine");
    int cottages = CountBuildings("Cottage");

    auto people = LivingPeople();
    int housed_capacity = cottages * 4;
    bool crowded = (int)people.size() > housed_capacity;

    int morale_sum = 0;
    for (int eid : people) {
        NeedsComponent& n = registry.needs[eid];
        bool away = IsOnMission(eid);

        if (away) {
            n.hunger = 0; // Parties forage on the road
        } else if (town.food_reserves > 0) {
            town.food_reserves--;
            n.hunger = 0;
        } else {
            n.hunger++;
            n.morale = std::max(0, n.morale - 4);
            if (n.hunger == 3)
                Log(registry.renderables[eid].name + " is going hungry - the stores are empty!");
            if (n.hunger > 6 && Chance(12)) {
                HandleDeath(eid, "starvation");
                continue;
            }
        }

        // Mood drifts toward a baseline shaped by conditions
        int target = 50 + (shrine > 0 ? 10 : 0) - (crowded ? 12 : 0) - (n.hunger > 0 ? 20 : 0);
        auto body = registry.bodies.find(eid);
        if (body != registry.bodies.end() && body->second.pain > 30.0f) target -= 10;
        if (n.morale < target) n.morale++;
        else if (n.morale > target) n.morale--;
        morale_sum += n.morale;
    }
    people = LivingPeople();
    if (!people.empty()) town.morale = morale_sum / (int)people.size();
    town.population = (int)people.size();

    // Food beyond storage capacity rots away (Granaries protect it)
    int capacity = 400 + CountBuildings("Granary") * 1000;
    if (town.food_reserves > capacity) {
        int excess = town.food_reserves - capacity;
        int rot = std::max(1, excess / 25);
        town.food_reserves -= rot;
        if (current_tick % 30 == 0)
            Log("Rats and rot claim the overflowing food stores (" + std::to_string(rot) +
                " lost today)." + (CountBuildings("Granary") == 0 ? " Build a Granary!" : ""));
    }
}

void SimEngine::TickBodies() {
    // Skilled healers speed recovery for the whole town
    float heal_bonus = 0.0f;
    for (const auto& [eid, j] : registry.jobs) {
        auto st = registry.stats.find(eid);
        if (st == registry.stats.end() || st->second.is_dead || IsOnMission(eid)) continue;
        if (j.job == "Healer") heal_bonus += 0.15f + 0.04f * (float)SkillOf(registry, eid, "Healing");
    }
    bool infirmary = CountBuildings("Infirmary") > 0;
    float heal_mult = 1.0f + std::min(heal_bonus, 1.5f) + (infirmary ? 0.6f : 0.0f);

    for (int eid : LivingPeople()) {
        auto body = registry.bodies.find(eid);
        if (body == registry.bodies.end()) continue;
        float mult = IsOnMission(eid) ? 1.0f : heal_mult;
        bool bled_out = Body::DailyTick(body->second, mult);
        if (bled_out) HandleDeath(eid, "blood loss");
        // Healers practice on the wounded
        if (!IsOnMission(eid) && body->second.pain > 10.0f)
            for (const auto& [hid, j] : registry.jobs)
                if (j.job == "Healer") TrainSkill(registry, hid, "Healing", 8);
    }
}

void SimEngine::TickMissions() {
    for (auto it = missions.begin(); it != missions.end();) {
        it->days_left--;
        if (it->days_left <= 0) {
            ResolveMission(*it);
            it = missions.erase(it);
        } else {
            ++it;
        }
    }
}

void SimEngine::ResolveMission(ActiveMission& m) {
    TownComponent& town = registry.towns[town_id];
    SectResourceComponent& res = registry.sect_resources[town_id];

    MissionReport rep;
    rep.type = m.type;
    rep.day = current_tick;
    rep.date = GetDateString();
    if (m.target_rival >= 0 && m.target_rival < (int)rivals.size()) rep.target = rivals[m.target_rival].name;

    // Everything logged through rlog lands in both the chronicle and the report
    auto rlog = [&](const std::string& s) { Log(s); rep.events.push_back(s); };

    // Records XP gains; level-ups go into the report
    auto train = [&](int eid, const std::string& skill, int xp_amount) {
        int lvl = TrainSkill(registry, eid, skill, xp_amount);
        if (lvl > 0 && registry.renderables.count(eid))
            rep.xp.push_back(registry.renderables[eid].name + ": " + skill + " rose to level " + std::to_string(lvl));
    };

    // Build the party (anyone who died earlier stays dead)
    std::vector<Combatant> party;
    for (int e : m.party) {
        auto st = registry.stats.find(e);
        if (st != registry.stats.end() && !st->second.is_dead) {
            party.push_back(MakeCitizenCombatant(e));
            rep.party.push_back(registry.renderables[e].name);
        }
    }
    if (party.empty()) {
        rep.success = false;
        rep.events.push_back("The party never returned. None survived.");
        Log("The " + m.type + " party never returned. None survived.");
        mission_reports.push_back(rep);
        return;
    }

    std::vector<BodyComponent> beast_bodies;
    beast_bodies.reserve(24); // Combatants hold pointers into this; must never reallocate
    std::vector<std::pair<std::string, int>> kills; // (beast name, tier) actually slain
    auto fight_beasts = [&](int count, int max_tier, const std::string& where) -> bool {
        std::vector<const BeastDefinition*> pool;
        for (const auto& b : GameData::beasts) if (b.tier <= max_tier) pool.push_back(&b);
        if (pool.empty()) {
            // Nothing that weak exists; the weakest beasts in the world will do
            int min_tier = 99;
            for (const auto& b : GameData::beasts) min_tier = std::min(min_tier, b.tier);
            for (const auto& b : GameData::beasts) if (b.tier == min_tier) pool.push_back(&b);
        }
        if (pool.empty()) return true;
        std::vector<Combatant> beasts;
        std::vector<int> tiers;
        std::string names;
        for (int i = 0; i < count; ++i) {
            const BeastDefinition* pick = pool[RandInt(0, (int)pool.size() - 1)];
            beasts.push_back(MakeBeastCombatant(*pick, beast_bodies));
            tiers.push_back(pick->tier);
            names += (names.empty() ? "" : ", ") + pick->name;
        }
        rlog("  The party " + std::string(m.type == "Hunt" ? "tracked down " : "was attacked by ") + names + " " + where + "!");
        CombatResult r = Combat::Fight(party, beasts, 0.7f);
        for (const auto& line : r.log) rlog(line);
        for (size_t i = 0; i < beasts.size(); ++i)
            if (beasts[i].dead) kills.push_back({beasts[i].name, tiers[i]});
        for (auto& c : party) {
            if (c.dead && c.entity_id != -1) {
                rep.casualties.push_back(c.name);
                HandleDeath(c.entity_id, "killed by a " + names);
            }
        }
        // Refresh the living party list
        std::vector<Combatant> alive;
        for (auto& c : party) if (!c.dead) alive.push_back(c);
        party = alive;
        return !party.empty();
    };

    // Beast carcass processing: meat, hides, materials, cores
    auto butcher_kills = [&]() {
        if (kills.empty()) return;
        std::vector<const ItemDefinition*> mats, cores;
        for (const auto& d : GameData::items) {
            if (d.category == "Material" && d.sub_category == "BeastPart") mats.push_back(&d);
            if (d.category == "Consumable" && d.sub_category == "Core") cores.push_back(&d);
        }
        int meat = 0, hides = 0;
        for (const auto& [bname, tier] : kills) {
            meat += 10 + tier * 12;
            hides += RandInt(1, 1 + tier);
            if (!mats.empty() && Chance(50)) {
                const ItemDefinition* pick = mats[RandInt(0, (int)mats.size() - 1)];
                InventoryComponent::ItemInstance it;
                it.item_id = pick->id;
                it.count = 1;
                res.stash.push_back(it);
                rep.items.push_back(pick->name + " (from " + bname + ") -> stockpile");
            }
            if (!cores.empty() && tier >= 4 && Chance(35)) {
                const ItemDefinition* pick = cores[RandInt(0, (int)cores.size() - 1)];
                InventoryComponent::ItemInstance it;
                it.item_id = pick->id;
                it.count = 1;
                res.stash.push_back(it);
                rep.items.push_back(pick->name + " (beast core of " + bname + ") -> stockpile");
            }
        }
        // Hunting skill improves the butchering yield
        for (auto& c : party) meat += SkillOf(registry, c.entity_id, "Hunting") * 3;
        town.food_reserves += meat;
        town.wealth += hides * 2;
        rep.loot.push_back(std::to_string(meat) + " food (beast meat)");
        rep.loot.push_back(std::to_string(hides) + " hides (sold: +" + std::to_string(hides * 2) + " wealth)");
    };
    if (m.type == "Hunt") {
        bool ok = fight_beasts(RandInt(1, 2), std::max(1, CurrentYear() / 3 + 1), "in the wilds");
        if (ok && Chance(45)) ok = fight_beasts(1, std::max(1, CurrentYear() / 3 + 1), "deeper in the wilds");
        if (ok) {
            butcher_kills();
            for (auto& c : party) train(c.entity_id, "Hunting", 60);
            if (kills.empty())
                rlog("The hunting party found no worthy prey and returned empty-handed.");
            else
                rlog("The hunting party returned dragging " + std::to_string(kills.size()) + " carcass(es).");
        }
        rep.success = ok && !kills.empty();
    } else if (m.type == "Gather") {
        if (Chance(30)) fight_beasts(1, 2, "while gathering");
        if (!party.empty()) {
            int herbs = 0, wood = 0;
            for (auto& c : party) {
                herbs += 5 + SkillOf(registry, c.entity_id, "Food Gathering");
                wood += RandInt(5, 12);
                train(c.entity_id, "Food Gathering", 50);
            }
            res.herbs += herbs;
            town.wood += wood;
            rep.loot.push_back(std::to_string(herbs) + " herbs");
            rep.loot.push_back(std::to_string(wood) + " wood");
            if (!kills.empty()) butcher_kills();
            rlog("The gathering party returned with " + std::to_string(herbs) + " herbs and " + std::to_string(wood) + " wood.");
            rep.success = true;
        }
    } else if (m.type == "Adventure") {
        // Scouting skill steers the party away from ambushes and toward finds
        int scout_avg = 0;
        for (auto& c : party) scout_avg += SkillOf(registry, c.entity_id, "Scouting");
        scout_avg /= (int)party.size();

        int events = RandInt(2, 4);
        for (int i = 0; i < events && !party.empty(); ++i) {
            int roll = std::min(110, RandInt(1, 100) + scout_avg * 2);
            if (roll <= 30) {
                if (!fight_beasts(RandInt(1, 3), std::max(2, CurrentYear() / 2 + 2), "in ancient ruins")) break;
            } else if (roll <= 50) {
                int stones = RandInt(20, 100);
                res.spirit_stones += stones;
                rlog("  The adventurers found a cache of " + std::to_string(stones) + " spirit stones!");
                rep.loot.push_back(std::to_string(stones) + " spirit stones");
            } else if (roll <= 65) {
                std::vector<const ItemDefinition*> pool;
                for (const auto& d : GameData::items)
                    if (d.category == "Weapon" && d.tier <= 4) pool.push_back(&d);
                if (!pool.empty()) {
                    const ItemDefinition* pick = pool[RandInt(0, (int)pool.size() - 1)];
                    InventoryComponent::ItemInstance it;
                    it.item_id = pick->id; it.count = 1;
                    res.stash.push_back(it);
                    rlog("  The adventurers recovered a " + pick->name + " from a forgotten tomb!");
                    rep.items.push_back(pick->name + " (tomb find) -> armory");
                }
            } else if (roll <= 85) {
                Combatant& lucky = party[RandInt(0, (int)party.size() - 1)];
                auto cc = registry.cultivations.find(lucky.entity_id);
                if (cc != registry.cultivations.end()) {
                    cc->second.qi += cc->second.max_qi * 0.5f;
                    cc->second.talent += 0.05f;
                    rlog("  " + lucky.name + " meditated at a spirit vein and gained profound insight!");
                    rep.xp.push_back(lucky.name + ": talent +0.05, qi surged (spirit vein insight)");
                }
            } else {
                int wealth = RandInt(10, 60);
                town.wealth += wealth;
                rlog("  The adventurers salvaged treasures worth " + std::to_string(wealth) + " wealth.");
                rep.loot.push_back(std::to_string(wealth) + " wealth (salvage)");
            }
        }
        if (!party.empty()) {
            if (!kills.empty()) butcher_kills();
            for (auto& c : party) train(c.entity_id, "Scouting", 50);
            rlog("The adventuring party has returned to town.");
            rep.success = true;
        }
    } else if (m.type == "Raid" || m.type == "Invade") {
        if (m.target_rival < 0 || m.target_rival >= (int)rivals.size()) return;
        RivalSettlement& rv = rivals[m.target_rival];
        bool full_assault = (m.type == "Invade");

        int soldiers = std::max(2, rv.power / (full_assault ? 8 : 14));
        std::vector<Combatant> defenders;
        std::vector<BodyComponent> soldier_bodies;
        soldier_bodies.reserve(soldiers);
        for (int i = 0; i < soldiers; ++i) {
            soldier_bodies.push_back(Body::CreateHumanoid(0.9f + (float)RandInt(0, 30) / 100.0f));
            Combatant c;
            c.name = rv.name + " disciple";
            c.body = &soldier_bodies.back();
            c.attack = 8 + rv.power / 8;
            c.defense = 2 + rv.power / 15;
            c.skill = 0.9f;
            c.damage_type = Chance(60) ? "slash" : "pierce";
            defenders.push_back(c);
        }

        rlog("=== " + std::string(full_assault ? "INVASION" : "RAID") + " of " + rv.name + " (" +
             std::to_string(party.size()) + " vs " + std::to_string(soldiers) + ") ===");
        CombatResult r = Combat::Fight(party, defenders, full_assault ? 0.75f : 0.5f);
        for (const auto& line : r.log) rlog(line);
        for (auto& c : party) {
            if (c.dead && c.entity_id != -1) {
                rep.casualties.push_back(c.name);
                HandleDeath(c.entity_id, "slain attacking " + rv.name);
            }
        }

        rv.relation = std::max(-100, rv.relation - (full_assault ? 60 : 35));
        rep.success = r.side_a_won;
        if (r.side_a_won) {
            if (full_assault) {
                rv.conquered = true;
                rv.power = std::max(5, rv.power - 30);
                int loot = rv.wealth / 2;
                rv.wealth -= loot;
                town.wealth += loot;
                rlog("VICTORY! " + rv.name + " has been conquered and will pay tribute. Looted " + std::to_string(loot) + " wealth.");
                rep.loot.push_back(std::to_string(loot) + " wealth (plunder)");
                rep.loot.push_back("Monthly tribute from " + rv.name);
            } else {
                int loot = rv.wealth / 3;
                rv.wealth -= loot;
                rv.power = std::max(5, rv.power - 10);
                town.wealth += loot;
                int stones = RandInt(20, 80);
                registry.sect_resources[town_id].spirit_stones += stones;
                rlog("The raid succeeded! Looted " + std::to_string(loot) + " wealth and " + std::to_string(stones) + " spirit stones.");
                rep.loot.push_back(std::to_string(loot) + " wealth (plunder)");
                rep.loot.push_back(std::to_string(stones) + " spirit stones");
            }
        } else {
            rlog("The attack on " + rv.name + " failed. The survivors limped home.");
        }
        for (auto& c : party) {
            if (c.entity_id == -1 || c.dead) continue;
            train(c.entity_id, "Warfare", 80);
            train(c.entity_id, "Weapons Mastery", 60);
        }
    }

    // Survivors' wounds go into the report and the chronicle
    for (auto& c : party) {
        if (c.entity_id == -1 || c.dead) continue;
        auto body = registry.bodies.find(c.entity_id);
        if (body != registry.bodies.end()) {
            std::string s = Body::Summarize(body->second);
            if (s != "unharmed") {
                rep.injuries.push_back(c.name + ": " + s);
                Log("  " + c.name + " returned: " + s + ".");
                // Losing a limb and walking home earns an epithet
                if (s.find("severed") != std::string::npos && registry.renderables.count(c.entity_id) &&
                    registry.renderables[c.entity_id].title.empty()) {
                    registry.renderables[c.entity_id].title = "the Unyielding";
                    Log("  " + c.name + " shall henceforth be known as " + c.name + " the Unyielding.");
                }
            }
        }
    }

    mission_reports.push_back(rep);
    if (mission_reports.size() > 60)
        mission_reports.erase(mission_reports.begin(), mission_reports.begin() + (mission_reports.size() - 60));
}

void SimEngine::ResolveTownDefense(const std::string& attacker_desc, std::vector<Combatant>& attackers,
                                   std::vector<BodyComponent>& beast_bodies) {
    (void)beast_bodies; // Storage owned by caller; bodies must outlive the fight
    TownComponent& town = registry.towns[town_id];

    // Guards first, then the bravest armed citizens
    std::vector<int> defender_ids;
    for (const auto& [eid, j] : registry.jobs) {
        auto st = registry.stats.find(eid);
        if (st == registry.stats.end() || st->second.is_dead || IsOnMission(eid)) continue;
        if (j.job == "Guard") defender_ids.push_back(eid);
    }
    if ((int)defender_ids.size() < (int)attackers.size()) {
        for (int eid : LivingPeople()) {
            if (IsOnMission(eid)) continue;
            if (std::find(defender_ids.begin(), defender_ids.end(), eid) != defender_ids.end()) continue;
            auto cc = registry.cultivations.find(eid);
            if (cc != registry.cultivations.end() && cc->second.age < 14) continue;
            defender_ids.push_back(eid);
            if ((int)defender_ids.size() >= (int)attackers.size() + 2) break;
        }
    }

    std::vector<Combatant> defenders;
    for (int eid : defender_ids) {
        Combatant c = MakeCitizenCombatant(eid);
        c.defense += town.defense_rating / 5; // Walls and works
        defenders.push_back(c);
    }

    Log("!!! " + attacker_desc + " attacks the town! " + std::to_string(defenders.size()) + " defenders rush out!");
    CombatResult r = Combat::Fight(attackers, defenders, 0.6f);
    for (const auto& line : r.log) Log(line);

    for (auto& c : defenders)
        if (c.dead && c.entity_id != -1) HandleDeath(c.entity_id, "killed defending the town");

    if (r.side_a_won) {
        int food_lost = std::min(town.food_reserves, RandInt(20, 60));
        int wealth_lost = std::min(town.wealth, RandInt(10, 40));
        town.food_reserves -= food_lost;
        town.wealth -= wealth_lost;
        town.morale = std::max(0, town.morale - 15);
        Log("The defenders were overwhelmed! The attackers pillaged " + std::to_string(food_lost) +
            " food and " + std::to_string(wealth_lost) + " wealth before withdrawing.");
    } else {
        int loot = (int)attackers.size() * RandInt(3, 8);
        town.wealth += loot;
        town.morale = std::min(100, town.morale + 5);
        Log("The town stands! The defenders salvaged " + std::to_string(loot) + " wealth from the corpses.");
    }
}

void SimEngine::TickRandomEvents() {
    TownComponent& town = registry.towns[town_id];
    SectResourceComponent& res = registry.sect_resources[town_id];

    // Wild beast attack
    bool beast_tide = (current_tick % 1800 == 0); // Every 5 years: a big one
    if (beast_tide || Chance(2)) {
        int max_tier = std::min(6, 1 + CurrentYear() / 3);
        std::vector<const BeastDefinition*> pool;
        for (const auto& b : GameData::beasts)
            if (b.tier <= max_tier && b.aggressive) pool.push_back(&b);
        if (pool.empty())
            for (const auto& b : GameData::beasts)
                if (b.tier <= max_tier) pool.push_back(&b);
        if (pool.empty()) {
            // World has no beasts that weak: send the weakest that exist
            int min_tier = 99;
            for (const auto& b : GameData::beasts) min_tier = std::min(min_tier, b.tier);
            for (const auto& b : GameData::beasts) if (b.tier == min_tier && b.aggressive) pool.push_back(&b);
            if (pool.empty())
                for (const auto& b : GameData::beasts) if (b.tier == min_tier) pool.push_back(&b);
        }
        if (!pool.empty()) {
            int count = beast_tide ? RandInt(4, 7) : RandInt(1, 3);
            std::vector<BodyComponent> beast_bodies;
            beast_bodies.reserve(count);
            std::vector<Combatant> beasts;
            std::string desc;
            for (int i = 0; i < count; ++i) {
                const BeastDefinition* pick = pool[RandInt(0, (int)pool.size() - 1)];
                beasts.push_back(MakeBeastCombatant(*pick, beast_bodies));
                if (i == 0) desc = pick->name;
            }
            if (beast_tide) Log("=== A BEAST TIDE descends upon the valley! ===");
            ResolveTownDefense(std::string(beast_tide ? "A beast tide led by " : "A pack of ") + desc + " (" + std::to_string(count) + ")", beasts, beast_bodies);
        }
    }

    // A wanderer asks to join
    if (Chance(1)) {
        int eid = CreatePerson(16, 50);
        Log(registry.renderables[eid].name + ", a wanderer, arrived and joined the town.");
    }

    // Travelling merchant: arrives with a cart of goods, stays a few days.
    // The player buys from the Town panel with spirit stones.
    if (merchant_days > 0) {
        merchant_days--;
        if (merchant_days == 0 && !merchant_stock.empty()) {
            merchant_stock.clear();
            Log("The travelling merchant packed up his cart and moved on.");
        }
    } else if (Chance(2)) {
        merchant_stock.clear();
        merchant_days = RandInt(3, 5);

        // Always offers provisions
        MerchantOffer food_offer;
        food_offer.label = "Provisions (50 food)";
        food_offer.price = 35;
        food_offer.food = 50;
        merchant_stock.push_back(food_offer);

        // Plus a few random goods: weapons, materials, consumables
        std::vector<const ItemDefinition*> goods;
        for (const auto& d : GameData::items)
            if (d.tier <= 4 && (d.category == "Weapon" || d.category == "Material" || d.category == "Consumable"))
                goods.push_back(&d);
        int n = RandInt(2, 4);
        for (int i = 0; i < n && !goods.empty(); ++i) {
            const ItemDefinition* pick = goods[RandInt(0, (int)goods.size() - 1)];
            MerchantOffer o;
            o.item_id = pick->id;
            o.label = pick->name;
            o.price = std::max(20, pick->base_value + RandInt(-10, 30));
            merchant_stock.push_back(o);
        }
        Log("A travelling merchant arrived with " + std::to_string(merchant_stock.size()) +
            " goods on his cart. He stays " + std::to_string(merchant_days) + " days (see Town panel).");
    }

    // Beast tide early warning: scouts spot it gathering five days out
    if ((current_tick + 5) % 1800 == 0) {
        Log("!!! Scouts report a BEAST TIDE gathering in the wilds - five days until it descends! Raise your guard!");
        town.morale = std::max(0, town.morale - 5);
    }

    // Lucky find
    if (Chance(1)) {
        int stones = RandInt(5, 30);
        res.spirit_stones += stones;
        Log("A child found " + std::to_string(stones) + " spirit stones in the riverbed.");
    }
}

std::string SimEngine::BuyFromMerchant(int index) {
    if (merchant_days <= 0 || index < 0 || index >= (int)merchant_stock.size())
        return "The merchant is not here.";
    SectResourceComponent& res = registry.sect_resources[town_id];
    TownComponent& town = registry.towns[town_id];
    MerchantOffer offer = merchant_stock[index];
    if (res.spirit_stones < offer.price)
        return "Not enough spirit stones (" + std::to_string(offer.price) + " needed).";

    res.spirit_stones -= offer.price;
    if (offer.food > 0) town.food_reserves += offer.food;
    if (!offer.item_id.empty()) {
        InventoryComponent::ItemInstance it;
        it.item_id = offer.item_id;
        it.count = 1;
        res.stash.push_back(it);
    }
    merchant_stock.erase(merchant_stock.begin() + index);
    Log("Bought " + offer.label + " from the merchant for " + std::to_string(offer.price) + " spirit stones.");
    return "Bought " + offer.label + ".";
}

// Fires the designed world events (Lucky Chances, Hazards, Encounters...)
// parsed from the Events & Encounters design document.
void SimEngine::TickWorldEvents() {
    if (!Chance(2) || GameData::events.empty()) return;
    TownComponent& town = registry.towns[town_id];
    SectResourceComponent& res = registry.sect_resources[town_id];

    std::vector<const EventDefinition*> pool;
    for (const auto& e : GameData::events)
        if (e.type != "Location") pool.push_back(&e);
    if (pool.empty()) return;
    const EventDefinition& ev = *pool[RandInt(0, (int)pool.size() - 1)];

    auto people = LivingPeople();
    if (people.empty()) return;
    int who = people[RandInt(0, (int)people.size() - 1)];
    std::string name = registry.renderables[who].name;

    if (ev.type == "LuckyChance") {
        int roll = RandInt(1, 3);
        if (roll == 1) {
            auto cc = registry.cultivations.find(who);
            if (cc != registry.cultivations.end()) cc->second.talent += 0.04f;
            Log("[EVENT] " + ev.name + ": " + ev.description + " " + name + "'s talent has grown!");
        } else if (roll == 2) {
            auto cc = registry.cultivations.find(who);
            if (cc != registry.cultivations.end()) cc->second.qi += cc->second.max_qi * 0.6f;
            Log("[EVENT] " + ev.name + ": " + ev.description + " " + name + "'s qi surges!");
        } else {
            int stones = RandInt(15, 70);
            res.spirit_stones += stones;
            Log("[EVENT] " + ev.name + ": " + ev.description + " The town gains " + std::to_string(stones) + " spirit stones.");
        }
    } else if (ev.type == "Hazard") {
        auto body = registry.bodies.find(who);
        if (body != registry.bodies.end()) {
            int part = Body::PickHitPart(body->second);
            if (part >= 0) {
                WoundResult w = Body::ApplyDamage(body->second, part, RandInt(8, 22),
                                                  Chance(50) ? "slash" : "blunt", name);
                Log("[EVENT] " + ev.name + ": " + ev.description + " " + w.text);
                if (w.fatal || Body::IsDead(body->second)) HandleDeath(who, ev.name);
            }
        }
        town.morale = std::max(0, town.morale - 3);
    } else if (ev.type == "Encounter") {
        if (Chance(50)) {
            int wealth = RandInt(10, 45);
            town.wealth += wealth;
            Log("[EVENT] " + ev.name + ": " + ev.description + " The town profits " + std::to_string(wealth) + " wealth.");
        } else {
            Log("[EVENT] " + ev.name + ": " + ev.description);
        }
    } else { // WorldEvent: the wider world shifts
        if (!rivals.empty()) {
            RivalSettlement& rv = rivals[RandInt(0, (int)rivals.size() - 1)];
            if (rv.destroyed) return;
            int delta = RandInt(-15, 15);
            rv.power = std::max(5, rv.power + delta);
            Log("[EVENT] " + ev.name + ": " + ev.description + " (" + rv.name +
                (delta >= 0 ? " grows stronger)" : " is weakened)"));
        } else {
            Log("[EVENT] " + ev.name + ": " + ev.description);
        }
    }
}

// Monthly sparring tournament: glory for the champion, grudges for the losers.
void SimEngine::TickTournament() {
    std::vector<int> contestants;
    for (int eid : LivingPeople()) {
        auto cc = registry.cultivations.find(eid);
        if (cc == registry.cultivations.end() || cc->second.age < 16) continue;
        if (IsOnMission(eid)) continue;
        auto body = registry.bodies.find(eid);
        if (body != registry.bodies.end() && body->second.blood < body->second.max_blood * 0.8f) continue;
        contestants.push_back(eid);
    }
    if ((int)contestants.size() < 4) return;

    // Random bracket of up to 8
    for (int i = (int)contestants.size() - 1; i > 0; --i)
        std::swap(contestants[i], contestants[RandInt(0, i)]);
    if (contestants.size() > 8) contestants.resize(8);

    Log("=== The monthly sparring tournament begins! " + std::to_string(contestants.size()) + " step into the ring. ===");
    auto spar_score = [&](int eid) {
        return EffectiveAttack(eid) + SkillOf(registry, eid, "Weapons Mastery") * 2 + RandInt(0, 25);
    };
    std::vector<int> round = contestants;
    while (round.size() > 1) {
        std::vector<int> next;
        for (size_t i = 0; i + 1 < round.size(); i += 2) {
            int a = round[i], b = round[i + 1];
            int winner = spar_score(a) >= spar_score(b) ? a : b;
            int loser = winner == a ? b : a;
            next.push_back(winner);
            TrainSkill(registry, a, "Weapons Mastery", 20);
            TrainSkill(registry, b, "Weapons Mastery", 20);
            if (round.size() <= 4)
                Log("  " + registry.renderables[winner].name + " defeats " + registry.renderables[loser].name +
                    (round.size() == 2 ? " in the final!" : " in the semifinal."));
            // The loser nurses a grudge (the doc's rivalry loop)
            auto& rl = registry.relations[loser].network[winner];
            rl.target_entity_id = winner;
            if (rl.type == "None") rl.type = "Rival";
            rl.affinity = std::max(-100, rl.affinity - 8);
        }
        if (round.size() % 2 == 1) next.push_back(round.back()); // Bye
        round = next;
    }

    int champ = round[0];
    registry.renderables[champ].title = "Champion";
    TrainSkill(registry, champ, "Warfare", 40);
    if (registry.needs.count(champ))
        registry.needs[champ].morale = std::min(100, registry.needs[champ].morale + 12);
    TownComponent& town = registry.towns[town_id];
    town.morale = std::min(100, town.morale + 3);
    SectResourceComponent& res = registry.sect_resources[town_id];
    int prize = std::min(25, res.spirit_stones);
    res.spirit_stones -= prize;
    registry.inventories[champ].spirit_stones += prize;
    Log("TOURNAMENT: " + registry.renderables[champ].name + " is crowned Champion! (+" +
        std::to_string(prize) + " spirit stones prize)");
}

void SimEngine::TickRivals() {
    TownComponent& town = registry.towns[town_id];
    for (auto& rv : rivals) {
        if (rv.destroyed) continue;
        // Rivals grow slowly
        if (Chance(3)) rv.power++;
        if (Chance(5)) rv.wealth += RandInt(1, 5);

        // Tribute from conquered settlements (monthly)
        if (rv.conquered && current_tick % 30 == 0) {
            int tribute = std::max(1, rv.power / 5);
            town.wealth += tribute;
            Log(rv.name + " delivered its tribute: " + std::to_string(tribute) + " wealth.");
            continue;
        }

        // Hostile rivals may raid us
        if (!rv.conquered && rv.relation < -40 && Chance(1)) {
            int soldiers = std::max(2, rv.power / 15);
            std::vector<BodyComponent> bodies;
            bodies.reserve(soldiers);
            std::vector<Combatant> raiders;
            for (int i = 0; i < soldiers; ++i) {
                bodies.push_back(Body::CreateHumanoid(0.9f + (float)RandInt(0, 30) / 100.0f));
                Combatant c;
                c.name = rv.name + " raider";
                c.body = &bodies.back();
                c.attack = 8 + rv.power / 8;
                c.defense = 2 + rv.power / 15;
                c.skill = 0.9f;
                c.damage_type = Chance(60) ? "slash" : "pierce";
                raiders.push_back(c);
            }
            ResolveTownDefense("A war party from " + rv.name, raiders, bodies);
        }
    }
}

void SimEngine::TickSocial() {
    auto people = LivingPeople();
    std::vector<int> adults;
    for (int e : people) {
        auto cc = registry.cultivations.find(e);
        if (cc != registry.cultivations.end() && cc->second.age >= 16 && !IsOnMission(e)) adults.push_back(e);
    }
    if (adults.size() < 2) return;

    // A few random social interactions per day
    int interactions = std::min(3, (int)adults.size() / 2);
    for (int i = 0; i < interactions; ++i) {
        int a = adults[RandInt(0, (int)adults.size() - 1)];
        int b = adults[RandInt(0, (int)adults.size() - 1)];
        if (a == b) continue;

        auto& ra = registry.relations[a];
        auto& rb = registry.relations[b];
        int social_a = registry.personalities.count(a) ? registry.personalities[a].social : 50;
        int delta = RandInt(-2, 4) + (social_a > 70 ? 1 : 0);

        auto& rel_ab = ra.network[b];
        rel_ab.target_entity_id = b;
        if (rel_ab.type == "None") rel_ab.type = "Acquaintance";
        rel_ab.affinity = std::clamp(rel_ab.affinity + delta, -100, 100);
        auto& rel_ba = rb.network[a];
        rel_ba.target_entity_id = a;
        if (rel_ba.type == "None") rel_ba.type = "Acquaintance";
        rel_ba.affinity = std::clamp(rel_ba.affinity + delta, -100, 100);

        // Marriage
        if (rel_ab.affinity > 60 && rel_ba.affinity > 60 &&
            ra.spouse_id == -1 && rb.spouse_id == -1 && ra.gender != rb.gender) {
            ra.spouse_id = b;
            rb.spouse_id = a;
            rel_ab.type = rel_ba.type = "Spouse";
            Log("WEDDING: " + registry.renderables[a].name + " and " + registry.renderables[b].name + " were married!");
        }
    }

    // Births to married couples
    for (int e : adults) {
        auto& re = registry.relations[e];
        if (re.gender != "Female" || re.spouse_id == -1) continue;
        auto spouse_stats = registry.stats.find(re.spouse_id);
        if (spouse_stats == registry.stats.end() || spouse_stats->second.is_dead) continue;
        auto cc = registry.cultivations.find(e);
        if (cc == registry.cultivations.end() || cc->second.age > 45) continue;
        if (!Chance(1)) continue; // ~once every 100 days per couple

        int child = CreatePerson(0, 0);
        auto& rel_child = registry.relations[child];
        rel_child.parents = {re.spouse_id, e};
        re.children.push_back(child);
        registry.relations[re.spouse_id].children.push_back(child);
        registry.bodies[child] = Body::CreateHumanoid(0.3f);
        Log("BIRTH: " + registry.renderables[e].name + " gave birth! The child is named " +
            registry.renderables[child].name + ".");
    }
}

void SimEngine::TickYearly() {
    for (int eid : LivingPeople()) {
        auto cc = registry.cultivations.find(eid);
        if (cc == registry.cultivations.end()) continue;
        cc->second.age++;
        // Children grow
        if (cc->second.age == 14) {
            auto body = registry.bodies.find(eid);
            if (body != registry.bodies.end() && body->second.plan == "humanoid") {
                BodyComponent grown = Body::CreateHumanoid(0.95f);
                // Carry permanent losses forward
                for (size_t i = 0; i < grown.parts.size() && i < body->second.parts.size(); ++i)
                    if (body->second.parts[i].IsGone()) {
                        grown.parts[i].wound = body->second.parts[i].wound;
                        grown.parts[i].hp = 0;
                    }
                body->second = grown;
            }
            Log(registry.renderables[eid].name + " has come of age and can now work.");
        }
        if (cc->second.age > cc->second.lifespan && Chance(30)) {
            HandleDeath(eid, "old age");
        }
    }
    Log("--- A new year dawns: it is now Year " + std::to_string(CurrentYear()) + ". ---");
}

void SimEngine::HandleDeath(int entity, const std::string& cause) {
    auto st = registry.stats.find(entity);
    if (st == registry.stats.end() || st->second.is_dead) return;
    st->second.is_dead = true;
    st->second.cause_of_death = cause;
    st->second.death_year = CurrentYear();

    // Their weapon goes back to the armory
    auto inv = registry.inventories.find(entity);
    if (inv != registry.inventories.end() && town_id != -1) {
        for (auto& item : inv->second.items) {
            if (item.equipped) {
                item.equipped = false;
                registry.sect_resources[town_id].stash.push_back(item);
            }
        }
        inv->second.items.clear();
    }
    registry.jobs.erase(entity);

    // Remove from any active mission
    for (auto& m : missions)
        m.party.erase(std::remove(m.party.begin(), m.party.end(), entity), m.party.end());

    if (town_id != -1) {
        TownComponent& town = registry.towns[town_id];
        town.population = std::max(0, town.population - 1);
        town.morale = std::max(0, town.morale - 4);
        if (town.mayor_id == entity) {
            town.mayor_id = -1;
            Log("The Mayor is dead! The town is leaderless until a new election is held.");
        }
    }

    std::string name = registry.renderables.count(entity) ? registry.renderables[entity].name : "Someone";
    Log("[DEATH] " + name + " died: " + cause + ".");
    HistoryGenerator::AddStructuredEvent(CurrentYear(), "Death", entity, -1, town_id, name + " died: " + cause + ".");

    // Grief
    auto rel = registry.relations.find(entity);
    if (rel != registry.relations.end()) {
        for (const auto& [other, r] : rel->second.network) {
            if (r.affinity > 50 && registry.needs.count(other))
                registry.needs[other].morale = std::max(0, registry.needs[other].morale - 15);
        }
        if (rel->second.spouse_id != -1 && registry.relations.count(rel->second.spouse_id))
            registry.relations[rel->second.spouse_id].spouse_id = -1;
    }
}

void SimEngine::RecomputeDefense() {
    if (town_id == -1) return;
    TownComponent& town = registry.towns[town_id];
    int guards = 0, guard_power = 0;
    for (const auto& [eid, j] : registry.jobs) {
        auto st = registry.stats.find(eid);
        if (st == registry.stats.end() || st->second.is_dead || IsOnMission(eid)) continue;
        if (j.job == "Guard") {
            guards++;
            guard_power += 3 + SkillOf(registry, eid, "Warfare");
        }
    }
    town.defense_rating = 5 + guard_power + CountBuildings("Wall") * 15 + CountBuildings("Barracks") * 8;
}

// ---------------------------------------------------------------------------
// Persistence
// ---------------------------------------------------------------------------

bool SimEngine::SaveGame(const std::string& slot) {
    try {
        if (!fs::exists("SaveData")) fs::create_directory("SaveData");
        json j;
        j["version"] = 2;
        j["tick"] = current_tick;
        j["town_id"] = town_id;
        j["game_started"] = game_started;
        j["map_seed"] = world_map.Seed();
        j["rivals"] = rivals;
        j["missions"] = missions;
        j["mission_reports"] = mission_reports;
        j["labor_priority"] = labor_priority;
        j["merchant_stock"] = merchant_stock;
        j["merchant_days"] = merchant_days;
        j["registry"] = registry.Serialize();
        j["history"] = HistoryGenerator::GetHistory();

        std::ofstream out("SaveData/" + slot + ".json");
        if (!out.is_open()) return false;
        out << j.dump();
        return true;
    } catch (...) {
        return false;
    }
}

bool SimEngine::LoadGame(const std::string& slot) {
    try {
        std::ifstream in("SaveData/" + slot + ".json");
        if (!in.is_open()) return false;
        json j;
        in >> j;

        LoadDataFiles();
        registry.Deserialize(j["registry"]);
        current_tick = j.value("tick", 0LL);
        town_id = j.value("town_id", -1);
        game_started = j.value("game_started", false);
        rivals = j.value("rivals", std::vector<RivalSettlement>{});
        missions = j.value("missions", std::vector<ActiveMission>{});
        mission_reports = j.value("mission_reports", std::vector<MissionReport>{});
        labor_priority = j.value("labor_priority", std::map<std::string, int>{
            {"Build", 5}, {"Heal", 5}, {"Farm", 4}, {"Forage", 3}, {"Woodcut", 3}, {"Mine", 2}, {"Guard", 2}});
        merchant_stock = j.value("merchant_stock", std::vector<MerchantOffer>{});
        merchant_days = j.value("merchant_days", 0);
        unsigned map_seed = j.value("map_seed", 0u);
        if (map_seed != 0) world_map.Generate(map_seed);
        else if (game_started) world_map.Generate((unsigned)RandInt(1, 0x7FFFFFFF)); // legacy save
        HistoryGenerator::Clear();
        if (j.contains("history"))
            HistoryGenerator::events = j["history"].get<std::vector<HistoryEvent>>();
        candidates.clear();
        recent_log.clear();
        return true;
    } catch (...) {
        return false;
    }
}
