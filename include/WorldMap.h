#pragma once
#include <vector>
#include <map>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct Tile {
    int x, y, z;
    std::string type; // "FOREST", "VOID", etc.
    float qi_density;
    bool is_walkable;
    // Entities in this tile (IDs)
    std::vector<int> entity_ids; 
    
    // Point of Interest
    std::string poi_type; // "SECT_RUINS", "CITY", "SPIRIT_VEIN", etc.
    
    // Environmental Hazard (v2.0)
    std::string hazard; // "Miasma", "Thin Air", "Spatial Shear"

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Tile, x, y, z, type, qi_density, is_walkable, entity_ids, poi_type, hazard)
};

struct MapLayer {
    int z_index;
    int width;
    int height;
    std::vector<Tile> tiles;
    std::string layer_name; // "Mortal Plane", "Void"
    
    Tile& GetTile(int x, int y);

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MapLayer, z_index, width, height, tiles, layer_name)
};

class WorldMap {
public:
    WorldMap();
    ~WorldMap();

    void Initialize();
    
    // Access
    MapLayer* GetLayer(int z);
    
    // Core Generation Entry Point
    void GenerateMultiverse();
    void Clear() { layers.clear(); }

    // Persistence
    void SaveToJSON(const std::string& path);
    void LoadFromJSON(const std::string& path);

private:
    std::map<int, MapLayer> layers;
    
    // Allow serialization of private map if needed, or implement wrapper in Save/Load
    friend void to_json(json& j, const WorldMap& wm);
    friend void from_json(const json& j, WorldMap& wm);
};

// Define serialization for WorldMap outside class or manually handle in Save methods
// Since we are adding specific methods SaveToJSON/LoadFromJSON, we can just access 'layers' inside them if they are members.
// But to make WorldMap serializeable directly via generic methods (if ever needed):
inline void to_json(json& j, const WorldMap& wm) {
    // We cannot access private members here unless friend.
    // However, the SaveToJSON method is a member, so it can just do the work.
}
