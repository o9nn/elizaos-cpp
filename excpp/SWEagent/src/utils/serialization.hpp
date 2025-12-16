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
 * Serialization utilities
 * Converted from sweagent/utils/serialization.py
 */

;

/**
 * Convert strings to YAML literal strings for better readability
 */


  if (Array.isArray(data)) {
    return data.map((item) => convertToYamlLiteralString(item));
  }

  if (data && typeof data === 'object') {
    const result: Record<string, unknown> = {};
    for (const [key, value] of Object.entries(data)) {
      result[key] = convertToYamlLiteralString(value);
    }
    return result;
  }

  return data;
}

/**
 * Serialize data to YAML with line breaks preserved
 */
,
  });
}

/**
 * Merge nested dictionaries/objects
 */

  if (!d2) {
    return d1;
  }

  // If either is not an object, return d2 (override)
  if (typeof d1 !== 'object' || typeof d2 !== 'object') {
    return d2;
  }

  // If either is an array, return d2 (override)
  if (Array.isArray(d1) || Array.isArray(d2)) {
    return d2;
  }

  // Merge objects
  const result: Record<string, unknown> = { ...(d1 as Record<string, unknown>) };

  for (const [key, value] of Object.entries(d2)) {
    if (key in result) {
      result[key] = mergeNestedDicts(result[key], value);
    } else {
      result[key] = value;
    }
  }

  return result;
}

} // namespace elizaos
