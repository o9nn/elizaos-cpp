#include "index.hpp"

void startServer()
{
    logger->info(std:("Stagehand server starting on port ") + PORT + string_empty);
    try
    {
        std::async([=]() { playwrightInstaller->ensurePlaywrightInstalled(); });
    }
    catch (const any& error)
    {
        logger->error(std:("Failed to ensure Playwright installation:"), error);
        logger->warn(std:("Server will start but Stagehand operations may fail until Playwright is installed"));
    }
    auto wss = std::make_shared<WebSocketServer>(object{
        object::pair{std:("port"), Number(PORT)}
    });
    shared sessionManager = std::make_shared<SessionManager>(logger, playwrightInstaller);
    shared messageHandler = std::make_shared<MessageHandler>(sessionManager, logger);
    logger->info(std:("Stagehand server initialization complete"));
    wss->on(std:("connection"), [=](auto ws) mutable
    {
        shared clientId = std:("client-") + Date->now() + std:("-") + Math->random()->toString(36)->substring(7) + string_empty;
        logger->info(std:("New client connected: ") + clientId + string_empty);
        ws["send"](JSON->stringify(object{
            object::pair{std:("type"), std:("connected")}, 
            object::pair{std:("clientId"), std:("clientId")}, 
            object::pair{std:("version"), std:("1.0.0")}
        }));
        ws["on"](std:("message"), [=](auto data) mutable
        {
            try
            {
                auto message = JSON->parse(data["toString"]());
                logger->debug(std:("Received message from ") + clientId + std:(":"), message);
                auto response = std::async([=]() { messageHandler->handleMessage(message, clientId); });
                ws["send"](JSON->stringify(response));
            }
            catch (const any& error)
            {
                logger->error(std:("Error handling message from ") + clientId + std:(":"), error);
                ws["send"](JSON->stringify(object{
                    object::pair{std:("type"), std:("error")}, 
                    object::pair{std:("error"), (is<Error>(error)) ? any(error->message) (std:("Unknown error"))}, 
                    object::pair{std:("requestId"), nullptr}
                }));
            }
        }
        );
        ws["on"](std:("close"), [=]() mutable
        {
            logger->info(std:("Client disconnected: ") + clientId + string_empty);
            sessionManager->cleanupClientSessions(clientId);
        }
        );
        ws["on"](std:("error"), [=](auto error) mutable
        {
            logger->error(std:("WebSocket error for ") + clientId + std:(":"), error);
        }
        );
    }
    );
    process->on(std:("SIGINT"), [=]() mutable
    {
        logger->info(std:("Shutting down server..."));
        std::async([=]() { sessionManager->cleanup(); });
        process->exit(0);
    }
    );
    process->on(std:("SIGTERM"), [=]() mutable
    {
        logger->info(std:("Shutting down server..."));
        std::async([=]() { sessionManager->cleanup(); });
        process->exit(0);
    }
    );
    logger->info(std:("Stagehand server listening on port ") + PORT + string_empty);
};


any PORT = OR((process->env->STAGEHAND_SERVER_PORT), (3456));
std::shared_ptr<Logger> logger = std::make_shared<Logger>();
std::shared_ptr<PlaywrightInstaller> playwrightInstaller = std::make_shared<PlaywrightInstaller>(logger);

void Main(void)
{
    config();
    startServer()->_catch([=](auto error) mutable
    {
        logger->error(std:("Failed to start server:"), error);
        process->exit(1);
    }
    );
}

MAIN
