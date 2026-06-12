#include "GameDataLoader.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

// Helper to split string by delimiter
static std::vector<std::string> split(const std::string& str, char delim) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delim)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Helper to trim whitespace and asterisks
static std::string trim(const std::string& str) {
    auto start = str.find_first_not_of(" \t*\\");
    if (start == std::string::npos) return "";
    auto end = str.find_last_not_of(" \t*\\");
    return str.substr(start, end - start + 1);
}

// Markdown table separator cells (":----", "---") must not become game objects
static bool IsSeparatorCell(const std::string& s) {
    return !s.empty() && s.find_first_not_of(":-") == std::string::npos;
}

void GameDataLoader::LoadAll() {
    std::cout << "[GameDataLoader] Loading Design Documents..." << std::endl;
    LoadBeasts("Master Bestiary List.md");
    LoadResources("Design Document_ Master Resource & Asset Database.md");
    LoadItems("Design Document_ Master Weapon Database (Polearms & Blades).md");
    LoadSkills("Design Document_ Master Cultivation & Skills.md");
    LoadEvents("Design Document_ Master Events & Encounters.md");
    // Other loaders can be added here
}

void GameDataLoader::LoadBeasts(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[GameDataLoader] Failed to open " << filepath << std::endl;
        return;
    }

    std::string line;
    bool in_table = false;
    int current_tier = 5;

    while (std::getline(file, line)) {
        std::string tline = trim(line);
        if (tline.empty()) continue;

        // Check for tier updates
        if (tline.find("Tier:") != std::string::npos) {
            if (tline.find("God") != std::string::npos) current_tier = 10;
            else if (tline.find("Saint") != std::string::npos) current_tier = 9;
            else if (tline.find("Heaven") != std::string::npos) current_tier = 7;
            else if (tline.find("Earth") != std::string::npos) current_tier = 5;
            else if (tline.find("Human") != std::string::npos) current_tier = 3;
        }

        if (tline.find("| ID") != std::string::npos) {
            in_table = true;
            continue;
        }
        
        if (tline.find("|---") != std::string::npos || tline.find("| ---") != std::string::npos || tline.find("|:-") != std::string::npos) {
            continue; // Header separator
        }

        if (in_table && line.find('|') != std::string::npos) {
            auto cols = split(line, '|');
            if (cols.size() < 5) {
                in_table = false;
                continue;
            }

            // The split will have an empty string at cols[0] if the line starts with |
            int offset = (cols[0].empty() || trim(cols[0]).empty()) ? 1 : 0;
            if (cols.size() < offset + 4) continue;

            std::string id = trim(cols[offset + 0]);
            std::string name = trim(cols[offset + 1]);
            std::string element = trim(cols[offset + 2]);
            std::string biome = trim(cols[offset + 3]);
            
            if (id.empty() || name.empty() || IsSeparatorCell(id) || IsSeparatorCell(name)) continue;

            // Clean element (e.g., "Chaos/Space" -> "Chaos")
            if (element.find('/') != std::string::npos) {
                element = split(element, '/')[0];
            }
            
            // Clean biome (e.g., "Void (Sky Layer 9)" -> "Void")
            if (biome.find('(') != std::string::npos) {
                biome = trim(split(biome, '(')[0]);
            }

            bool aggressive = false;
            for (const auto& c : cols) {
                std::string lower = c;
                std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
                if (lower.find("aggressive") != std::string::npos || 
                    lower.find("territorial") != std::string::npos ||
                    lower.find("hunter") != std::string::npos) {
                    aggressive = true;
                }
            }
            
            char symbol = name.empty() ? 'B' : name[0];
            uint32_t color = 0xFFFFFFFF;
            std::string ele_lower = element;
            std::transform(ele_lower.begin(), ele_lower.end(), ele_lower.begin(), ::tolower);
            if (ele_lower.find("fire") != std::string::npos) color = 0xFF4500FF;
            else if (ele_lower.find("water") != std::string::npos || ele_lower.find("ice") != std::string::npos) color = 0x00BFFFFF;
            else if (ele_lower.find("wood") != std::string::npos || ele_lower.find("poison") != std::string::npos) color = 0x32CD32FF;
            else if (ele_lower.find("earth") != std::string::npos) color = 0x8B4513FF;
            else if (ele_lower.find("metal") != std::string::npos) color = 0xC0C0C0FF;
            else if (ele_lower.find("thunder") != std::string::npos || ele_lower.find("lightning") != std::string::npos) color = 0x9370DBFF;
            else if (ele_lower.find("wind") != std::string::npos) color = 0xF5F5DCFF;
            else if (ele_lower.find("dark") != std::string::npos || ele_lower.find("death") != std::string::npos) color = 0x8B0000FF;

            BeastDefinition def;
            def.id = id;
            def.name = name;
            def.element = element;
            def.biome = biome;
            def.tier = current_tier;
            def.aggressive = aggressive;
            def.symbol = symbol;
            def.color = color;
            GameData::beasts.push_back(def);
        } else {
            in_table = false;
        }
    }
    std::cout << "[GameDataLoader] Loaded " << GameData::beasts.size() << " beasts." << std::endl;
}

