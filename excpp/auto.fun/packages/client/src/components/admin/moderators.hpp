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



struct Moderator {
    std::string id;
    std::string address;
    std::optional<std::string> name;
    bool isAdmin;
    std::string createdAt;
};


} // namespace elizaos
