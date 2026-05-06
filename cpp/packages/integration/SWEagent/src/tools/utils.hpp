#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "commands.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Tool utilities
 * Converted from sweagent/tools/utils.py
 */

/**
 * Guard multiline input
 */

  // Handle multiline input by ensuring proper formatting

/**
 * Check if a value should be quoted
 */
bool shouldQuote(const std::any& value, Command command);

/**
 * Get command signature
 */
std::string getSignature(Command cmd);

/**
 * Generate command documentation
 */
std::string generateCommandDocs(const std::vector<Command>& commands, const std::vector<std::string>& subroutineTypes, const std::unordered_map<std::string, std::any>& _kwargs);

/**
 * Format a single command for documentation
 */
std::string formatCommand(Command cmd);

} // namespace elizaos
