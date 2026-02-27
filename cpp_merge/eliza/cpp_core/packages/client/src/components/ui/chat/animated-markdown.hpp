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
#include "markdown.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct AnimatedMarkdownProps {
    std: children;
    std::optional<std:> className;
    std::optional<std:> std::variant;
    std::optional<bool> shouldAnimate;
    std::optional<std:> messageId;
};

  // Reset animation state when message changes
      // Estimate animation time based on text length (50ms per character roughly)

  // If not animating or animation is complete, render markdown

  // During animation, show AIWriter with plain text

} // namespace elizaos
