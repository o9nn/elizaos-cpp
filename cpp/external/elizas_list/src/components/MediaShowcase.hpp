#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct MediaShowcaseProps {
    std::variant<'video', 'gif'> type;
    std::string url;
    std::optional<std::string> thumbnail;
    std::string title;
};

void MediaShowcase(MediaShowcaseProps { media }); 
} // namespace elizaos
