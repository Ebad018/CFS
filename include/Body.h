#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// --- Anatomy System ---
// Every living being is composed of a tree of body parts.
// Parts have individual HP, wound states, and functions. Losing parts
// degrades the functions they provide (sight, grasping, standing, etc.).

namespace BodyFunc {
    // Bitmask flags describing what a part contributes to the whole body.
    constexpr uint32_t None      = 0;
    constexpr uint32_t Vital     = 1 << 0;  // Destruction = death (heart, brain)
    constexpr uint32_t Thought   = 1 << 1;  // Consciousness
    constexpr uint32_t Sight     = 1 << 2;
    constexpr uint32_t Hearing   = 1 << 3;
    constexpr uint32_t Smell     = 1 << 4;
    constexpr uint32_t Breathe   = 1 << 5;
    constexpr uint32_t Grasp     = 1 << 6;  // Needed to hold weapons/tools
    constexpr uint32_t Stand     = 1 << 7;  // Needed to walk/run
    constexpr uint32_t Pump      = 1 << 8;  // Blood circulation
    constexpr uint32_t Digest    = 1 << 9;
    constexpr uint32_t Connector = 1 << 10; // Severing cuts off children (neck, limbs)
    constexpr uint32_t Internal  = 1 << 11; // Protected: only hit through parent
    constexpr uint32_t Flight    = 1 << 12; // Wings
}

enum class WoundLevel : int {
    Fine = 0,
    Bruised = 1,
    Cut = 2,
    Broken = 3,
    Mangled = 4,
    Severed = 5,   // Gone (sliced off)
    Destroyed = 6  // Gone (pulped/crushed)
};

std::string WoundLevelName(WoundLevel w);

struct BodyPart {
    std::string name;       // "left hand"
    int parent = -1;        // Index into parts vector, -1 = root
    uint32_t functions = 0; // BodyFunc bitmask
    int size = 10;          // Relative hit-targeting weight
    int hp = 10;
    int max_hp = 10;
    int wound = 0;          // WoundLevel as int (for serialization)
    float bleed = 0.0f;     // Blood loss per day caused by this part's wounds

    bool IsGone() const { return wound >= (int)WoundLevel::Severed; }
    WoundLevel Wound() const { return (WoundLevel)wound; }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(BodyPart, name, parent, functions, size, hp, max_hp, wound, bleed)
};

struct BodyComponent {
    std::string plan = "humanoid"; // "humanoid", "quadruped", "avian", "serpent"
    std::vector<BodyPart> parts;
    float blood = 100.0f;
    float max_blood = 100.0f;
    float pain = 0.0f; // 0-100

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(BodyComponent, plan, parts, blood, max_blood, pain)
};

// Result of applying damage to a single part.
struct WoundResult {
    std::string text;       // Human-readable description of what happened
    bool fatal = false;     // A vital part was destroyed
    bool severed = false;   // The part (and children) came off
    int part_index = -1;
};

class Body {
public:
    // --- Factories ---
    // scale: 1.0 = average human. Bigger creatures have more part HP and blood.
    static BodyComponent CreateHumanoid(float scale = 1.0f);
    static BodyComponent CreateQuadruped(float scale = 1.0f); // wolves, tigers, deer...
    static BodyComponent CreateAvian(float scale = 1.0f);     // birds
    static BodyComponent CreateSerpent(float scale = 1.0f);   // snakes, wyrms
    // Picks a plan heuristically from a beast's name/biome.
    static BodyComponent CreateForBeast(const std::string& beast_name, int tier);

    // --- Queries ---
    // 0.0-1.0: how well the body performs a function (e.g. one eye gone = 0.5 Sight).
    static float Capacity(const BodyComponent& body, uint32_t func);
    static bool IsDead(const BodyComponent& body);
    static int CountGraspingHands(const BodyComponent& body);
    // Weighted random hit location (never returns Internal parts unless through==true).
    static int PickHitPart(const BodyComponent& body, bool through = false);
    // Short health summary: "missing left hand, broken nose, bleeding"
    static std::string Summarize(const BodyComponent& body);

    // --- Mutation ---
    // damage_type: "slash" (can sever), "blunt" (breaks), "pierce" (hits internals)
    static WoundResult ApplyDamage(BodyComponent& body, int part_index, int damage,
                                   const std::string& damage_type, const std::string& victim_name);
    // Daily regeneration: clotting, healing, blood recovery. heal_mult >= 1.0 speeds it up.
    // Returns true if the body died of blood loss this tick.
    static bool DailyTick(BodyComponent& body, float heal_mult = 1.0f);
};
