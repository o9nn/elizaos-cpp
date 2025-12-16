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
 * Configuration utilities
 * Converted from sweagent/utils/config.py
 */

;
;
;
;

/**
 * Convert path relative to repository root
 */

  const rootPath = root || process.cwd();
  return typeof inputPath === 'string'
    ? path.resolve(rootPath, inputPath)
    : path.parse(path.resolve(rootPath, path.format(inputPath)));
}

/**
 * Check if a value could be a path
 */

  return value.includes('/') || value.includes('\\') || value.includes('.');
}

/**
 * Strip absolute paths from dictionary
 */

    return value;
  }

  if (Array.isArray(value)) {
    return value.map((v) => stripAbspathFromDict(v, root));
  }

  if (value && typeof value === 'object') {
    const result: Record<string, unknown> = {};
    for (const [k, v] of Object.entries(value)) {
      result[k] = stripAbspathFromDict(v, root);
    }
    return result;
  }

  return value;
}

/**
 * Convert path to absolute path
 */

    return path.parse(path.resolve(inputPath));
  }
  return inputPath;
}

/**
 * Convert paths array to absolute paths
 */


/**
 * Load environment variables from .env file
 */
);
  } else {
    dotenv.config();
  }
}

/**
 * Parse config file content
 */
 else if (format === 'json') {
    return JSON.parse(content);
  } else {
    throw new Error(`Unsupported format: ${format}`);
  }
}

/**
 * Merge two config objects deeply
 */
;

  for (const key in overrideConfig) {
    if (overrideConfig.hasOwnProperty(key)) {
      if (
        typeof overrideConfig[key] === 'object' &&
        overrideConfig[key] !== null &&
        !Array.isArray(overrideConfig[key]) &&
        typeof baseConfig[key] === 'object' &&
        baseConfig[key] !== null &&
        !Array.isArray(baseConfig[key])
      ) {
        // Recursively merge nested objects
        result[key] = mergeConfigs(baseConfig[key], overrideConfig[key]);
      } else {
        // Replace value
        result[key] = overrideConfig[key];
      }
    }
  }

  return result;
}

/**
 * Validate config structure
 */


  if (config.agent && typeof config.agent !== 'object') {
    throw new Error('Config agent property must be an object');
  }

  // Add more validation as needed
}

} // namespace elizaos
