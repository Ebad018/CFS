#include "WorldMap.h"
#include "SimEngine.h"
#include <iostream>

Tile& MapLayer::GetTile(int x, int y) {
    // Simple 1D layout or 2D. 
    // Assuming vector size = width * height
    if (x < 0 || x >= width || y < 0 || y >= height) {
        // Return dummy or handle error. For now, referencing a static dummy or exception.
        static Tile dummy;
        return dummy;
    }
    return tiles[y * width + x];
}

WorldMap::WorldMap() {
}

WorldMap::~WorldMap() {
    layers.clear();
}

void WorldMap::Initialize() {
    std::cout << "[WorldMap] Initializing Multiverse..." << std::endl;
}

MapLayer* WorldMap::GetLayer(int z) {
    if (layers.find(z) == layers.end()) {
        return nullptr;
    }
    return &layers[z];
}

#include "WorldGen.h"

void WorldMap::GenerateMultiverse() {
    // Generate a random seed for this run
    int seed = rand(); 
    std::cout << "[WorldMap] Generating Multiverse Layers (-9 to +9) with SEED: " << seed << "..." << std::endl;
    
    // Config Base
    WorldGen::Config config;
    config.seed = seed; // Use dynamic seed
    config.scale = 0.2f; // Higher frequency for more biomes
    config.persistence = 0.5f;
    config.lacunarity = 2.0f;
    config.octaves = 4;

    for (int z = -9; z <= 9; ++z) {
        MapLayer layer;
        layer.z_index = z;
        layer.width = 200; // Expanded Map
        layer.height = 200;
        layer.tiles.resize(200 * 200);
        
        // Name the layer
        if (z == 9) layer.layer_name = "Void (+9)";
        else if (z >= 5) layer.layer_name = "Sky Layer (+" + std::to_string(z) + ")";
        else if (z >= 1) layer.layer_name = "Spirit Peaks (+" + std::to_string(z) + ")";
        else if (z == 0) layer.layer_name = "Mortal Plane";
        else if (z >= -4) layer.layer_name = "Underdark (" + std::to_string(z) + ")";
        else layer.layer_name = "Demon Abyss (" + std::to_string(z) + ")";
        
        // Generate Terrain
        if (z == 9) WorldGen::GenerateVoid(layer, config);
        else if (z >= 5) WorldGen::GenerateSky(layer, config);
        else if (z >= 1) WorldGen::GenerateLayer(layer, config); // Spirit Peaks use standard generation for now, maybe biased?
        else if (z == 0) WorldGen::GenerateLayer(layer, config);
        else if (z >= -4) WorldGen::GenerateUnderdark(layer, config);
        else WorldGen::GenerateAbyss(layer, config);
        
        // Populate (Only Mortal for now, or based on rules)
        if (z == 0) {
             WorldGen::PopulateSects(SimEngine::Get().GetRegistry(), layer);
             WorldGen::PopulateWorld(SimEngine::Get().GetRegistry(), layer);
        }
        
        layers[z] = layer;
    }
    
    std::cout << "[WorldMap] Generation Complete." << std::endl;
}

#include <fstream>

void WorldMap::SaveToJSON(const std::string& path) {
    std::cout << "[WorldMap] Saving to " << path << "..." << std::endl;
    json j;
    j["layers"] = layers;
    
    std::ofstream o(path);
    o << j << std::endl;
    std::cout << "[WorldMap] Saved." << std::endl;
}

void WorldMap::LoadFromJSON(const std::string& path) {
    std::cout << "[WorldMap] Loading from " << path << "..." << std::endl;
    std::ifstream i(path);
    if (!i.is_open()) {
        std::cerr << "[WorldMap] Failed to open file: " << path << std::endl;
        return;
    }
    json j;
    i >> j;
    
    if (j.contains("layers")) {
        layers = j["layers"].get<std::map<int, MapLayer>>();
    }
    std::cout << "[WorldMap] Loaded." << std::endl;
}
