#include "config/manager.js.hpp"
#include "core/gateway.js.hpp"
#include "transports/http-wrapper.js.hpp"
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

#!/usr/bin/env node

;
;
;
;

using TransportMode = std::variant<'stdio', 'sse'>;

/**
 * Main entry point for the MCP Gateway Server
 */
std::future<void> main();

      logger.info(`Starting MCP Gateway in SSE mode on port ${port}`);
      const wrapper = new HTTPGatewayWrapper(configFile, port, logger);
      wrapper.start();
      return;
    }

    // STDIO mode - run gateway directly
    logger.info('Starting MCP Gateway in STDIO mode');

    // Load configuration
    let config;
    if (configFile) {
      logger.info(`Loading configuration from file: ${configFile}`);
      config = configManager.loadFromFile(configFile);
    } else {
      logger.info('Loading configuration from environment variables');
      config = configManager.loadFromEnv();
    }

    // Update logger with config log level
    const configLogger = createLogger(config.settings?.logLevel || 'info');
    
    // Create and start the gateway server
    const gateway = new GatewayServer(config, configLogger);
    await gateway.start();

    // Create transport and connect
    const transport = new StdioServerTransport();
    await gateway.connect(transport);

    configLogger.info('MCP Gateway is now serving on stdio');
    // Compatibility message for older tests looking for this exact phrase
    configLogger.info('Listening on stdio');

    // Handle graceful shutdown
    const shutdown = async (signal: string) => {
      configLogger.info(`Received ${signal}, shutting down gracefully...`);
      try {
        await gateway.stop();
        process.exit(0);
      } catch (error) {
        configLogger.error(`Error during shutdown: ${error}`);
        process.exit(1);
      }
    };

    process.on('SIGINT', () => shutdown('SIGINT'));
    process.on('SIGTERM', () => shutdown('SIGTERM'));
    process.on('uncaughtException', (error) => {
      configLogger.error(`Uncaught exception: ${error}`);
      shutdown('uncaughtException');
    });
    process.on('unhandledRejection', (reason) => {
      configLogger.error(`Unhandled rejection: ${reason}`);
      shutdown('unhandledRejection');
    });

  } catch (error) {
    logger.error(`Failed to start MCP Gateway: ${error}`);
    process.exit(1);
  }
}

/**
 * Create a logger with the specified log level
 */
,
    warn: (...args: unknown[]) => {
      if (levelIndex >= 1) console.warn('[WARN]', ...args);
    },
    info: (...args: unknown[]) => {
      if (levelIndex >= 2) console.info('[INFO]', ...args);
    },
    log: (...args: unknown[]) => {
      if (levelIndex >= 2) console.log('[INFO]', ...args);
    },
    debug: (...args: unknown[]) => {
      if (levelIndex >= 3) console.debug('[DEBUG]', ...args);
    },
    trace: (...args: unknown[]) => {
      if (levelIndex >= 3) console.trace('[TRACE]', ...args);
    },
    // Add other console methods to satisfy the Console interface
    assert: console.assert,
    clear: console.clear,
    count: console.count,
    countReset: console.countReset,
    dir: console.dir,
    dirxml: console.dirxml,
    group: console.group,
    groupCollapsed: console.groupCollapsed,
    groupEnd: console.groupEnd,
    table: console.table,
    time: console.time,
    timeEnd: console.timeEnd,
    timeLog: console.timeLog,
    timeStamp: console.timeStamp,
    profile: console.profile,
    profileEnd: console.profileEnd
  } as Console;
}

/**
 * Display help information
 */


// Handle help flag
if (process.argv.includes('--help') || process.argv.includes('-h')) {
  showHelp();
  process.exit(0);
}

// Start the server
main().catch((error) => {
  console.error('Fatal error:', error);
  process.exit(1);
});

} // namespace elizaos
