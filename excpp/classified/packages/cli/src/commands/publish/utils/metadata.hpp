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

;

/**
 * Generate package metadata for the registry
 */
std::future<PackageMetadata> generatePackageMetadata(PackageJson packageJson, const std::string& cliVersion, const std::string& username);,
    tags: packageJson.keywords || [],
    license: packageJson.license || 'UNLICENSED',
  };

  // Add npm or GitHub specific data
  if (packageJson.npmPackage) {
    metadata.npmPackage = packageJson.npmPackage;
  }

  if (packageJson.githubRepo) {
    metadata.githubRepo = packageJson.githubRepo;
  }

  // Ensure appropriate tag is included based on type
  if (metadata.type === 'plugin' && !metadata.tags.includes('plugin')) {
    metadata.tags.push('plugin');
  } else if (metadata.type === 'project' && !metadata.tags.includes('project')) {
    metadata.tags.push('project');
  }

  return metadata;
}

} // namespace elizaos
