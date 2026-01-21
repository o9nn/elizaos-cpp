#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


// Types
using BirdeyeSupportedChain = (typeof BIRDEYE_SUPPORTED_CHAINS)[number];

struct BaseAddress {
    std::optional<std::string> type;
    std::optional<std::string> symbol;
    std::string address;
    BirdeyeSupportedChain chain;
};


} // namespace elizaos
