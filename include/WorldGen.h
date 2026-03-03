#pragma once
#include "WorldMap.h"
#include <vector>

class WorldGen {
public:
    // Parameters for generation
    struct Config {
        int seed;
        float scale;
        float persistence;
        float lacunarity;
        int octaves;
    };

    static void GenerateLayer(MapLayer& layer, const Config& config);
    // Multiverse Generators
    static void GenerateSky(MapLayer& layer, const Config& config);       // +5 to +8
    static void GenerateVoid(MapLayer& layer, const Config& config);      // +9
    static void GenerateUnderdark(MapLayer& layer, const Config& config); // -1 to -4
    static void GenerateAbyss(MapLayer& layer, const Config& config);     // -5 to -9
    
    static void PopulateWorld(class Registry& registry, MapLayer& layer);
    static void PopulateSects(class Registry& registry, MapLayer& layer);
    static void PopulateResources(class Registry& registry, MapLayer& layer);

private:
    static float Noise(int x, int y, int seed);
    static float SmoothedNoise(int x, int y, int seed);
    static float InterpolatedNoise(float x, float y, int seed);
    static float PerlinNoise(float x, float y, const Config& config);
    static float CellularAutomata(int x, int y, int seed); // For Caves
};
