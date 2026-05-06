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



// Registry integration constants

/**
 * Update the registry index with the package information
 */
std::future<bool> updateRegistryIndex(PackageMetadata packageMetadata, auto dryRun = false);

/**
 * Save package metadata to registry
 */
std::future<bool> savePackageToRegistry(PackageMetadata packageMetadata, auto dryRun = false);

} // namespace elizaos