void GameDataLoader::LoadResources(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[GameDataLoader] Failed to open " << filepath << std::endl;
        return;
    }

    std::string line;
    bool in_table = false;
    int current_part = 0; // 1=Cores, 2=Materials, 3=Herbs, 4=Ores, 5=God

    while (std::getline(file, line)) {
        std::string tline = trim(line);
        if (tline.empty()) continue;

        if (tline.find("Part 1:") != std::string::npos) current_part = 1;
        else if (tline.find("Part 2:") != std::string::npos) current_part = 2;
        else if (tline.find("Part 3:") != std::string::npos) current_part = 3;
        else if (tline.find("Part 4:") != std::string::npos) current_part = 4;
        else if (tline.find("Part 5:") != std::string::npos) current_part = 5;

        if (tline.find("| ID") != std::string::npos) {
            in_table = true;
            continue;
        }
        
        if (tline.find("|---") != std::string::npos || tline.find("| ---") != std::string::npos || tline.find("|:-") != std::string::npos) {
            continue;
        }

        if (in_table && line.find('|') != std::string::npos) {
            auto cols = split(line, '|');
            if (cols.size() < 4) {
                in_table = false;
                continue;
            }

            int offset = (cols[0].empty() || trim(cols[0]).empty()) ? 1 : 0;
            if (cols.size() < offset + 4) continue;

            std::string id = trim(cols[offset + 0]);
            std::string name = trim(cols[offset + 1]);
            
            if (id.empty() || name.empty() || id.find("ID") != std::string::npos || IsSeparatorCell(id) || IsSeparatorCell(name)) continue;

            if (current_part == 3 || current_part == 4) {
                // Herbs & Ores -> ResourceDefinition
                if (cols.size() < offset + 5) continue;
                std::string tier_str = trim(cols[offset + 2]);
                std::string biome = trim(cols[offset + 3]);
                std::string tag = trim(cols[offset + 4]);
                std::string effect = cols.size() > offset + 5 ? trim(cols[offset + 5]) : "";
                
                ResourceDefinition def;
                def.id = id;
                def.name = name;
                def.type = (current_part == 3) ? "Herb" : "Ore";
                def.biome = biome;
                try { def.tier = std::stoi(tier_str); } catch(...) { def.tier = 1; }
                def.symbol = (def.type == "Herb") ? '"' : '*';
                def.color = (def.type == "Herb") ? 0x00FF00FF : 0x808080FF;
                def.alchemy_tag = tag;
                def.effect = effect;
                GameData::resources.push_back(def);

                // Also add them as Items so they can be picked up
                ItemDefinition idef;
                idef.id = id;
                idef.name = name;
                idef.category = "Material";
                idef.sub_category = def.type;
                idef.tier = def.tier;
                idef.base_value = def.tier * 10;
                idef.description = effect;
                idef.effect_tag = tag;
                GameData::items.push_back(idef);
            } else {
                // Cores, Materials, God Tier -> ItemDefinition
                ItemDefinition def;
                def.id = id;
                def.name = name;
                
                if (current_part == 5) { // God Tier (no tier column, source is cols 2)
                    def.tier = 10;
                    def.category = "Artifact";
                    def.sub_category = "God";
                    std::string tag = trim(cols[offset + 3]);
                    std::string desc = cols.size() > offset + 4 ? trim(cols[offset + 4]) : "";
                    def.effect_tag = tag;
                    def.description = desc;
                    def.base_value = 99999;
                } else {
                    std::string tier_str = trim(cols[offset + 2]);
                    try { def.tier = std::stoi(tier_str); } catch(...) { def.tier = 1; }
                    std::string tag = cols.size() > offset + 4 ? trim(cols[offset + 4]) : "";
                    std::string desc = cols.size() > offset + 5 ? trim(cols[offset + 5]) : "";
                    def.effect_tag = tag;
                    def.description = desc;
                    def.base_value = def.tier * 50;

                    if (current_part == 1) {
                        def.category = "Consumable";
                        def.sub_category = "Core";
                    } else if (current_part == 2) {
                        def.category = "Material";
                        def.sub_category = "BeastPart";
                    }
                }
                def.attack = 0;
                def.defense = 0;
                GameData::items.push_back(def);
            }
        } else {
            in_table = false;
        }
    }
    std::cout << "[GameDataLoader] Loaded " << GameData::resources.size() << " resources and " << GameData::items.size() << " items." << std::endl;
}

