#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".agent/agents.hpp"
#include ".agent/models.hpp"
#include ".agent/problem-statement.hpp"
#include ".environment/deployment.hpp"
#include ".environment/swe-env.hpp"
#include ".utils/config.hpp"
#include ".utils/log.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Replay agent trajectory
 * Converted from sweagent/run/run_replay.py
 */

/**
 * Run replay configuration
 */
struct RunReplayConfig {
    std::string trajPath;
    std::optional<DeploymentConfig> deployment;
    std::optional<std::string> outputDir;
    std::optional<std::string> envVarPath;
    std::optional<std::vector<std::string>> updateConfig;
};

/**
 * Configuration schema
 */

/**
 * Run replay - replay an agent trajectory
 */
class RunReplay {
public:
    RunReplay(std::optional<std::any> config);
    static RunReplay fromConfig(RunReplayConfig config);
    std::string instanceId() const;
    std::optional<RunSingleConfig> getConfigFromAgent();
    void createActionsFile();
    std::future<SWEEnv> getEnv();
    std::future<DefaultAgent> getAgent();
    std::future<void> main();

private:
    std::string trajPath_;
    std::string outputDir_;
    bool catchErrors_;
    bool requireZeroExitCode_;
    AgentLogger logger_;
    TrajectoryData trajData_;
};

/**
 * Run from configuration
 */
std::future<void> runReplayFromConfig(RunReplayConfig config);

} // namespace elizaos
