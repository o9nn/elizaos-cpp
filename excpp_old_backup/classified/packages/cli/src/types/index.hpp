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

struct EnvVarConfig {
    std::string type;
    std::string description;
    bool required;
    std::optional<bool> sensitive;
    std::optional<std::string> default;
};


} // namespace elizaos
