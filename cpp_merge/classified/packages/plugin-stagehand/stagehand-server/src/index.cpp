#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-stagehand/stagehand-server/src/index.h"

void startServer()
{
    logger->info(std::string("Stagehand server starting on port ") + PORT + string_empty);
    try
    {
        std::async([=]() { playwrightInstaller->ensurePlaywrightInstalled(); });
    }
    catch (const any& error)
    {
        logger->error(std::string("Failed to ensure Playwright installation:"), error);
        logger->warn(std::string("Server will start but Stagehand operations may fail until Playwright is installed"));
    }
    auto wss = std::make_shared<WebSocketServer>(object{
        object::pair{std::string("port"), Number(PORT)}
    });
    shared sessionManager = std::make_shared<SessionManager>(logger, playwrightInstaller);
    shared messageHandler = std::make_shared<MessageHandler>(sessionManager, logger);
    logger->info(std::string("Stagehand server initialization complete"));
    wss->on(std::string("connection"), [=](auto ws) mutable
    {
        shared clientId = std::string("client-") + Date->now() + std::string("-") + Math->random()->toString(36)->substring(7) + string_empty;
        logger->info(std::string("New client connected: ") + clientId + string_empty);
        ws["send"](JSON->stringify(object{
            object::pair{std::string("type"), std::string("connected")}, 
            object::pair{std::string("clientId"), std::string("clientId")}, 
            object::pair{std::string("version"), std::string("1.0.0")}
        }));
        ws["on"](std::string("message"), [=](auto data) mutable
        {
            try
            {
                auto message = JSON->parse(data["toString"]());
                logger->debug(std::string("Received message from ") + clientId + std::string(":"), message);
                auto response = std::async([=]() { messageHandler->handleMessage(message, clientId); });
                ws["send"](JSON->stringify(response));
            }
            catch (const any& error)
            {
                logger->error(std::string("Error handling message from ") + clientId + std::string(":"), error);
                ws["send"](JSON->stringify(object{
                    object::pair{std::string("type"), std::string("error")}, 
                    object::pair{std::string("error"), (is<Error>(error)) ? any(error->message) : any(std::string("Unknown error"))}, 
                    object::pair{std::string("requestId"), nullptr}
                }));
            }
        }
        );
        ws["on"](std::string("close"), [=]() mutable
        {
            logger->info(std::string("Client disconnected: ") + clientId + string_empty);
            sessionManager->cleanupClientSessions(clientId);
        }
        );
        ws["on"](std::string("error"), [=](auto error) mutable
        {
            logger->error(std::string("WebSocket error for ") + clientId + std::string(":"), error);
        }
        );
    }
    );
    process->on(std::string("SIGINT"), [=]() mutable
    {
        logger->info(std::string("Shutting down server..."));
        std::async([=]() { sessionManager->cleanup(); });
        process->exit(0);
    }
    );
    process->on(std::string("SIGTERM"), [=]() mutable
    {
        logger->info(std::string("Shutting down server..."));
        std::async([=]() { sessionManager->cleanup(); });
        process->exit(0);
    }
    );
    logger->info(std::string("Stagehand server listening on port ") + PORT + string_empty);
};


any PORT = OR((process->env->STAGEHAND_SERVER_PORT), (3456));
std::shared_ptr<Logger> logger = std::make_shared<Logger>();
std::shared_ptr<PlaywrightInstaller> playwrightInstaller = std::make_shared<PlaywrightInstaller>(logger);

void Main(void)
{
    config();
    startServer()->_catch([=](auto error) mutable
    {
        logger->error(std::string("Failed to start server:"), error);
        process->exit(1);
    }
    );
}

MAIN
