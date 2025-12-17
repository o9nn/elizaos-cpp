#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class SecretsManagerService {
public:
    SecretsManagerService(IAgentRuntime runtime);
    std::string capabilityDescription() const;
    std::future<Service> start(IAgentRuntime runtime);
    std::future<void> start();
    std::future<void> stop();
    void loadSecretsFromRuntime();
    std::variant<Promise<string, null>> getSecret(const std::string& key);
    std::future<void> setSecret(const std::string& key, const std::string& value);
    std::future<bool> hasSecret(const std::string& key);
    std::future<std::vector<std::string>> listSecretKeys();

private:
    IAgentRuntime runtime_;
};


} // namespace elizaos
