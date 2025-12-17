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

// Types for the registry API
using RawRegistry = std::unordered_map<std::string, std::string>; // <npmName> → "github:owner/repo"

struct VersionInfo {
    std::string repo;
    std::string version;
    std::string branch;
    std::string version;
    std::string branch;
    std::string repo;
    std::optional<std::string> v0;
    std::optional<std::string> v1;
    bool v0;
    bool v1;
};

struct CachedRegistry {
    std::string lastUpdatedAt;
};

struct NpmPackageMetadata {
};


} // namespace elizaos
