#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



std::future<void> main();

std::future<void> updateWorkflowFile(Octokit octokit, const std::string& repoName, const std::string& newWorkflowContent);

std::future<void> updatePackageJsonDependencies(Octokit octokit, const std::string& repoName);

std::future<void> removeBunLockfile(Octokit octokit, const std::string& repoName);


} // namespace elizaos
