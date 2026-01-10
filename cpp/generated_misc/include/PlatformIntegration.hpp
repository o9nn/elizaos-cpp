#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-ELIZAOS-SERVICES_SRC_AUTH_PLATFORMINTEGRATION_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-ELIZAOS-SERVICES_SRC_AUTH_PLATFORMINTEGRATION_H
#include "core.h"
#include "@elizaos/core.h"
#include "@elizaos/core.h"
#include "./AuthenticationService.js.h"

class PlatformAuthConfig;
class ClientSession;
class KeyDistributionRequest;
class KeyDistributionResponse;
class PlatformIntegrationService;
class PlatformIntegrationFactory;
class PlatformAuthUtils;

class PlatformAuthConfig : public object, public std::enable_shared_from_this<PlatformAuthConfig> {
public:
    using std::enable_shared_from_this<PlatformAuthConfig>::shared_from_this;
    string platformId;

    any clientType;

    any distributionMode;

    boolean allowTestKeys;
};

class ClientSession : public object, public std::enable_shared_from_this<ClientSession> {
public:
    using std::enable_shared_from_this<ClientSession>::shared_from_this;
    string sessionId;

    any clientType;

    string platformId;

    any authStatus;

    std::shared_ptr<Date> lastActivity;

    array<string> validatedKeys;
};

class KeyDistributionRequest : public object, public std::enable_shared_from_this<KeyDistributionRequest> {
public:
    using std::enable_shared_from_this<KeyDistributionRequest>::shared_from_this;
    string sessionId;

    string provider;

    any keyType;

    array<string> clientCapabilities;
};

class KeyDistributionResponse : public object, public std::enable_shared_from_this<KeyDistributionResponse> {
public:
    using std::enable_shared_from_this<KeyDistributionResponse>::shared_from_this;
    boolean success;

    string apiKey;

    any keyType;

    array<string> capabilities;

    std::shared_ptr<Date> expiresAt;

    string error;
};

class PlatformIntegrationService : public object, public std::enable_shared_from_this<PlatformIntegrationService> {
public:
    using std::enable_shared_from_this<PlatformIntegrationService>::shared_from_this;
    std::shared_ptr<IAgentRuntime> runtime;

    std::shared_ptr<AuthenticationService> authService;

    std::shared_ptr<Map<string, std::shared_ptr<ClientSession>>> activeSessions = std::make_shared<Map<string, std::shared_ptr<ClientSession>>>();

    array<object> keyDistributionLog = array<object>();

    PlatformIntegrationService(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<PlatformAuthConfig> config);
    template <typename P1>
    std::shared_ptr<Promise<std::shared_ptr<ClientSession>>> registerSession(string sessionId, P1 clientType, string platformId);
    virtual std::shared_ptr<Promise<std::shared_ptr<KeyDistributionResponse>>> distributeKey(std::shared_ptr<KeyDistributionRequest> request);
    virtual std::shared_ptr<Promise<object>> validateDistributedKey(string sessionId, string provider, string apiKey);
    virtual std::shared_ptr<Promise<object>> invalidateSession(string sessionId);
    virtual std::shared_ptr<Promise<object>> getSessionStatus(string sessionId);
    virtual object getAnalytics();
    virtual double cleanupExpiredSessions();
    virtual std::shared_ptr<Promise<std::shared_ptr<KeyDistributionResponse>>> distributeTestKey(string provider, string clientType);
    virtual std::shared_ptr<Promise<std::shared_ptr<KeyDistributionResponse>>> distributeProductionKey(string provider, std::shared_ptr<ClientSession> session);
};

class PlatformIntegrationFactory : public object, public std::enable_shared_from_this<PlatformIntegrationFactory> {
public:
    using std::enable_shared_from_this<PlatformIntegrationFactory>::shared_from_this;
    static std::shared_ptr<PlatformIntegrationService> createForCLI(std::shared_ptr<IAgentRuntime> runtime);
    static std::shared_ptr<PlatformIntegrationService> createForGUI(std::shared_ptr<IAgentRuntime> runtime);
    static std::shared_ptr<PlatformIntegrationService> createForAgent(std::shared_ptr<IAgentRuntime> runtime);
};

class PlatformAuthUtils : public object, public std::enable_shared_from_this<PlatformAuthUtils> {
public:
    using std::enable_shared_from_this<PlatformAuthUtils>::shared_from_this;
    static string generateSessionId();
    static boolean isValidSessionId(string sessionId);
    template <typename P0>
    static array<string> getClientCapabilities(P0 clientType);
    static boolean isProviderCompatible(array<string> providerCapabilities, array<string> clientCapabilities);
};

template <typename P1>
std::shared_ptr<Promise<std::shared_ptr<ClientSession>>> PlatformIntegrationService::registerSession(string sessionId, P1 clientType, string platformId)
{
    logger->debug(std::string("Registering session ") + sessionId + std::string(" for ") + clientType + std::string(" client"));
    auto session = object{
        object::pair{std::string("sessionId"), std::string("sessionId")}, 
        object::pair{std::string("clientType"), std::string("clientType")}, 
        object::pair{std::string("platformId"), std::string("platformId")}, 
        object::pair{std::string("authStatus"), nullptr}, 
        object::pair{std::string("lastActivity"), std::make_shared<Date>()}, 
        object::pair{std::string("validatedKeys"), array<any>()}
    };
    this->activeSessions->set(sessionId, session);
    try
    {
        session->authStatus = std::async([=]() { this->authService->getAuthStatus(); });
    }
    catch (const any& error)
    {
        logger->warn(std::string("Failed to get initial auth status for session ") + sessionId + std::string(":"), error);
    }
    return session;
}

template <typename P0>
array<string> PlatformAuthUtils::getClientCapabilities(P0 clientType)
{
    static switch_type __switch12683_13063 = {
        { any(std::string("cli")), 1 },
        { any(std::string("gui")), 2 },
        { any(std::string("agent")), 3 }
    };
    switch (__switch12683_13063[clientType])
    {
    case 1:
        return array<string>{ std::string("text_generation"), std::string("embeddings"), std::string("validation"), std::string("testing") };
    case 2:
        return array<string>{ std::string("text_generation"), std::string("embeddings"), std::string("image_description"), std::string("validation"), std::string("monitoring") };
    case 3:
        return array<string>{ std::string("text_generation"), std::string("embeddings"), std::string("image_description"), std::string("reasoning") };
    default:
        return array<any>();
    }
}

#endif
