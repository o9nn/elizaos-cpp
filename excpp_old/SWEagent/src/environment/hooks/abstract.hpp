#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include ".repo.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Environment hooks
 * Converted from sweagent/environment/hooks/abstract.py
 */

// Environment interface for hooks
struct EnvironmentInstance {
    std::optional<unknown> deployment;
    std::optional<std::optional<Repo>> repo;
};

/**
 * Abstract environment hook interface
 */
    void onInit(EnvironmentInstance _env);
    void onCopyRepoStarted(const std::variant<Repo, RepoConfig>& _repo);
    void onStartDeployment();
    void onInstallEnvStarted();
    void onClose();
    void onEnvironmentStartup();

/**
 * Combined environment hooks - allows multiple hooks to be registered
 */
class CombinedEnvHooks {
public:
    void addHook(EnvHook hook);
    void onInit(EnvironmentInstance env);
    void onCopyRepoStarted(const std::variant<Repo, RepoConfig>& repo);
    void onStartDeployment();
    void onInstallEnvStarted();
    void onClose();
    void onEnvironmentStartup();
};


} // namespace elizaos
