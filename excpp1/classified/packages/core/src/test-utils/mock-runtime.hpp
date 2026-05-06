#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Simple mock runtime for tests - just use createMockRuntime() and cast to IAgentRuntime when needed
 */
std::optional<IAgentRuntime> createMockRuntime(const std::optional<IAgentRuntime>& overrides);

/**
 * Simple mock memory creator
 */
Memory createMockMemory(const std::string& text, const std::optional<Memory>& overrides);

/**
 * Simple mock state creator
 */
State createMockState(const std::optional<State>& overrides);

} // namespace elizaos
