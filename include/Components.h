#pragma once
#include <string>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Basic Components

struct TransformComponent {
    int x = 0;
    int y = 0;
    int z = 0; // Layer

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(TransformComponent, x, y, z)
};

struct StatsComponent {
    // Derived from Cultivation + Race
    long long hp = 100;
    long long max_hp = 100;
    long long attack = 10;
    long long defense = 0;
    float speed = 1.0f;
    bool is_dead = false;
    std::string cause_of_death = "";
    int death_x = 0;
    int death_y = 0;
    int death_year = 0;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(StatsComponent, hp, max_hp, attack, defense, speed, is_dead, cause_of_death, death_x, death_y, death_year)
};

struct InjuryComponent {
    // Injury Levels: 0=None, 1=Minor, 2=Major, 3=Severe, 4=Critical
    int severity = 0;
    int duration = 0; // Ticks remaining
    std::string description = "Healthy";
    std::string source = ""; // e.g. "Tiger Beast", "Duel with Ye Chen"
    int year_sustained = 0;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(InjuryComponent, severity, duration, description, source, year_sustained)
};

struct CultivationComponent {
    float qi = 0.0f;
    float max_qi = 100.0f;
    
    // Realm: 0=Mortal, 1=Qi Condensation, 2=Foundation, etc.
    int realm_major = 0; 
    int realm_minor = 1; // 1-9
    
    std::string element = "None"; // Fire, Water, etc.
    float talent = 1.0f; // Multiplier for Qi gain
    
    int age = 16;
    long long age_ticks = 0; // Tracks time between years
    int lifespan = 100;
    
    // Birthdate (Fixed at generation)
    int birth_year = 0;
    int birth_month = 1;
    int birth_day = 1;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(CultivationComponent, qi, max_qi, realm_major, realm_minor, element, talent, age, age_ticks, lifespan, birth_year, birth_month, birth_day)
};

struct RenderComponent {
    char symbol = '?';
    uint32_t color = 0xFFFFFFFF;
    std::string name = "Unknown";
    bool visible_on_map = true;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(RenderComponent, symbol, color, name, visible_on_map)
};

struct ResourceComponent {
    std::string id;
    std::string type; // "Herb", "Ore"
    int amount = 1;
    bool harvestable = true;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ResourceComponent, id, type, amount, harvestable)
};

struct AIComponent {
    std::string behavior = "Idle"; // "Aggressive", "Flee", etc.
    int target_entity_id = -1;
    int perception_range = 10;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(AIComponent, behavior, target_entity_id, perception_range)
};

struct SectComponent {
    std::string name = "None";
    int rank = 0; // 0=None, 1=Outer, 2=Inner, 3=Core, 4=Elder, 5=Leader
    int sect_id = -1;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SectComponent, name, rank, sect_id)
};

struct RaceComponent {
    std::string race_id = "RACE_01"; // Human
    std::string race_name = "Human";
    // Stat multipliers could go here
    float hp_mult = 1.0f;
    float atk_mult = 1.0f;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(RaceComponent, race_id, race_name, hp_mult, atk_mult)
};

// --- Living World Components (v2.0) ---

struct PersonalityComponent {
    // scale 0-100
    int ambition = 50;  // High: Wants to expand/conquer
    int greed = 50;     // High: Hoards resources, raids
    int bravery = 50;   // High: Fights strong enemies
    int loyalty = 50;   // High: Sticks to Sect
    int ruthlessness = 50; // High: Kills enemies, no mercy
    int caution = 50;   // High: Avoids risky fights
    int wisdom = 50;    // High: Better decisions/Cultivation
    int social = 50;    // High: More likely to recruit/negotiate
    int vengeance = 50; // High: Remembers attacks
    int diligence = 50; // High: Faster cultivation

    std::string current_goal = "None"; // "Expand", "Cultivate", "Raid", "Recover", "Reviewing Sect"

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(PersonalityComponent, ambition, greed, bravery, loyalty, ruthlessness, caution, wisdom, social, vengeance, diligence, current_goal)
};

struct InventoryComponent {
    struct ItemInstance {
        std::string item_id;
        int count = 1;
        int durability = 100;
        bool equipped = false;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(ItemInstance, item_id, count, durability, equipped)
    };
    std::vector<ItemInstance> items;
    int spirit_stones = 0;
    int contribution_points = 0;
    int sect_prestige = 0;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(InventoryComponent, items, spirit_stones, contribution_points, sect_prestige)
};

struct SectResourceComponent {
    // Attached to Sect Leader or Base
    int spirit_stones = 100;
    int herbs = 50;
    int ores = 50;
    int prestige = 0; // Gained by winning wars/events
    
    // Scarcity Flags
    bool starving = false;
    int starvation_years = 0;
    
    struct Mission {
        std::string type; // "Adventure", "Raid", "Gather"
        int target_location_id; // -1 if generic
        int reward_contribution;
        int required_strength; 
        bool active;
        
        // Requirements
        std::string required_item_id;
        int required_count;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Mission, type, target_location_id, reward_contribution, required_strength, active, required_item_id, required_count)
    };
    std::vector<Mission> mission_queue;
    
    // V3.0: Item Stash
    std::vector<InventoryComponent::ItemInstance> stash;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SectResourceComponent, spirit_stones, herbs, ores, prestige, starving, starvation_years, mission_queue, stash)
};

struct SectDiplomacyComponent {
    struct Relation {
        int target_sect_id;
        int affinity = 0; // -100 to 100
        std::string status = "Neutral"; // "Neutral", "Ally", "War"

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Relation, target_sect_id, affinity, status)
    };
    std::map<int, Relation> relations; // Target Sect ID -> Relation Data

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SectDiplomacyComponent, relations)
};

// --- New Components for v2.1 ---

struct SkillsComponent {
    std::string active_method_id = "MTH_GEN_01";
    std::vector<std::string> known_skills;
    int mastery_level = 0; // % towards breakthrough

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SkillsComponent, active_method_id, known_skills, mastery_level)
};

struct RelationComponent {
    struct Relationship {
        int target_entity_id;
        int affinity = 0; // -100 to 100
        int intimacy = 0; // 0 to 100
        std::string type = "None"; // "Parent", "Child", "Spouse", "Master", "Disciple"
        std::vector<std::string> history; // e.g. "Year 10: Killed brother"

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Relationship, target_entity_id, affinity, intimacy, type, history)
    };
    std::string gender = "Male"; // "Male" or "Female"
    // TargetID -> Relationship Data
    std::map<int, Relationship> network;

    // Lineage & Cached Socials
    std::pair<int, int> parents = {-1, -1};
    std::vector<int> children;
    int spouse_id = -1;
    int master_id = -1;
    std::vector<int> disciples;
    
    std::vector<int> friends; // Cached list of Affinity > 50
    std::vector<int> enemies; // Cached list of Affinity < -50

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(RelationComponent, gender, network, parents, children, spouse_id, master_id, disciples, friends, enemies)
};

struct LocationComponent {
    bool is_inside = false;
    int location_entity_id = -1; // ID of the Map Object (Sect/City/Ruin) they are inside

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(LocationComponent, is_inside, location_entity_id)
};

// Exact position for pixel-movement simulation
struct PrecisePositionComponent {
    float x = 0.0f;
    float y = 0.0f;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(PrecisePositionComponent, x, y)
};
