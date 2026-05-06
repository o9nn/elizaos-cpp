#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-elizaos-services/src/auth/PlatformIntegration.h"

PlatformIntegrationService::PlatformIntegrationService(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<PlatformAuthConfig> config) {
    this->runtime = runtime;
    this->authService = std::make_shared<AuthenticationService>(runtime);
    logger->info(std::string("Initializing platform integration for ") + config->clientType + std::string(" client"));
}

std::shared_ptr<Promise<std::shared_ptr<KeyDistributionResponse>>> PlatformIntegrationService::distributeKey(std::shared_ptr<KeyDistributionRequest> request)
{
    auto session = this->activeSessions->get(request->sessionId);
    if (!session) {
        return object{
            object::pair{std::string("success"), false}, 
            object::pair{std::string("keyType"), request->keyType}, 
            object::pair{std::string("capabilities"), array<any>()}, 
            object::pair{std::string("error"), std::string("Invalid session ID")}
        };
    }
    session->lastActivity = std::make_shared<Date>();
    try
    {
        string apiKey;
        array<string> capabilities;
        if (request->keyType == std::string("test")) {
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
            object::pair{std::string("timestamp"), std::make_shared<Date>()}, 
            object::pair{std::string("sessionId"), request->sessionId}, 
            object::pair{std::string("provider"), request->provider}, 
            object::pair{std::string("keyType"), request->keyType}, 
            object::pair{std::string("success"), true}
        });
        session->validatedKeys->push(string_empty + request->provider + std::string(":") + request->keyType + string_empty);
        return object{
            object::pair{std::string("success"), true}, 
            object::pair{std::string("apiKey"), std::string("apiKey")}, 
            object::pair{std::string("keyType"), request->keyType}, 
            object::pair{std::string("capabilities"), std::string("capabilities")}, 
            object::pair{std::string("expiresAt"), (request->keyType == std::string("test")) ? any(std::make_shared<Date>(Date->now() + 24 * 60 * 60 * 1000)) : any(undefined)}
        };
    }
    catch (const any& error)
    {
        logger->error(std::string("Failed to distribute ") + request->keyType + std::string(" key for ") + request->provider + std::string(":"), error);
        this->keyDistributionLog->push(object{
            object::pair{std::string("timestamp"), std::make_shared<Date>()}, 
            object::pair{std::string("sessionId"), request->sessionId}, 
            object::pair{std::string("provider"), request->provider}, 
            object::pair{std::string("keyType"), request->keyType}, 
            object::pair{std::string("success"), false}
        });
        return object{
            object::pair{std::string("success"), false}, 
            object::pair{std::string("keyType"), request->keyType}, 
            object::pair{std::string("capabilities"), array<any>()}, 
            object::pair{std::string("error"), (is<Error>(error)) ? any(error->message) : any(std::string("Unknown distribution error"))}
        };
    }
}

std::shared_ptr<Promise<object>> PlatformIntegrationService::validateDistributedKey(string sessionId, string provider, string apiKey)
{
    auto session = this->activeSessions->get(sessionId);
    if (!session) {
        return object{
            object::pair{std::string("isValid"), false}, 
            object::pair{std::string("error"), std::string("Invalid session ID")}
        };
    }
    try
    {
        auto result = std::async([=]() { this->authService->validateApiKey(provider, apiKey); });
        if (result->isValid) {
            session->authStatus = std::async([=]() { this->authService->getAuthStatus(); });
            session->lastActivity = std::make_shared<Date>();
            logger->debug(std::string("Successfully validated ") + provider + std::string(" key for session ") + sessionId + string_empty);
            return object{
                object::pair{std::string("isValid"), true}
            };
        } else {
            logger->warn(std::string("Key validation failed for ") + provider + std::string(" in session ") + sessionId + std::string(": ") + result->errorMessage + string_empty);
            return object{
                object::pair{std::string("isValid"), false}, 
                object::pair{std::string("error"), result->errorMessage}
            };
        }
    }
    catch (const any& error)
    {
        logger->error(std::string("Key validation error for ") + provider + std::string(" in session ") + sessionId + std::string(":"), error);
        return object{
            object::pair{std::string("isValid"), false}, 
            object::pair{std::string("error"), (is<Error>(error)) ? any(error->message) : any(std::string("Validation error"))}
        };
    }
}