void GameDataLoader::LoadItems(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[GameDataLoader] Failed to open " << filepath << std::endl;
        return;
    }

    std::string line;
    bool in_table = false;
    
    while (std::getline(file, line)) {
        std::string tline = trim(line);
        if (tline.empty()) continue;

        if (tline.find("| ID") != std::string::npos) {
            in_table = true;
            continue;
        }
        
        if (tline.find("|---") != std::string::npos || tline.find("| ---") != std::string::npos || tline.find("|:-") != std::string::npos) {
            continue;
        }

        if (in_table && line.find('|') != std::string::npos) {
            auto cols = split(line, '|');
            int offset = (cols[0].empty() || trim(cols[0]).empty()) ? 1 : 0;
            if (cols.size() < offset + 6) continue;

            std::string id = trim(cols[offset + 0]);
            std::string name = trim(cols[offset + 1]);
            
            if (id.empty() || name.empty() || id.find("ID") != std::string::npos || IsSeparatorCell(id) || IsSeparatorCell(name)) continue;

            ItemDefinition def;
            def.id = id;
            def.name = name;
            def.category = "Weapon";
            
            std::string tier_str = trim(cols[offset + 2]);
            try { def.tier = std::stoi(tier_str); } catch(...) { def.tier = 1; }
            
            def.sub_category = trim(cols[offset + 3]); // SWD, SPR, GLV, HLB
            
            std::string recipe = trim(cols[offset + 4]);
            def.required_resource_id = split(recipe, ' ')[0]; // Extract ID
            
            def.effect_tag = trim(cols[offset + 5]);
            def.description = cols.size() > offset + 6 ? trim(cols[offset + 6]) : "";
            
            def.base_value = def.tier * 100;
            def.attack = def.tier * 15;
            def.defense = 0;
            
            GameData::items.push_back(def);
        } else {
            in_table = false;
        }
    }
}

