#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct Comment {
    std::string id;
    std::string content;
    std::string createdAt;
    std::string name;
    std::string image;
};

void Comments(const std::any& { projectId });

} // namespace elizaos
