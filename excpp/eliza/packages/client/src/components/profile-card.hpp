#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "ui/button.hpp"
#include "ui/card.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct ButtonConfig {
    std::optional<std::string> label;
    std::optional<ReactNode> icon;
    std::optional<std::string> className;
    std::optional<std::string> size;
    std::optional<bool> disabled;
};

struct ProfileCardProps {
    ReactNode title;
    ReactNode content;
    std::vector<ButtonConfig> buttons;
    std::optional<std::string> className;
};


} // namespace elizaos
