#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct LoadingPanelState {
    bool isVisible;
    std::variant<'loading', 'success', 'error'> type;
    std::string title;
    std::variant<std::string, std::vector<std::string>> messages;
    std::optional<std::string> id;
};

struct LoadingPanelContextType {
    bool isVisible;
};

void LoadingPanelProvider(const std::any& { children });

// Custom hook to use the loading panel
void useLoadingPanel();


} // namespace elizaos
