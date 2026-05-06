#include "...utils/bun-exec.js.hpp"
#include ".types.hpp"
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
 * Build and publish package to npm
 */
std::future<void> publishToNpm(const std::string& cwd, PackageJson packageJson, const std::string& npmUsername);

} // namespace elizaos
