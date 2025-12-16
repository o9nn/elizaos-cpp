#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct ChainSelectorProps {
    std::vector<Chain> selected;
    (chains: Chain[]) => void onChange;
};


} // namespace elizaos
