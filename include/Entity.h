#pragma once
#include <string>
#include <vector>
#include <map>

// Entity Component System - Entity Definition
// In this engine, Entities are just IDs with attached components (data).
// For now, we define a basic struct.

struct Entity {
    int id; // Unique ID
    std::string name;
    
    // Components (Simplified for now)
    int hp;
    int max_hp;
    int qi;
    int max_qi;
    
    // Position
    int x;
    int y;
    int z; // Map Layer
    
    // Rendering
    char symbol; // 'A', '@', 'D'
    // Color in hex? Or SFML Color type if we want coupling. 
    // Decoupling: Store color as uint32_t 0xRRGGBBAA
    uint32_t color; 

    bool active = true;
};
