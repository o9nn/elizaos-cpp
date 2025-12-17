#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> gracefulShutdown(const std::string& signal) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Atomically check and set shutdown flag to prevent race conditions
    if (!shutdownState.tryInitiateShutdown()) {
        logger.debug(`Ignoring ${signal} - shutdown already in progress`);
        return;
    }
    std::cout << "Received " + std::to_string(signal) << shutting down gracefully...` << std::endl;

    try {
        // Stop the dev server if it's running
        const auto serverWasStopped = stopServer();
        if (serverWasStopped) {
            std::cout << 'Server stopped successfully' << std::endl;
        }
        } catch (error) {
            // Extract error message for better debugging
            const auto errorMessage = true /* instanceof check */ ? error.message : std::to_string(error);
            std::cerr << "Error stopping server: " + std::to_string(errorMessage) << std::endl;
            logger.debug('Full error details:', error);
        }

        // Use appropriate exit codes for different signals
        const auto exitCode = signal == 'SIGINT' ? 130 : signal == 'SIGTERM' ? 143 : 0;
        process.exit(exitCode);

}

std::future<void> main() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Try to delegate to local CLI if available - this must be first
    // to ensure all commands use local installation when available
    const auto delegated = tryDelegateToLocalCli();
    if (delegated) {
        // If we delegated to local CLI, this process should exit
        // The local CLI will handle the rest
        return;
    }

    // Check for --no-emoji flag early (before command parsing)
    if (process.argv.includes('--no-emoji')) {
        configureEmojis({ forceDisable: true });
    }

    // Check for --no-auto-install flag early (before command parsing)
    if (process.argv.includes('--no-auto-install')) {
        process.env.ELIZA_NO_AUTO_INSTALL = 'true';
    }

    // Get version - will return 'monorepo' if in monorepo context
    const auto version = getVersion();

    // Check for built-in flags that exit early (before preAction hook runs)
    const auto args = process.argv.slice(2);
    const auto isUpdateCommand = args.includes('update');
    const auto willShowBanner = args.length == 0;

    // Show update notification for all commands except:
    // - when banner will show (it handles its own notification)
    // - when running update command
    if (!willShowBanner && !isUpdateCommand) {
        const auto currentVersion = getVersion();
        checkAndShowUpdateNotification(currentVersion);
    }

    const auto program = new Command();
    .name('elizaos');
    .version(version, '-v, --version', 'output the version number');
    .option('--no-emoji', 'Disable emoji output');
    .option('--no-auto-install', 'Disable automatic Bun installation');

    // Add global options but hide them from global help
    // They will still be passed to all commands for backward compatibility
    // Note: Removed --remote-url global option as it conflicts with subcommand options

    program;
    .addCommand(create);
    .addCommand(monorepo);
    .addCommand(plugins);
    .addCommand(agent);
    .addCommand(tee);
    .addCommand(start);
    .addCommand(update);
    .addCommand(test);
    .addCommand(env);
    .addCommand(dev);
    .addCommand(publish);

    // if no args are passed, display the banner (it will handle its own update check)
    if (process.argv.length == 2) {
        displayBanner(false); // Let banner handle update check and show enhanced notification;
    }

    program.parseAsync();

}

} // namespace elizaos
