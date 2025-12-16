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

import type { Plugin } from "@elizaos/core";
;
;
;
;

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
const envPlugin: Plugin = {
  name: "plugin-env",
  description:
    "Environment variable management with auto-generation and validation capabilities",

  services: [EnvManagerService],

  providers: [envStatusProvider],

  actions: [setEnvVarAction, generateEnvVarAction],

  init: async (config, runtime) => {
    // Initialize the environment manager service
    // The service will automatically scan for required environment variables
    // and set up the initial metadata structure
    // No additional initialization needed as the service handles everything
    // in its start() method
  },
};

default envPlugin;

// Export types for use by other plugins
type {
  EnvVarConfig,
  EnvVarMetadata,
  EnvVarUpdate,
  GenerationScript,
  GenerationScriptMetadata,
  ValidationResult,
} from "./types";

// Export utility functions
{
  canGenerateEnvVar,
  generateScript,
  getGenerationDescription,
} from "./generation";

{ validateEnvVar, validationStrategies } from "./validation";

// Export service for direct access if needed
{ EnvManagerService } from "./service";

} // namespace elizaos
