#include "...package.json.hpp"
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

struct VersionInfo {
    std::string version;
    std::string source;
    std::string timestamp;
    std::string environment;
    double uptime;
    std::optional<std::string> error;
};


/**
 * Gets version information using CLI-compatible logic
 */
;
  } catch (error) {
    console.error('Error getting version info:', error);

    return {
      version: 'unknown',
      source: 'server',
      timestamp,
      environment: process.env.NODE_ENV || 'development',
      uptime: process.uptime(),
      error: 'Failed to retrieve version information',
    };
  }
}

/**
 * Creates the version router for system version information
 */
);

  return router;
}

} // namespace elizaos
