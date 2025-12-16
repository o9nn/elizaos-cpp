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
 * Load environment configuration for runtime
 *
 * Loads environment variables from the project's .env file and returns them as runtime settings.
 */
std::future<RuntimeSettings> loadEnvConfig();

} // namespace elizaos
