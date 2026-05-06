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
 * Publish package to GitHub and optionally to registry
 */
std::future<PublishResult> publishToGitHubAction(const std::string& cwd, PackageJson packageJson, Credentials credentials, boolean = false skipRegistry, boolean = false dryRun);

} // namespace elizaos
