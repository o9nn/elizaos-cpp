#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "bullet.hpp"
#include "button.hpp"
#include "card.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct LoadingPanelProps {
    std::string title;
    std::variant<std::string, std::vector<std::string>> messages;
    std::optional<std::string> type;
};

void LoadingPanel(auto messages, auto type);


} // namespace elizaos
