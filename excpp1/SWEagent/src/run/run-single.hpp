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
#include ".agent/problem-statement.hpp"
#include ".environment/swe-env.hpp"
#include ".types.hpp"
#include ".utils/config.hpp"
#include ".utils/log.hpp"
#include "common.hpp"
#include "hooks/types.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Run single instance
 * Converted from sweagent/run/run_single.py
 */

// Import the proper types from our new types module

/**
 * Configuration schema using Zod
 */

/**
 * Get default output directory
 */
std::string getDefaultOutputDir(const std::string& outputDir, ProblemStatement problemStatement, AgentConfig agent);

/**
 * Run single instance
 */
class RunSingle {
public:
    RunSingle(std::optional<std::any> config);
    std::future<RunSingle> fromConfig(RunSingleConfig config);
    void addHook(RunHook hook);
    std::future<AgentRunResult> run();
    std::future<void> applyPatchLocally(const std::string& patch);
    std::future<void> openPullRequest(const std::string& _patch);

private:
    SWEEnv env_;
    AbstractAgent agent_;
    std::variant<ProblemStatement, ProblemStatementConfig> problemStatement_;
    std::string outputDir_;
    std::vector<RunHook> hooks_;
    RunSingleActionConfig actions_;
    AgentLogger logger_;
};

/**
 * Run from configuration
 */
std::future<AgentRunResult> runFromConfig(RunSingleConfig config);

} // namespace elizaos
