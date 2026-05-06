#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "AuthenticationService.js.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Platform Integration for ElizaOS Services Authentication
 * Handles platform-to-client key distribution and validation
 */

struct PlatformAuthConfig {
    std::string platformId;
    std::variant<'cli', 'gui', 'agent'> clientType;
    std::variant<'test', 'production', 'auto'> distributionMode;
    bool allowTestKeys;
};

struct ClientSession {
    std::string sessionId;
    std::variant<'cli', 'gui', 'agent'> clientType;
    std::string platformId;
    std::optional<AuthStatus> authStatus;
    Date lastActivity;
    std::vector<std::string> validatedKeys;
};

struct KeyDistributionRequest {
    std::string sessionId;
    std::string provider;
    std::variant<'test', 'production'> keyType;
    std::vector<std::string> clientCapabilities;
};

struct KeyDistributionResponse {
    bool success;
    std::optional<std::string> apiKey;
    std::variant<'test', 'production'> keyType;
    std::vector<std::string> capabilities;
    std::optional<Date> expiresAt;
    std::optional<std::string> error;
};

/**
 * Platform Integration Service
 * Manages authentication across different client modalities
 */
class PlatformIntegrationService {
public:
    PlatformIntegrationService(IAgentRuntime runtime, PlatformAuthConfig config);
    std::future<ClientSession> registerSession(const std::string& sessionId, const std::variant<'cli', 'gui', 'agent'>& clientType, const std::string& platformId);
    std::future<KeyDistributionResponse> distributeKey(KeyDistributionRequest request);
    Promise< validateDistributedKey(const std::string& sessionId, const std::string& provider, const std::string& apiKey);
    void if(auto !session);
    void if(auto result.isValid);
    void catch(auto error);
    Promise< invalidateSession(const std::string& sessionId);
    void if(auto !session);
    Promise< getSessionStatus(const std::string& sessionId);
    void if(auto !session);
    void catch(auto error);
     getAnalytics();
    double cleanupExpiredSessions();
    std::future<KeyDistributionResponse> distributeTestKey(const std::string& provider, const std::string& clientType);
    std::future<KeyDistributionResponse> distributeProductionKey(const std::string& provider, ClientSession session);

private:
    IAgentRuntime runtime_;
    AuthenticationService authService_;
    Date timestamp_;
    std::string sessionId_;
    std::string provider_;
    std::string keyType_;
    bool success_;
};

/**
 * Platform Integration Factory
 */
class PlatformIntegrationFactory {
public:
    PlatformIntegrationService createForCLI(IAgentRuntime runtime);
    PlatformIntegrationService createForGUI(IAgentRuntime runtime);
    PlatformIntegrationService createForAgent(IAgentRuntime runtime);
};

/**
 * Utility functions for platform integration
 */
class PlatformAuthUtils {
public:
    std::string generateSessionId();
    bool isValidSessionId(const std::string& sessionId);
    std::vector<std::string> getClientCapabilities(const std::variant<'cli', 'gui', 'agent'>& clientType);
    bool isProviderCompatible(const std::vector<std::string>& providerCapabilities, const std::vector<std::string>& clientCapabilities);
};


} // namespace elizaos
