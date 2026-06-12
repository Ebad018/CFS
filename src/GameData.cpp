#include "GameData.h"

std::vector<RaceDefinition> GameData::races = {
    // Basic races
    {"RACE_01", "Human", true, "Mortal Plane", 1.0f, 1.0f, 1.0f, 1.0f, "None", "High Insight: +20% Learning Speed"}
};
std::vector<BeastDefinition> GameData::beasts;
std::vector<ResourceDefinition> GameData::resources;
std::vector<ItemDefinition> GameData::items;
std::vector<SkillDefinition> GameData::skills;
std::vector<CultivationDefinition> GameData::cultivationMethods;
std::vector<EventDefinition> GameData::events;
