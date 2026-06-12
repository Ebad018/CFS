#pragma once
#include <map>
#include <string>

// --- Skill metadata & progression curve ---
// Lives in the core so every front-end shows the same truthful tooltips.

struct SkillMeta {
    std::string description;      // What the skill represents
    std::string effect_per_level; // What each level actually does in the sim
    std::string how_to_train;     // How XP is earned
};

inline int SkillXpToNext(int level) { return 50 + level * 25; }
constexpr int SKILL_CAP = 20;

inline const std::map<std::string, SkillMeta>& SkillMetadata() {
    static const std::map<std::string, SkillMeta> meta = {
        {"Farming",
         {"Working soil, planting and harvesting crops.",
          "Farmers produce +0.5 food/day per level (when working a Farm).",
          "Assigned farm labor earns XP daily."}},
        {"Food Gathering",
         {"Finding edible plants, roots and wild herbs.",
          "Foragers: +1 food/day per 3 levels. Gather missions return more herbs.",
          "Forage labor or Gather missions."}},
        {"Hunting",
         {"Tracking, trapping and butchering wild beasts.",
          "Hunt missions: +3 food per level, better hides and materials.",
          "Go on Hunt missions."}},
        {"Construction",
         {"Carpentry, masonry and raising buildings.",
          "Builders: +25% build speed per level. Woodcutters: +1 wood per 4 levels.",
          "Build labor or woodcutting."}},
        {"Forging",
         {"Mining, smelting and metalwork.",
          "Miners: +1 stone/day per 4 levels. Feeds the Blacksmith.",
          "Mine labor."}},
        {"Healing",
         {"Medicine, bonesetting and qi-infused treatment.",
          "Healers make the whole town's wounded recover +4% faster per level.",
          "Tend the wounded as assigned Heal labor."}},
        {"Warfare",
         {"Tactics, discipline and battle-hardiness.",
          "+1 defense per level, +2% hit chance. Guards add it to town defense.",
          "Guard duty, raids, invasions and defending the town."}},
        {"Weapons Mastery",
         {"Pure skill at arms - blade, spear and fist.",
          "+2 attack per level, +4% hit chance.",
          "Guard duty (faster with a Barracks) and combat."}},
        {"Cultivation",
         {"Refining qi and tempering the spirit.",
          "+5% qi gained per level while cultivating.",
          "Train as a Cultivator (idle disciplined people do this on their own)."}},
        {"Scouting",
         {"Pathfinding, foraging routes and reading the wilds.",
          "Adventure missions: fewer ambushes, richer finds.",
          "Go on Adventure missions."}},
        {"Alchemy", {"Brewing pills and potions from herbs.", "(No effect yet - planned.)", "(Not yet trainable.)"}},
        {"Stealth", {"Moving unseen.", "(No effect yet - planned.)", "(Not yet trainable.)"}},
        {"Intellect", {"Memory, logic and learning.", "(No effect yet - planned.)", "(Not yet trainable.)"}},
        {"Formations", {"Arrays and barrier formations.", "(No effect yet - planned.)", "(Not yet trainable.)"}},
        {"Scribing", {"Records, talismans and calligraphy.", "(No effect yet - planned.)", "(Not yet trainable.)"}},
    };
    return meta;
}

// Labor categories the task system can assign, and the skill that governs each.
inline const std::map<std::string, std::string>& LaborSkillMap() {
    static const std::map<std::string, std::string> m = {
        {"Farm", "Farming"},   {"Forage", "Food Gathering"}, {"Woodcut", "Construction"},
        {"Mine", "Forging"},   {"Build", "Construction"},    {"Guard", "Warfare"},
        {"Heal", "Healing"},
    };
    return m;
}

// The skill that matters most for each mission type (used for party sorting).
inline std::string MissionRelevantSkill(const std::string& type) {
    if (type == "Hunt") return "Hunting";
    if (type == "Gather") return "Food Gathering";
    if (type == "Adventure") return "Scouting";
    return "Warfare"; // Raid / Invade
}
