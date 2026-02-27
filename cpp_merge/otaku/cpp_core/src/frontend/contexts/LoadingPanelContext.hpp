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



struct LoadingPanelState {
    bool isVisible;
    'loading' | 'success' | 'error' type;
    std::string title;
    std::vector<std::string | string> messages;
    std::string | null; // Track which component is showing the panel id;
};

struct LoadingPanelContextType {
    [&](title: std::string, messages: std::string | std::string[], id?: std::string) { return void showLoading; };
    [&](title: std::string, messages: std::string | std::string[], id?: std::string, autoClose?) { return void showSuccess; };
    [&](title: std::string, messages: std::string | std::string[], id?: std::string) { return void showError; };
    [&](id?: std::string) { return void hide; };
    bool isVisible;
};

void LoadingPanelProvider({ children: ReactNode } { children });

// Custom hook to use the loading panel
void useLoadingPanel();


} // namespace elizaos
