#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * TagPatternType defines the different types of pattern matching
 */
enum TagPatternType {
}

/**
 * TagCategory defines the main categories for tag classification
 */
enum TagCategory {
}

// Zod schemas for validation

/**
 * Schema for tag pattern scoring mechanics
 */

/**
 * Schema for a tag pattern that defines how to match activities to tags
 */

/**
 * Schema for a tag rule that groups patterns for a specific skill/area
 */

// Type exports from zod schemas
using TagScoring = z::infer<typeof TagScoringSchema>;
using TagPattern = z::infer<typeof TagPatternSchema>;
using TagRule = z::infer<typeof TagRuleSchema>;

} // namespace elizaos
