#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


/**
 * Information describing the target of a message.
 */
struct TargetInfo {
    std::string source;
    std::optional<UUID> roomId;
    std::optional<std::string> channelId;
    std::optional<std::string> serverId;
    std::optional<UUID> entityId;
    std::optional<std::string> threadId;
};

/**
 * Function signature for handlers responsible for sending messages to specific platforms.
 */
using SendHandlerFunction = std::function<std::future<void>(IAgentRuntime, TargetInfo, Content)>;

enum SOCKET_MESSAGE_TYPE {
}

/**
 * Interface for control messages sent from the backend to the frontend
 * to manage UI state and interaction capabilities
 */
struct ControlMessage {
    std::string type;
    std::string action;
    std::optional<std::string> target;
    UUID roomId;
};


} // namespace elizaos
