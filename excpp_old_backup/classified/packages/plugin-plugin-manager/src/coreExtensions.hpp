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
void extendRuntimeWithEventUnregistration(IAgentRuntime runtime);

/**
 * Extends the runtime with component unregistration methods
 * These are needed for proper plugin unloading
 */
void extendRuntimeWithComponentUnregistration(IAgentRuntime runtime);

/**
 * Apply all runtime extensions
 */
void applyRuntimeExtensions(IAgentRuntime runtime);

} // namespace elizaos
