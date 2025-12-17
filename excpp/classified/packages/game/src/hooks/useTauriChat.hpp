#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".services/TauriService.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct UseTauriChatReturn {
    bool isConnected;
    std::vector<TauriMessage> messages;
    std::string error;
    bool isLoading;
};

// Hook for using Tauri-based chat functionality
UseTauriChatReturn useTauriChat();

} // namespace elizaos
