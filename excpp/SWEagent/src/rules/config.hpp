#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "general.hpp"
#include "project-overview.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Complete rules configuration
 * This file consolidates all rules into a single configuration object
 */

/**
 * Cursor IDE rules converted to TypeScript format
 */

/**
 * Complete rules configuration
 */

/**
 * Helper function to rules to Cursor IDE format
 */
std::string exportToCursorFormat(Rule rule);

/**
 * Convert all rules to Cursor IDE format
 */
std::unordered_map<std::string, std::string> exportAllRulesToCursor();

/**
 * Get configuration for a specific language
 */
void getLanguageConfig(const std::variant<'python', 'typescript'>& language);

/**
 * Check if a file should have rules applied based on globs
 */
bool shouldApplyRules(const std::string& filePath, Rule rule);

} // namespace elizaos
