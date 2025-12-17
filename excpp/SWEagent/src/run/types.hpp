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
    std::string outputDir;
    RunSingleActionConfig actions;
    std::optional<std::string> envVarPath;
};

/**
 * Configuration for batch instances source
 */
struct BatchInstanceSourceConfig {
    std::optional<std::string> type;
    std::optional<std::string> path;
    std::optional<std::string> filter;
    std::optional<std::string> slice;
    std::optional<bool> shuffle;
    std::optional<DeploymentConfig> deployment;
    std::optional<std::string> subset;
    std::optional<std::string> split;
    std::optional<std::string> pathOverride;
    std::optional<bool> evaluate;
    std::optional<std::string> dataset_name;
};

/**
 * Configuration for running batch instances
 */
struct RunBatchConfig {
    BatchInstanceSourceConfig instances;
    AgentConfig agent;
    std::string outputDir;
    std::optional<std::string> suffix;
    std::optional<bool> raiseExceptions;
    std::optional<bool> redoExisting;
    std::optional<std::string> envVarPath;
    std::optional<double> numWorkers;
    std::optional<double> randomDelayMultiplier;
    std::optional<bool> progressBar;
};

/**
 * Command line configuration
 */
struct CLIConfig {
    std::optional<std::string> config;
    std::optional<EnvironmentConfig> env;
    std::optional<AgentConfig> agent;
    std::optional<ProblemStatementConfig> problemStatement;
    std::optional<std::string> outputDir;
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
    std::string content;
    std::optional<std::variant<std::string, RunSingleConfig>> replay_config;
    std::optional<std::string> submission;
    std::optional<ProblemStatementConfig> problemStatement;
    std::optional<std::string> action;
    std::optional<std::string> observation;
    std::optional<std::string> response;
};

/**
 * SWE-Bench instance data
 */
struct SWEBenchInstanceData {
    std::string instance_id;
    std::string repo;
    std::string version;
    std::string base_commit;
    std::string problem_statement;
    std::optional<std::string> hints_text;
    std::optional<std::string> created_at;
    std::optional<std::string> test_patch;
};

/**
 * Demo data structure for trajectory conversion
 */
struct DemoStep {
    std::string role;
    std::string content;
    std::optional<std::string> agent;
    std::optional<bool> isDemo;
};


} // namespace elizaos
