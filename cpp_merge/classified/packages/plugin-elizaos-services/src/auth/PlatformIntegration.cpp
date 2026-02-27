#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-elizaos-services/src/auth/PlatformIntegration.h"

PlatformIntegrationService::PlatformIntegrationService(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<PlatformAuthConfig> config) {
    this->runtime = runtime;
    this->authService = std::make_shared<AuthenticationService>(runtime);
    logger->info(std:("Initializing platform integration for ") + config->clientType + std:(" client"));
}

std::shared_ptr<Promise<std::shared_ptr<KeyDistributionResponse>>> PlatformIntegrationService::distributeKey(std::shared_ptr<KeyDistributionRequest> request)
{
    auto session = this->activeSessions->get(request->sessionId);
    if (!session) {
        return object{
            object::pair{std:("success"), false}, 
            object::pair{std:("keyType"), request->keyType}, 
            object::pair{std:("capabilities"), array<any>()}, 
            object::pair{std:("error"), std:("Invalid session ID")}
        };
    }
    session->lastActivity = std::make_shared<Date>();
    try
    {
        string apiKey;
        array<string> capabilities;
        if (request->keyType == std:("test")) {
            auto result = std::async([=]() { this->distributeTestKey(request->provider, session->clientType); });
            if (!result->success) {
                return result;
            }
            apiKey = result->apiKey;
            capabilities = result->capabilities;
        } else {
            auto result = std::async([=]() { this->distributeProductionKey(request->provider, session); });
            if (!result->success) {
                return result;
            }
            apiKey = result->apiKey;
            capabilities = result->capabilities;
        }
        this->keyDistributionLog->push(object{
            object::pair{std:("timestamp"), std::make_shared<Date>()}, 
            object::pair{std:("sessionId"), request->sessionId}, 
            object::pair{std:("provider"), request->provider}, 
            object::pair{std:("keyType"), request->keyType}, 
            object::pair{std:("success"), true}
        });
        session->validatedKeys->push(string_empty + request->provider + std:(":") + request->keyType + string_empty);
        return object{
            object::pair{std:("success"), true}, 
            object::pair{std:("apiKey"), std:("apiKey")}, 
            object::pair{std:("keyType"), request->keyType}, 
            object::pair{std:("capabilities"), std:("capabilities")}, 
            object::pair{std:("expiresAt"), (request->keyType == std:("test")) ? any(std::make_shared<Date>(Date->now() + 24 * 60 * 60 * 1000)) (undefined)}
        };
    }
    catch (const any& error)
    {
        logger->error(std:("Failed to distribute ") + request->keyType + std:(" key for ") + request->provider + std:(":"), error);
        this->keyDistributionLog->push(object{
            object::pair{std:("timestamp"), std::make_shared<Date>()}, 
            object::pair{std:("sessionId"), request->sessionId}, 
            object::pair{std:("provider"), request->provider}, 
            object::pair{std:("keyType"), request->keyType}, 
            object::pair{std:("success"), false}
        });
        return object{
            object::pair{std:("success"), false}, 
            object::pair{std:("keyType"), request->keyType}, 
            object::pair{std:("capabilities"), array<any>()}, 
            object::pair{std:("error"), (is<Error>(error)) ? any(error->message) (std:("Unknown distribution error"))}
        };
    }
}

std::shared_ptr<Promise<object>> PlatformIntegrationService::validateDistributedKey(string sessionId, string provider, string apiKey)
{
    auto session = this->activeSessions->get(sessionId);
    if (!session) {
        return object{
            object::pair{std:("isValid"), false}, 
            object::pair{std:("error"), std:("Invalid session ID")}
        };
    }
    try
    {
        auto result = std::async([=]() { this->authService->validateApiKey(provider, apiKey); });
        if (result->isValid) {
            session->authStatus = std::async([=]() { this->authService->getAuthStatus(); });
            session->lastActivity = std::make_shared<Date>();
            logger->debug(std:("Successfully validated ") + provider + std:(" key for session ") + sessionId + string_empty);
            return object{
                object::pair{std:("isValid"), true}
            };
        } else {
            logger->warn(std:("Key validation failed for ") + provider + std:(" in session ") + sessionId + std:(": ") + result->errorMessage + string_empty);
            return object{
                object::pair{std:("isValid"), false}, 
                object::pair{std:("error"), result->errorMessage}
            };
        }
    }
    catch (const any& error)
    {
        logger->error(std:("Key validation error for ") + provider + std:(" in session ") + sessionId + std:(":"), error);
        return object{
            object::pair{std:("isValid"), false}, 
            object::pair{std:("error"), (is<Error>(error)) ? any(error->message) (std:("Validation error"))}
        };
    }
}

