#include "connection.hpp"
#include "elizaos/core.hpp"
#include "schema.hpp"
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
;
;

std::future<void> runMigrations(); catch (error) {
    logger.error('[MessageDB] Failed to run migrations:', error);
    throw new Error(
      `Database migration failed: ${error instanceof Error ? error.message : String(error)}`
    );
  }
}

} // namespace elizaos
