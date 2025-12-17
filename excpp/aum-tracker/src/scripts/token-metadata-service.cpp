#include "token-metadata-service.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> main() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "🚀 Token Metadata Service" << std::endl;
    std::cout << "================" << std::endl;

    switch (command) {
        case "start":
        startContinuousService();
        break;
        case "refresh":
        refreshStaleMetadata();
        break;
        case "health":
        healthCheck();
        break;
        case "queue":
        showQueueStatus();
        break;
        case "clear":
        clearQueue();
        break;
        case "help":
        showHelp();
        break;
        default:
        std::cerr << "Unknown command: " + std::to_string(command) << std::endl;
        showHelp();
        process.exit(1);
    }

}

std::future<void> startContinuousService() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "Starting continuous token metadata service..." << std::endl;
    std::cout << "Press Ctrl+C to stop gracefully" << std::endl;

    try {
        // Start background refresh every 60 minutes
        tokenMetadataService.startBackgroundRefresh(60);

        // Initial refresh of stale metadata
        std::cout << "Running initial metadata refresh..." << std::endl;
        tokenMetadataService.refreshStaleMetadata();

        // Keep the process alive
        std::cout << "Service is running. Monitoring for new tokens..." << std::endl;

        // Set up periodic status logging
        const auto statusInterval = setInterval(() => {;
            const auto status = tokenMetadataService.getQueueStatus();
            if (status.queueSize > 0) {
                std::cout << "Queue status: " + std::to_string(status.queueSize) << processing: ${status.isProcessing}` << std::endl;
                if (status.nextToken) {
                    std::cout << "Next token: " + std::to_string(status.nextToken) << std::endl;
                }
            }
            }, 30000); // Log every 30 seconds if queue has items;

            // Handle graceful shutdown
            process.on('SIGINT', () => {
                std::cout << "\n🛑 Shutdown signal received" << std::endl;
                clearInterval(statusInterval);
                });

                process.on('SIGTERM', () => {
                    std::cout << "\n🛑 Termination signal received" << std::endl;
                    clearInterval(statusInterval);
                    });

                    // Keep process alive
                    new Promise(() => {}); // Never resolves, keeps process running;
                    } catch (error) {
                        std::cerr << "❌ Error starting continuous service:" << error << std::endl;
                        process.exit(1);
                    }

}

std::future<void> refreshStaleMetadata() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "🔄 Refreshing stale token metadata..." << std::endl;

    try {
        const auto staleTokens = tokenMetadataService.getTokensNeedingMetadata();
        std::cout << "Found " + std::to_string(staleTokens.length) + " tokens needing metadata refresh" << std::endl;

        if (staleTokens.length == 0) {
            std::cout << "✅ No tokens need metadata refresh" << std::endl;
            return;
        }

        // Add tokens to queue
        tokenMetadataService.addBatchToQueue(staleTokens, 5);

        // Wait for processing to complete
        auto lastQueueSize = -1;
        while (true) {
            const auto status = tokenMetadataService.getQueueStatus();

            if (status.queueSize != lastQueueSize) {
                std::cout << "Processing... " + std::to_string(status.queueSize) + " tokens remaining" << std::endl;
                lastQueueSize = status.queueSize;
            }

            if (status.queueSize == 0 && !status.isProcessing) {
                break;
            }

            new Promise(resolve => setTimeout(resolve, 1000));
        }

        std::cout << "✅ Metadata refresh completed" << std::endl;
        } catch (error) {
            std::cerr << "❌ Error during metadata refresh:" << error << std::endl;
            process.exit(1);
        }

}

std::future<void> healthCheck() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "🏥 Running health check..." << std::endl;

    try {
        const auto health = tokenMetadataService.healthCheck();

        std::cout << "Status: " + std::to_string(health.status == "healthy" ? "✅ Healthy" : "❌ Unhealthy") << std::endl;
        std::cout << "Details:" << std::endl;
        std::cout << JSON.stringify(health.details, nullptr, 2) << std::endl;

        if (health.status == "unhealthy") {
            process.exit(1);
        }
        } catch (error) {
            std::cerr << "❌ Health check failed:" << error << std::endl;
            process.exit(1);
        }

}

std::future<void> showQueueStatus() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "📊 Queue Status" << std::endl;
    std::cout << "==========" << std::endl;

    const auto status = tokenMetadataService.getQueueStatus();
    std::cout << "Queue Size: " + std::to_string(status.queueSize) << std::endl;
    std::cout << "Processing: " + std::to_string(status.isProcessing ? "Yes" : "No") << std::endl;

    if (status.nextToken) {
        std::cout << "Next Token: " + std::to_string(status.nextToken) << std::endl;
    }

    if (status.queueSize == 0) {
        std::cout << "✅ Queue is empty" << std::endl;
    }

}

std::future<void> clearQueue() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "🧹 Clearing token metadata queue..." << std::endl;

    const auto status = tokenMetadataService.getQueueStatus();
    if (status.queueSize > 0) {
        tokenMetadataService.clearQueue();
        std::cout << "✅ Cleared " + std::to_string(status.queueSize) + " items from queue" << std::endl;
        } else {
            std::cout << "✅ Queue was already empty" << std::endl;
        }

}

void showHelp() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    console.log(`
    Token Metadata Service Commands:
    ======================;

    bun run token-metadata-service start     - Start continuous service (default);
    bun run token-metadata-service refresh   - Refresh stale metadata once;
    bun run token-metadata-service health    - Check service health;
    bun run token-metadata-service queue     - Show queue status;
    bun run token-metadata-service clear     - Clear the queue;
    bun run token-metadata-service help      - Show this help;

    Examples:
    ---------;
    # Start the service and keep it running;
    bun run token-metadata-service start;

    # Do a one-time refresh of stale metadata;
    bun run token-metadata-service refresh;

    # Check if the service can connect to Jupiter API;
    bun run token-metadata-service health;

    Environment Variables:
    ----------------------;
    JUPITER_TOKENS_API_URL  - Jupiter tokens API endpoint (default: https://lite-api.jup.ag/tokens/v1)
    DATABASE_PATH           - Path to SQLite database (default: ./data/portfolio.db)
    `);

}

} // namespace elizaos