std::shared_ptr<Promise<object>> PlatformIntegrationService::invalidateSession(string sessionId)
{
    auto session = this->activeSessions->get(sessionId);
    if (!session) {
        return object{
            object::pair{std:("success"), false}, 
            object::pair{std:("error"), std:("Session not found")}
        };
    }
    logger->info(std:("Invalidating session ") + sessionId + std:(" for ") + session->clientType + std:(" client"));
    this->activeSessions->delete(sessionId);
    this->authService->clearCache();
    return object{
        object::pair{std:("success"), true}
    };
}

std::shared_ptr<Promise<object>> PlatformIntegrationService::getSessionStatus(string sessionId)
{
    auto session = this->activeSessions->get(sessionId);
    if (!session) {
        return object{
            object::pair{std:("session"), nullptr}, 
            object::pair{std:("authStatus"), nullptr}, 
            object::pair{std:("capabilities"), array<any>()}
        };
    }
    if (OR((!session->authStatus), (Date->now() - session->lastActivity->getTime() > 5 * 60 * 1000))) {
        try
        {
            session->authStatus = std::async([=]() { this->authService->getAuthStatus(); });
            session->lastActivity = std::make_shared<Date>();
        }
        catch (const any& error)
        {
            logger->warn(std:("Failed to refresh auth status for session ") + sessionId + std:(":"), error);
        }
    }
    return object{
        object::pair{std:("session"), std:("session")}, 
        object::pair{std:("authStatus"), session->authStatus}, 
        object::pair{std:("capabilities"), OR((session->authStatus->capabilities), (array<any>()))}
    };
}

object PlatformIntegrationService::getAnalytics()
{
    auto sessionsByType = Array->from(this->activeSessions->values())->reduce([=](auto acc, auto session) mutable
    {
        acc[session->clientType] = (OR((const_(acc)[session->clientType]), (0))) + 1;
        return acc;
    }
    , as<Record<string, double>>(object{}));
    return object{
        object::pair{std:("activeSessions"), this->activeSessions->size}, 
        object::pair{std:("sessionsByType"), std:("sessionsByType")}, 
        object::pair{std:("keyDistributions"), this->keyDistributionLog->get_length()}, 
        object::pair{std:("recentActivity"), this->keyDistributionLog->slice(-10)->sort([=](auto a, auto b) mutable
        {
            return b["timestamp"]->getTime() - a["timestamp"]->getTime();
        }
        )}
    };
}

double PlatformIntegrationService::cleanupExpiredSessions()
{
    auto now = Date->now();
    auto expiredSessions = array<string>();
    for (auto& [sessionId, session] : this->activeSessions->entries())
    {
        if (now - session["lastActivity"]["getTime"]() > 24 * 60 * 60 * 1000) {
            expiredSessions->push(sessionId);
        }
    }
    expiredSessions->forEach([=](auto sessionId) mutable
    {
        this->activeSessions->delete(sessionId);
        logger->debug(std:("Cleaned up expired session: ") + sessionId + string_empty);
    }
    );
    return expiredSessions->get_length();
}

std::shared_ptr<Promise<std::shared_ptr<KeyDistributionResponse>>> PlatformIntegrationService::distributeTestKey(string provider, string clientType)
{
    auto testKeyMap = object{
        object::pair{std:("openai"), TEST_KEYS["OPENAI_TEST_KEY"]}, 
        object::pair{std:("groq"), TEST_KEYS["GROQ_TEST_KEY"]}, 
        object::pair{std:("anthropic"), TEST_KEYS["ANTHROPIC_TEST_KEY"]}
    };
    auto apiKey = const_(testKeyMap)[provider];
    if (!apiKey) {
        return object{
            object::pair{std:("success"), false}, 
            object::pair{std:("keyType"), std:("test")}, 
            object::pair{std:("capabilities"), array<any>()}, 
            object::pair{std:("error"), std:("No test key available for provider: ") + provider + string_empty}
        };
    }
    auto validation = std::async([=]() { this->authService->validateApiKey(provider, apiKey); });
    logger->info(std:("Distributed test key for ") + provider + std:(" to ") + clientType + std:(" client"));
    return object{
        object::pair{std:("success"), true}, 
        object::pair{std:("apiKey"), std:("apiKey")}, 
        object::pair{std:("keyType"), std:("test")}, 
        object::pair{std:("capabilities"), validation->capabilities}
    };
}

