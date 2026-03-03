#pragma once
#include "Components.h"
#include <vector>
#include <map>
#include <optional>

// A very simple ECS Registry
// In a real engine, use EnTT. Here, we implement a naive sparse set or map approach for simplicity.

class Registry {
public:
    int CreateEntity() {
        return next_id++;
    }

    // Storage maps: EntityID -> Component
    std::map<int, TransformComponent> transforms;
    std::map<int, StatsComponent> stats;
    std::map<int, CultivationComponent> cultivations;
    std::map<int, RenderComponent> renderables;
    std::map<int, ResourceComponent> resources;
    std::map<int, AIComponent> ai;
    std::map<int, SectComponent> sects;
    std::map<int, RaceComponent> races;
    std::map<int, PersonalityComponent> personalities;
    std::map<int, SectResourceComponent> sect_resources;
    
    // New V2.1 Components
    std::map<int, struct InventoryComponent> inventories;
    std::map<int, struct LocationComponent> locations;
    std::map<int, struct PrecisePositionComponent> precise_positions;
    std::map<int, struct SkillsComponent> skills;
    std::map<int, struct RelationComponent> relations;

    // Helpers
    template<typename T>
    T* GetComponent(int entity) {
        // Template specialization or overloads needed for this naive approach
        return nullptr;
    }

    // Specific accessors to avoid template complexity in this simple implementation
    TransformComponent* GetTransform(int entity) {
        if (transforms.count(entity)) return &transforms[entity];
        return nullptr;
    }
    
    StatsComponent* GetStats(int entity) {
        if (stats.count(entity)) return &stats[entity];
        return nullptr;
    }

    CultivationComponent* GetCultivation(int entity) {
        if (cultivations.count(entity)) return &cultivations[entity];
        return nullptr;
    }

    RenderComponent* GetRenderable(int entity) {
        if (renderables.count(entity)) return &renderables[entity];
        return nullptr;
    }

    SectComponent* GetSect(int entity) {
        if (sects.count(entity)) return &sects[entity];
        return nullptr;
    }

    RaceComponent* GetRace(int entity) {
        if (races.count(entity)) return &races[entity];
        return nullptr;
    }

    AIComponent* GetAI(int entity) {
        if (ai.count(entity)) return &ai[entity];
        return nullptr;
    }
    
    PersonalityComponent* GetPersonality(int entity) {
        if (personalities.count(entity)) return &personalities[entity];
        return nullptr;
    }
    
    SectResourceComponent* GetSectResource(int entity) {
        if (sect_resources.count(entity)) return &sect_resources[entity];
        return nullptr;
    }

    InjuryComponent* GetInjury(int entity) {
        if (injuries.count(entity)) return &injuries[entity];
        return nullptr;
    }

    InventoryComponent* GetInventory(int entity) {
        if (inventories.count(entity)) return &inventories[entity];
        return nullptr;
    }

    LocationComponent* GetLocation(int entity) {
        if (locations.count(entity)) return &locations[entity];
        return nullptr;
    }

    PrecisePositionComponent* GetPrecisePosition(int entity) {
        if (precise_positions.count(entity)) return &precise_positions[entity];
        return nullptr;
    }

    SkillsComponent* GetSkills(int entity) {
        if (skills.count(entity)) return &skills[entity];
        return nullptr;
    }

    RelationComponent* GetRelation(int entity) {
        if (relations.count(entity)) return &relations[entity];
        return nullptr;
    }

    void Clear() {
        transforms.clear();
        stats.clear();
        cultivations.clear();
        renderables.clear();
        resources.clear();
        ai.clear();
        sects.clear();
        races.clear();
        personalities.clear();
        sect_resources.clear();
        injuries.clear();
        inventories.clear();
        locations.clear();
        precise_positions.clear();
        skills.clear();
        relations.clear();
        sect_diplomacy.clear();
        next_id = 0;
    }

    json Serialize() {
        json j;
        j["next_id"] = next_id;
        j["transforms"] = transforms;
        j["stats"] = stats;
        j["cultivations"] = cultivations;
        j["renderables"] = renderables;
        j["resources"] = resources;
        j["ai"] = ai;
        j["sects"] = sects;
        j["races"] = races;
        j["personalities"] = personalities;
        j["sect_resources"] = sect_resources;
        j["injuries"] = injuries;
        j["inventories"] = inventories;
        j["locations"] = locations;
        j["precise_positions"] = precise_positions;
        j["skills"] = skills;
        j["relations"] = relations;
        j["sect_diplomacy"] = sect_diplomacy;
        return j;
    }

    void Deserialize(const json& j) {
        Clear();
        if(j.contains("next_id")) next_id = j["next_id"];
        if(j.contains("transforms")) transforms = j["transforms"].get<std::map<int, TransformComponent>>();
        if(j.contains("stats")) stats = j["stats"].get<std::map<int, StatsComponent>>();
        if(j.contains("cultivations")) cultivations = j["cultivations"].get<std::map<int, CultivationComponent>>();
        if(j.contains("renderables")) renderables = j["renderables"].get<std::map<int, RenderComponent>>();
        if(j.contains("resources")) resources = j["resources"].get<std::map<int, ResourceComponent>>();
        if(j.contains("ai")) ai = j["ai"].get<std::map<int, AIComponent>>();
        if(j.contains("sects")) sects = j["sects"].get<std::map<int, SectComponent>>();
        if(j.contains("races")) races = j["races"].get<std::map<int, RaceComponent>>();
        if(j.contains("personalities")) personalities = j["personalities"].get<std::map<int, PersonalityComponent>>();
        if(j.contains("sect_resources")) sect_resources = j["sect_resources"].get<std::map<int, SectResourceComponent>>();
        if(j.contains("injuries")) injuries = j["injuries"].get<std::map<int, InjuryComponent>>();
        if(j.contains("inventories")) inventories = j["inventories"].get<std::map<int, InventoryComponent>>();
        if(j.contains("locations")) locations = j["locations"].get<std::map<int, LocationComponent>>();
        if(j.contains("precise_positions")) precise_positions = j["precise_positions"].get<std::map<int, PrecisePositionComponent>>();
        if(j.contains("skills")) skills = j["skills"].get<std::map<int, SkillsComponent>>();
        if(j.contains("relations")) relations = j["relations"].get<std::map<int, RelationComponent>>();
        if(j.contains("sect_diplomacy")) sect_diplomacy = j["sect_diplomacy"].get<std::map<int, SectDiplomacyComponent>>();
    }

private:
    int next_id = 0;
    
    // Add missing map
public:
    std::map<int, InjuryComponent> injuries; // Injury storage
    std::map<int, SectDiplomacyComponent> sect_diplomacy;

    SectDiplomacyComponent* GetSectDiplomacy(int entity) {
        if (sect_diplomacy.count(entity)) return &sect_diplomacy[entity];
        return nullptr;
    }
};
