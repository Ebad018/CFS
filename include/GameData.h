#pragma once
#include <string>
#include <vector>
#include <map>

struct BeastDefinition {
    std::string id;
    std::string name;
    std::string element; // "Metal", "Fire", etc.
    std::string biome;   // "Forest", "Plains", etc.
    int tier;            // 1-10
    bool aggressive;
    char symbol;
    uint32_t color;
};

struct ResourceDefinition {
    std::string id;
    std::string name;
    std::string type;    // "Herb", "Ore"
    std::string biome;
    int tier;
    char symbol;
    uint32_t color;
    std::string alchemy_tag; // "Healing", "Poison"
    std::string effect;      // "Restores HP"
};

struct RaceDefinition {
    std::string id;
    std::string name;
    bool playable;
    std::string origin;
    // Base Stat Multipliers (1.0 is standard Human)
    float hp_mult;
    float atk_mult;
    float def_mult;
    float spd_mult;
    std::string affinity; // Fire, Wood, None, etc.
    std::string trait_desc;
};

struct EventDefinition {
    int id; // 1-100
    std::string title;
    std::string type; // "POI", "Global", "Lucky", "Encounter", "Hazard"
    std::string description;
    // Logic tags for EventManager
    std::string trigger_biome; // "Forest", "Any", "Volcano"
    std::string reward_item_id; // Optional
    std::string spawn_entity_id; // Optional
    std::string effect_tag; // "Damage:Lightning", "GainXP", "Merchant"
};

class GameData {
public:
    static const std::vector<RaceDefinition>& GetRaces() {
        static std::vector<RaceDefinition> races = {
            // Part 1: Humanoids
            {"RACE_01", "Human", true, "Mortal Plane", 1.0f, 1.0f, 1.0f, 1.0f, "None", "High Insight: +20% Learning Speed"},
            {"RACE_02", "Phoenix Scion", true, "Volcano", 1.2f, 1.3f, 1.0f, 1.1f, "Fire", "Rebirth: Chance to revive"},
            {"RACE_03", "Tiger Scion", true, "Mountains", 1.1f, 1.4f, 1.1f, 1.2f, "Metal", "Slaughter Aura: Stuns lower tiers"},
            {"RACE_04", "Giant Demon (Orc)", true, "Abyss", 2.0f, 1.5f, 0.5f, 0.8f, "Body", "Physique Supreme: High Str/Def, No Magic"},
            
            // Part 2: Demihumans
            {"RACE_05", "Spirit-Wood Fey", true, "Forest", 0.7f, 0.8f, 0.6f, 1.2f, "Wood", "Photosynthesis: No food needed in sun"},
            {"RACE_06", "Abyssal Octoman", true, "Deep Sea", 1.5f, 1.2f, 1.0f, 1.0f, "Water", "Amphibious: Bonus in Water"},
            {"RACE_07", "Stone Spirit", false, "Caves", 1.5f, 0.8f, 2.0f, 0.5f, "Earth", "Lithovore: Eats stones"},
            {"RACE_08", "Sky-Winged Celestial", true, "Sky", 0.6f, 1.2f, 0.5f, 1.5f, "Wind", "Flight: Can fly over obstacles"},
            
            // Part 3: Monstrous
            {"RACE_09", "Fox-Kin", true, "Swamp", 0.9f, 0.9f, 0.8f, 1.4f, "Illusion", "Shapeshift: Human/Beast modes"},
            {"RACE_10", "Void Shade", true, "Void", 0.5f, 2.0f, 0.2f, 2.0f, "Space", "Phase: Ignore physical attacks"},
            {"RACE_11", "Hive-Born", false, "Underground", 0.4f, 0.6f, 0.4f, 1.0f, "Metal", "Hive Mind: Group Aggro"},
            {"RACE_12", "Asura", true, "Battlefield", 1.5f, 1.8f, 0.8f, 1.2f, "Blood", "Battle Hunger: XP from Kills"}
        };
        return races;
    }

