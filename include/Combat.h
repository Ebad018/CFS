#pragma once
#include "Body.h"
#include <string>
#include <vector>

// --- Body-part targeted combat resolution ---
// Used for beast attacks, missions, duels and invasions.
// Fully decoupled from any UI: results come back as log lines.

struct Combatant {
    int entity_id = -1;          // -1 for ad-hoc creatures (wild beasts, enemy soldiers)
    std::string name = "Unknown";
    BodyComponent* body = nullptr; // Must outlive the fight
    int attack = 10;             // Weapon + stats
    int defense = 0;             // Armor + toughness
    float skill = 1.0f;          // Hit/dodge modifier (Weapons Mastery, Warfare...)
    std::string damage_type = "slash"; // "slash", "blunt", "pierce"
    bool dead = false;
    bool fled = false;
};

struct CombatResult {
    std::vector<std::string> log;
    bool side_a_won = false;
    int a_deaths = 0;
    int b_deaths = 0;
};

class Combat {
public:
    // Pitched fight between two groups. Mutates the bodies in place.
    // morale_break: fraction of casualties at which a side routs (1.0 = fight to the death).
    static CombatResult Fight(std::vector<Combatant>& side_a, std::vector<Combatant>& side_b,
                              float morale_break = 0.6f, int max_rounds = 60);

    // One attack swing from attacker onto defender. Appends to log.
    static void Strike(Combatant& attacker, Combatant& defender, std::vector<std::string>& log);
};
