#pragma once
#include <string>
#include <vector>
#include <cstdint>

// --- Static Definition Database ---
// Populated at startup by GameDataLoader from the markdown design documents.
// These are read-only templates; live game state lives in the ECS Registry.

struct BeastDefinition {
    std::string id;
    std::string name;
    std::string element; // "Metal", "Fire", etc.
    std::string biome;   // "Forest", "Plains", etc.
    int tier = 1;        // 1-10
    bool aggressive = false;
    char symbol = 'B';
    uint32_t color = 0xFFFFFFFF;
};

struct ResourceDefinition {
    std::string id;
    std::string name;
    std::string type;    // "Herb", "Ore"
    std::string biome;
    int tier = 1;
    char symbol = '*';
    uint32_t color = 0xFFFFFFFF;
    std::string alchemy_tag; // "Healing", "Poison"
    std::string effect;      // "Restores HP"
};

struct RaceDefinition {
    std::string id;
    std::string name;
    bool playable = false;
    std::string origin;
    // Base stat multipliers (1.0 is standard Human)
    float hp_mult = 1.0f;
    float atk_mult = 1.0f;
    float def_mult = 1.0f;
    float spd_mult = 1.0f;
    std::string affinity; // Fire, Wood, None, etc.
    std::string trait_desc;
};

struct ItemDefinition {
    std::string id;
    std::string name;
    std::string category;     // "Weapon", "Material", "Consumable", "Artifact", "Manual"
    std::string sub_category; // "SWD", "Herb", "Core", "Scripture", ...
    int tier = 1;
    int base_value = 0;       // Spirit Stones
    std::string description;
    int attack = 0;
    int defense = 0;
    std::string effect_tag;            // "Bleed", "Burn"
    std::string required_resource_id;  // Crafting input
};

struct SkillDefinition {
    std::string id;
    std::string name;
    int tier = 1;
    std::string element;
    int qi_cost = 10;
    std::string description;
};

struct CultivationDefinition {
    std::string id;
    std::string name;
    int tier = 1;
    std::string element;
    std::string required_item_id; // Item needed for breakthrough/learning
    std::string description;
    float hp_multiplier = 1.0f;
    float attack_multiplier = 1.0f;
};

struct EventDefinition {
    std::string id;
    std::string name;
    std::string type; // "Location", "WorldEvent", "LuckyChance", "Encounter", "Hazard"
    std::string description;
    int danger_level = 3;
};

class GameData {
public:
    static std::vector<RaceDefinition> races;
    static std::vector<BeastDefinition> beasts;
    static std::vector<ResourceDefinition> resources;
    static std::vector<ItemDefinition> items;
    static std::vector<SkillDefinition> skills;
    static std::vector<CultivationDefinition> cultivationMethods;
    static std::vector<EventDefinition> events;

    static const std::vector<RaceDefinition>& GetRaces() { return races; }
    static const std::vector<BeastDefinition>& GetBeasts() { return beasts; }
    static const std::vector<ResourceDefinition>& GetResources() { return resources; }
    static const std::vector<ItemDefinition>& GetItems() { return items; }
    static const std::vector<SkillDefinition>& GetSkills() { return skills; }
    static const std::vector<CultivationDefinition>& GetCultivationMethods() { return cultivationMethods; }
    static const std::vector<EventDefinition>& GetEvents() { return events; }

    static const ItemDefinition* FindItem(const std::string& id) {
        for (const auto& d : items) if (d.id == id) return &d;
        return nullptr;
    }
};
