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

// Raw registry types
using RawRegistry = std::unordered_map<std::string, std::string>; // <npmName> → "github:owner/repo"

struct VersionInfo {
    std::optional<{> git;
    std::string repo;
    std::optional<{> v0;
    std::string | null version;
    std::string | null branch;
    std::optional<{> v1;
    std::string | null version;
    std::string | null branch;
    std::optional<{> npm;
    std::string repo;
    std::optional<std::string | null> v0;
    std::optional<std::string | null> v1;
    { supports;
    bool v0;
    bool v1;
};

struct CachedRegistry {
    std::string lastUpdatedAt;
    std::unordered_map<std::string, VersionInfo> registry;
};


} // namespace elizaos
