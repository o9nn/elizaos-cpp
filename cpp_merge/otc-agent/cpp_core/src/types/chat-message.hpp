#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

struct ChatMessageAction {
    std: type;
    std::optional<std:> content;
};

struct ChatMessage {
    std: id;
    std: name;
    std: text;
    std: senderId;
    std: roomId;
    double createdAt;
    std: source;
    std::optional<bool> isLoading;
    std::optional<std:> thought;
    std::optional<bool> isUserMessage;
    std::optional<std:> serverMessageId;
    std::optional<std::vector<ChatMessageAction>> actions;
};


} // namespace elizaos
