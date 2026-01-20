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

    string provider;

    any keyType;

    array<string> capabilities;

    string errorMessage;

    object rateLimits;

    object usage;
};

class AuthStatus : public object, public std::enable_shared_from_this<AuthStatus> {
public:
    using std::enable_shared_from_this<AuthStatus>::shared_from_this;
    any overall;

    object providers;

    std::shared_ptr<Date> lastChecked;

    array<string> capabilities;
};

extern object TEST_KEYS;
class AuthenticationService : public object, public std::enable_shared_from_this<AuthenticationService> {
public:
    using std::enable_shared_from_this<AuthenticationService>::shared_from_this;
    std::shared_ptr<IAgentRuntime> runtime;

    any lastAuthStatus = nullptr;

    std::shared_ptr<Map<string, object>> validationCache = std::make_shared<Map<string, object>>();

    double CACHE_TTL = 5 * 60 * 1000;

    AuthenticationService(std::shared_ptr<IAgentRuntime> runtime);
    virtual std::shared_ptr<Promise<std::shared_ptr<ApiKeyValidationResult>>> validateApiKey(string provider, string apiKey);
    virtual std::shared_ptr<Promise<std::shared_ptr<AuthStatus>>> getAuthStatus();
    virtual std::shared_ptr<Promise<object>> testApiFunctionality(string provider);
    virtual std::shared_ptr<Promise<object>> validateAllProviders();
    virtual any getApiKey(string provider);
    virtual boolean isTestKey(string apiKey);
    virtual array<string> getTestKeyCapabilities(string provider);
    virtual std::shared_ptr<Promise<std::shared_ptr<ApiKeyValidationResult>>> performRealKeyValidation(string provider, string apiKey);
    virtual std::shared_ptr<Promise<std::shared_ptr<ApiKeyValidationResult>>> validateOpenAIKey(string apiKey);
    virtual std::shared_ptr<Promise<std::shared_ptr<ApiKeyValidationResult>>> validateGroqKey(string apiKey);
    virtual std::shared_ptr<Promise<std::shared_ptr<ApiKeyValidationResult>>> validateAnthropicKey(string apiKey);
    virtual std::shared_ptr<Promise<object>> performApiTest(string provider, string apiKey);
    virtual std::shared_ptr<Promise<object>> testOpenAI(string apiKey);
    virtual std::shared_ptr<Promise<object>> testGroq(string apiKey);
    virtual std::shared_ptr<Promise<object>> testAnthropic(string apiKey);
    virtual void clearCache();
    virtual any getCachedAuthStatus();
};

#endif
