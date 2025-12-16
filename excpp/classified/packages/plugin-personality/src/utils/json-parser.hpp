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
 * Helper  else if (cleaned.startsWith('```')) {
    cleaned = cleaned.substring(3);
  }

  // Remove trailing ``` if present
  if (cleaned.endsWith('```')) {
    cleaned = cleaned.substring(0, cleaned.length - 3);
  }

  // Trim any remaining whitespace
  cleaned = cleaned.trim();

  // Try to parse the JSON
  try {
    return JSON.parse(cleaned);
  } catch (error) {
    // If parsing fails, try to find JSON object in the response
    const jsonMatch = response.match(/\{[\s\S]*\}/);
    if (jsonMatch) {
      return JSON.parse(jsonMatch[0]);
    }
    throw error;
  }
}

} // namespace elizaos
