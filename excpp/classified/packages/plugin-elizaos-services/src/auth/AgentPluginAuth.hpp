#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "AuthenticationService.js.hpp"
#include "CLIAuthCommands.js.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Agent Plugin Authentication Integration
 * Integrates authentication service with ElizaOS plugin runtime
 */

/**
 * Authentication Service for Plugin Integration
 */
class AgentAuthService {
public:
    AgentAuthService(std::optional<IAgentRuntime> runtime);
    std::future<AgentAuthService> start(IAgentRuntime runtime);
    void getAuthStatus();
    void validateApiKey(const std::string& provider, const std::string& apiKey);
    void testApiFunctionality(const std::string& provider);
    std::future<bool> isProviderReady(const std::string& provider, std::optional<std::string> capability);
    std::variant<Promise<string, null>> getBestProvider(const std::string& capability);
    void validateAllProviders();
    void clearCache();
    std::future<void> stop();

private:
    AuthenticationService authService_;
};

/**
 * Plugin Integration for Authentication
 */

    // Register CLI commands if in CLI context

    // Validate configuration on startup

/**
 * Helper functions for plugin usage
 */
class AuthHelper {
public:
    std::future<bool> isProviderReady(IAgentRuntime runtime, const std::string& provider, std::optional<std::string> capability);
    std::variant<Promise<string, null>> getBestProvider(IAgentRuntime runtime, const std::string& capability);
    Promise< validateBeforeUse(IAgentRuntime runtime, const std::string& provider, const std::string& capability);
    void if(auto !isReady);
    void catch(auto error);
    std::future<std::any> getDebugInfo(IAgentRuntime runtime);
};


} // namespace elizaos
