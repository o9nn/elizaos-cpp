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



struct AboutModalContentProps {
};

struct CapabilityItem {
    std::string title;
    std::string description;
    std::optional<std::string> emphasis;
};

struct PluginItem {
    std::string name;
    std::string category;
    std::string summary;
    std::vector<std::string> points;
    std::string example;
};

void AboutModalContent();


} // namespace elizaos
