#pragma once
#include "ECS.h"
#include "GameData.h"
#include <string>
#include <vector>

class EventManager {
public:
    static EventManager& Get() {
        static EventManager instance;
        return instance;
    }

    // Called when a player/entity enters a tile
    void CheckRandomEncounter(int entityID, int x, int y, const std::string& biome);

    // Called periodically (e.g. annually)
    void UpdateGlobalEvents(int currentYear);

    // Specific Trigger
    void TriggerEvent(int eventID, int entityID);

private:
    EventManager() {}
    
    void HandleCombatEncounter(int entityID, const EventDefinition& evt);
    void HandleChoiceEncounter(int entityID, const EventDefinition& evt);
    void HandleHazard(int entityID, const EventDefinition& evt);
    
    // Helper to spawn temporary event entities
    int SpawnEventEntity(const std::string& templateID, int x, int y);
};
