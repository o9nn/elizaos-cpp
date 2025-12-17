#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct SectionCardProps {
    std::string title;
    std::optional<LucideIcon> icon;
    ReactNode children;
    std::optional<std::string> className;
    std::optional<std::string> contentClassName;
    std::optional<std::string> headerClassName;
    std::optional<bool> noPadding;
};


} // namespace elizaos
