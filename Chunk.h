#pragma once
#include <iostream>
#include <vector>
#include <bitset>
#include "utils.h"
#include "FastNoiseLite.h"

typedef uint16_t Node;

class Chunk {
public:
	Chunk(SM::vec3<int> _pos);
	void traverse_chunk(size_t& index, int level, int quadrant) const;
	void parse_chunk(size_t& index, int level, uint8_t quadrant, 
		std::vector<SM::DrawParams>& param_sets, SM::vec3<float> prev_origin) const;
	std::vector<SM::DrawParams> get_parsed_chunk() const;
	std::vector<Node> get_chunk();

	/*
	voxel coord = chunk_size - () ?
	*/
	void insert(uint16_t data, SM::vec3<int> pos, int level);
	SM::vec3<int> get_pos() const { return pos; }

private:
	SM::vec3<int> pos;
	Node* get_insertion_node(SM::vec3<int> range_start, SM::vec3<int> range_end, 
		SM::vec3<int> pos, int fin_level, int curr_level, int node_idx);
	int count_children(int node_idx);
	int get_child_offset(int node_idx, uint8_t child_bit);

	std::vector<Node> chunk;
};

