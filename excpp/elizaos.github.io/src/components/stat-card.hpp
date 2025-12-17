#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct StatCardProps {
    std::string title;
    LucideIcon icon;
    std::optional<std::string> bgColor;
    ReactNode children;
    std::optional<std::string> className;
    std::optional<ReactNode> modalContent;
    std::optional<std::string> modalTitle;
    std::optional<std::string> modalDescription;
};

void StatCard(Icon icon, auto bgColor, auto children, auto className, auto modalContent, auto modalTitle, auto modalDescription);

} // namespace elizaos
