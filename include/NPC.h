#pragma once
#include <string>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// --- Sub-Components ---

struct NPCIdentity {
    std::string surname;
    std::string given_name;
    std::string namesakes_meaning;
    std::string title;
    std::string gender;
    int age = 0;
    int lifespan = 100;
    int birth_year = 0;
    int birth_month = 1;
    int birth_day = 1;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(NPCIdentity, surname, given_name, namesakes_meaning, title, gender, age, lifespan, birth_year, birth_month, birth_day)
};

struct NPCBiology {
    std::string race_id;
    float strength_mod = 1.0f;
    float fire_resist = 0.0f;
    std::vector<std::string> mutations;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(NPCBiology, race_id, strength_mod, fire_resist, mutations)
};

struct NPCRealm {
    std::string major;     // "Qi Condensation"
    int minor = 0;         // 1-9
    float purity = 0.0f;   // 0.0 - 1.0

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(NPCRealm, major, minor, purity)
};

struct NPCCultivation {
    NPCRealm realm;
    int qi_current = 0;
    int qi_max = 100;
    std::string attribute; // "Fire", "Water"
    std::string spirit_root_type; // "Heavenly Fire"
    
    // Methods: ID -> Mastery Level
    std::vector<std::string> known_methods;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(NPCCultivation, realm, qi_current, qi_max, attribute, spirit_root_type, known_methods)
};

struct NPCRelation {
    std::string target_uid;
    int value = 0;
    std::vector<std::string> tags; // "Master", "Rival"

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(NPCRelation, target_uid, value, tags)
};

struct NPCSocial {
    std::string sect_id;
    std::string rank; // "Elder"
    int contribution_points = 0;
    int loyalty = 50;
    int morality = 0;
    std::vector<NPCRelation> network;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(NPCSocial, sect_id, rank, contribution_points, loyalty, morality, network)
};

struct NPCPersonality {
    int courage = 50;
    int greed = 50;
    int honor = 50;
    int loyalty = 50;
    int curiosity = 50;
    int temper = 50;
    int compassion = 50;
    int ambition = 50;
    int diligence = 50;
    int pride = 50;
    std::string current_goal; // "Train", "Meditate"

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(NPCPersonality, courage, greed, honor, loyalty, curiosity, temper, compassion, ambition, diligence, pride, current_goal)
};

struct NPCGoal {
    std::string type; // "Breakthrough"
    int priority = 0;
    std::string target_item;
    std::string state; // "Gathering"

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(NPCGoal, type, priority, target_item, state)
};

struct NPCAIState {
    std::string current_location_region;
    int x = 0, y = 0, z = 0;
    NPCGoal active_goal;
    std::vector<NPCGoal> goal_queue;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(NPCAIState, current_location_region, x, y, z, active_goal, goal_queue)
};

// --- Main Entity ---

struct NPC {
    std::string uid;
    long long last_update_tick = 0;

    NPCIdentity identity;
    NPCBiology biology;
    NPCCultivation cultivation;
    NPCSocial social;
    NPCPersonality personality; // Added this
    NPCAIState ai_state;

    // Direct JSON serialization macro
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(NPC, 
        uid, last_update_tick, 
        identity, biology, cultivation, social, personality, ai_state
    )
};
