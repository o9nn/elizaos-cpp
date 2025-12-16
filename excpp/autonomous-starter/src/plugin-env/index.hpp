#include "actions/generateEnvVar.hpp"
#include "actions/setEnvVar.hpp"
#include "providers/envStatus.hpp"
#include "service.hpp"
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
 * Environment Variable Management Plugin
 *
 * This plugin provides comprehensive environment variable management for autonomous agents:
 * - Automatic detection of required environment variables from plugins
 * - Auto-generation of variables that can be created programmatically (keys, secrets, etc.)
 * - User interaction for variables that require manual input (API keys, etc.)
 * - Validation of environment variables to ensure they work correctly
 * - Persistent storage in world metadata following the same pattern as settings
 */

    // Initialize the environment manager service
    // The service will automatically scan for required environment variables
    // and set up the initial metadata structure
    // No additional initialization needed as the service handles everything
    // in its start() method

// Export types for use by other plugins

// Export utility functions

// Export service for direct access if needed

} // namespace elizaos
