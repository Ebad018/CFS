#pragma once
#include "ECS.h"
#include "Combat.h"
#include "GameData.h"
#include "GameMap.h"
#include <string>
#include <vector>
#include <map>

// --- The Core Simulation Engine ---
// Fully decoupled from any UI. A front-end (text console, ImGui, tests...)
// issues player decisions through the public API and drains RecentLog()
// to display what happened. The simulation runs itself day by day:
// citizens work, eat, bleed, heal, train, socialize, fight and die.

struct RivalSettlement {
    int id = 0;
    std::string name = "Unknown";
    int power = 50;        // Military strength
    int wealth = 100;
    int relation = 0;      // -100 (war) .. 100 (ally)
    int distance_days = 3; // Travel time for missions
    bool destroyed = false;
    bool conquered = false; // Pays tribute to us
    int x = 0;             // Overworld position
    int y = 0;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(RivalSettlement, id, name, power, wealth, relation, distance_days, destroyed, conquered, x, y)
};

struct ActiveMission {
    std::string type;       // "Hunt", "Gather", "Adventure", "Raid", "Invade"
    std::vector<int> party; // Entity IDs away from town
    int days_left = 0;
    int total_days = 0;
    int target_rival = -1;  // For Raid/Invade

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ActiveMission, type, party, days_left, total_days, target_rival)
};

// Structured after-action report produced when a mission resolves.
struct MissionReport {
    std::string type;                 // Mission kind
    std::string target;               // Rival name (if war mission)
    long long day = 0;                // Tick the report was filed
    std::string date;                 // Human-readable date
    bool success = false;
    std::vector<std::string> party;     // Who was sent
    std::vector<std::string> events;    // Blow-by-blow of what happened
    std::vector<std::string> casualties;// Who died
    std::vector<std::string> injuries;  // Survivors' wounds
    std::vector<std::string> loot;      // Resources brought back ("38 food (beast meat)")
    std::vector<std::string> items;     // Items gained ("Iron Wolf Sword -> armory")
    std::vector<std::string> xp;        // Skill level-ups ("Gu Wei: Hunting rose to 7")

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(MissionReport, type, target, day, date, success, party, events, casualties, injuries, loot, items, xp)
};

struct BuildingSpec {
    std::string type;
    int wood_cost = 0;
    int stone_cost = 0;
    int build_days = 0;
    std::string description;
};

// One item on a travelling merchant's cart.
struct MerchantOffer {
    std::string item_id;  // Empty for pure-resource bundles
    std::string label;    // Display name ("50 food", "Iron Wolf Sword")
    int price = 0;        // Spirit stones
    int food = 0;         // Food granted if this is a provisions bundle

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(MerchantOffer, item_id, label, price, food)
};

class SimEngine {
public:
    static SimEngine& Get();

    Registry& GetRegistry() { return registry; }

    // --- Game setup ---
    void LoadDataFiles();                  // Parses the markdown design docs once
    void NewGame();                        // Clears state and rolls the candidate pool
    void RerollCandidates();               // Replace all 10
    void RerollCandidate(int slot);        // Replace one (0-9)
    const std::vector<int>& Candidates() const { return candidates; }
    void StartGame();                      // Founds the settlement with the candidates
    bool IsGameStarted() const { return game_started; }

    // --- Time (synchronous; the sim plays itself while time passes) ---
    void AdvanceDays(int days);
    std::string GetDateString() const;
    int CurrentYear() const { return (int)(current_tick / 360) + 1; }
    long long CurrentDay() const { return current_tick; }

    // --- Player decisions ---
    static const std::vector<BuildingSpec>& BuildingCatalog();
    std::string Build(const std::string& type);
    std::string AssignJob(int entity, const std::string& job);
    std::string AssignWeapon(int entity, const std::string& item_query); // from town armory
    std::string UnassignWeapon(int entity);
    std::string StartMission(const std::string& type, const std::vector<int>& party, int target_rival = -1);
    std::string HoldElection();

    // --- Labor priorities (ONI-style task system) ---
    // The player sets a 0-5 priority per category; every day the engine assigns
    // the best-skilled available people to the highest-priority work.
    std::map<std::string, int>& LaborPriorities() { return labor_priority; }
    void SetLaborPriority(const std::string& category, int priority);

    // --- Travelling merchant ---
    std::vector<MerchantOffer>& MerchantStock() { return merchant_stock; }
    int MerchantDaysLeft() const { return merchant_days; }
    std::string BuyFromMerchant(int index);

    // --- Queries (for any front-end) ---
    std::vector<int> LivingPeople() const;
    int FindPersonByName(const std::string& query) const; // case-insensitive prefix match
    int TownID() const { return town_id; }
    GameMap& Map() { return world_map; }
    std::vector<RivalSettlement>& Rivals() { return rivals; }
    std::vector<ActiveMission>& Missions() { return missions; }
    std::vector<MissionReport>& MissionReports() { return mission_reports; }
    bool IsOnMission(int entity) const;
    int GetSkill(int entity, const std::string& skill) const;
    // Effective combat stats (weapon + realm + skills)
    int EffectiveAttack(int entity) const;
    int EffectiveDefense(int entity) const;
    std::string EquippedWeaponName(int entity) const;

    // Day-by-day report buffer; the front-end drains this after AdvanceDays
    std::vector<std::string>& RecentLog() { return recent_log; }
    void Log(const std::string& line);

    // --- Persistence ---
    bool SaveGame(const std::string& slot);
    bool LoadGame(const std::string& slot);

private:
    SimEngine() = default;

    // Daily systems (the "game plays itself" loop)
    void AssignLabor();
    void TickConstruction();
    void TickProduction();
    void TickNeeds();
    void TickBodies();
    void TickMissions();
    void TickRandomEvents();
    void TickRivals();
    void TickSocial();
    void TickTournament();   // Monthly sparring bracket: glory, grudges, XP
    void TickWorldEvents();  // Data-driven events from the design documents
    void TickYearly();

    // Helpers
    int CreatePerson(int age_min, int age_max);
    void HandleDeath(int entity, const std::string& cause);
    int CountBuildings(const std::string& type, bool finished_only = true) const;
    int CountJob(const std::string& job) const;
    Combatant MakeCitizenCombatant(int entity);
    Combatant MakeBeastCombatant(const BeastDefinition& def, std::vector<BodyComponent>& storage);
    void ResolveTownDefense(const std::string& attacker_desc, std::vector<Combatant>& attackers,
                            std::vector<BodyComponent>& beast_bodies);
    void ResolveMission(ActiveMission& m);
    void RecomputeDefense();

    Registry registry;
    GameMap world_map;
    long long current_tick = 0;
    bool game_started = false;
    bool data_loaded = false;
    int town_id = -1;
    std::vector<int> candidates;
    std::vector<RivalSettlement> rivals;
    std::vector<ActiveMission> missions;
    std::vector<MissionReport> mission_reports;
    std::map<std::string, int> labor_priority; // category -> 0 (off) .. 5 (highest)
    std::vector<MerchantOffer> merchant_stock;
    int merchant_days = 0;
    std::vector<std::string> recent_log;
};
