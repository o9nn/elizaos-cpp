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

struct SystemEnvironment {
    std::string nodeVersion;
    std::string platform;
    'development' | 'production' | 'test' environment;
    { features;
    bool authentication;
    bool tee;
    std::vector<std::string> plugins;
    std::unordered_map<std::string, std::any> configuration;
};


struct LocalEnvironmentUpdateParams {
    std::unordered_map<std::string, std::string> variables;
    std::optional<bool> merge;
};


} // namespace elizaos
