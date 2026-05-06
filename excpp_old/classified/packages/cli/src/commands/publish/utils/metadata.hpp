#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Generate package metadata for the registry
 */
std::future<PackageMetadata> generatePackageMetadata(PackageJson packageJson, const std::string& cliVersion, const std::string& username);

} // namespace elizaos
