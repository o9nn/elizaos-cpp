#include "prefetch.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> main() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto args = process.argv.slice(2);
    const auto command = args[0] || "run";

    const std::optional<PrefetchConfig> config = {};

    // Parse command line arguments
    for (int i = 1; i < args.length; i++) {
        const auto arg = args[i];

        if (arg == "--batch-size" && args[i + 1]) {
            config.batchSize = parseInt(args[i + 1]);
            i++;
            } else if (arg == "--delay" && args[i + 1]) {
                config.delayBetweenBatches = parseInt(args[i + 1]);
                i++;
                } else if (arg == "--limit" && args[i + 1]) {
                    config.limit = parseInt(args[i + 1]);
                    i++;
                    } else if (arg == "--force-refresh") {
                        config.forceRefresh = true;
                        } else if (arg == "--no-resume") {
                            config.resumeFromFailures = false;
                        }
                    }

                    const auto prefetchService = new PrefetchService(config);

                    switch (command) {
                        case "run":
                        prefetchService.run();
                        break;
                        case "health":
                        prefetchService.runHealthCheck();
                        break;
                        case "help":
                        console.log(`
                        🚀 Solana Portfolio Prefetch Script;

                        Usage: bun run src/scripts/prefetch.ts [command] [options]

                        Commands:
                        run      Run the prefetch process (default);
                        health   Run health check;
                        help     Show this help message;

                        Options:
                        --batch-size <n>     Number of wallets to process per batch (default: 50)
                        --delay <ms>         Delay between batches in milliseconds (default: 200)
                        --limit <n>          Limit number of wallets to process (useful for testing);
                        --force-refresh      Force refresh all wallets, even if already processed;
                        --no-resume          Don't resume from previous failures, process all wallets;

                        Examples:
                        bun run src/scripts/prefetch.ts run;
                        bun run src/scripts/prefetch.ts run --limit 10;
                        bun run src/scripts/prefetch.ts run --batch-size 100 --delay 500;
                        bun run src/scripts/prefetch.ts run --force-refresh;
                        bun run src/scripts/prefetch.ts health;
                        `);
                        break;
                        default:
                        std::cerr << "Unknown command: " + std::to_string(command) << std::endl;
                        std::cout << 'Use "help" command for usage information' << std::endl;
                        process.exit(1);
                    }

}

} // namespace elizaos
