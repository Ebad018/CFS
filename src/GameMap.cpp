#include "GameMap.h"
#include <algorithm>
#include <cmath>

namespace {
    // Deterministic hash -> gradient direction
    uint32_t Hash(int x, int y, unsigned seed) {
        uint32_t h = (uint32_t)x * 374761393u + (uint32_t)y * 668265263u + seed * 2246822519u;
        h = (h ^ (h >> 13)) * 1274126177u;
        return h ^ (h >> 16);
    }

    void Gradient(int ix, int iy, unsigned seed, float& gx, float& gy) {
        uint32_t h = Hash(ix, iy, seed);
        float angle = (float)(h & 0xFFFF) / 65536.0f * 6.28318530718f;
        gx = std::cos(angle);
        gy = std::sin(angle);
    }

    float Fade(float t) { return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f); }
    float Lerp(float a, float b, float t) { return a + (b - a) * t; }

    float DotGrid(int ix, int iy, float x, float y, unsigned seed) {
        float gx, gy;
        Gradient(ix, iy, seed, gx, gy);
        return (x - (float)ix) * gx + (y - (float)iy) * gy;
    }
}

namespace Perlin {

float Noise2D(float x, float y, unsigned seed) {
    int x0 = (int)std::floor(x);
    int y0 = (int)std::floor(y);
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    float sx = Fade(x - (float)x0);
    float sy = Fade(y - (float)y0);

    float n0 = DotGrid(x0, y0, x, y, seed);
    float n1 = DotGrid(x1, y0, x, y, seed);
    float ix0 = Lerp(n0, n1, sx);
    n0 = DotGrid(x0, y1, x, y, seed);
    n1 = DotGrid(x1, y1, x, y, seed);
    float ix1 = Lerp(n0, n1, sx);
    return Lerp(ix0, ix1, sy) * 1.41421f; // normalize roughly to -1..1
}

float Fbm(float x, float y, unsigned seed, int octaves, float lacunarity, float gain) {
    float sum = 0.0f, amp = 1.0f, freq = 1.0f, norm = 0.0f;
    for (int i = 0; i < octaves; ++i) {
        sum += Noise2D(x * freq, y * freq, seed + (unsigned)i * 1013u) * amp;
        norm += amp;
        amp *= gain;
        freq *= lacunarity;
    }
    return norm > 0.0f ? sum / norm : 0.0f;
}

} // namespace Perlin

std::string BiomeName(Biome b) {
    switch (b) {
        case Biome::DeepWater: return "Deep Water";
        case Biome::Water: return "Water";
        case Biome::Sand: return "Sand";
        case Biome::Grass: return "Grassland";
        case Biome::Forest: return "Forest";
        case Biome::Swamp: return "Swamp";
        case Biome::Hills: return "Hills";
        case Biome::Mountain: return "Mountain";
        case Biome::Snow: return "Snowy Peak";
    }
    return "Unknown";
}

void GameMap::Generate(unsigned s, int w, int h) {
    seed = s;
    width = w;
    height = h;
    tiles.assign((size_t)w * h, WorldTile{});

    const float scale = 1.0f / 28.0f;       // Elevation feature size
    const float moist_scale = 1.0f / 40.0f; // Broader moisture bands
    const float qi_scale = 1.0f / 18.0f;

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            float e = Perlin::Fbm((float)x * scale, (float)y * scale, seed, 5);
            e = (e + 1.0f) * 0.5f; // 0..1

            // Push the map edges underwater so the world reads as an island continent
            float nx = (float)x / (float)w * 2.0f - 1.0f;
            float ny = (float)y / (float)h * 2.0f - 1.0f;
            float edge = std::max(std::abs(nx), std::abs(ny));
            e -= std::max(0.0f, edge - 0.55f) * 0.9f;

            float m = (Perlin::Fbm((float)x * moist_scale, (float)y * moist_scale, seed + 7777u, 4) + 1.0f) * 0.5f;
            float q = (Perlin::Fbm((float)x * qi_scale, (float)y * qi_scale, seed + 31337u, 3) + 1.0f) * 0.5f;

            Biome b;
            if (e < 0.30f) b = Biome::DeepWater;
            else if (e < 0.40f) b = Biome::Water;
            else if (e < 0.44f) b = Biome::Sand;
            else if (e > 0.82f) b = Biome::Snow;
            else if (e > 0.72f) b = Biome::Mountain;
            else if (e > 0.64f) b = Biome::Hills;
            else if (m > 0.72f && e < 0.55f) b = Biome::Swamp;
            else if (m > 0.45f) b = Biome::Forest;
            else b = Biome::Grass;

            WorldTile& t = tiles[(size_t)y * w + x];
            t.biome = (uint8_t)b;
            // Mountains and deep forests are richer in qi
            float bonus = (b == Biome::Mountain || b == Biome::Snow) ? 0.25f : (b == Biome::Forest ? 0.1f : 0.0f);
            t.qi = std::clamp(q + bonus, 0.0f, 1.0f);
        }
    }

    FindSettleSpot(w / 2, h / 2, town_x, town_y);
}

void GameMap::FindSettleSpot(int near_x, int near_y, int& out_x, int& out_y) const {
    // Spiral outward looking for hospitable ground
    for (int r = 0; r < std::max(width, height); ++r) {
        for (int dy = -r; dy <= r; ++dy) {
            for (int dx = -r; dx <= r; ++dx) {
                if (std::max(std::abs(dx), std::abs(dy)) != r) continue;
                int x = near_x + dx, y = near_y + dy;
                if (x < 1 || y < 1 || x >= width - 1 || y >= height - 1) continue;
                Biome b = BiomeAt(x, y);
                if (b == Biome::Grass || b == Biome::Forest) { out_x = x; out_y = y; return; }
            }
        }
    }
    out_x = width / 2;
    out_y = height / 2;
}
