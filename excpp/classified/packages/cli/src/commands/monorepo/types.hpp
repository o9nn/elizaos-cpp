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

/**
 * Options for the monorepo command
 */
struct MonorepoOptions {
    std::optional<std::string> branch;
    std::optional<std::string> dir;
};

/**
 * Information about the cloning operation
 */
struct CloneInfo {
    std::string repo;
    std::string branch;
    std::string destination;
};

/**
 * Platform-specific installation instructions
 */
struct PlatformInstructions {
    std::string platform;
    std::vector<std::string> commands;
    std::optional<std::vector<std::string>> alternatives;
};


} // namespace elizaos
