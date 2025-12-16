#include "build.config.hpp"
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
 * Build script using bun build
 * Replaces tsup with native bun build functionality
 */

std::future<void> build();


} // namespace elizaos
