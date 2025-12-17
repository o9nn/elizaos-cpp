#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".button.hpp"
#include "message-loading.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// ChatBubble

// ChatBubbleAvatar
struct ChatBubbleAvatarProps {
    std::optional<std::string> src;
    std::optional<std::string> fallback;
    std::optional<std::string> className;
};

// ChatBubbleMessage

// ChatBubbleTimestamp

// ChatBubbleAction
using ChatBubbleActionProps = ComponentProps<typeof Button>;


} // namespace elizaos
