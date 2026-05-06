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

/**
 * Utility functions for parsing JSON from LLM responses
 */

/**
 * Helper function to extract JSON from LLM responses that might be wrapped in markdown
 * @param response - The raw LLM response that may contain JSON
 * @returns The parsed JSON object
 * @throws Error if JSON cannot be parsed
 */
std::any extractJsonFromResponse(const std::string& response);

} // namespace elizaos
