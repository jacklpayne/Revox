#include "Chunk.h"

Chunk::Chunk(SM::vec3<int> _pos) : pos(_pos) {
    chunk.push_back({ 0b0000000000000000 });
}

void Chunk::traverse_chunk(size_t& index, int level, int quadrant) const {
    if (index >= chunk.size()) return;

    const Node& node = chunk[index];
    bool leaf = (node & 0x8000);
    uint16_t mask_or_data = (node & 0x7FFF);

    // Print node
    std::cout << std::string(level * 2, ' ')
        << "- Level: " << level
        << ", Quadrant: " << quadrant
        << ", Is Leaf: " << (leaf ? "true" : "false")
        << ", Mask/Data: " << std::bitset<16>(mask_or_data)
        << '\n';

    index++;

    if (leaf) return;
    for (int child_quadrant = 0; child_quadrant < 8; ++child_quadrant) {
        if (mask_or_data & (1 << child_quadrant)) {
            traverse_chunk(index, level + 1, child_quadrant + 1);
        }
    }
}

void Chunk::parse_chunk(size_t& index, int level, uint8_t quadrant, 
    std::vector<SM::DrawParams>& param_sets, SM::vec3<float> prev_origin) const {

    if (index >= chunk.size()) return;

    const Node& node = chunk[index];
    bool is_leaf = (node & 0x8000);
    uint16_t mask_or_data = (node & 0x7FFF);

    index++;

    // Position relative to the parent octant
    SM::vec3<float> rel_pos{0,0,0};
    if (level != 0) {
        rel_pos = {
               (quadrant & 0b001) ? 1.0f : -1.0f, // X from bit 0
               (quadrant & 0b010) ? 1.0f : -1.0f, // Y from bit 1
               (quadrant & 0b100) ? 1.0f : -1.0f  // Z from bit 2
        };
    };

    float size_mult = (level == 0) ? SM::STANDARD_CHUNK_SIZE : (float)SM::STANDARD_CHUNK_SIZE / (std::pow(2,level));
    SM::vec3<float> origin = (rel_pos * size_mult * 0.5f) + prev_origin;

    if (is_leaf) {
        uint16_t color_bits = node & 0x1FF;  // 111111111
        // Extract RGB components
        uint8_t r = (color_bits >> 6) & 0x7; // Get the top 3 bits (RRR)
        uint8_t g = (color_bits >> 3) & 0x7; // Get the middle 3 bits (GGG)
        uint8_t b = color_bits & 0x7;         // Get the lowest 3 bits (BBB)

        float r_float = static_cast<float>(r) / 7.0f;
        float g_float = static_cast<float>(g) / 7.0f;
        float b_float = static_cast<float>(b) / 7.0f;
        param_sets.push_back({ origin, SM::vec3<float>{r_float, g_float, b_float}, size_mult });
        return;
    };   
    for (uint8_t child_quadrant = 0; child_quadrant < 8; ++child_quadrant) {
        if (mask_or_data & (1 << child_quadrant)) {
            parse_chunk(index, level + 1, child_quadrant, param_sets, origin);
        }
    }
}

// std::move?
std::vector<SM::DrawParams> Chunk::get_parsed_chunk() const {
    std::vector<SM::DrawParams> parsed_chunk{};
    size_t index = 0;
    SM::vec3<float> recursive_pos;
    parse_chunk(index, 0, 7, parsed_chunk, {0,0,0});

    return parsed_chunk;
}

std::vector<Node> Chunk::get_chunk() {
    return chunk;
}

void Chunk::insert(uint16_t data, SM::vec3<int> pos, int level) {
    // The range (length in octants) at the target level of recursion
    int range_end = std::pow(2, level);
    SM::vec3<int> range_vec = SM::vec3<int>{ range_end, range_end, range_end };

    Node* node = get_insertion_node(SM::vec3<int>{0,0,0}, range_vec, pos, level, 0, 0);
    *node = data;
}

Node* Chunk::get_insertion_node(SM::vec3<int> range_start, SM::vec3<int> range_end, SM::vec3<int> pos, 
    int fin_level, int curr_level, int node_idx) {
    if (fin_level == curr_level) {
        return &chunk[node_idx];
    }

    // If the position along the given axis is in the second half of the range mapped to the current octant,
    // return a 1
    auto get_octant = [](int pos, int start, int end) -> bool {
        return pos >= (end + start)/2;
    };
    bool in_right_octant = get_octant(pos.x, range_start.x, range_end.x);
    bool in_upper_octant = get_octant(pos.y, range_start.y, range_end.y);
    bool in_front_octant = get_octant(pos.z, range_start.z, range_end.z);

    // The standard index of the next octant at the current level
    // Also the nth bit of the child bitmask of the present node
    // corresponding to the target octant
    uint8_t child_bit = (in_front_octant << 2) | 
                        (in_upper_octant << 1) | 
                        (in_right_octant);

    // Convert to branch if leaf
    bool is_leaf = (chunk[node_idx] & 0x8000);
    if (is_leaf) {
        Node leaf_data = chunk[node_idx];
        chunk[node_idx] = 0x00FF;
        // Insert eight new children with the same leaf data
        std::vector<uint16_t> new_children(8, leaf_data);
        chunk.insert(chunk.begin() + node_idx + 1, new_children.begin(), new_children.end());
    }

    int child_offset = get_child_offset(node_idx, child_bit);

    bool child_is_empty = (chunk[node_idx] & (1 << child_bit)) == 0;
    if (child_is_empty) {
        chunk[node_idx] |= (1 << child_bit); // Set child bit equal to 1
        chunk.insert(chunk.begin() + node_idx + child_offset, 0x0000); // Insert a new empty child branch node
    }
    node_idx += child_offset;
 
    if (in_right_octant) {
        range_start.x = (range_start.x + range_end.x)/2;
    }
    else {
        range_end.x = (range_start.x + range_end.x) / 2;
    }
    if (in_upper_octant) {
        range_start.y = (range_start.y + range_end.y) / 2;
    }
    else {
        range_end.y = (range_start.y + range_end.y) / 2;
    }
    if (in_front_octant) {
        range_start.z = (range_start.z + range_end.z) / 2;
    }
    else {
        range_end.z = (range_start.z + range_end.z) / 2;
    }

    return get_insertion_node(range_start, range_end, pos, fin_level, curr_level+1, node_idx);
}

int Chunk::count_children(int node_idx) {
    int count = 1; // Count this node itself
    bool is_leaf = (chunk[node_idx] & 0x8000);
    if (!is_leaf) {
        int child_mask = chunk[node_idx] & 0xFF;
        int child_count = __popcnt(child_mask); // Number of non-empty children
        int offset = 1; // Start after the current node
        for (int i = 0; i < 8; ++i) {
            if (child_mask & (1 << i)) { // Check if the i-th child exists
                count += count_children(node_idx + offset); // Add child count
                offset += count_children(node_idx + offset); // Move to the next child
            }
        }
    }
    return count;
}

int Chunk::get_child_offset(int node_idx, uint8_t child_bit) {
    int offset = 1; // Start after the current node
    int child_mask = chunk[node_idx] & 0xFF;
    for (int i = 0; i < child_bit; ++i) {
        if (child_mask & (1 << i)) {
            offset += count_children(node_idx + offset); // Add size of child subtree
        }
    }
    return offset;
}

