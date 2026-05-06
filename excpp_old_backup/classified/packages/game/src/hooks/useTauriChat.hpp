#include ".services/TauriService.hpp"
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



struct UseTauriChatReturn {
    bool isConnected;
    (content: string) => Promise<void> sendMessage;
    std::vector<TauriMessage> messages;
    string | null error;
    bool isLoading;
};

// Hook for using Tauri-based chat functionality
UseTauriChatReturn useTauriChat();

} // namespace elizaos
