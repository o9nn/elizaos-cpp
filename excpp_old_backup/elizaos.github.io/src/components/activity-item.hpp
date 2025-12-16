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



struct ActivityItemProps {
    std::string id;
    std::string title;
    std::string author;
    double number;
    std::string href;
    React.ReactNode icon;
    std::optional<React.ReactNode> metadata;
    std::optional<std::string> className;
};


} // namespace elizaos
