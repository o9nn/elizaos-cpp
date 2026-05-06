#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"
#include "elizaos/server.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * TypeScript Example: Standalone Server Usage
 *
 * This example demonstrates how to use @elizaos/server as a standalone package
 * to create a custom agent server without the CLI dependency.
 */

// Custom middleware example

// Server configuration

std::future<void> createStandaloneServer();

std::future<void> startServer();

// Export for programmatic usage

// Direct execution

} // namespace elizaos
