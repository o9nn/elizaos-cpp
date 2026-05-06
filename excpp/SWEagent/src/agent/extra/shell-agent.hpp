#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "agents.hpp"
#include "history-processors.hpp"
#include "models.hpp"
#include "problem-statement.hpp"
#include "tools/parsing.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Shell agent implementation with human-in-the-loop capability
 * Converted from sweagent/agent/extra/shell_agent.py
 */

/**
 * Shell agent that allows human intervention during execution
 */
class ShellAgent {
public:
    ShellAgent(std::optional<std::any> config);
    static ShellAgent fromConfig(AgentConfig config);
    void humanStepIn();
    void humanStepOut();
};


} // namespace elizaos
