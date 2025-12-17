#include "run-single.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string getDefaultOutputDir(const std::string& outputDir, ProblemStatement problemStatement, AgentConfig agent) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (outputDir != 'DEFAULT') {
        return outputDir;
    }

    // Generate default based on problem statement and agent
    const auto timestamp = new Date().toISOString().replace(/[:.]/g, '-');
    const auto psId = problemStatement.id || 'unknown';
    const auto agentName = agent.type || 'agent';

    return std::to_string(psId) + "_" + std::to_string(agentName) + "_" + std::to_string(timestamp);

}

std::future<AgentRunResult> runFromConfig(RunSingleConfig config) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto runner = RunSingle.fromConfig(config);
    return runner.run();

}

} // namespace elizaos
