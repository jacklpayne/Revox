#include "World.h"

World::World(int _render_distance) : render_distance(_render_distance) {
    // Start with the origin
    std::vector<SM::vec3<int>> spiral_coordinates;
    spiral_coordinates.push_back(SM::vec3<int>{0, 0, 0});

    // Generate coordinates ring by ring
    for (int ring = 1; ring < render_distance; ++ring) {
        for (int x = -ring; x <= ring; ++x) {
            for (int z = -ring; z <= ring; ++z) {
                if (std::max(abs(x), abs(z)) == ring) {
                    spiral_coordinates.push_back(SM::vec3<int>{x, 0, z});
                }
            }
        }
    }

    // Populate chunks in spiral order
    for (const auto& coord : spiral_coordinates) {
        world.push_back(Chunk{ coord });
    }
    generate_world(5);
}

void World::generate_world(int LOD) {
    // Create and configure FastNoise object
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetFrequency(0.005f);
    for (auto& chunk : world) {
        int size = std::pow(2, LOD);
        float x_start = chunk.get_pos().x * SM::STANDARD_CHUNK_SIZE * 10;
        float z_start = chunk.get_pos().z * SM::STANDARD_CHUNK_SIZE * 10;
        float x_step = (SM::STANDARD_CHUNK_SIZE * 10) / size;
        float z_step = (SM::STANDARD_CHUNK_SIZE * 10) / size;

        for (int x = 0; x < size; ++x) {
            for (int z = 0; z < size; ++z) {
                float x_sample = x_start + x * x_step;
                float z_sample = z_start + z * z_step;
                int y = noise.GetNoise(x_sample, z_sample) * 10;
                if (y <= 3) {
                    chunk.insert(0b1001011000111000, SM::vec3<int>{x, y, z}, LOD);
                }
                else if (y <= 7) {
                    
                    chunk.insert(0b1001011101101000, SM::vec3<int>{x, y, z}, LOD);
                }
                else if (y <= 10) {
                    chunk.insert(0b1001011000101101, SM::vec3<int>{x, y, z}, LOD);
                }
                
            }
        }
    }
}

std::vector<SM::DrawParams> World::get_parsed_world() {
    std::vector<SM::DrawParams>parsed_world;
    for (const auto& chunk : world) {
        auto parsed_chunk = chunk.get_parsed_chunk();
        // Translate from chunk coordinates to world coordinates
        for (auto& c : parsed_chunk) {
           c.origin.x += (float)(chunk.get_pos().x * SM::STANDARD_CHUNK_SIZE);
           c.origin.y += (float)(chunk.get_pos().y * SM::STANDARD_CHUNK_SIZE);
           c.origin.z += (float)(chunk.get_pos().z * SM::STANDARD_CHUNK_SIZE);
        }
        parsed_world.insert(parsed_world.end(), parsed_chunk.begin(), parsed_chunk.end());
    }
    return parsed_world;
}

std::vector<SM::DrawParams> World::get_parsed_chunk(int idx) {
    return world[idx].get_parsed_chunk();
}
