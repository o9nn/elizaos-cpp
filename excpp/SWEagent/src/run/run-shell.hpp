#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
#include "agent/agents.hpp"
#include "agent/hooks.hpp"
#include "agent/problem-statement.hpp"
#include "environment/swe-env.hpp"
#include "utils/log.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Run shell mode
 * Converted from sweagent/run/run_shell.py
 */

/**
 * Run shell - interactive shell mode
 */
class RunShell {
public:
    RunShell(std::optional<std::any> config);
    void addHook(AbstractAgentHook hook);
    std::future<void> run();
    std::future<void> interactiveLoop();
    void printHelp();
    std::future<void> printStatus();
    void saveState();

private:
    SWEEnv env_;
    AbstractAgent agent_;
    std::variant<ProblemStatement, ProblemStatementConfig> problemStatement_;
    std::string outputDir_;
    std::vector<AbstractAgentHook> hooks_;
    AgentLogger logger_;
    readline::Interface rl_;
};

/**
 * Run a shell session from configuration options
 * @param _options - Configuration options for the shell session
 * @param _options.repo - Repository URL or path
 * @param _options.config - Configuration file path
 * @param _options.problemStatement - Problem statement to solve
 */
std::future<void> runShellFromConfig(std::optional<std::any> _options);

} // namespace elizaos