void GameDataLoader::LoadSkills(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[GameDataLoader] Failed to open " << filepath << std::endl;
        return;
    }

    std::string line;
    bool in_table = false;
    int current_part = 0; // 1 = Cultivation, 2 = Skill, 3 = Manual

    while (std::getline(file, line)) {
        std::string tline = trim(line);
        if (tline.empty()) continue;

        if (tline.find("Part 1:") != std::string::npos) current_part = 1;
        else if (tline.find("Part 2") != std::string::npos) current_part = 2; // Catches 2.1, 2.2, etc
        else if (tline.find("Part 3:") != std::string::npos) current_part = 3;

        if (tline.find("| ID") != std::string::npos) {
            in_table = true;
            continue;
        }
        
        if (tline.find("|---") != std::string::npos || tline.find("| ---") != std::string::npos || tline.find("|:-") != std::string::npos) {
            continue;
        }

        if (in_table && line.find('|') != std::string::npos) {
            auto cols = split(line, '|');
            int offset = (cols[0].empty() || trim(cols[0]).empty()) ? 1 : 0;
            if (cols.size() < offset + 3) continue;

            std::string id = trim(cols[offset + 0]);
            std::string name = trim(cols[offset + 1]);
            
            if (id.empty() || name.empty() || id.find("ID") != std::string::npos || IsSeparatorCell(id) || IsSeparatorCell(name)) continue;

            if (current_part == 1) { // Cultivation Method
                if (cols.size() < offset + 5) continue;
                CultivationDefinition def;
                def.id = id;
                def.name = name;
                
                std::string tier_str = trim(cols[offset + 2]);
                try { def.tier = std::stoi(tier_str); } catch(...) { def.tier = 1; }
                
                def.element = trim(cols[offset + 3]);
                def.required_item_id = split(trim(cols[offset + 4]), ' ')[0];
                def.description = cols.size() > offset + 5 ? trim(cols[offset + 5]) : "";
                
                // Set default multipliers based on element for now
                def.hp_multiplier = 1.0f;
                def.attack_multiplier = 1.0f;
                if (def.element == "Body") def.hp_multiplier = 1.5f;
                if (def.element == "Attack" || def.element == "Metal") def.attack_multiplier = 1.5f;
                
                GameData::cultivationMethods.push_back(def);
            } 
            else if (current_part == 2) { // Active Skill
                if (cols.size() < offset + 5) continue;
                SkillDefinition def;
                def.id = id;
                def.name = name;
                
                std::string tier_str = trim(cols[offset + 2]);
                try { def.tier = std::stoi(tier_str); } catch(...) { def.tier = 1; }
                
                def.element = trim(cols[offset + 3]);
                
                std::string cost_str = trim(cols[offset + 4]);
                try { def.qi_cost = std::stoi(split(cost_str, ' ')[0]); } catch(...) { def.qi_cost = 10; }
                
                def.description = cols.size() > offset + 5 ? trim(cols[offset + 5]) : "";
                
                GameData::skills.push_back(def);
            }
            else if (current_part == 3) { // Secret Manual (Item)
                ItemDefinition def;
                def.id = id;
                def.name = name;
                def.category = "Manual";
                def.sub_category = "Scripture";
                def.tier = 8;
                def.base_value = 5000;
                
                def.required_resource_id = trim(cols[offset + 2]);
                def.description = cols.size() > offset + 3 ? trim(cols[offset + 3]) : "";
                
                def.attack = 0;
                def.defense = 0;
                GameData::items.push_back(def);
            }
        } else {
            in_table = false;
        }
    }
}

void GameDataLoader::LoadEvents(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[GameDataLoader] Failed to open " << filepath << std::endl;
        return;
    }

    std::string line;
    std::string current_type = "Event";
    
    while (std::getline(file, line)) {
        std::string tline = trim(line);
        if (tline.empty()) continue;

        if (tline.find("Part 1:") != std::string::npos) current_type = "Location";
        else if (tline.find("Part 2:") != std::string::npos) current_type = "WorldEvent";
        else if (tline.find("Part 3:") != std::string::npos) current_type = "LuckyChance";
        else if (tline.find("Part 4:") != std::string::npos) current_type = "Encounter";
        else if (tline.find("Part 5:") != std::string::npos) current_type = "Hazard";

        // Check if line starts with number
        if (std::isdigit(tline[0])) {
            auto pos1 = tline.find("**");
            if (pos1 != std::string::npos) {
                auto pos2 = tline.find("**", pos1 + 2);
                if (pos2 != std::string::npos) {
                    std::string id = "EVT_" + std::to_string(GameData::events.size() + 1);
                    std::string title = tline.substr(pos1 + 2, pos2 - pos1 - 2);
                    
                    // Remove trailing colon from title if exists
                    if (!title.empty() && title.back() == ':') {
                        title.pop_back();
                    }
                    
                    std::string desc = trim(tline.substr(pos2 + 2));
                    
                    EventDefinition def;
                    def.id = id;
                    def.name = title;
                    def.description = desc;
                    def.type = current_type;
                    def.danger_level = (current_type == "Hazard" || current_type == "WorldEvent") ? 5 : 3;
                    
                    GameData::events.push_back(def);
                }
            }
        }
    }
    std::cout << "[GameDataLoader] Loaded " << GameData::events.size() << " events." << std::endl;
}