std::shared_ptr<Promise<object>> PlatformIntegrationService::invalidateSession(string sessionId)
{
    auto session = this->activeSessions->get(sessionId);
    if (!session) {
        return object{
            object::pair{std::string("success"), false}, 
            object::pair{std::string("error"), std::string("Session not found")}
        };
    }
    logger->info(std::string("Invalidating session ") + sessionId + std::string(" for ") + session->clientType + std::string(" client"));
    this->activeSessions->delete(sessionId);
    this->authService->clearCache();
    return object{
        object::pair{std::string("success"), true}
    };
}

std::shared_ptr<Promise<object>> PlatformIntegrationService::getSessionStatus(string sessionId)
{
    auto session = this->activeSessions->get(sessionId);
    if (!session) {
        return object{
            object::pair{std::string("session"), nullptr}, 
            object::pair{std::string("authStatus"), nullptr}, 
            object::pair{std::string("capabilities"), array<any>()}
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
            logger->warn(std::string("Failed to refresh auth status for session ") + sessionId + std::string(":"), error);
        }
    }
    return object{
        object::pair{std::string("session"), std::string("session")}, 
        object::pair{std::string("authStatus"), session->authStatus}, 
        object::pair{std::string("capabilities"), OR((session->authStatus->capabilities), (array<any>()))}
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
        object::pair{std::string("activeSessions"), this->activeSessions->size}, 
        object::pair{std::string("sessionsByType"), std::string("sessionsByType")}, 
        object::pair{std::string("keyDistributions"), this->keyDistributionLog->get_length()}, 
        object::pair{std::string("recentActivity"), this->keyDistributionLog->slice(-10)->sort([=](auto a, auto b) mutable
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
        logger->debug(std::string("Cleaned up expired session: ") + sessionId + string_empty);
    }
    );
    return expiredSessions->get_length();
}

std::shared_ptr<Promise<std::shared_ptr<KeyDistributionResponse>>> PlatformIntegrationService::distributeTestKey(string provider, string clientType)
{
    auto testKeyMap = object{
        object::pair{std::string("openai"), TEST_KEYS["OPENAI_TEST_KEY"]}, 
        object::pair{std::string("groq"), TEST_KEYS["GROQ_TEST_KEY"]}, 
        object::pair{std::string("anthropic"), TEST_KEYS["ANTHROPIC_TEST_KEY"]}
    };
    auto apiKey = const_(testKeyMap)[provider];
    if (!apiKey) {
        return object{
            object::pair{std::string("success"), false}, 
            object::pair{std::string("keyType"), std::string("test")}, 
            object::pair{std::string("capabilities"), array<any>()}, 
            object::pair{std::string("error"), std::string("No test key available for provider: ") + provider + string_empty}
        };
    }
    auto validation = std::async([=]() { this->authService->validateApiKey(provider, apiKey); });
    logger->info(std::string("Distributed test key for ") + provider + std::string(" to ") + clientType + std::string(" client"));
    return object{
        object::pair{std::string("success"), true}, 
        object::pair{std::string("apiKey"), std::string("apiKey")}, 
        object::pair{std::string("keyType"), std::string("test")}, 
        object::pair{std::string("capabilities"), validation->capabilities}
    };
}

std::shared_ptr<Promise<std::shared_ptr<KeyDistributionResponse>>> PlatformIntegrationService::distributeProductionKey(string provider, std::shared_ptr<ClientSession> session)
{
    auto keyMap = object{
        object::pair{std::string("openai"), std::string("OPENAI_API_KEY")}, 
        object::pair{std::string("groq"), std::string("GROQ_API_KEY")}, 
        object::pair{std::string("anthropic"), std::string("ANTHROPIC_API_KEY")}
    };
    auto envKey = const_(keyMap)[provider];
    if (!envKey) {
        return object{
            object::pair{std::string("success"), false}, 
            object::pair{std::string("keyType"), std::string("production")}, 
            object::pair{std::string("capabilities"), array<any>()}, 
            object::pair{std::string("error"), std::string("Unsupported provider: ") + provider + string_empty}
        };
    }
    auto apiKey = OR((this->runtime->getSetting(envKey)), (const_(process->env)[envKey]));
    if (!apiKey) {
        return object{
            object::pair{std::string("success"), false}, 
            object::pair{std::string("keyType"), std::string("production")}, 
            object::pair{std::string("capabilities"), array<any>()}, 
            object::pair{std::string("error"), std::string("Production key not configured for ") + provider + string_empty}
        };
    }
    auto validation = std::async([=]() { this->authService->validateApiKey(provider, apiKey); });
    if (!validation->isValid) {
        return object{
            object::pair{std::string("success"), false}, 
            object::pair{std::string("keyType"), std::string("production")}, 
            object::pair{std::string("capabilities"), array<any>()}, 
            object::pair{std::string("error"), std::string("Production key validation failed: ") + validation->errorMessage + string_empty}
        };
    }
    logger->info(std::string("Distributed production key for ") + provider + std::string(" to ") + session->clientType + std::string(" client (session: ") + session->sessionId + std::string(")"));
    return object{
        object::pair{std::string("success"), true}, 
        object::pair{std::string("apiKey"), std::string("apiKey")}, 
        object::pair{std::string("keyType"), std::string("production")}, 
        object::pair{std::string("capabilities"), validation->capabilities}
    };
}

std::shared_ptr<PlatformIntegrationService> PlatformIntegrationFactory::createForCLI(std::shared_ptr<IAgentRuntime> runtime)
{
    return std::make_shared<PlatformIntegrationService>(runtime, object{
        object::pair{std::string("platformId"), std::string("elizaos-cli")}, 
        object::pair{std::string("clientType"), std::string("cli")}, 
        object::pair{std::string("distributionMode"), std::string("auto")}, 
        object::pair{std::string("allowTestKeys"), true}
    });
}

std::shared_ptr<PlatformIntegrationService> PlatformIntegrationFactory::createForGUI(std::shared_ptr<IAgentRuntime> runtime)
{
    return std::make_shared<PlatformIntegrationService>(runtime, object{
        object::pair{std::string("platformId"), std::string("elizaos-gui")}, 
        object::pair{std::string("clientType"), std::string("gui")}, 
        object::pair{std::string("distributionMode"), std::string("auto")}, 
        object::pair{std::string("allowTestKeys"), true}
    });
}

std::shared_ptr<PlatformIntegrationService> PlatformIntegrationFactory::createForAgent(std::shared_ptr<IAgentRuntime> runtime)
{
    return std::make_shared<PlatformIntegrationService>(runtime, object{
        object::pair{std::string("platformId"), std::string("elizaos-agent")}, 
        object::pair{std::string("clientType"), std::string("agent")}, 
        object::pair{std::string("distributionMode"), std::string("production")}, 
        object::pair{std::string("allowTestKeys"), false}
    });
}

string PlatformAuthUtils::generateSessionId()
{
    return std::string("session_") + Date->now() + std::string("_") + Math->random()->toString(36)->substring(2, 15) + string_empty;
}

boolean PlatformAuthUtils::isValidSessionId(string sessionId)
{
    return (new RegExp(std::string("^session_\d+_[a-z0-9]{13}")))->test(sessionId);
}

boolean PlatformAuthUtils::isProviderCompatible(array<string> providerCapabilities, array<string> clientCapabilities)
{
    return clientCapabilities->some([=](auto cap) mutable
    {
        return providerCapabilities->includes(cap);
    }
    );
}

