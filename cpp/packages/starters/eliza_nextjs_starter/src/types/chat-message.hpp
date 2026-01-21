#pragma once
#include <algorithm>
#include <any>
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

struct ChatMessage {
    std::string id;
    std::string name;
    std::string text;
    std::string senderId;
    std::string roomId;
    double createdAt;
    std::string source;
    std::optional<bool> isLoading;
    std::optional<std::string> thought;
    std::optional<std::vector<std::any>> actions;
};


} // namespace elizaos