    static const std::vector<BeastDefinition>& GetBeasts() {
        static std::vector<BeastDefinition> beasts = {
             // --- GOD BEASTS (Unique) ---
            {"GB_01", "Primordial Chaos Dragon", "Chaos", "Void", 10, true, 'D', 0xFF00FFFF},
            {"GB_02", "Nirvana Phoenix", "Fire", "Volcano", 10, true, 'P', 0xFF4500FF},
            {"GB_03", "Black Turtle", "Earth", "Deep Sea", 10, false, 'T', 0x8B4513FF},
            {"GB_04", "Void Leviathan", "Water", "Deep Sea", 10, true, 'L', 0x000080FF},
            {"GB_05", "White Tiger", "Metal", "Battlefield", 10, true, 'W', 0xC0C0C0FF},
            {"GB_06", "Five-Colored Qilin", "Balance", "Spirit Vein", 10, false, 'Q', 0xFFD700FF},
            {"GB_07", "Solar Golden Crow", "Fire", "Sky", 10, true, 'C', 0xFFFF00FF},
            {"GB_08", "Ghost Butterfly", "Soul", "Abyss", 10, true, 'B', 0x9370DBFF},
            {"GB_09", "Time Spirit Carp", "Time", "Water", 10, false, 'f', 0x00FFFFFF},
            {"GB_10", "Six-Eared Ape", "Wind", "Jungle", 10, true, 'A', 0xA52A2AFF},

            // --- SAINT BEASTS (Regional Bosses) ---
            {"SB_01", "Purple Lightning Roc", "Thunder", "Peak", 9, true, 'R', 0x800080FF},
            {"SB_02", "Cryo-Jade Wyrm", "Ice", "Tundra", 9, true, 'W', 0xE0FFFFFF},
            {"SB_03", "Blood-Eye Demon Ape", "Blood", "Abyss", 8, true, 'A', 0x8B0000FF},
            {"SB_04", "Thousand-Leg Earth Dragon", "Poison", "Underground", 8, true, 'D', 0x556B2FFF},
            {"SB_05", "Abyssal Jellyfish", "Thunder", "Deep Sea", 8, true, 'J', 0x483D8BFF},
            {"SB_06", "Nine-Tailed Spirit Fox", "Illusion", "Forest", 9, true, 'F', 0xFF69B4FF},
            {"SB_07", "Golden-Horn Mammoth", "Metal", "Plains", 8, true, 'M', 0xDAA520FF},
            {"SB_08", "Netherfire Shark", "Fire", "Volcano", 8, true, 'S', 0xFF4500FF},
            {"SB_09", "Sword-Wing Mantis", "Metal", "Bamboo", 8, true, 'm', 0x32CD32FF},
            {"SB_10", "Celestial Moon Rabbit", "Yin", "Moon", 8, false, 'r', 0xF0F8FFFF},
            
            // --- STANDARD BEASTS (Partial List for Stability) ---
            // Plains
            {"SD_01", "Iron-Back Wolf", "Metal", "Plains", 1, true, 'w', 0xA9A9A9FF},
            {"SD_02", "Wind-Chaser Leopard", "Wind", "Plains", 2, true, 'l', 0xFFFFE0FF},
            {"SD_08", "Golden-Mane Lion", "Metal", "Plains", 4, true, 'L', 0xFFD700FF},
            {"SD_47", "Sun-Flower Sprite", "Wood", "Plains", 3, true, 'f', 0xFFA500FF},

            // Forest
            {"SD_07", "Green-Scale Python", "Wood", "Forest", 2, true, 's', 0x008000FF},
            {"SD_09", "Shadow Cat", "Dark", "Forest", 2, true, 'c', 0x2F4F4FFF},
            {"SD_43", "Jade-Horned Deer", "Wood", "Forest", 3, false, 'd', 0x98FB98FF},
            {"SD_35", "Parasite Vine", "Wood", "Forest", 2, true, 'v', 0x006400FF},

            // Mountain
            {"SD_03", "Rock-Skin Boar", "Earth", "Mountain", 2, true, 'b', 0x8B4513FF},
            {"SD_10", "Steel-Claw Bear", "Metal", "Mountain", 3, true, 'B', 0x708090FF},
            {"SD_41", "Thunder-Horn Goat", "Thunder", "Mountain", 3, false, 'g', 0x9370DBFF},
            {"SD_33", "Stone Gargoyle", "Earth", "Mountain", 3, true, 'G', 0x696969FF},

            // Desert
            {"SD_04", "Flame-Spitting Lizard", "Fire", "Sand", 1, true, 'l', 0xFF4500FF},
            {"SD_11", "Diamond-Back Scorpion", "Poison", "Sand", 2, true, 'x', 0xD2691EFF},
            {"SD_44", "Sand Burrower", "Earth", "Sand", 3, true, 'w', 0xF4A460FF}, 
            {"SD_36", "Silver-Wing Ant", "Metal", "Sand", 2, true, 'a', 0xC0C0C0FF},

            // Swamp
            {"SD_15", "Myriad-Eye Toad", "Illusion", "Swamp", 2, true, 't', 0x800080FF},
            {"SD_13", "Venom-Crown Cobra", "Poison", "Swamp", 3, true, 's', 0x8B008BFF},
            {"SD_28", "Bone-Eating Leech", "Blood", "Swamp", 1, true, '~', 0x800000FF},
            {"SD_42", "Blood Mosquito", "Blood", "Swamp", 1, true, '.', 0xDC143CFF},

            // Water
            {"SD_16", "Razor-Tooth Piranha", "Water", "Water", 1, true, 'p', 0x00CED1FF},
            {"SD_19", "Spirit Carp", "Water", "Water", 1, false, 'f', 0xFF69B4FF},
            {"SD_20", "Blue-Scale Makara", "Water", "Water", 3, true, 'm', 0x000080FF},
            {"SD_27", "Ice-Fin Shark", "Ice", "Deep Water", 3, true, 'S', 0xE0FFFFFF},
            
            // Special
            {"SD_31", "Magma Beetle", "Fire", "Volcano", 2, true, 'b', 0xFF0000FF},
            {"SD_39", "Obsidian Golem", "Fire", "Volcano", 4, true, 'G', 0x2F4F4FFF},
            {"SD_12", "Crimson-Beak Eagle", "Fire", "Sky", 2, true, 'e', 0xB22222FF},
            {"SD_34", "Cloud-Mist Horse", "Wind", "Sky", 4, false, 'h', 0xF0F8FFFF},
            {"SD_46", "Void Chameleon", "Space", "Void", 5, false, 'c', 0x9932CCFF},
            {"SD_50", "Twin-Headed Ogre", "Dark", "Cave", 5, true, 'O', 0x556B2FFF}
        };
        // Note: Full list is 80, adding representative subset first to ensure stability
        return beasts;
    }

