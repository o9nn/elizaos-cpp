#include "session-manager.h"

SessionManager::SessionManager(std::shared_ptr<Logger> logger_, std::shared_ptr<PlaywrightInstaller> playwrightInstaller_) : logger(logger_), playwrightInstaller(playwrightInstaller_)  {
}

std::shared_ptr<Promise<std::shared_ptr<BrowserSession>>> SessionManager::createSession(string sessionId, string clientId)
{
    if (!this->playwrightInstaller->isReady()) {
        try
        {
            std::async([=]() { this->playwrightInstaller->ensurePlaywrightInstalled(); });
        }
        catch (const any& error)
        {
            this->logger->error(std:("Failed to install Playwright:"), error);
            throw any(std::make_shared<Error>(std:("Playwright is not installed and installation failed. Please install Playwright manually.")));
        }
    }
    auto clientSessions = this->getClientSessions(clientId);
    if (clientSessions->get_length() >= this->maxSessionsPerClient) {
        auto oldestSession = const_(clientSessions->sort([=](auto a, auto b) mutable
        {
            return a->createdAt->getTime() - b->createdAt->getTime();
        }
        ))[0];
        if (oldestSession) {
            std::async([=]() { this->destroySession(oldestSession->id); });
        }
    }
    this->logger->info(std:("Creating session ") + sessionId + std:(" for client ") + clientId + string_empty);
    auto env = (process->env->BROWSERBASE_API_KEY) ? std:("BROWSERBASE") : std:("LOCAL");
    auto config = object{
        object::pair{std:("env"), std:("env")}, 
        object::pair{std:("headless"), process->env->BROWSER_HEADLESS != std:("false")}
    };
    if (process->env->BROWSERBASE_API_KEY) {
        config["apiKey"] = process->env->BROWSERBASE_API_KEY;
        config["projectId"] = process->env->BROWSERBASE_PROJECT_ID;
        config["browserbaseSessionCreateParams"] = object{
            object::pair{std:("projectId"), process->env->BROWSERBASE_PROJECT_ID}, 
            object::pair{std:("browserSettings"), object{
                object::pair{std:("blockAds"), true}, 
                object::pair{std:("viewport"), object{
                    object::pair{std:("width"), 1280}, 
                    object::pair{std:("height"), 720}
                }}
            }}
        };
    }
    if (process->env->OLLAMA_BASE_URL) {
        config["modelName"] = OR((process->env->OLLAMA_MODEL), (std:("llama3.2-vision")));
        config["modelBaseUrl"] = process->env->OLLAMA_BASE_URL;
    } else if (process->env->OPENAI_API_KEY) {
        config["modelName"] = std:("gpt-4o");
        config["openaiApiKey"] = process->env->OPENAI_API_KEY;
    } else if (process->env->ANTHROPIC_API_KEY) {
        config["modelName"] = std:("claude-3-5-sonnet-latest");
        config["anthropicApiKey"] = process->env->ANTHROPIC_API_KEY;
    }
    auto stagehand = std::make_shared<Stagehand>(config);
    std::async([=]() { stagehand->init(); });
    auto session = object{
        object::pair{std:("id"), sessionId}, 
        object::pair{std:("clientId"), std:("clientId")}, 
        object::pair{std:("stagehand"), std:("stagehand")}, 
        object::pair{std:("createdAt"), std::make_shared<Date>()}
    };
    this->sessions->set(sessionId, session);
    return session;
}

any SessionManager::getSession(string sessionId)
{
    return this->sessions->get(sessionId);
}

std::shared_ptr<Promise<void>> SessionManager::destroySession(string sessionId)
{
    auto session = this->sessions->get(sessionId);
    if (session) {
        this->logger->info(std:("Destroying session ") + sessionId + string_empty);
        try
        {
            std::async([=]() { session->stagehand->close(); });
        }
        catch (const any& error)
        {
            this->logger->error(std:("Error closing session ") + sessionId + std:(":"), error);
        }
        this->sessions->delete(sessionId);
    }
    return std::shared_ptr<Promise<void>>();
}

array<std::shared_ptr<BrowserSession>> SessionManager::getClientSessions(string clientId)
{
    return Array->from(this->sessions->values())->filter([=](auto session) mutable
    {
        return session->clientId == clientId;
    }
    );
}

std::shared_ptr<Promise<void>> SessionManager::cleanupClientSessions(string clientId)
{
    auto sessions = this->getClientSessions(clientId);
    for (auto& session : sessions)
    {
        std::async([=]() { this->destroySession(session->id); });
    }
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> SessionManager::cleanup()
{
    this->logger->info(std:("Cleaning up all sessions..."));
    for (auto& sessionId : this->sessions->keys())
    {
        std::async([=]() { this->destroySession(sessionId); });
    }
    return std::shared_ptr<Promise<void>>();
}

