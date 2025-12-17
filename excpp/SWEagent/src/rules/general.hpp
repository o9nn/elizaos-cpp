#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * General coding guidelines project
 */

/**
 * Python coding rules (original project language)
 */

/**
 * TypeScript coding rules (for converted modules)
 */

/**
 * General coding guidelines configuration
 */

/**
 * TypeScript coding guidelines configuration
 */

/**
 * Helper function to validate code against rules
 */

  // Basic validation examples (extend as needed)
    // Check for os.path usage

    // Check for open() without pathlib

    // Check for any type usage

/**
 * Get applicable rules for a file
 */
std::vector<CodingRule> getApplicableRules(const std::string& filePath, std::optional<std::string> language);

} // namespace elizaos
