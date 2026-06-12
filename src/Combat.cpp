#include "Combat.h"
#include <random>
#include <algorithm>

namespace {
    int RandInt(int min, int max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(min, max);
        return dis(gen);
    }
    float RandFloat() { return (float)RandInt(0, 1000) / 1000.0f; }
}

void Combat::Strike(Combatant& attacker, Combatant& defender, std::vector<std::string>& log) {
    if (!attacker.body || !defender.body) return;

    // Capability checks: blind fighters miss, crippled fighters are slow
    float a_sight = Body::Capacity(*attacker.body, BodyFunc::Sight);
    float a_stand = Body::Capacity(*attacker.body, BodyFunc::Stand);
    float d_stand = Body::Capacity(*defender.body, BodyFunc::Stand);

    float hit_chance = 0.55f * attacker.skill * (0.3f + 0.7f * a_sight) * (0.5f + 0.5f * a_stand);
    float dodge = 0.25f * defender.skill * (0.4f + 0.6f * d_stand);
    if (RandFloat() < dodge || RandFloat() > hit_chance + 0.25f) {
        log.push_back("  " + attacker.name + " strikes at " + defender.name + ", but " + defender.name + " evades.");
        return;
    }

    int part = Body::PickHitPart(*defender.body);
    if (part < 0) return;

    int raw = attacker.attack + RandInt(-attacker.attack / 4, attacker.attack / 4);
    int dmg = std::max(1, raw - defender.defense / 2);

    // Piercing weapons can punch through into internal organs
    std::string dtype = attacker.damage_type;
    if (dtype == "pierce" && RandInt(1, 100) <= 30) {
        int deep = Body::PickHitPart(*defender.body, true);
        if (deep >= 0) part = deep;
    }

    WoundResult w = Body::ApplyDamage(*defender.body, part, dmg, dtype, defender.name);
    log.push_back("  " + attacker.name + " hits " + defender.name + ": " + w.text);

    if (w.fatal || Body::IsDead(*defender.body)) {
        defender.dead = true;
        log.push_back("  ** " + defender.name + " has been slain by " + attacker.name + "! **");
    } else if (defender.body->pain > 85.0f && RandInt(1, 100) <= 40) {
        defender.fled = true;
        log.push_back("  " + defender.name + " collapses in agony and is out of the fight!");
    }
}

CombatResult Combat::Fight(std::vector<Combatant>& side_a, std::vector<Combatant>& side_b,
                           float morale_break, int max_rounds) {
    CombatResult res;
    auto active = [](std::vector<Combatant>& side) {
        std::vector<Combatant*> out;
        for (auto& c : side) if (!c.dead && !c.fled) out.push_back(&c);
        return out;
    };
    auto deaths = [](std::vector<Combatant>& side) {
        int n = 0;
        for (auto& c : side) if (c.dead) n++;
        return n;
    };

    for (int round = 1; round <= max_rounds; ++round) {
        auto a = active(side_a);
        auto b = active(side_b);
        if (a.empty() || b.empty()) break;

        // Morale: a side routs after too many losses
        if (!side_a.empty() && (float)deaths(side_a) / (float)side_a.size() >= morale_break) {
            for (auto* c : a) c->fled = true;
            res.log.push_back("  The attackers' morale breaks - the survivors flee!");
            break;
        }
        if (!side_b.empty() && (float)deaths(side_b) / (float)side_b.size() >= morale_break) {
            for (auto* c : b) c->fled = true;
            res.log.push_back("  The defenders' morale breaks - the survivors flee!");
            break;
        }

        // Everyone still standing takes a swing at a random foe
        for (auto* atk : a) {
            if (atk->dead || atk->fled) continue;
            auto targets = active(side_b);
            if (targets.empty()) break;
            Strike(*atk, *targets[RandInt(0, (int)targets.size() - 1)], res.log);
        }
        for (auto* atk : b) {
            if (atk->dead || atk->fled) continue;
            auto targets = active(side_a);
            if (targets.empty()) break;
            Strike(*atk, *targets[RandInt(0, (int)targets.size() - 1)], res.log);
        }
    }

    res.a_deaths = deaths(side_a);
    res.b_deaths = deaths(side_b);
    res.side_a_won = !active(side_a).empty() && active(side_b).empty();
    if (active(side_a).empty() && active(side_b).empty()) res.side_a_won = false;
    return res;
}
