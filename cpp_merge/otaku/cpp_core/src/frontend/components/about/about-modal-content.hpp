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



struct AboutModalContentProps {
    [&]() { return void onClose; };
};

struct CapabilityItem {
    std: title;
    std: description;
    std::optional<std:> emphasis;
};

struct PluginItem {
    std: name;
    std: category;
    std: summary;
    std::vector<std::string> points;
    std: example;
};

void AboutModalContent(AboutModalContentProps { onClose });


} // namespace elizaos
