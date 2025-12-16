#include "elizaos/core.hpp"
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

/**
 * Core Runtime Extensions
 *
 * This module provides extensions to the core runtime for plugin management.
 * Since we cannot modify the core runtime directly, we extend it with additional
 * methods needed for proper plugin lifecycle management.
 */

/**
 * Extends the runtime with an unregisterEvent method
 * This allows plugins to remove their event handlers when unloaded
 */
 else {
          this.events.delete(event);
        }
      }
    };
  }
}

/**
 * Extends the runtime with component unregistration methods
 * These are needed for proper plugin unloading
 */

    };
  }

  // Add unregisterProvider method if it doesn't exist
  if (!extendedRuntime.unregisterProvider) {
    extendedRuntime.unregisterProvider = function (providerName: string) {
      const index = this.providers.findIndex((p) => p.name === providerName);
      if (index !== -1) {
        this.providers.splice(index, 1);
      }
    };
  }

  // Add unregisterEvaluator method if it doesn't exist
  if (!extendedRuntime.unregisterEvaluator) {
    extendedRuntime.unregisterEvaluator = function (evaluatorName: string) {
      const index = this.evaluators.findIndex((e) => e.name === evaluatorName);
      if (index !== -1) {
        this.evaluators.splice(index, 1);
      }
    };
  }

  // Add unregisterService method if it doesn't exist
  if (!extendedRuntime.unregisterService) {
    extendedRuntime.unregisterService = async function (serviceType: string) {
      const service = this.services.get(serviceType);
      if (service) {
        await service.stop();
        this.services.delete(serviceType);
      }
    };
  }
}

/**
 * Apply all runtime extensions
 */


} // namespace elizaos
