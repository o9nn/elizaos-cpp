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
#include "getMatcherFunction.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Matches a piece of content against a list of patterns and returns the patterns that matched.
 * Uses a cache for matcher functions for efficiency.
 * Handles different pattern types, content types, options, and negation implicitly
 * through the cached matcher functions.
 *
 * @param patterns An array of TagPattern configurations.
 * @param content The MatchContent object to test against the patterns.
 * @returns An array containing the TagPattern objects that matched the content.
 */
std::vector<TagPattern> matchPatterns(const std::vector<TagPattern>& patterns, MatchContent content);

/**
 * Checks if *any* pattern in the list matches the given content.
 * More efficient than calling matchPatterns and checking the length if only existence is needed.
 *
 * @param patterns An array of TagPattern configurations.
 * @param content The MatchContent object to test against the patterns.
 * @returns True if at least one pattern matches the content, false otherwise.
 */
bool matchAnyPattern(const std::vector<TagPattern>& patterns, MatchContent content);

} // namespace elizaos
