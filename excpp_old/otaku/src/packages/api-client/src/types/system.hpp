#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

struct SystemEnvironment {
    std::string nodeVersion;
    std::string platform;
    std::variant<'development', 'production', 'test'> environment;
    bool authentication;
    bool tee;
    std::vector<std::string> plugins;
};

struct LocalEnvironmentUpdateParams {
    std::optional<bool> merge;
};


} // namespace elizaos
