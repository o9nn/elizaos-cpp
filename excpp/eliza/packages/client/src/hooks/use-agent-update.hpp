#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * A custom hook for handling Agent updates with specific handling for JSONb fields.
 * This hook builds on usePartialUpdate but adds Agent-specific convenience methods
 * organized by the UI tabs (Basic Info, Content, Style, Plugins, etc.).
 *
 * @param initialAgent The initial Agent object
 * @returns Object with agent state and update methods
 */
void useAgentUpdate(Agent initialAgent);

} // namespace elizaos
