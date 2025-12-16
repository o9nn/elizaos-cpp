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
 * File utilities
 * Converted from sweagent/utils/files.py
 */

;
;
;

/**
 * Load a file (JSON or YAML)
 */


  const ext = path.extname(filepath).toLowerCase();
  const content = fs.readFileSync(filepath, 'utf-8');

  if (ext === '.json') {
    return JSON.parse(content);
  } else if (ext === '.yaml' || ext === '.yml') {
    return yaml.load(content);
  } else {
    // Try to parse as JSON first, then YAML
    try {
      return JSON.parse(content);
    } catch {
      try {
        return yaml.load(content);
      } catch {
        // Return raw content if neither works
        return content;
      }
    }
  }
}

} // namespace elizaos
