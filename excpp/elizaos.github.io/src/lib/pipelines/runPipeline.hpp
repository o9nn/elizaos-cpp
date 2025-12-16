#include "types.hpp"
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
 * Pipeline runner - executes a pipeline with config
 */

);

      return result;
    })
    .catch((error: unknown) => {
      if (error instanceof Error) {
        context.logger?.error("Pipeline failed", {
          error: error.message,
          stack: error.stack,
        });
      } else {
        context.logger?.error("Pipeline failed with unknown error", {
          error,
        });
      }
      throw error;
    });
}

} // namespace elizaos
