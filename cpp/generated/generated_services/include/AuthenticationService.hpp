#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-ELIZAOS-SERVICES_SRC_AUTH_AUTHENTICATIONSERVICE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-ELIZAOS-SERVICES_SRC_AUTH_AUTHENTICATIONSERVICE_H
#include "core.h"
#include "@elizaos/core.h"

class ApiKeyValidationResult;
class AuthStatus;
class AuthenticationService;

class ApiKeyValidationResult : public object, public std::enable_shared_from_this<ApiKeyValidationResult> {
public:
    using std::enable_shared_from_this<ApiKeyValidationResult>::shared_from_this;
    boolean isValid;

    std::string provider;

    std::any keyType;

    array<string> capabilities;

    std::string errorMessage;

    object rateLimits;

    object usage;
};

class AuthStatus : public object, public std::enable_shared_from_this<AuthStatus> {
public:
    using std::enable_shared_from_this<AuthStatus>::shared_from_this;
    std::any overall;

    object providers;

    std::shared_ptr<Date> lastChecked;

    array<string> capabilities;
};

extern object TEST_KEYS;
class AuthenticationService : public object, public std::enable_shared_from_this<AuthenticationService> {
public:
    using std::enable_shared_from_this<AuthenticationService>::shared_from_this;
    std::shared_ptr<IAgentRuntime> runtime;

    std::any lastAuthStatus = nullptr;

    std::shared_ptr<Map<std::string, object>> validationCache = std::make_shared<Map<std::string, object>>();

    double CACHE_TTL = 5 * 60 * 1000;

    AuthenticationService(std::shared_ptr<IAgentRuntime> runtime);
    virtual std::shared_ptr<Promise<std::shared_ptr<ApiKeyValidationResult>>> validateApiKey(std::string provider, std::string apiKey);
    virtual std::shared_ptr<Promise<std::shared_ptr<AuthStatus>>> getAuthStatus();
    virtual std::shared_ptr<Promise<object>> testApiFunctionality(std::string provider);
    virtual std::shared_ptr<Promise<object>> validateAllProviders();
    virtual std::any getApiKey(std::string provider);
    virtual boolean isTestKey(std::string apiKey);
    virtual array<string> getTestKeyCapabilities(std::string provider);
    virtual std::shared_ptr<Promise<std::shared_ptr<ApiKeyValidationResult>>> performRealKeyValidation(std::string provider, std::string apiKey);
    virtual std::shared_ptr<Promise<std::shared_ptr<ApiKeyValidationResult>>> validateOpenAIKey(std::string apiKey);
    virtual std::shared_ptr<Promise<std::shared_ptr<ApiKeyValidationResult>>> validateGroqKey(std::string apiKey);
    virtual std::shared_ptr<Promise<std::shared_ptr<ApiKeyValidationResult>>> validateAnthropicKey(std::string apiKey);
    virtual std::shared_ptr<Promise<object>> performApiTest(std::string provider, std::string apiKey);
    virtual std::shared_ptr<Promise<object>> testOpenAI(std::string apiKey);
    virtual std::shared_ptr<Promise<object>> testGroq(std::string apiKey);
    virtual std::shared_ptr<Promise<object>> testAnthropic(std::string apiKey);
    virtual void clearCache();
    virtual std::any getCachedAuthStatus();
};

#endif
