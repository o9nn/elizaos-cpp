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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct LoadingPanelState {
    bool isVisible;
    std::string type;
    std::string title;
    std::variant<std::string, std::vector<std::string>> messages;
    std::string id;
};

struct LoadingPanelContextType {
    bool isVisible;
};

void LoadingPanelProvider();

// Custom hook to use the loading panel
void useLoadingPanel();


} // namespace elizaos