    static const std::vector<ResourceDefinition>& GetResources() {
        static std::vector<ResourceDefinition> resources = {
            // --- Herbs ---
            {"HRB_001", "Spirit Grass", "Herb", "Plains", 1, '"', 0x90EE90FF, "Qi_Base", "Basic XP gain."},
            {"HRB_002", "Blood Ginseng", "Herb", "Forest", 1, '!', 0xFF0000FF, "Healing", "Restores HP."},
            {"HRB_003", "Silver Leaf", "Herb", "Forest", 1, '"', 0xC0C0C0FF, "Energy", "Restores Stamina."},
            {"HRB_004", "Moon Orchid", "Herb", "Cave", 2, '*', 0xE6E6FAFF, "Yin", "Cools body."},
            {"HRB_005", "Sun Flower", "Herb", "Plains", 2, '*', 0xFFA500FF, "Yang", "Warms body."},
            {"HRB_006", "Iron-Bark Root", "Herb", "Forest", 2, 'T', 0xA0522DFF, "Body", "Hardens skin."},
            {"HRB_007", "Mist Mushroom", "Herb", "Swamp", 2, 'm', 0xD8BFD8FF, "Illusion", "Causes hallucinations."},
            {"HRB_008", "Water Lotus", "Herb", "Water", 2, 'W', 0xFF69B4FF, "Water", "Water breathing pill base."},
            {"HRB_009", "Fire Petal", "Herb", "Volcano", 3, 'F', 0xFF4500FF, "Fire", "Fire resistance pill base."},
            {"HRB_010", "Thunder Fruit", "Herb", "Mountain", 3, '4', 0xFFFF00FF, "Thunder", "Numbing effect."},
            {"HRB_011", "Ice Crystal Flower", "Herb", "Snow", 3, '*', 0xE0FFFFFF, "Ice", "Preserves corpse freshness."},
            {"HRB_012", "Wind Chime Vine", "Herb", "Peak", 3, '&', 0xADD8E6FF, "Wind", "Speed pill base."},
            {"HRB_013", "Earth Potato", "Herb", "Plains", 1, 'o', 0xCD853FFF, "Food", "High satiety food."},
            {"HRB_014", "Golden Wheat", "Herb", "Plains", 1, '"', 0xFFD700FF, "Food", "Spirit grain."},
            {"HRB_015", "Seven-Color Fruit", "Herb", "Spirit Vein", 5, '@', 0xFFFFFFFF, "All_Elem", "Boosts all stats."},
            {"HRB_016", "Ghost Pepper", "Herb", "Abyss", 2, 'p', 0x800080FF, "Soul", "Burns the soul."},
            {"HRB_017", "Dragon Blood Tree Sap", "Herb", "Rare", 6, 'S', 0x8B0000FF, "Mutation", "Random positive mutation."},
            {"HRB_018", "Star Dust Pollen", "Herb", "Sky", 4, '.', 0xFFFFFFAA, "Space", "Void pill base."},
            {"HRB_019", "Corpse Flower", "Herb", "Graveyard", 3, 'X', 0x4B0082FF, "Necro", "Zombie control pill."},
            {"HRB_020", "Golden Apple", "Herb", "Rare", 5, '0', 0xFFD700FF, "Longevity", "Extends life by 10 years."},
            {"HRB_021", "Vermilion Fruit", "Herb", "Volcano", 4, '%', 0xFF4500FF, "Fire_Exp", "Massive Fire XP."},
            {"HRB_022", "Azure Root", "Herb", "Water", 4, 'r', 0x00BFFFFF, "Water_Exp", "Massive Water XP."},
            {"HRB_023", "Topaz Melon", "Herb", "Mountain", 4, 'O', 0xDAA520FF, "Earth_Exp", "Massive Earth XP."},
            {"HRB_024", "Emerald Leaf", "Herb", "Forest", 4, 'L', 0x00FF00FF, "Wood_Exp", "Massive Wood XP."},
            {"HRB_025", "Platinum Berry", "Herb", "Mine", 4, 'o', 0xE5E4E2FF, "Metal_Exp", "Massive Metal XP."},
            {"HRB_026", "Soul Severing Grass", "Herb", "Abyss", 7, '!', 0x000000FF, "Poison", "Kills soul, leaves body."},
            {"HRB_027", "Nirvana Seed", "Herb", "Phoenix Nest", 8, '0', 0xFF8C00FF, "Rebirth", "Cheat death once."},
            {"HRB_028", "Chaos Vine", "Herb", "Void", 9, '~', 0x808080FF, "Chaos", "Unknown effects."},
            {"HRB_029", "Buddha Hand Citron", "Herb", "Temple", 6, 'B', 0xFFD700FF, "Calm", "Removes Heart Demons."},
            {"HRB_030", "Demon Face Root", "Herb", "Abyss", 6, 'D', 0x8B0000FF, "Berserk", "Trigger berserk mode."},
            {"HRB_037", "Air Bubble Algae", "Herb", "Ocean", 2, 'o', 0xAFEEEEFF, "Breath", "Oxygen source underwater."},
            
            // --- Ores ---
            {"ORE_001", "Iron Ore", "Ore", "Mountain", 1, '^', 0x778899FF, "Base", "Common metal."},
            {"ORE_002", "Copper Ore", "Ore", "Mountain", 1, '^', 0xB87333FF, "Conductive", "Energy transfer."},
            {"ORE_003", "Silver Ore", "Ore", "Mountain", 2, '^', 0xC0C0C0FF, "Holy", "Harmful to undead."},
            {"ORE_004", "Gold Ore", "Ore", "Mountain", 2, '^', 0xFFD700FF, "Currency", "Trade value."},
            {"ORE_005", "Spirit Stone", "Ore", "Mountain", 1, '$', 0x00FFFFFF, "Energy", "Unrefined currency."},
            {"ORE_006", "Cold Iron", "Ore", "Snow", 2, '^', 0xB0C4DEFF, "Cold", "Retains cold."},
            {"ORE_007", "Fire Copper", "Ore", "Volcano", 2, '^', 0xD2691EFF, "Heat", "Retains heat."},
            {"ORE_008", "Star Steel", "Ore", "Impact Site", 3, '*', 0x483D8BFF, "Heavy", "Very dense."},
            {"ORE_009", "Mithril", "Ore", "Deep Mine", 4, 'M', 0xE0FFFFFF, "Magic", "High Mana flow."},
            {"ORE_010", "Adamantite", "Ore", "Deep Mine", 5, 'A', 0x00FA9AFF, "Hard", "Unbreakable."},
            {"ORE_011", "Obsidian", "Ore", "Volcano", 2, '^', 0x2F4F4FFF, "Sharp", "Natural glass edge."},
            {"ORE_012", "Crystal (Clear)", "Ore", "Cave", 1, 'x', 0xFFFFFFFF, "Storage", "Stores data/light."},
            {"ORE_013", "Ruby (Fire)", "Ore", "Volcano", 3, '+', 0xFF0000FF, "Gem_Fire", "Fire focus."},
            {"ORE_014", "Sapphire (Water)", "Ore", "Ocean", 3, '+', 0x0000FFFF, "Gem_Water", "Water focus."},
            {"ORE_015", "Emerald (Wood)", "Ore", "Jungle", 3, '+', 0x008000FF, "Gem_Wood", "Wood focus."},
            {"ORE_016", "Topaz (Earth)", "Ore", "Mountain", 3, '+', 0xDAA520FF, "Gem_Earth", "Earth focus."},
            {"ORE_017", "Amethyst (Thunder)", "Ore", "Peak", 3, '+', 0x9932CCFF, "Gem_Thunder", "Thunder focus."},
            {"ORE_018", "Onyx (Dark)", "Ore", "Abyss", 3, '+', 0x000000FF, "Gem_Dark", "Dark focus."},
            {"ORE_019", "Diamond (Light)", "Ore", "Deep Mine", 4, '+', 0xE0FFFF00, "Gem_Light", "Light focus."},
            {"ORE_020", "Soul Stone", "Ore", "Graveyard", 5, 'S', 0x4B0082FF, "Soul_Trap", "Captures souls."},
            {"ORE_021", "Blood Stone", "Ore", "Battlefield", 4, 'B', 0x8B0000FF, "Blood", "Enhances bleed."},
            {"ORE_022", "Void Crystal", "Ore", "Void", 6, 'V', 0x9400D3FF, "Space", "Spatial expansion."},
            {"ORE_023", "Time Sand", "Ore", "Rare", 8, 'T', 0xD2B48CFF, "Time", "Slows degradation."},
            {"ORE_024", "Heaven Gold", "Ore", "Sky", 9, 'H', 0xFFD700AA, "Divine", "God weapon base."},
            {"ORE_025", "Abyss Iron", "Ore", "Abyss", 6, 'I', 0x2F4F4FAA, "Cursed", "Corrupts user."},
            {"ORE_030", "Sulfur", "Ore", "Volcano", 1, '.', 0xFFFFE0FF, "Explosive", "Gunpowder base."}
        };
        return resources;
    }
    
struct ItemDefinition {
    std::string id;
    std::string name;
    std::string type; // "Weapon", "Material", "Consumable", "Artifact"
    std::string subtype; // "Sword", "Ore", "Pill"
    int tier;
    int value; // Spirit Stones
    std::string description;
    // Weapon Stats
    int attack_bonus = 0;
    int defense_bonus = 0;
    std::string effect_tag; // "Bleed", "Burn"
};

struct SkillDefinition {
    std::string id;
    std::string name;
    int tier;
    std::string element;
    int qi_cost;
    std::string description;
    std::string effect_logic; // "Projectile", "AoE", "Buff"
    int range = 1;
    std::string required_weapon_type; // "Sword", "Polearm", "Fist", "None"
};

struct CultivationDefinition {
    std::string id;
    std::string name;
    int tier;
    std::string element;
    std::string passive_effect;
    std::string description;
    std::string required_resource_id; // Item needed for breakthrough/learning
};

