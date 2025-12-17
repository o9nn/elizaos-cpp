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
#include "response.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct AnimatedResponseProps {
    std::string children;
    std::optional<std::string> className;
    std::optional<bool> shouldAnimate;
    std::optional<std::string> messageId;
    std::optional<double> maxDurationMs;
};

      // Notify parent that text was updated so it can handle scrolling


} // namespace elizaos