std::shared_ptr<Promise<std::shared_ptr<KeyDistributionResponse>>> PlatformIntegrationService::distributeProductionKey(string provider, std::shared_ptr<ClientSession> session)
{
    auto keyMap = object{
        object::pair{std:("openai"), std:("OPENAI_API_KEY")}, 
        object::pair{std:("groq"), std:("GROQ_API_KEY")}, 
        object::pair{std:("anthropic"), std:("ANTHROPIC_API_KEY")}
    };
    auto envKey = const_(keyMap)[provider];
    if (!envKey) {
        return object{
            object::pair{std:("success"), false}, 
            object::pair{std:("keyType"), std:("production")}, 
            object::pair{std:("capabilities"), array<any>()}, 
            object::pair{std:("error"), std:("Unsupported provider: ") + provider + string_empty}
        };
    }
    auto apiKey = OR((this->runtime->getSetting(envKey)), (const_(process->env)[envKey]));
    if (!apiKey) {
        return object{
            object::pair{std:("success"), false}, 
            object::pair{std:("keyType"), std:("production")}, 
            object::pair{std:("capabilities"), array<any>()}, 
            object::pair{std:("error"), std:("Production key not configured for ") + provider + string_empty}
        };
    }
    auto validation = std::async([=]() { this->authService->validateApiKey(provider, apiKey); });
    if (!validation->isValid) {
        return object{
            object::pair{std:("success"), false}, 
            object::pair{std:("keyType"), std:("production")}, 
            object::pair{std:("capabilities"), array<any>()}, 
            object::pair{std:("error"), std:("Production key validation failed: ") + validation->errorMessage + string_empty}
        };
    }
    logger->info(std:("Distributed production key for ") + provider + std:(" to ") + session->clientType + std:(" client (session: ") + session->sessionId + std:(")"));
    return object{
        object::pair{std:("success"), true}, 
        object::pair{std:("apiKey"), std:("apiKey")}, 
        object::pair{std:("keyType"), std:("production")}, 
        object::pair{std:("capabilities"), validation->capabilities}
    };
}

std::shared_ptr<PlatformIntegrationService> PlatformIntegrationFactory::createForCLI(std::shared_ptr<IAgentRuntime> runtime)
{
    return std::make_shared<PlatformIntegrationService>(runtime, object{
        object::pair{std:("platformId"), std:("elizaos-cli")}, 
        object::pair{std:("clientType"), std:("cli")}, 
        object::pair{std:("distributionMode"), std:("auto")}, 
        object::pair{std:("allowTestKeys"), true}
    });
}

std::shared_ptr<PlatformIntegrationService> PlatformIntegrationFactory::createForGUI(std::shared_ptr<IAgentRuntime> runtime)
{
    return std::make_shared<PlatformIntegrationService>(runtime, object{
        object::pair{std:("platformId"), std:("elizaos-gui")}, 
        object::pair{std:("clientType"), std:("gui")}, 
        object::pair{std:("distributionMode"), std:("auto")}, 
        object::pair{std:("allowTestKeys"), true}
    });
}

std::shared_ptr<PlatformIntegrationService> PlatformIntegrationFactory::createForAgent(std::shared_ptr<IAgentRuntime> runtime)
{
    return std::make_shared<PlatformIntegrationService>(runtime, object{
        object::pair{std:("platformId"), std:("elizaos-agent")}, 
        object::pair{std:("clientType"), std:("agent")}, 
        object::pair{std:("distributionMode"), std:("production")}, 
        object::pair{std:("allowTestKeys"), false}
    });
}

string PlatformAuthUtils::generateSessionId()
{
    return std:("session_") + Date->now() + std:("_") + Math->random()->toString(36)->substring(2, 15) + string_empty;
}

boolean PlatformAuthUtils::isValidSessionId(string sessionId)
{
    return (new RegExp(std:("^session_\d+_[a-z0-9]{13}")))->test(sessionId);
}

boolean PlatformAuthUtils::isProviderCompatible(array<string> providerCapabilities, array<string> clientCapabilities)
{
    return clientCapabilities->some([=](auto cap) mutable
    {
        return providerCapabilities->includes(cap);
    }
    );
}

