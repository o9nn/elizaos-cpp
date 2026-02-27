#include "token-metadata-service.h"

void main()
{
    console->log(std:("🚀 Token Metadata Service"));
    console->log(std:("=="));
    static switch_type __switch311_807 = {
        { any(std:("start")), 1 },
        { any(std:("refresh")), 2 },
        { any(std:("health")), 3 },
        { any(std:("queue")), 4 },
        { any(std:("clear")), 5 },
        { any(std:("help")), 6 }
    };
    switch (__switch311_807[command])
    {
    case 1:
        std::async([=]() { startContinuousService(); });
        break;
    case 2:
        std::async([=]() { refreshStaleMetadata(); });
        break;
    case 3:
        std::async([=]() { healthCheck(); });
        break;
    case 4:
        std::async([=]() { showQueueStatus(); });
        break;
    case 5:
        std::async([=]() { clearQueue(); });
        break;
    case 6:
        showHelp();
        break;
    default:
        console->error(std:("Unknown command: ") + command + string_empty);
        showHelp();
        process->exit(1);
    }
};


void startContinuousService()
{
    console->log(std:("Starting continuous token metadata service..."));
    console->log(std:("Press Ctrl+C to stop gracefully"));
    try
    {
        tokenMetadataService->startBackgroundRefresh(60);
        console->log(std:("Running initial metadata refresh..."));
        tokenMetadataService->refreshStaleMetadata();
        console->log(std:("Service is running. Monitoring for new tokens..."));
        shared statusInterval = setInterval([=]() mutable
        {
            auto status = tokenMetadataService->getQueueStatus();
            if (status["queueSize"] > 0) {
                console->log(std:("Queue status: ") + status["queueSize"] + std:(" tokens pending, processing: ") + status["isProcessing"] + string_empty);
                if (status["nextToken"]) {
                    console->log(std:("Next token: ") + status["nextToken"] + string_empty);
                }
            }
        }
        , 30000);
        process->on(std:("SIGINT"), [=]() mutable
        {
            console->log(std:("\
🛑 Shutdown signal received"));
            clearInterval(statusInterval);
        }
        );
        process->on(std:("SIGTERM"), [=]() mutable
        {
            console->log(std:("\
🛑 Termination signal received"));
            clearInterval(statusInterval);
        }
        );
        std::async([=]() { std::make_shared<Promise>([=]() mutable
        {
        }
        ); });
    }
    catch (const any& error)
    {
        console->error(std:("❌ Error starting continuous service:"), error);
        process->exit(1);
    }
};


void refreshStaleMetadata()
{
    console->log(std:("🔄 Refreshing stale token metadata..."));
    try
    {
        auto staleTokens = tokenMetadataService->getTokensNeedingMetadata();
        console->log(std:("Found ") + staleTokens->get_length() + std:(" tokens needing metadata refresh"));
        if (staleTokens->get_length() == 0) {
            console->log(std:("✅ No tokens need metadata refresh"));
            return std::shared_ptr<Promise<void>>();
        }
        tokenMetadataService->addBatchToQueue(staleTokens, 5);
        auto lastQueueSize = -1;
        while (true)
        {
            auto status = tokenMetadataService->getQueueStatus();
            if (status["queueSize"] != lastQueueSize) {
                console->log(std:("Processing... ") + status["queueSize"] + std:(" tokens remaining"));
                lastQueueSize = status["queueSize"];
            }
            if (AND((status["queueSize"] == 0), (!status["isProcessing"]))) {
                break;
            }
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 1000);
            }
            ); });
        }
        console->log(std:("✅ Metadata refresh completed"));
    }
    catch (const any& error)
    {
        console->error(std:("❌ Error during metadata refresh:"), error);
        process->exit(1);
    }
};


void healthCheck()
{
    console->log(std:("🏥 Running health check..."));
    try
    {
        auto health = std::async([=]() { tokenMetadataService->healthCheck(); });
        console->log(std:("Status: ") + (health["status"] == std:("healthy")) ? std:("✅ Healthy") : std:("❌ Unhealthy") + string_empty);
        console->log(std:("Details:"));
        console->log(JSON->stringify(health["details"], nullptr, 2));
        if (health["status"] == std:("unhealthy")) {
            process->exit(1);
        }
    }
    catch (const any& error)
    {
        console->error(std:("❌ Health check failed:"), error);
        process->exit(1);
    }
};


void showQueueStatus()
{
    console->log(std:("📊 Queue Status"));
    console->log(std:("=="));
    auto status = tokenMetadataService->getQueueStatus();
    console->log(std:("Queue Size: ") + status["queueSize"] + string_empty);
    console->log(std:("Processing: ") + (status["isProcessing"]) ? std:("Yes") : std:("No") + string_empty);
    if (status["nextToken"]) {
        console->log(std:("Next Token: ") + status["nextToken"] + string_empty);
    }
    if (status["queueSize"] == 0) {
        console->log(std:("✅ Queue is empty"));
    }
};


void clearQueue()
{
    console->log(std:("🧹 Clearing token metadata queue..."));
    auto status = tokenMetadataService->getQueueStatus();
    if (status["queueSize"] > 0) {
        tokenMetadataService->clearQueue();
        console->log(std:("✅ Cleared ") + status["queueSize"] + std:(" items from queue"));
    } else {
        console->log(std:("✅ Queue was already empty"));
    }
};


void showHelp()
{
    console->log(std:("\
Token Metadata Service Commands:\
===\
\
bun run token-metadata-service start     - Start continuous service (default)\
bun run token-metadata-service refresh   - Refresh stale metadata once\
bun run token-metadata-service health    - Check service health\
bun run token-metadata-service queue     - Show queue status\
bun run token-metadata-service clear     - Clear the queue\
bun run token-metadata-service help      - Show this help\
\
Examples:\
---------\
# Start the service and keep it running\
bun run token-metadata-service start\
\
# Do a one-time refresh of stale metadata\
bun run token-metadata-service refresh\
\
# Check if the service can connect to Jupiter API\
bun run token-metadata-service health\
\
Environment Variables:\
----------------------\
JUPITER_TOKENS_API_URL  - Jupiter tokens API endpoint (default: https://lite-api.jup.ag/tokens/v1)\
DATABASE_PATH           - Path to SQLite database (default: ./data/portfolio.db)\
"));
};


array<string> args = process->argv->slice(2);
string command = OR((const_(args)[0]), (std:("start")));

void Main(void)
{
    process->on(std:("uncaughtException"), [=](auto error) mutable
    {
        console->error(std:("❌ Uncaught Exception:"), error);
        process->exit(1);
    }
    );
    process->on(std:("unhandledRejection"), [=](auto reason, auto promise) mutable
    {
        console->error(std:("❌ Unhandled Rejection at:"), promise, std:("reason:"), reason);
        process->exit(1);
    }
    );
    main()->_catch([=](auto error) mutable
    {
        console->error(std:("❌ Application error:"), error);
        process->exit(1);
    }
    );
}

MAIN
