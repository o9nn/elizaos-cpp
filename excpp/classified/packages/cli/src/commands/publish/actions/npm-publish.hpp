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
#include "types.hpp"
#include "utils/bun-exec.js.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Build and publish package to npm
 */
std::future<void> publishToNpm(const std::string& cwd, PackageJson packageJson, const std::string& npmUsername);

} // namespace elizaos
