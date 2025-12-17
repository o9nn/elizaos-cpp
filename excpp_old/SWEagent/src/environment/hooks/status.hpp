#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include ".repo.hpp"
#include "abstract.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Status environment hook
 * Converted from sweagent/environment/hooks/status.py
 */

/**
 * Status update callback type
 */
using StatusCallback = std::function<void(std::string, std::string)>;

/**
 * Environment hook that updates status messages
 */
class SetStatusEnvironmentHook {
public:
    SetStatusEnvironmentHook(const std::string& id, StatusCallback callable);
    void update(const std::string& message);
    void onCopyRepoStarted(const std::variant<Repo, RepoConfig>& repo);
    void onStartDeployment();
    void onInstallEnvStarted();
    void onEnvironmentStartup();
    void onClose();

private:
    StatusCallback callable_;
    std::string id_;
};


} // namespace elizaos
