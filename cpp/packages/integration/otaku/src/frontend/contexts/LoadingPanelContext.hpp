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
    std::vector<string | string> messages;
    string | null; // Track which component is showing the panel id;
};

struct LoadingPanelContextType {
    (title: string, messages: string | string[], id?: string) => void showLoading;
    (title: string, messages: string | string[], id?: string, autoClose?: boolean) => void showSuccess;
    (title: string, messages: string | string[], id?: string) => void showError;
    (id?: string) => void hide;
    bool isVisible;
};

void LoadingPanelProvider({ children: ReactNode } { children });

// Custom hook to use the loading panel
void useLoadingPanel();


} // namespace elizaos
