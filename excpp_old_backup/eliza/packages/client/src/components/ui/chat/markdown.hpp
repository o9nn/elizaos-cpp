#include "code-block.hpp"
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



struct MarkdownProps {
    std::string children;
    std::optional<std::string> className;
    std::optional<std::unordered_map<std::string, React.ComponentType<any>>> components;
    std::optional<'user' | 'agent'> variant;
};

  // Code blocks
    // Check if this contains a code element with a language class

      // Find the code element

    // Regular pre block (not code)

  // Inline code
    // If this has a language class, it's likely part of a code block handled by pre above

      // Let the pre component handle this

    // Regular inline code

  // Headers

  // Paragraphs

  // Lists

  // Links

  // Blockquotes

  // Tables

  // Images

  // Horizontal rule

  // Strong/Bold

  // Emphasis/Italic


} // namespace elizaos
