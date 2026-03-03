#pragma once
#include "ECS.h"
#include "WorldMap.h"
#include <atomic>
#include <thread>

// The Core Simulation Engine
// Decoupled from rendering. Handles the "Roguelike Time" ticks.
class SimEngine {
public:
    SimEngine();
    ~SimEngine();

    // Singleton access
    static SimEngine& Get();

    // Core Loop
    bool AdvanceTime(int ticks);

    // World Management
    void InitializeWorld();
    
    // Core Loop
    void SystemCultivation(int ticks);
    void SystemAI(int ticks);
    void SystemDailyUpdate(); // New Daily Logic

    // Simulation Controls
    enum class TimeUnit { Days, Months, Years };
    void SimulateTime(int amount, TimeUnit unit);
    
    // Data Access
    Registry& GetRegistry() { return registry; }
    
    // Player Actions
    void PlayerMove(int dx, int dy);
    void PlayerCultivate(int ticks);

    // Simulation
    void SimulateHistory(int years);

    // Sect Economy (CP System)
    // Sect Economy (CP System)
    void DonateToSect(int entityID, const std::string& itemID, int count);
    void ExchangeFromSect(int entityID, const std::string& itemID);
    
    // Quest System
    void SystemQuestGeneration();
    bool CompleteMission(int entityID, int sectLeaderID, int missionIndex);

    // Helpers
    int GetPlayerEntityID() const { return player_id; }
    
    // Relations
    // Relations
    void UpdateRelations(int entityA, int entityB, int change);
    int GetRelation(int sectA, int sectB);
    
    // Key: SectID, Val: Map<TargetSectID, RelationValue (-100 to 100)>
    // Key: SectID, Val: Map<TargetSectID, RelationValue (-100 to 100)>
    std::map<int, std::map<int, int>> sectRelations; 

    // Time Helpers
    std::string GetDateString() const; 

private:
    long long current_tick = 0;
    Registry registry;
    int player_id = -1;
    
    // Systems
    // (Moved to Public)
    void CreateLootBag(int x, int y, const struct InventoryComponent& inv);
    void HandleDeath(int entity, const std::string& cause = "Unknown");

    // Async Simulation
public:
    void StartAsyncSimulation(int amount, TimeUnit unit);
    bool IsSimulating() const { return is_simulating.load(); }
    float GetSimulationProgress() const { return current_progress.load(); }
    void WaitSimulation(); // Join thread if needed

    // Persistence
    void SaveGame(const std::string& slotName, WorldMap& map);
    void LoadGame(const std::string& slotName, WorldMap& map);

    // Async Persistence
    void StartAsyncSave(const std::string& slotName, WorldMap& map);
    void StartAsyncLoad(const std::string& slotName, WorldMap& map);
    bool IsSaving() const { return is_saving.load(); }
    bool IsLoading() const { return is_loading.load(); }

private:
    std::atomic<bool> is_simulating{false};
    std::atomic<float> current_progress{0.0f};
    std::atomic<bool> is_saving{false};
    std::atomic<bool> is_loading{false};
    std::thread simulation_thread;
    std::thread persistence_thread;
};