    static const std::vector<ItemDefinition>& GetItems() {
        static std::vector<ItemDefinition> items = {
            // --- Weapons (Swords) ---
            {"WPN_SWD_01", "Iron Wolf Sword", "Weapon", "Sword", 1, 50, "Basic sword forged with wolf claws.", 10, 0, "Bleed"},
            {"WPN_SWD_02", "Cold Iron Blade", "Weapon", "Sword", 2, 200, "Retains a chill that slows enemy Qi.", 25, 0, "Slow"},
            {"WPN_SWD_03", "Snake-Belly Jian", "Weapon", "Sword", 2, 250, "Flexible blade.", 22, 0, "Pierce"},
            {"WPN_SWD_04", "Crimson Fire Saber", "Weapon", "Sword", 2, 300, "Ignites on impact.", 28, 0, "Fire_Dmg"},
            {"WPN_SWD_05", "Swordfish Rapier", "Weapon", "Sword", 3, 400, "Penetrates heavy armor.", 35, 0, "Pierce_High"},
            {"WPN_SWD_06", "Thunder-Spark Sword", "Weapon", "Sword", 3, 450, "Shocks enemies on contact.", 30, 0, "Stun_Chance"},
            {"WPN_SWD_07", "Shadow Fang", "Weapon", "Sword", 3, 500, "Invisible in low light.", 32, 0, "Stealth_Atk"},
            {"WPN_SWD_08", "Mantis Wing Blade", "Weapon", "Sword", 7, 2000, "Releases vacuum blades.", 80, 0, "Wind_Slash"},
            {"WPN_SWD_09", "Ice-Soul Katana", "Weapon", "Sword", 4, 800, "Freezes blood.", 45, 0, "Freeze_Core"},
            {"WPN_SWD_10", "Obsidian Heavy Sword", "Weapon", "Sword", 3, 600, "Massive, heat-resistant blade.", 50, 5, "Heat_Res"},
            {"WPN_SWD_11", "Star-Fall Greatsword", "Weapon", "Sword", 4, 1000, "Uses weight rather than edge.", 60, 10, "Crushing"},
            {"WPN_SWD_12", "Void Cutter", "Weapon", "Sword", 6, 2500, "Teleports past armor.", 90, 0, "Ignore_Armor"},
            {"WPN_SWD_13", "White Tiger Slaughter Sword", "Weapon", "Sword", 10, 9999, "Radiates killing intent.", 200, 0, "Insta_Kill"},
            {"WPN_SWD_14", "Dragon-Tooth Saber", "Weapon", "Sword", 7, 3000, "Suppresses Beast-type enemies.", 110, 0, "Dragon_Fear"},
            {"WPN_SWD_15", "Phoenix Feather Jian", "Weapon", "Sword", 8, 5000, "Heals wielder on hit.", 130, 0, "Rebirth_Flame"},

            // --- Weapons (Spears) ---
            {"WPN_SPR_01", "Boar-Tusk Spear", "Weapon", "Spear", 1, 45, "Crude bone-tipped spear.", 12, 0, "Thrust"},
            {"WPN_SPR_02", "Scorpion Stinger Lance", "Weapon", "Spear", 2, 180, "Injects neurotoxin.", 20, 0, "Poison_Inj"},
            {"WPN_SPR_03", "Eagle Beak Pike", "Weapon", "Spear", 2, 220, "Punches through Spirit Shields.", 25, 0, "Armor_Pen"},
            {"WPN_SPR_04", "Lightning Rod", "Weapon", "Spear", 3, 400, "Acts as lightning rod.", 35, 0, "Chain_Light"},
            {"WPN_SPR_05", "Blood-Drinker Spear", "Weapon", "Spear", 4, 800, "Drains enemy Qi.", 45, 0, "Life_Steal"},
            {"WPN_SPR_06", "Golden Lion Spear", "Weapon", "Spear", 3, 500, "Create sonic booms.", 40, 0, "Sonic_Boom"},
            {"WPN_SPR_07", "Termite Acid Spear", "Weapon", "Spear", 3, 450, "Melts enemy weapons.", 38, 0, "Corrosion"},
            {"WPN_SPR_08", "Cryo-Wyrm Lance", "Weapon", "Spear", 7, 2500, "Creates an ice wall.", 85, 10, "Glacial_Wall"},
            {"WPN_SPR_09", "Magma Spine", "Weapon", "Spear", 4, 1000, "Drips molten lava.", 60, 0, "Splash_Dmg"},
            {"WPN_SPR_10", "Wind-Chaser Pike", "Weapon", "Spear", 2, 300, "Extremely light.", 28, 0, "Atk_Speed"},
            {"WPN_SPR_11", "Ivory God-Spear", "Weapon", "Spear", 7, 3000, "Cannot be blocked.", 100, 0, "Unstoppable"},
            {"WPN_SPR_12", "Sun-God Lance", "Weapon", "Spear", 10, 9999, "Emits blinding light.", 220, 0, "Solar_Flare"},
            {"WPN_SPR_13", "Silver Ant Pike", "Weapon", "Spear", 3, 500, "Refining tool.", 35, 0, "Refining"},
            {"WPN_SPR_14", "Bamboo Spirit Spear", "Weapon", "Spear", 2, 250, "Repairs itself.", 22, 0, "Regen"},
            {"WPN_SPR_15", "Nether-Bone Lance", "Weapon", "Spear", 7, 2800, "Raises skeletons.", 95, 0, "Necro_Raise"},

            // --- Weapons (Glaives) ---
            {"WPN_GLV_01", "Iron-Back Glaive", "Weapon", "Glaive", 1, 60, "Heavy blade for sweeping.", 15, 0, "Cleave"},
            {"WPN_GLV_02", "Shark-Fin Guandao", "Weapon", "Glaive", 3, 450, "Leave a trail of razor water.", 40, 0, "Hydro_Cut"},
            {"WPN_GLV_03", "Obsidian Cleaver", "Weapon", "Glaive", 4, 700, "High crit, low durability.", 65, 0, "Shatter"},
            {"WPN_GLV_04", "Green Dragon Crescent", "Weapon", "Glaive", 5, 1200, "Damages soul and body.", 80, 0, "Spirit_Wound"},
            {"WPN_GLV_05", "Venom-Crescent Glaive", "Weapon", "Glaive", 7, 2500, "Cloud of toxic gas.", 100, 0, "Zone_Poison"},
            {"WPN_GLV_06", "Thunder-Roc Scythe", "Weapon", "Glaive", 8, 4000, "Blinks to target.", 130, 0, "Teleport_Hit"},
            {"WPN_GLV_07", "Sand-Worm Slicer", "Weapon", "Glaive", 2, 350, "Strike enemies underground.", 30, 0, "Burrow_Strike"},
            {"WPN_GLV_08", "Vulture-Wing Glaive", "Weapon", "Glaive", 3, 500, "Wounds rot instantly.", 45, 0, "Disease"},
            {"WPN_GLV_09", "Crystal-Light Glaive", "Weapon", "Glaive", 4, 800, "Reflects spells.", 55, 10, "Reflect"},
            {"WPN_GLV_10", "Chaos-Eater Scythe", "Weapon", "Glaive", 10, 9999, "Erases map tile.", 250, 0, "Delete_Tile"},
            {"WPN_GLV_11", "Blood-Ape Moon Blade", "Weapon", "Glaive", 7, 3000, "Rage build.", 110, 0, "Rage_Build"},
            {"WPN_GLV_12", "Ogre-Bone Heavy Glaive", "Weapon", "Glaive", 4, 900, "Knockback.", 70, 0, "Knockback"},
            {"WPN_GLV_13", "Silk-Cutter", "Weapon", "Glaive", 3, 420, "Anti-Air.", 38, 0, "Anti_Air"},
            {"WPN_GLV_14", "Siren-Song Glaive", "Weapon", "Glaive", 4, 850, "Confuses enemies.", 50, 0, "Confusion"},
            {"WPN_GLV_15", "Netherfire Warblade", "Weapon", "Glaive", 7, 2700, "Burns Cultivation.", 105, 0, "Soul_Burn"},

            // --- Weapons (Halberds) ---
            {"WPN_HLB_01", "Crab-Claw Halberd", "Weapon", "Halberd", 2, 200, "Hook can disarm.", 25, 5, "Grapple"},
            {"WPN_HLB_02", "Bear-Paw Poleaxe", "Weapon", "Halberd", 3, 550, "Crush + Bleed.", 45, 0, "Maul"},
            {"WPN_HLB_03", "Turtle-Breaker", "Weapon", "Halberd", 4, 800, "Shatter formations.", 60, 0, "Shield_Break"},
            {"WPN_HLB_04", "Demon-Face Halberd", "Weapon", "Halberd", 6, 1800, "Fear aura.", 90, 0, "Fear_Aura"},
            {"WPN_HLB_05", "Thunder-Crash Halberd", "Weapon", "Halberd", 4, 900, "AoE Shock.", 70, 0, "AoE_Shock"},
            {"WPN_HLB_06", "Crocodile-Tooth Halberd", "Weapon", "Halberd", 7, 2400, "Drag into lava.", 100, 0, "Magma_Trap"},
            {"WPN_HLB_07", "Gargoyle Stone-Axe", "Weapon", "Halberd", 3, 600, "Petrify chance.", 50, 5, "Petrify"},
            {"WPN_HLB_08", "Void-Anchor Halberd", "Weapon", "Halberd", 6, 2000, "Pins enemies.", 95, 0, "Root"},
            {"WPN_HLB_09", "Time-River Halberd", "Weapon", "Halberd", 10, 9999, "Slow Time.", 210, 0, "Slow_Time"},
            {"WPN_HLB_10", "Fox-Fire Ji", "Weapon", "Halberd", 7, 2600, "Illusion hits.", 105, 0, "Illusion_Hit"},
            {"WPN_HLB_11", "Toad-Tongue Halberd", "Weapon", "Halberd", 3, 400, "Extended reach.", 35, 0, "Reach+"},
            {"WPN_HLB_12", "Mithril Crescent", "Weapon", "Halberd", 4, 1000, "Spell channel.", 65, 0, "Spell_Channel"},
            {"WPN_HLB_13", "Blood-Ginseng Axe", "Weapon", "Halberd", 2, 300, "Self Heal.", 30, 0, "Self_Heal"},
            {"WPN_HLB_14", "Abyssal Anchor", "Weapon", "Halberd", 5, 1400, "Gravity Up.", 85, 0, "Gravity_Up"},
            {"WPN_HLB_15", "World-Shell Breaker", "Weapon", "Halberd", 10, 9999, "Indestructible.", 230, 100, "Indestructible"},
            
            // --- Beast Cores ---
            {"CORE_001", "Iron-Back Wolf Core", "Consumable", "Core", 1, 100, "Metal essence.", 0, 0, "Metal_Qi"},
            {"CORE_002", "Wind Leopard Core", "Consumable", "Core", 2, 200, "Speed essence.", 0, 0, "Speed_Qi"},
            {"CORE_003", "Boar Earth Core", "Consumable", "Core", 1, 100, "Defense essence.", 0, 0, "Defense_Qi"},
            {"CORE_004", "Flame Lizard Core", "Consumable", "Core", 1, 100, "Fire essence.", 0, 0, "Fire_Qi"},
            {"CORE_005", "Venom Spider Core", "Consumable", "Core", 1, 100, "Poison essence.", 0, 0, "Poison_Qi"},
            {"CORE_006", "Python Wood Core", "Consumable", "Core", 2, 200, "Wood essence.", 0, 0, "Wood_Qi"},
            {"CORE_007", "Lion Golden Core", "Consumable", "Core", 3, 500, "Sonic essence.", 0, 0, "Sonic_Qi"},
            {"CORE_008", "Shadow Cat Core", "Consumable", "Core", 2, 200, "Dark essence.", 0, 0, "Dark_Qi"},
            {"CORE_009", "Bear Vitality Core", "Consumable", "Core", 3, 500, "Life essence.", 0, 0, "Life_Qi"},
            {"CORE_010", "Scorpion Earth Core", "Consumable", "Core", 2, 200, "Earth Toxin.", 0, 0, "Earth_Toxin"},
            {"CORE_011", "Eagle Fire Core", "Consumable", "Core", 2, 200, "Yang essence.", 0, 0, "Yang_Qi"},
            {"CORE_012", "Sable Lightning Core", "Consumable", "Core", 4, 1000, "Thunder essence.", 0, 0, "Thunder_Qi"},
            {"CORE_013", "Monkey Soul Core", "Consumable", "Core", 3, 500, "Soul essence.", 0, 0, "Soul_Qi"},
            {"CORE_014", "Toad Illusion Core", "Consumable", "Core", 2, 200, "Mind essence.", 0, 0, "Mind_Qi"},
            {"CORE_015", "Eel Thunder Core", "Consumable", "Core", 3, 500, "Shock essence.", 0, 0, "Shock_Qi"},
            {"CORE_016", "Squid Ink Core", "Consumable", "Core", 2, 200, "Blind essence.", 0, 0, "Blind_Qi"},
            {"CORE_017", "Angler Lure Core", "Consumable", "Core", 4, 1000, "Light essence.", 0, 0, "Light_Qi"},
            {"CORE_018", "Shark Frost Core", "Consumable", "Core", 3, 500, "Ice essence.", 0, 0, "Ice_Qi"},
            {"CORE_019", "Beetle Magma Core", "Consumable", "Core", 3, 500, "Lava essence.", 0, 0, "Lava_Qi"},
            {"CORE_020", "Bat Sonic Core", "Consumable", "Core", 2, 200, "Echo essence.", 0, 0, "Echo_Qi"},
            {"CORE_021", "Wisp Spirit Core", "Consumable", "Core", 4, 1000, "Ghost essence.", 0, 0, "Ghost_Qi"},
            {"CORE_022", "Obsidian Heart Core", "Consumable", "Core", 5, 2000, "Magma essence.", 0, 0, "Magma_Qi"},
            {"CORE_023", "Goat Thunder Core", "Consumable", "Core", 3, 500, "Cloud essence.", 0, 0, "Cloud_Qi"},
            {"CORE_024", "Mosquito Blood Core", "Consumable", "Core", 2, 200, "Blood essence.", 0, 0, "Blood_Qi"},
            {"CORE_025", "Deer Jade Core", "Consumable", "Core", 4, 1000, "Heal essence.", 0, 0, "Heal_Qi"},
            {"CORE_026", "Bear Glacial Core", "Consumable", "Core", 4, 1000, "Frost essence.", 0, 0, "Frost_Qi"},
            {"CORE_027", "Chameleon Void Core", "Consumable", "Core", 5, 2000, "Space essence.", 0, 0, "Space_Qi"},
            {"CORE_028", "Sprite Solar Core", "Consumable", "Core", 5, 2000, "Sun essence.", 0, 0, "Sun_Qi"},
            {"CORE_029", "Moth Dream Core", "Consumable", "Core", 4, 1000, "Sleep essence.", 0, 0, "Sleep_Qi"},
            {"CORE_030", "Ogre Chaos Core", "Consumable", "Core", 5, 2000, "Chaos essence.", 0, 0, "Chaos_Qi"},

            // --- Materials ---
            {"MAT_001", "Iron Claw", "Material", "BeastPart", 1, 10, "Sharp metal claw.", 0, 0, "Weapon_Edge"},
            {"MAT_002", "Agility Tendon", "Material", "BeastPart", 2, 20, "Elastic tendon.", 0, 0, "Bow_String"},
            {"MAT_003", "Rock Boar Skin", "Material", "BeastPart", 1, 15, "Tough hide.", 0, 0, "Armor_Leather"},
            {"MAT_004", "Flame Sac", "Material", "Organ", 1, 50, "Burns when exposed to air.", 0, 0, "Alch_Fire"},
            {"MAT_005", "Venom Gland", "Material", "Organ", 1, 50, "Potent neurotoxin.", 0, 0, "Alch_Poison"},
            {"MAT_006", "Rat Whiskers", "Material", "BeastPart", 2, 30, "Used in dowsing.", 0, 0, "Divination"},
            {"MAT_007", "Python Scale", "Material", "BeastPart", 2, 40, "Flexible armor.", 0, 0, "Armor_Flex"},
            {"MAT_008", "Lion Mane Fur", "Material", "BeastPart", 3, 60, "Talisman material.", 0, 0, "Talisman_Ink"},
            {"MAT_009", "Shadow Fur", "Material", "BeastPart", 2, 40, "Stealth material.", 0, 0, "Cloak_Mat"},
            {"MAT_010", "Bear Gallbladder", "Material", "Organ", 3, 100, "Healing ingredient.", 0, 0, "Alch_Vitality"},
            {"MAT_014", "Ghost Skull", "Material", "Bone", 3, 80, "Retains soul energy.", 0, 0, "Artifact_Base"},
            {"MAT_017", "Crab Chitin", "Material", "Shell", 2, 45, "Hard shield material.", 0, 0, "Shield_Mat"},
            {"MAT_024", "Leech Blood Sac", "Material", "Organ", 2, 35, "Blood essence.", 0, 0, "Alch_Blood"},
            {"MAT_028", "Gargoyle Stone", "Material", "Stone", 3, 120, "Living stone.", 0, 0, "Golem_Core"},
            {"MAT_039", "Void Scale", "Material", "Scale", 5, 500, "Teleportation material.", 0, 0, "Talisman_Void"},
            
            // --- God Tier Drops ---
            {"GOD_001", "Chaos Heart", "Artifact", "God", 10, 99999, "World Ender.", 0, 0, "World_Ender"},
            {"GOD_002", "Phoenix Fire Essence", "Artifact", "God", 10, 99999, "Grants Rebirth.", 0, 0, "Immortality"},
            {"GOD_003", "World-Shell Fragment", "Artifact", "God", 10, 99999, "Invincible Armor.", 0, 0, "Invincible"},
            {"GOD_004", "Devour Core", "Artifact", "God", 10, 99999, "Infinite Qi.", 0, 0, "Infinite_Qi"},
            {"GOD_005", "Killing Law Fragment", "Artifact", "God", 10, 99999, "Insta Kill Chance.", 0, 0, "Insta_Kill"},
            {"GOD_006", "Blessing Elixir", "Artifact", "God", 10, 99999, "Max Luck.", 0, 0, "Luck_Max"},
            {"GOD_007", "Sunflame Core", "Artifact", "God", 10, 99999, "Sun God.", 0, 0, "Sun_God"},
            {"GOD_008", "Soul Guide Silk", "Artifact", "God", 10, 99999, "Dream Walk.", 0, 0, "Dream_Walk"},
            {"GOD_009", "Time-River Scale", "Artifact", "God", 10, 99999, "Save Scum.", 0, 0, "Save_Scum"},
            {"GOD_010", "Wisdom Essence", "Artifact", "God", 10, 99999, "Skill Master.", 0, 0, "Skill_Master"},
            
            // --- Ores (Added as Items for Inventory) ---
            {"ORE_001", "Iron Ore", "Material", "Ore", 1, 5, "Common metal.", 0, 0, "Base"},
            {"ORE_002", "Copper Ore", "Material", "Ore", 1, 5, "Conductive.", 0, 0, "Conductive"},
            {"ORE_003", "Silver Ore", "Material", "Ore", 2, 10, "Holy.", 0, 0, "Holy"},
            {"ORE_004", "Gold Ore", "Material", "Ore", 2, 50, "Currency.", 0, 0, "Currency"},
            {"ORE_005", "Spirit Stone", "Material", "Ore", 1, 1, "Unrefined currency.", 0, 0, "Energy"},
            {"ORE_008", "Star Steel", "Material", "Ore", 3, 200, "Very dense.", 0, 0, "Heavy"},
            {"ORE_009", "Mithril", "Material", "Ore", 4, 500, "High Mana flow.", 0, 0, "Magic"},
            {"ORE_010", "Adamantite", "Material", "Ore", 5, 1000, "Unbreakable.", 0, 0, "Hard"},
            
            // --- Secret Manuals ---
            {"MAN_001", "Nine-Revolutions Rebirth", "Manual", "Scripture", 9, 5000, "Allows user to craft CSM_013 (Nirvana Pill).", 0, 0, "Recipe"},
            {"MAN_002", "Heaven-Swallowing Art", "Manual", "Scripture", 10, 8000, "Allows user to eat ORE and WPN items for XP.", 0, 0, "Passive"},
            {"MAN_003", "Puppet Master Scripture", "Manual", "Scripture", 8, 3000, "Unlocks Crafting: Bone Golem.", 0, 0, "Recipe"},
            {"MAN_004", "Myriad-Poison Manual", "Manual", "Scripture", 8, 3500, "Immunity to toxins.", 0, 0, "Passive"},
            {"MAN_005", "Star-Fall Sword Array", "Manual", "SkillBook", 9, 6000, "Teaches a skill to control 7 Flying Swords.", 0, 0, "LearnSkill"},

            // --- Consumables ---
            {"CSM_001", "Spirit Recovery Pill", "Consumable", "Pill", 1, 20, "Restores 50 Qi.", 0, 0, "HealQi"},
            {"CSM_002", "Blood Healing Pill", "Consumable", "Pill", 1, 25, "Restores 50 HP.", 0, 0, "HealHP"}
        };
        return items;
    }

