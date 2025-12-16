#include "elizaos/core-plugin-v2.hpp"
#include "state.hpp"
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

;
;

/**
 * Template type definition for v1 compatibility
 * A template can be either a string or a ) => string)>;

/**
 * Generic template values interface for typed access to state.values
 * Users can extend this interface for type safety in their templates
 */
struct TemplateValues {
};


/**
 * Create a template  else {
    // For 
      return template({ state });
    };
  }
}

/**
 * Process a template with the given state
 * @param template The template to process (string or function)
 * @param state The state to use for processing
 * @returns The processed template string
 */


  // Handle null/undefined state
  if (!state) {
    return typeof template === 'string' ? template : '';
  }

  if (typeof template === 'string') {
    return template;
  } else {
    return template({ state });
  }
}

/**
 * Type-safe accessor for template values
 * @param state The state containing the values
 * @param defaultValues Optional default values to use if values are missing
 * @returns The values object with type information
 */
) as T;
  }

  // First cast state.values to a valid object type to use with spread
  const stateValues = state.values as Record<string, unknown>;
  const defaults = defaultValues || ({} as Partial<T>);

  // Create a new object with both default values and state values
  return { ...defaults, ...stateValues } as T;
}

} // namespace elizaos
