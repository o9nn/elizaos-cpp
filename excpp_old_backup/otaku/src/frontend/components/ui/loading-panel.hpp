#include "bullet.hpp"
#include "button.hpp"
#include "card.hpp"
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



struct LoadingPanelProps {
    std::string title;
    std::vector<string | string> messages;
    std::optional<'loading' | 'success' | 'error'> type;
    std::optional<() => void> onClose;
};

void LoadingPanel(auto { title, auto messages, auto type = 'loading', LoadingPanelProps onClose });


} // namespace elizaos
