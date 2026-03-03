#include "EventManager.h"
#include "SimEngine.h"
#include "Components.h"
#include "HistoryGenerator.h"
#include <iostream>
#include <cstdlib>

void EventManager::CheckRandomEncounter(int entityID, int x, int y, const std::string& biome) {
    if (rand() % 100 > 5) return; // 5% chance per move

    // Filter events by Biome and Type
    const auto& events = GameData::GetEvents();
    std::vector<const EventDefinition*> possible;
    
    for (const auto& ev : events) {
        if (ev.type == "Encounter" || ev.type == "Hazard") {
            if (ev.trigger_biome == "Any" || ev.trigger_biome == biome) {
                possible.push_back(&ev);
            }
        }
    }

    if (possible.empty()) return;

    const EventDefinition* chosen = possible[rand() % possible.size()];
    TriggerEvent(chosen->id, entityID);
}

void EventManager::TriggerEvent(int eventID, int entityID) {
    const auto& events = GameData::GetEvents();
    const EventDefinition* evt = nullptr;
    for(const auto& e : events) { if (e.id == eventID) { evt = &e; break; } }
    
    if (!evt) return;

    auto& reg = SimEngine::Get().GetRegistry();
    auto* render = reg.GetRenderable(entityID);
    std::string entityName = render ? render->name : "Entity";

    std::cout << "[EVENT] " << entityName << " encountered: " << evt->title << " - " << evt->description << std::endl;
    // In a real UI, this would push a modal or log entry

    // Logic Dispatch
    if (evt->effect_tag == "Combat") {
        HandleCombatEncounter(entityID, *evt);
    } else if (evt->effect_tag == "Choice") {
        HandleChoiceEncounter(entityID, *evt);
    } else if (evt->effect_tag.find("Debuff") != std::string::npos) {
        HandleHazard(entityID, *evt);
    } else if (evt->effect_tag == "Duel") {
         // Same as combat for now
         HandleCombatEncounter(entityID, *evt);
    }
}

void EventManager::HandleCombatEncounter(int entityID, const EventDefinition& evt) {
    if (evt.spawn_entity_id.empty()) return;

    auto& reg = SimEngine::Get().GetRegistry();
    auto* tc = reg.GetTransform(entityID);
    if (!tc) return;

    // Spawn Enemy
    int enemyID = SpawnEventEntity(evt.spawn_entity_id, tc->x, tc->y);
    auto* enemyRender = reg.GetRenderable(enemyID);

    HistoryGenerator::AddEvent("[COMBAT] " + 
        (reg.GetRenderable(entityID)->name) + " was ambushed by " + (enemyRender ? enemyRender->name : "Unknown"));
        
    // In a turn-based system, this would initiate combat state.
    // For this simulation, we might do an instant resolve or set aggressive flag.
}

void EventManager::HandleHazard(int entityID, const EventDefinition& evt) {
    // Apply damage or status
    // Simple HP deduction for now
    auto& reg = SimEngine::Get().GetRegistry();
    auto* stats = reg.GetStats(entityID);
    if (stats) {
        stats->hp -= 10;
        if (stats->hp < 0) stats->hp = 0;
        // Log
    }
}

void EventManager::HandleChoiceEncounter(int entityID, const EventDefinition& evt) {
    // e.g. "Injured Beast" -> Heal or Kill
    // Randomize decision for NPC
    if (rand() % 2 == 0) {
        // Kill -> Get Loot
        if (!evt.reward_item_id.empty()) {
            auto* inv = SimEngine::Get().GetRegistry().GetInventory(entityID);
            if(inv) inv->items.push_back({evt.reward_item_id, 1});
        }
    } else {
        // Heal -> Good Karma (Feature not fully impl, maybe XP)
    }
}

int EventManager::SpawnEventEntity(const std::string& templateID, int x, int y) {
    // Reuse SimEngine logic or WorldGen logic
    // For now, create a basic entity
    return -1; // Placeholder: Need access to WorldGen::CreateBeast or similar
}

void EventManager::UpdateGlobalEvents(int currentYear) {
    // Check for yearly events
    if (currentYear % 10 == 0) {
        // Beast Tide?
    }
}
