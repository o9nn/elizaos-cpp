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

struct ChatMessage {
    std::string id;
    std::string name;
    string | null | undefined text;
    std::string senderId;
    std::string roomId;
    double createdAt;
    std::string source;
    std::optional<bool> isLoading;
    std::optional<std::string> thought;
    std::optional<any[]; // Consider defining a more specific type if the structure is known> actions;
};


} // namespace elizaos
