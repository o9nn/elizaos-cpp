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
    std: id;
    std: name;
    std: text;
    std: senderId;
    std: roomId;
    double createdAt;
    std: source;
    std::optional<bool> isLoading;
    std::optional<std:> thought;
    std::optional<std::vector<std::string>> actions;
};


} // namespace elizaos
