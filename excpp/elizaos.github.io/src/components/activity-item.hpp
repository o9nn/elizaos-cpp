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



struct ActivityItemProps {
    std::string id;
    std::string title;
    std::string author;
    double number;
    std::string href;
    ReactNode icon;
    std::optional<ReactNode> metadata;
    std::optional<std::string> className;
};

void ActivityItem(auto title, auto author, auto number, auto href, auto icon, auto metadata, auto className);

} // namespace elizaos
