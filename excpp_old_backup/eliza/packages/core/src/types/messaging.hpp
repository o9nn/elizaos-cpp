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
 * Information describing the target of a message.
 */
struct TargetInfo {
    string; // Platform identifier (e.g., 'discord', 'telegram', 'websocket-api') source;
    std::optional<UUID; // Target room ID (platform-specific or runtime-specific)> roomId;
    std::optional<string; // Platform-specific channel/chat ID> channelId;
    std::optional<string; // Platform-specific server/guild ID> serverId;
    std::optional<UUID; // Target user ID (for DMs)> entityId;
    std::optional<string; // Platform-specific thread ID (e.g., Telegram topics)> threadId;
};

/**
 * Function signature for handlers responsible for sending messages to specific platforms.
 */
using SendHandlerFunction = (

enum SOCKET_MESSAGE_TYPE {
  ROOM_JOINING = 1,
  SEND_MESSAGE = 2,
  MESSAGE = 3,
  ACK = 4,
  THINKING = 5,
  CONTROL = 6,
}

/**
 * Interface for control messages sent from the backend to the frontend
 * to manage UI state and interaction capabilities
 */
struct ControlMessage {
    'control' type;
    { payload;
    'disable_input' | 'enable_input' action;
    std::optional<std::string> target;
    UUID roomId;
};


} // namespace elizaos
