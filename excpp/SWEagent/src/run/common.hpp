#include ".types.hpp"
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
 * Common utilities for run module
 * Converted from sweagent/run/common.py
 */

;
;
;

/**
 * Shorten a string to a maximum length
 */


  if (shortenLeft) {
    return '...' + s.slice(s.length - maxLength + 3);
  } else {
    return s.slice(0, maxLength - 3) + '...';
  }
}

/**
 * Shorten strings in a nested object/array
 */


  if (Array.isArray(data)) {
    return data.map((item) => shortenStrings(item, maxLength));
  }

  if (data && typeof data === 'object') {
    const result: Record<string, unknown> = {};
    for (const [key, value] of Object.entries(data)) {
      result[key] = shortenStrings(value, maxLength);
    }
    return result;
  }

  return data;
}

/**
 * Save predictions from agent run result
 */
;
  if (fs.existsSync(predPath)) {
    const content = fs.readFileSync(predPath, 'utf-8');
    predictions = JSON.parse(content);
  }

  // Add/update prediction for this instance
  predictions[instanceId] = {
    model_patch: result.info.submission || '',
    model_name_or_path: result.info.modelStats?.model || 'unknown',
    cost: result.info.modelStats?.instanceCost || 0,
    api_calls: result.info.modelStats?.apiCalls || 0,
    instance_id: instanceId,
  };

  // Save predictions
  fs.writeFileSync(predPath, JSON.stringify(predictions, null, 2));
}

/**
 * Check if a patch is promising (not empty/trivial)
 */


  // Check if patch is empty or only whitespace
  if (submission.trim() === '') {
    return false;
  }

  // Check if patch only contains diff headers but no actual changes
  const lines = submission.split('\n');
  let hasChanges = false;

  for (const line of lines) {
    if (line.startsWith('+') && !line.startsWith('+++')) {
      hasChanges = true;
      break;
    }
    if (line.startsWith('-') && !line.startsWith('---')) {
      hasChanges = true;
      break;
    }
  }

  return hasChanges;
}

/**
 * Create a nested dictionary structure
 */
,
    {
      get: (target: Record<string, unknown>, prop: string) => {
        if (!(prop in target)) {
          target[prop] = createNestedDict();
        }
        return target[prop];
      },
    },
  );
}

/**
 * Parse command-line arguments into nested dictionary
 */
;
        }
        current = current[keys[i]] as Record<string, unknown>;
      }

      // Try to parse value as JSON, number, or boolean
      let parsedValue: unknown = value;
      try {
        parsedValue = JSON.parse(value);
      } catch {
        if (value === 'true') {
          parsedValue = true;
        } else if (value === 'false') {
          parsedValue = false;
        } else if (!isNaN(Number(value))) {
          parsedValue = Number(value);
        }
      }

      current[keys[keys.length - 1]] = parsedValue;
    }
  }

  return result;
}

} // namespace elizaos
