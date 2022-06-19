#pragma once

#include <memory>

namespace muon::text {

// TODO: don't use shared pointer, we want to use a
// chunk allocator for faster insertions
struct RopeNode {
    std::shared_ptr<char[]> data;
    size_t size;
    uint32_t weight;
    std::shared_ptr<RopeNode> left;
    std::shared_ptr<RopeNode> right;
    std::shared_ptr<RopeNode> parent;
};

class Rope {
    
};

}
