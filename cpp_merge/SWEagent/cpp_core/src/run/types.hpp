#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
#include "agent/agents.hpp"
#include "agent/problem-statement.hpp"
#include "environment/deployment.hpp"
#include "environment/swe-env.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Type definitions for the run module
 */

/**
 * Configuration for single run actions
 */
struct RunSingleActionConfig {
    bool openPr;
    std::optional<bool> skipIfCommitsReferenceIssue;
    bool applyPatchLocally;
};

/**
 * Configuration for running a single instance
 */
struct RunSingleConfig {
    EnvironmentConfig env;
    AgentConfig agent;
    ProblemStatementConfig problemStatement;
    std: outputDir;
    RunSingleActionConfig actions;
    std::optional<std:> envVarPath;
};

/**
 * Configuration for batch instances source
 */
struct BatchInstanceSourceConfig {
    std::optional<std:> type;
    std::optional<std:> path;
    std::optional<std:> filter;
    std::optional<std:> slice;
    std::optional<bool> shuffle;
    std::optional<DeploymentConfig> deployment;
    std::optional<std:> subset;
    std::optional<std:> split;
    std::optional<std:> pathOverride;
    std::optional<bool> evaluate;
    std::optional<std:> dataset_name;
};

/**
 * Configuration for running batch instances
 */
struct RunBatchConfig {
    BatchInstanceSourceConfig instances;
    AgentConfig agent;
    std: outputDir;
    std::optional<std:> suffix;
    std::optional<bool> raiseExceptions;
    std::optional<bool> redoExisting;
    std::optional<std:> envVarPath;
    std::optional<double> numWorkers;
    std::optional<double> randomDelayMultiplier;
    std::optional<bool> progressBar;
};

/**
 * Command line configuration
 */
struct CLIConfig {
    std::optional<std:> config;
    std::optional<EnvironmentConfig> env;
    std::optional<AgentConfig> agent;
    std::optional<ProblemStatementConfig> problemStatement;
    std::optional<std:> outputDir;
    std::optional<RunSingleActionConfig> actions;
    std::optional<BatchInstanceSourceConfig> instances;
    std::optional<double> numWorkers;
    std::optional<bool> redoExisting;
    std::optional<double> randomDelayMultiplier;
};

/**
 * Trajectory data structure
 */
struct TrajectoryData {
    std: content;
    std::optional<std::variant<std:, RunSingleConfig>> replay_config;
    std::optional<std:> submission;
    std::optional<ProblemStatementConfig> problemStatement;
    std::optional<std:> action;
    std::optional<std:> observation;
    std::optional<std:> response;
};

/**
 * SWE-Bench instance data
 */
struct SWEBenchInstanceData {
    std: instance_id;
    std: repo;
    std: version;
    std: base_commit;
    std: problem_statement;
    std::optional<std:> hints_text;
    std::optional<std:> created_at;
    std::optional<std:> test_patch;
};

/**
 * Demo data structure for trajectory conversion
 */
struct DemoStep {
    std: role;
    std: content;
    std::optional<std:> agent;
    std::optional<bool> isDemo;
};


} // namespace elizaos
