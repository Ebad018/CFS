#pragma once
#include "GameData.h"
#include <string>

class GameDataLoader {
public:
    static void LoadAll();
    
private:
    static void LoadBeasts(const std::string& filepath);
    static void LoadResources(const std::string& filepath);
    static void LoadItems(const std::string& filepath);
    static void LoadSkills(const std::string& filepath);
    static void LoadEvents(const std::string& filepath);
};
