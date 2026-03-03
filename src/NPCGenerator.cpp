#include "NPCGenerator.h"
#include <fstream>
#include <random>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

// --- RNG Helpers ---
static int RandInt(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

static std::string PickOne(const std::vector<std::string>& list) {
    if (list.empty()) return "None";
    return list[RandInt(0, list.size() - 1)];
}

// --- Generator ---

NPC NPCGenerator::Generate(const NPCGenContext& ctx) {
    NPC npc;
    npc.uid = "NPC_" + std::to_string(RandInt(100000, 999999));
    
    RollIdentity(npc, ctx);
    RollBiology(npc, ctx);
    RollCultivation(npc, ctx);
    RollSocial(npc, ctx);
    RollAI(npc, ctx);

    return npc;
}

void NPCGenerator::SaveNPC(const NPC& npc, const std::string& directory) {
    if (!fs::exists(directory)) {
        fs::create_directories(directory);
    }
    std::string path = directory + "/" + npc.uid + ".json";
    json j = npc;
    std::ofstream file(path);
    if (file.is_open()) {
        file << j.dump(4);
    }
}

NPC NPCGenerator::LoadNPC(const std::string& filepath) {
    std::ifstream file(filepath);
    json j;
    if (file.is_open()) {
        file >> j;
    }
    NPC npc = j;
    return npc;
}

// --- Roll Logic ---

void NPCGenerator::RollIdentity(NPC& npc, const NPCGenContext& ctx) {
    // Determine Gender
    npc.identity.gender = (RandInt(0, 1) == 0) ? "Male" : "Female";
    
    // Simple Name Pools (Placeholder)
    std::vector<std::string> surnames = {"Ye", "Li", "Wang", "Zhang", "Chen", "Liu", "Yang", "Huang", "Zhao", "Wu"};
    std::vector<std::string> male_names = {"Chen", "Feng", "Wei", "Jian", "Lei", "Huo", "Gang", "Ming", "Tao"};
    std::vector<std::string> female_names = {"Yan", "Ling", "Xue", "Yue", "Hua", "Lan", "Qing", "Mei", "Xin"};
    
    npc.identity.surname = PickOne(surnames);
    npc.identity.given_name = (npc.identity.gender == "Male") ? PickOne(male_names) : PickOne(female_names);
    
    npc.identity.age = 16 + RandInt(0, 50); // Base age
    npc.identity.lifespan = 80 + RandInt(0, 40);
    
    // Calculate Birth Date relative to Year 1
    int currentYear = 1;
    npc.identity.birth_year = currentYear - npc.identity.age;
    npc.identity.birth_month = RandInt(1, 12);
    npc.identity.birth_day = RandInt(1, 30);
}

void NPCGenerator::RollBiology(NPC& npc, const NPCGenContext& ctx) {
    if (!ctx.force_race.empty()) {
        npc.biology.race_id = ctx.force_race;
    } else {
        // Weighted roll
        int roll = RandInt(0, 100);
        if (roll < 80) npc.biology.race_id = "RACE_01"; // Human
        else npc.biology.race_id = "RACE_02"; // Phoenix Scion (Placeholder)
    }

    npc.biology.strength_mod = 1.0f + (RandInt(-20, 20) / 100.0f);
    npc.biology.fire_resist = 0.0f;
    if (npc.biology.race_id == "RACE_02") {
        npc.biology.fire_resist = 100.0f;
        npc.biology.mutations.push_back("BURNING_BLOOD");
    }
}

void NPCGenerator::RollCultivation(NPC& npc, const NPCGenContext& ctx) {
    // Realm
    int realm_idx = ctx.min_realm_idx + RandInt(0, 1);
    const char* realms[] = {"Mortal", "Qi Condensation", "Foundation", "Golden Core"};
    if (realm_idx > 3) realm_idx = 3;
    
    npc.cultivation.realm.major = realms[realm_idx];
    npc.cultivation.realm.minor = RandInt(1, 9);
    npc.cultivation.realm.purity = RandInt(50, 100) / 100.0f;
    
    npc.cultivation.qi_max = 100 * (realm_idx + 1) * 10;
    npc.cultivation.qi_current = npc.cultivation.qi_max;

    // Element
    if (!ctx.force_element.empty()) {
        npc.cultivation.attribute = ctx.force_element;
    } else {
        std::vector<std::string> elements = {"Fire", "Water", "Wood", "Metal", "Earth"};
        npc.cultivation.attribute = PickOne(elements);
    }
    
    npc.cultivation.spirit_root_type = "Common " + npc.cultivation.attribute;
}

void NPCGenerator::RollSocial(NPC& npc, const NPCGenContext& ctx) {
    npc.social.sect_id = ctx.sect_id;
    npc.social.rank = ctx.role;
    npc.social.loyalty = 50 + RandInt(-20, 20);
    npc.social.contribution_points = RandInt(0, 500);
}

void NPCGenerator::RollAI(NPC& npc, const NPCGenContext& ctx) {
    // Assign starting goal based on role
    if (ctx.role == "DISCIPLE") {
        npc.ai_state.active_goal.type = "CULTIVATE";
        npc.ai_state.active_goal.priority = 10;
        npc.ai_state.active_goal.state = "MEDITATING";
    } else {
        npc.ai_state.active_goal.type = "WANDER";
        npc.ai_state.active_goal.priority = 1;
    }

    // Roll Personality
    npc.personality.courage = RandInt(0, 100);
    npc.personality.greed = RandInt(0, 100);
    npc.personality.honor = RandInt(0, 100);
    npc.personality.loyalty = 50 + RandInt(-20, 20); // Skewed by sect
    npc.personality.curiosity = RandInt(0, 100);
    npc.personality.temper = RandInt(0, 100);
    npc.personality.compassion = RandInt(0, 100);
    npc.personality.ambition = RandInt(0, 100);
    npc.personality.diligence = RandInt(0, 100);
    npc.personality.pride = RandInt(0, 100);
    
    // Default current goal text (Flavor)
    npc.personality.current_goal = "Meditate"; 
}
