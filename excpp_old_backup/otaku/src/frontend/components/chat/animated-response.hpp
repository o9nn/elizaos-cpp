#include "response.hpp"
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



struct AnimatedResponseProps {
    std::string children;
    std::optional<std::string> className;
    std::optional<bool> shouldAnimate;
    std::optional<std::string> messageId;
    std::optional<double> maxDurationMs;
    std::optional<() => void> onTextUpdate;
};

      // Notify parent that text was updated so it can handle scrolling


} // namespace elizaos
