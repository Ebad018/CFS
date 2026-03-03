#pragma once
#include <string>
#include <vector>
#include <map>

struct Sect {
    int id;
    std::string name;
    int grade; // 1 (Saint) to 9 (Iron)
    
    int territory_radius;
    int x, y, z; // Location of Sect Core
    
    // Resources in Vault
    // Key: Resource ID (e.g., "ORE_005"), Value: Quantity
    std::map<std::string, int> vault;
    
    std::vector<int> disciple_ids;
    std::string guardian_beast_id;
    
    // Relationships with other sects
    // Key: Sect ID, Value: -100 to 100
    std::map<int, int> relations;
};
