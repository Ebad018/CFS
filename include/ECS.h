#pragma once
#include "Components.h"
#include "Body.h"
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
    std::map<int, struct SkillSetComponent> skill_sets;
    std::map<int, struct RelationComponent> relations;

    // Phase 1 Simulation Components
    std::map<int, TownComponent> towns;
    std::map<int, BuildingComponent> buildings;
    std::map<int, ClanComponent> clans;
    std::map<int, GoalComponent> goals;

    // Fortress-mode Components
    std::map<int, BodyComponent> bodies;
    std::map<int, JobComponent> jobs;
    std::map<int, NeedsComponent> needs;

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

    SkillSetComponent* GetSkillSet(int entity) {
        if (skill_sets.count(entity)) return &skill_sets[entity];
        return nullptr;
    }

    RelationComponent* GetRelation(int entity) {
        if (relations.count(entity)) return &relations[entity];
        return nullptr;
    }

    TownComponent* GetTown(int entity) {
        if (towns.count(entity)) return &towns[entity];
        return nullptr;
    }

    BuildingComponent* GetBuilding(int entity) {
        if (buildings.count(entity)) return &buildings[entity];
        return nullptr;
    }

    ClanComponent* GetClan(int entity) {
        if (clans.count(entity)) return &clans[entity];
        return nullptr;
    }

    GoalComponent* GetGoal(int entity) {
        if (goals.count(entity)) return &goals[entity];
        return nullptr;
    }

    BodyComponent* GetBody(int entity) {
        if (bodies.count(entity)) return &bodies[entity];
        return nullptr;
    }

    JobComponent* GetJob(int entity) {
        if (jobs.count(entity)) return &jobs[entity];
        return nullptr;
    }

    NeedsComponent* GetNeeds(int entity) {
        if (needs.count(entity)) return &needs[entity];
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
        skill_sets.clear();
        relations.clear();
        sect_diplomacy.clear();
        
        towns.clear();
        buildings.clear();
        clans.clear();
        goals.clear();
        bodies.clear();
        jobs.clear();
        needs.clear();

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
        j["skill_sets"] = skill_sets;
        j["relations"] = relations;
        j["sect_diplomacy"] = sect_diplomacy;
        
        j["towns"] = towns;
        j["buildings"] = buildings;
        j["clans"] = clans;
        j["goals"] = goals;
        j["bodies"] = bodies;
        j["jobs"] = jobs;
        j["needs"] = needs;

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
        if(j.contains("skill_sets")) skill_sets = j["skill_sets"].get<std::map<int, SkillSetComponent>>();
        if(j.contains("relations")) relations = j["relations"].get<std::map<int, RelationComponent>>();
        if(j.contains("sect_diplomacy")) sect_diplomacy = j["sect_diplomacy"].get<std::map<int, SectDiplomacyComponent>>();
        
        if(j.contains("towns")) towns = j["towns"].get<std::map<int, TownComponent>>();
        if(j.contains("buildings")) buildings = j["buildings"].get<std::map<int, BuildingComponent>>();
        if(j.contains("clans")) clans = j["clans"].get<std::map<int, ClanComponent>>();
        if(j.contains("goals")) goals = j["goals"].get<std::map<int, GoalComponent>>();
        if(j.contains("bodies")) bodies = j["bodies"].get<std::map<int, BodyComponent>>();
        if(j.contains("jobs")) jobs = j["jobs"].get<std::map<int, JobComponent>>();
        if(j.contains("needs")) needs = j["needs"].get<std::map<int, NeedsComponent>>();
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
