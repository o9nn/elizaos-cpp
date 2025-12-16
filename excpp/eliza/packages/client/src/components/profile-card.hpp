#include "ui/button.hpp"
#include "ui/card.hpp"
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



struct ButtonConfig {
    std::optional<std::string> label;
    std::optional<React.ReactNode> icon;
    std::optional<() => void> action;
    std::optional<std::string> className;
    std::optional<'default' | 'sm' | 'lg' | 'icon' | null | undefined> size;
    std::optional<bool> disabled;
};

struct ProfileCardProps {
    React.ReactNode title;
    React.ReactNode content;
    std::vector<ButtonConfig> buttons;
    std::optional<std::string> className;
};


} // namespace elizaos