    static const std::vector<SkillDefinition>& GetSkills() {
        static std::vector<SkillDefinition> skills = {
            // Sword Skills
            {"SKL_SWD_01", "Iron Cleave", 1, "Metal", 5, "Simple downward slash.", "Damage", 1, "Sword"},
            {"SKL_SWD_02", "Flowing Water Strike", 2, "Water", 8, "Blocks next attack then counters.", "Parry", 1, "Sword"},
            {"SKL_SWD_03", "Fire-Raven Slash", 3, "Fire", 15, "Shoots a bird of fire.", "Projectile", 3, "Sword"},
            {"SKL_SWD_04", "Vacuum Blade", 5, "Wind", 20, "Cuts through air and armor.", "Ignore_Def", 5, "Sword"},
            {"SKL_SWD_05", "Thousand Sword Rain", 7, "Metal", 100, "AoE nuke around player.", "AoE_Circle", 5, "Sword"},
            {"SKL_SWD_06", "Killing Intent Slash", 10, "Death", 999, "Instant death if low HP.", "Execute", 1, "Sword"},
            
            // Polearm Skills
            {"SKL_POL_01", "Boar Rush", 1, "Earth", 10, "Charge and stun.", "Charge", 2, "Polearm"},
            {"SKL_POL_02", "Sweeping River", 2, "Water", 15, "Hits all enemies in front.", "AoE_Cone", 2, "Polearm"},
            {"SKL_POL_03", "Viper Thrust", 3, "Poison", 10, "Applies poison.", "DoT", 2, "Polearm"},
            {"SKL_POL_04", "Dragon Drill", 5, "Metal", 30, "Ignores all Shield/Defense stats.", "Pierce", 2, "Polearm"},
            {"SKL_POL_05", "Thunder-God Slam", 6, "Thunder", 50, "Massive AoE stun.", "AoE_Stun", 3, "Polearm"},
            {"SKL_POL_06", "Void Anchor", 7, "Space", 40, "Prevents enemy movement.", "Root", 4, "Polearm"},
            
            // Fist/Body
            {"SKL_FST_01", "Stone Breaking Fist", 1, "Earth", 5, "Bonus damage to structure.", "Damage", 1, "Fist"},
            {"SKL_FST_02", "White Tiger Claw", 3, "Metal", 15, "Rips armor.", "Bleed", 1, "Fist"},
            {"SKL_FST_03", "Seismic Stomp", 4, "Earth", 20, "Grounds flying enemies.", "Knockdown", 1, "Fist"},
            {"SKL_FST_04", "Heart-Stopping Palm", 6, "Yin", 30, "Internal damage.", "Bypass_Armor", 1, "Fist"},
            {"SKL_FST_05", "Devour Essence", 8, "Void", 50, "Lifesteal.", "Heal", 1, "Fist"},
            
            // Movement/Utility
            {"SKL_MOV_01", "Cloud Step", 2, "Air", 5, "Hop over obstacles.", "Jump", 3, "None"},
            {"SKL_MOV_02", "Shadow Meld", 3, "Dark", 10, "Invisibility until attack.", "Stealth", 1, "None"},
            {"SKL_MOV_03", "Blood Escape", 5, "Blood", 0, "Emergency flee. Costs HP.", "Teleport", 50, "None"},
            {"SKL_UTL_01", "Spirit Eye", 1, "Mind", 1, "See Qi density/Herbs.", "Reveal_Map", 10, "None"},
            {"SKL_UTL_02", "Soul Search", 6, "Soul", 50, "Extract map data.", "Get_Info", 1, "None"},
            
            // Magic/Spells
            {"SKL_MAG_01", "Fireball", 1, "Fire", 5, "Basic ranged attack.", "Projectile", 4, "None"},
            {"SKL_MAG_02", "Earth Wall", 2, "Earth", 10, "Blocks movement.", "SummonObject", 2, "None"},
            {"SKL_MAG_03", "Lightning Chain", 4, "Thunder", 25, "Hits 3 targets.", "Chain", 4, "None"},
            {"SKL_MAG_04", "Summon Golem", 5, "Earth", 40, "Summons Gargoyle ally.", "Summon", 1, "None"},
            {"SKL_MAG_05", "Blizzard Domain", 7, "Ice", 50, "Changes local biome to Ice.", "Environment", 10, "None"}
        };
        return skills;
    }

