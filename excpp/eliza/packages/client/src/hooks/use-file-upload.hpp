#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Direct error handling

using UploadingFile = {

struct UseFileUploadProps {
    std::optional<UUID> agentId;
    std::optional<UUID> channelId;
    std::variant<ChannelType::DM, ChannelType::GROUP> chatType;
};

void useFileUpload(auto channelId);

} // namespace elizaos
