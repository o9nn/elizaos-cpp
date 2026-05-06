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

// Raw registry types
using RawRegistry = std::unordered_map<std::string, std::string>; // <npmName> → "github:owner/repo"

struct VersionInfo {
    std::string repo;
    std::optional<std::string> version;
    std::optional<std::string> branch;
    std::optional<std::string> version;
    std::optional<std::string> branch;
    std::string repo;
    std::optional<std::optional<std::string>> v0;
    std::optional<std::optional<std::string>> v1;
    bool v0;
    bool v1;
};

struct CachedRegistry {
    std::string lastUpdatedAt;
};


} // namespace elizaos
