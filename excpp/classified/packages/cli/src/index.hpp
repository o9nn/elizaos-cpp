#include "elizaos/core.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

#!/usr/bin/env bun
process.env.NODE_OPTIONS = '--no-deprecation';
process.env.NODE_NO_WARNINGS = '1';
process.env.QUIET_MODE = process.env.QUIET_MODE || 'true';

;
;
;
;
;
;
;
;
;
;
;
;
;
;
;
;
;

/**
 * Shutdown state management to prevent race conditions
 * Using an object to encapsulate state and provide atomic operations
 */
const shutdownState = {
  isShuttingDown: false,

  /**
   * Atomically check and set the shutdown flag
   * @returns true if shutdown was initiated, false if already in progress
   */
  tryInitiateShutdown(): boolean {
    if (this.isShuttingDown) {
      return false;
    }
    this.isShuttingDown = true;
    return true;
  },
};

/**
 * Graceful shutdown handler for SIGINT and SIGTERM signals
 * Ensures proper cleanup of server processes before exiting
 * Prevents race conditions from multiple rapid signal events
 */
async  - shutdown already in progress`);
    return;
  }
  logger.info(`Received ${signal}, shutting down gracefully...`);

  try {
    // Stop the dev server if it's running
    const serverWasStopped = await stopServer();
    if (serverWasStopped) {
      logger.info('Server stopped successfully');
    }
  } catch (error) {
    // Extract error message for better debugging
    const errorMessage = error instanceof Error ? error.message : String(error);
    logger.error(`Error stopping server: ${errorMessage}`);
    logger.debug('Full error details:', error);
  }

  // Use appropriate exit codes for different signals
  const exitCode = signal === 'SIGINT' ? 130 : signal === 'SIGTERM' ? 143 : 0;
  process.exit(exitCode);
}

process.on('SIGINT', () => gracefulShutdown('SIGINT'));
process.on('SIGTERM', () => gracefulShutdown('SIGTERM'));

/**
 * Asynchronous 
 */
async 

  // Check for --no-emoji flag early (before command parsing)
  if (process.argv.includes('--no-emoji')) {
    configureEmojis({ forceDisable: true });
  }

  // Check for --no-auto-install flag early (before command parsing)
  if (process.argv.includes('--no-auto-install')) {
    process.env.ELIZA_NO_AUTO_INSTALL = 'true';
  }

  // Get version - will return 'monorepo' if in monorepo context
  const version = getVersion();

  // Check for built-in flags that exit early (before preAction hook runs)
  const args = process.argv.slice(2);
  const isUpdateCommand = args.includes('update');
  const willShowBanner = args.length === 0;

  // Show update notification for all commands except:
  // - when banner will show (it handles its own notification)
  // - when running update command
  if (!willShowBanner && !isUpdateCommand) {
    const currentVersion = getVersion();
    await checkAndShowUpdateNotification(currentVersion);
  }

  const program = new Command()
    .name('elizaos')
    .version(version, '-v, --version', 'output the version number')
    .option('--no-emoji', 'Disable emoji output')
    .option('--no-auto-install', 'Disable automatic Bun installation');

  // Add global options but hide them from global help
  // They will still be passed to all commands for backward compatibility
  // Note: Removed --remote-url global option as it conflicts with subcommand options

  program
    .addCommand(create)
    .addCommand(monorepo)
    .addCommand(plugins)
    .addCommand(agent)
    .addCommand(tee)
    .addCommand(start)
    .addCommand(update)
    .addCommand(test)
    .addCommand(env)
    .addCommand(dev)
    .addCommand(publish);

  // if no args are passed, display the banner (it will handle its own update check)
  if (process.argv.length === 2) {
    await displayBanner(false); // Let banner handle update check and show enhanced notification
  }

  await program.parseAsync();
}

main().catch((error) => {
  logger.error('An error occurred:', error);
  process.exit(1);
});

} // namespace elizaos
