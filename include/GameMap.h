#pragma once
#include <cstdint>
#include <string>
#include <vector>

// --- Overworld map ---
// Generated with Perlin-noise fBm (elevation + moisture + qi density).
// Lives in the sim core; any front-end may render it however it likes.

enum class Biome : uint8_t {
    DeepWater = 0,
    Water,
    Sand,
    Grass,
    Forest,
    Swamp,
    Hills,
    Mountain,
    Snow,
};

std::string BiomeName(Biome b);

struct WorldTile {
    uint8_t biome = (uint8_t)Biome::Grass;
    float qi = 0.0f; // 0-1 spiritual energy density
};

class GameMap {
public:
    void Generate(unsigned seed, int width = 220, int height = 160);
    bool IsGenerated() const { return !tiles.empty(); }

    int Width() const { return width; }
    int Height() const { return height; }
    unsigned Seed() const { return seed; }

    const WorldTile& At(int x, int y) const { return tiles[(size_t)y * width + x]; }
    Biome BiomeAt(int x, int y) const { return (Biome)At(x, y).biome; }
    bool IsLand(int x, int y) const {
        Biome b = BiomeAt(x, y);
        return b != Biome::DeepWater && b != Biome::Water;
    }

    // Finds a habitable (grass/forest) tile near the given point.
    void FindSettleSpot(int near_x, int near_y, int& out_x, int& out_y) const;

    int town_x = 0;
    int town_y = 0;

private:
    int width = 0;
    int height = 0;
    unsigned seed = 0;
    std::vector<WorldTile> tiles;
};

// Classic 2D Perlin gradient noise with fBm octaves, deterministic per seed.
namespace Perlin {
    float Noise2D(float x, float y, unsigned seed);                  // -1..1
    float Fbm(float x, float y, unsigned seed, int octaves,
              float lacunarity = 2.0f, float gain = 0.5f);           // ~-1..1
}
