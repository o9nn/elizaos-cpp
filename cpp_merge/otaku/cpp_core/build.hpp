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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Build script for backend
 */

std::future<void> cleanBuild(auto outdir);

std::future<void> copySharedModules();

std::future<void> build();

// Execute the build


} // namespace elizaos