    static const std::vector<CultivationDefinition>& GetCultivationMethods() {
        static std::vector<CultivationDefinition> methods = {
            {"MTH_GEN_01", "Basic Breathing Scripture", 1, "Balance", "Qi_Regen += 0.5", "Base method for mortals.", "HRB_001"},
            {"MTH_BDY_01", "Iron-Skin Manual", 1, "Body", "Def +10, Spd -2", "Hardens skin.", "HRB_006"},
            {"MTH_FIRE_01", "Raging Flame Mantra", 2, "Fire", "Fire_Dmg +10%", "Body temperature rises. Immune to Cold.", "CORE_004"},
            {"MTH_WTR_01", "Tranquil River Sutra", 2, "Water", "Healing_Eff +20%", "Qi acts as a passive heal over time.", "HRB_008"},
            {"MTH_SWD_01", "Azure Sword Heart", 3, "Attack", "Sword_Mastery +15%", "Learning speed for Sword Skills doubled.", "WPN_SWD_01"},
            {"MTH_PSN_01", "Five-Venom Art", 2, "Poison", "Psn_Resist += 100%", "Body blood becomes toxic to attackers.", "CORE_005"},
            {"MTH_SPD_01", "Wind-Chaser Steps", 2, "Speed", "Move_Speed +20%", "Can traverse 2 tiles per tick.", "CORE_002"},
            {"MTH_ICE_01", "Glacial Bone Secret", 4, "Ice", "Chill_Aura = True", "Enemies within 1 tile slowed by 10%.", "CORE_026"},
            {"MTH_LGT_01", "Thunder-Forging Art", 4, "Thunder", "Crit_Rate += 10%", "Qi attacks have chance to paralyze self.", "CORE_012"},
            {"MTH_BLD_01", "Blood Devil Scripture", 5, "Forbidden", "Life_Steal += 5%", "Drains HP on hit. Lowers Reputation daily.", "MAT_024"},
            {"MTH_MND_01", "Fox-Spirit Charm", 5, "Illusion", "Charisma += 50", "NPC aggression range reduced by 50%.", "MAT_050"},
            {"MTH_DRG_01", "Dragon-Transformation", 7, "Body", "Str *= 2.0", "Can equip Heavy Weapons without penalty.", "MAT_048"},
            {"MTH_VOID_01", "Void-Step Canon", 6, "Space", "Dodge +25%", "Phasing in and out of reality.", "CORE_027"},
            {"MTH_NEC_01", "Yin-Corpse Way", 4, "Necro", "Minion_Cap += 3", "Can control raised skeletons.", "MAT_014"},
            {"MTH_GLD_01", "Indestructible Golden Body", 7, "Metal", "Dmg_Reduction = 50%", "Skin reflects metal weapons.", "SB_07"},
            {"MTH_SUN_01", "Solar God Manual", 9, "Yang", "AoE_Burn = True", "Passive heat damage to all adjacent tiles.", "CORE_028"},
            {"MTH_TIME_01", "River of Time Flow", 10, "Time", "Lifespan = Infinite", "Character stops aging.", "GOD_009"},
            {"MTH_CHS_01", "Primordial Chaos Sutra", 10, "Chaos", "All_Stats *= 1.5", "Can consume ANY item for XP.", "GOD_001"},
            {"MTH_DREAM_01", "Eternal Dream Art", 8, "Soul", "Sleep_Cultivation = True", "Gains XP while sleeping/offline.", "CORE_029"},
            {"MTH_STR_01", "Star-Constellation Body", 5, "Cosmic", "Weight += 1000kg", "Cannot be knocked back.", "ORE_008"}
        };
        return methods;
    }
    static const std::vector<EventDefinition>& GetEvents() {
        static std::vector<EventDefinition> events = {
            // POIs (Static)
            {1, "The Sword Tomb", "POI", "Graveyard of rusted swords. High Metal Qi.", "Metal", "WPN_SWD_01", "SD_38", "Spawn_Loot"},
            {2, "Dragon-Fall Ridge", "POI", "Mountain shaped like a spine.", "Mountain", "ORE_008", "", "Spawn_Node"},
            {6, "The Ant Hive", "POI", "Desert mesa with infinite ants.", "Desert", "MAT_031", "SD_36", "Dungeon"},
            {10, "The Floating Pavilion", "POI", "Structure in Sky Layer.", "Sky", "ORE_024", "RACE_08", "Trader"},
            
            // World Events (Dynamic)
            {26, "The Beast Tide", "Global", "Swarm of Wolves moves to nearest city.", "Any", "", "SD_01", "Invasion"},
            {30, "Spirit Vein Eruption", "Global", "Random tile gains High Qi.", "Any", "", "", "Spawn_Vein"},
            
            // Encounters (Random)
            {66, "The Arrogant Young Master", "Encounter", "Demands you give up your seat.", "City", "", "RACE_01", "Duel"},
            {67, "Bandit Ambush", "Encounter", "5 Rogues demand toll.", "Road", "", "RACE_01", "Combat"},
            {69, "Injured Beast", "Encounter", "Deer caught in trap.", "Forest", "MAT_036", "SD_43", "Choice"},
            {70, "Mysterious Merchant", "Encounter", "Sells items at 2x price.", "Any", "CORE_010", "", "Shop"},
            {74, "Wild Beast Pack", "Encounter", "5 Wolves encircle you.", "Wild", "", "SD_01", "Combat"},
            
            // Hazards
            {92, "Poison Mist", "Hazard", "Found in Swamp. Applies Toxic.", "Swamp", "", "", "Debuff:Toxic"},
            {93, "Razor Grass", "Hazard", "Deals Bleed dmg if moving.", "Plains", "", "", "Damage:Bleed"}
        };
        return events;
    }
};
