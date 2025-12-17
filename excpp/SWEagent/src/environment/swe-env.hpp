#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include ".agent/agents.hpp"
#include ".utils/log.hpp"
#include "deployment.hpp"
#include "hooks/abstract.hpp"
#include "repo.hpp"
#include "runtime.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Main environment class
 * Converted from sweagent/environment/swe_env.py
 */

/**
 * Environment configuration
 */

using EnvironmentConfig = z::infer<typeof EnvironmentConfigSchema>;

/**
 * Main SWE environment class
 */
class SWEEnv {
public:
    SWEEnv(std::optional<std::any> config);
    Repo resolveRepo(std::optional<std::variant<Repo, RepoConfig>> repo);
    static SWEEnv fromConfig(EnvironmentConfig config);
    void addHook(EnvHook hook);
    std::future<void> start();
    std::future<void> copyRepo();
    std::future<void> resetRepository();
    std::future<void> initDeployment();
    std::future<std::string> communicate(const std::string& input, double timeout = 25, std::any options = {});
    std::future<std::string> readFile(const std::string& filePath, std::optional<std::string> encoding, std::optional<std::string> errors);
    std::future<void> writeFile(const std::string& filePath, const std::string& content);
    std::future<void> setEnvVariables(const std::unordered_map<std::string, std::string>& envVars);
    std::future<void> executeCommand(const std::string& command, std::any options = {});
    std::future<void> interruptSession();
    std::string getCwd();
    bool isAlive();
    bool isStarted();
    std::future<void> reset();
    std::future<void> hardReset();
    std::future<void> close();


} // namespace elizaos
