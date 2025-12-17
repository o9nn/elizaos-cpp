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



struct Citation {
    std::string url;
    std::string content;
    std::string title;
};

struct ChatStreamData {
    std::optional<std::vector<Citation>> citations;
};

struct ChatRequest {
    std::vector<Message> messages;
};


} // namespace elizaos
