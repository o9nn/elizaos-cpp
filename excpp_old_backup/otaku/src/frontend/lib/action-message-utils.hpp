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
 * Convert an agent action message to ToolPart format for display
 * This handles messages with type/sourceType 'agent_action' and parses their rawMessage
 */
ToolPart convertActionMessageToToolPart(const std::any& message);

/**
 * Check if a message is an agent action message
 */
bool isActionMessage(const std::any& message);


} // namespace elizaos
