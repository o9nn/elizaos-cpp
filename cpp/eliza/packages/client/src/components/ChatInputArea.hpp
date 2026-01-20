#include "elizaos/core.hpp"
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



struct ChatInputAreaProps {
    std::string input;
    (value: string) => void setInput;
    bool inputDisabled;
    std::vector<UploadingFile> selectedFiles;
    (fileId: string) => void removeFile;
    (e: React.ChangeEvent<HTMLInputElement>) => void handleFileChange;
    (e: React.FormEvent<HTMLFormElement>) => void handleSendMessage;
    (e: React.KeyboardEvent<HTMLTextAreaElement>) => void handleKeyDown;
    ChannelType.DM | ChannelType.GROUP chatType;
    std::optional<Agent> targetAgentData;
    React.RefObject<HTMLFormElement | null> formRef;
    React.RefObject<HTMLTextAreaElement | null> inputRef;
    React.RefObject<HTMLInputElement | null> fileInputRef;
};


} // namespace elizaos
