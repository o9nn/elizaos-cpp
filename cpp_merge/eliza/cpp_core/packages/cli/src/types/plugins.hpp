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
using RawRegistry = std::unordered_map<std:, std:>; // <npmName> → "github:owner/repo"

struct VersionInfo {
    std::optional<{> git;
    std: repo;
    std::optional<{> v0;
    std: | null version;
    std: | null branch;
    std::optional<{> v1;
    std: | null version;
    std: | null branch;
    std::optional<{> npm;
    std: repo;
    std::optional<std: | null> v0;
    std::optional<std: | null> v1;
    { supports;
    bool v0;
    bool v1;
};

struct CachedRegistry {
    std: lastUpdatedAt;
    std::unordered_map<std:, VersionInfo> registry;
};


} // namespace elizaos
