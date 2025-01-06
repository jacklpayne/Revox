#pragma once
#include <random>
#include <chrono>

#include "Chunk.h"
#include "FastNoiseLite.h"
class World {
public:
	World(int render_distance);
	void generate_world(int LOD);
	std::vector<std::vector<SM::DrawParams>> get_parsed_world();
	std::vector<SM::DrawParams> get_parsed_chunk(int idx);

	void init_world();
	void set_render_distance(int _render_distance) { render_distance = _render_distance; }

private:
	std::vector<Chunk> world;
	int render_distance;
};

