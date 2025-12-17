#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct TypeCheckResult {
    bool success;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
};

std::future<TypeCheckResult> runTypeCheck(const std::string& projectPath, boolean = true strict);

} // namespace elizaos
