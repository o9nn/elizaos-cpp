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
    std: title;
    std::vector<std: | string> messages;
    std: | null; // Track which component is showing the panel id;
};

struct LoadingPanelContextType {
    [&](title: std:, messages: std: | std:[], id?: std:) { return void showLoading; };
    [&](title: std:, messages: std: | std:[], id?: std:, autoClose?) { return void showSuccess; };
    [&](title: std:, messages: std: | std:[], id?: std:) { return void showError; };
    [&](id?: std:) { return void hide; };
    bool isVisible;
};

void LoadingPanelProvider({ children: ReactNode } { children });

// Custom hook to use the loading panel
void useLoadingPanel();


} // namespace elizaos
