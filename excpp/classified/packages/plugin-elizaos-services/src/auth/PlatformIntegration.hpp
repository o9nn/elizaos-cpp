#pragma once
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
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
    std::string clientType;
    std::string distributionMode;
    bool allowTestKeys;
};

struct ClientSession {
    std::string sessionId;
    std::string clientType;
    std::string platformId;
    std::optional<AuthStatus> authStatus;
    std::chrono::system_clock::time_point lastActivity;
    std::vector<std::string> validatedKeys;
};

struct KeyDistributionRequest {
    std::string sessionId;
    std::string provider;
    std::string keyType;
    std::vector<std::string> clientCapabilities;
};

struct KeyDistributionResponse {
    bool success;
    std::optional<std::string> apiKey;
    std::string keyType;
    std::vector<std::string> capabilities;
    std::optional<std::chrono::system_clock::time_point> expiresAt;
    std::optional<std::string> error;
};

/**
 * Platform Integration Service
 * Manages authentication across different client modalities
 */
class PlatformIntegrationService {
public:
    PlatformIntegrationService(IAgentRuntime runtime, PlatformAuthConfig config);
    std::future<ClientSession> registerSession(const std::string& sessionId, const std::string& clientType, const std::string& platformId);
    std::future<KeyDistributionResponse> distributeKey(KeyDistributionRequest request);
    Promise< validateDistributedKey(const std::string& sessionId, const std::string& provider, const std::string& apiKey);
    Promise< invalidateSession(const std::string& sessionId);
    Promise< getSessionStatus(const std::string& sessionId);
     getAnalytics();
    double cleanupExpiredSessions();
    std::future<KeyDistributionResponse> distributeTestKey(const std::string& provider, const std::string& clientType);
    std::future<KeyDistributionResponse> distributeProductionKey(const std::string& provider, ClientSession session);

private:
    IAgentRuntime runtime_;
    AuthenticationService authService_;
    std::chrono::system_clock::time_point timestamp_;
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
    static PlatformIntegrationService createForCLI(IAgentRuntime runtime);
    static PlatformIntegrationService createForGUI(IAgentRuntime runtime);
    static PlatformIntegrationService createForAgent(IAgentRuntime runtime);
};

/**
 * Utility functions for platform integration
 */
class PlatformAuthUtils {
public:
    static std::string generateSessionId();
    static bool isValidSessionId(const std::string& sessionId);
    static std::vector<std::string> getClientCapabilities(const std::string& clientType);
    static bool isProviderCompatible(const std::vector<std::string>& providerCapabilities, const std::vector<std::string>& clientCapabilities);
};


} // namespace elizaos
