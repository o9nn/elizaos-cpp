#include "markdown.hpp"
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



struct AnimatedMarkdownProps {
    std::string children;
    std::optional<std::string> className;
    std::optional<'user' | 'agent'> variant;
    std::optional<bool> shouldAnimate;
    std::optional<std::string> messageId;
};

  // Reset animation state when message changes
      // Estimate animation time based on text length (50ms per character roughly)

  // If not animating or animation is complete, render markdown

  // During animation, show AIWriter with plain text

} // namespace elizaos
