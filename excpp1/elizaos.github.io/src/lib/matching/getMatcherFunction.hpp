#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".data/tags.hpp"
#include "matching-logic.hpp"
#include "preprocessContent.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Simple cache implementation using a Map

/**
 * Creates a unique cache key for a TagPattern configuration.
 * Includes pattern, type, target, and all options.
 *
 * @param tagPattern The pattern configuration.
 * @returns A unique string key.
 */
std::string createCacheKey(TagPattern tagPattern);

/**
 * Gets a matcher function for the given pattern configuration.
 * Retrieves from cache if available, otherwise creates a new one, caches it, and returns it.
 *
 * @param tagPattern The pattern configuration.
 * @returns A matcher function `(content: MatchContent) => boolean`.
 */
MatcherFunction getMatcherFunction(TagPattern tagPattern);

/**
 * Clears the cache of matcher functions.
 */
void clearMatcherCache();

/**
 * Gets the current number of cached matcher functions.
 *
 * @returns The size of the cache.
 */
double getMatcherCacheSize(); /**
 * Type alias for a function that tests content against a specific pattern.
 */

using MatcherFunction = std::function<bool(MatchContent)>;
/**
 * Creates a matcher function for a given TagPattern configuration.
 * This function encapsulates the specific matching logic (string, regex, glob),
 * content preprocessing, and option handling (case sensitivity, word boundary, negation).
 *
 * @param tagPattern The pattern configuration.
 * @returns A function that takes MatchContent and returns true if it matches the pattern, false otherwise.
 * @throws Error if the pattern type is incompatible with the target type.
 */

MatcherFunction createMatcher(TagPattern tagPattern); /**
 * Validates if a pattern type is compatible with a given target content type.
 *
 * @param patternType The pattern type (STRING, REGEX, GLOB)
 * @param targetType The target content type
 * @returns True if compatible, false otherwise
 */

bool validatePatternForTarget(PatternType patternType, TagPatternType targetType);

} // namespace elizaos
