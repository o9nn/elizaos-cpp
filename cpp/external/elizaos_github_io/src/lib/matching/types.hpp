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
#include "data/tags.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Enum for pattern matching method types
 */
enum PatternType {
}

/**
 * Interface for pattern matching options
 */
struct PatternOptions {
    std::optional<bool> caseSensitive;
    std::optional<bool> wordBoundary;
    std::optional<bool> negative;
};

/**
 * Interface defining a pattern to be matched against content
 */
struct TagPattern {
    std::string pattern;
    PatternType patternType;
    TagPatternType target;
    PatternOptions options;
};

/**
 * Type for content that can be matched
 */
using MatchContent = {

} // namespace elizaos
