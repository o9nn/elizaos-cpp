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



struct ChatInputAreaProps {
    std::string input;
    bool inputDisabled;
    std::vector<UploadingFile> selectedFiles;
    std::variant<ChannelType::DM, ChannelType::GROUP> chatType;
    std::optional<Agent> targetAgentData;
    std::optional<std::shared_ptr<HTMLFormElement>> formRef;
    std::optional<std::shared_ptr<HTMLTextAreaElement>> inputRef;
    std::optional<std::shared_ptr<HTMLInputElement>> fileInputRef;
};


} // namespace elizaos
