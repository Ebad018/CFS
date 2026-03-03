#pragma once
#include "NPC.h"
#include <string>

struct NPCGenContext {
    std::string sect_id = "NONE";
    std::string role = "ROGUE"; // "Disciple", "Elder", "Rogue"
    
    // Constraints
    std::string force_race = "";    // If empty, random based on conditions
    std::string force_element = ""; // If empty, random
    int min_realm_idx = 0;          // 0=Mortal, 1=Qi Condensation
};

class NPCGenerator {
public:
    // Factory Method
    static NPC Generate(const NPCGenContext& ctx);

    // Persistence
    static void SaveNPC(const NPC& npc, const std::string& directory);
    static NPC LoadNPC(const std::string& filepath);

private:
    // Helpers
    static void RollIdentity(NPC& npc, const NPCGenContext& ctx);
    static void RollBiology(NPC& npc, const NPCGenContext& ctx);
    static void RollCultivation(NPC& npc, const NPCGenContext& ctx);
    static void RollSocial(NPC& npc, const NPCGenContext& ctx);
    static void RollAI(NPC& npc, const NPCGenContext& ctx);
};
