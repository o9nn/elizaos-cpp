#include "elizaos/core.hpp"
#include "elizaos/server.hpp"
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

/**
 * TypeScript Example: Standalone Server Usage
 *
 * This example demonstrates how to use @elizaos/server as a standalone package
 * to create a custom agent server without the CLI dependency.
 */

;
;
;

// Custom middleware example
const customLoggingMiddleware: ServerMiddleware = (
  req: Request,
  res: Response,
  next: NextFunction
) => {
  const start = Date.now();

  res.on('finish', () => {
    const duration = Date.now() - start;
    logger.info(`${req.method} ${req.path} - ${res.statusCode} (${duration}ms)`);
  });

  next();
};

// Server configuration
const serverOptions: ServerOptions = {
  dataDir: './data/eliza-server',
  middlewares: [customLoggingMiddleware],
  // postgresUrl: process.env.DATABASE_URL, // Optional PostgreSQL
};

async );

    logger.success('✅ Server initialized successfully');

    return server;
  } catch (error) {
    logger.error('❌ Failed to create server:', error);
    throw error;
  }
}

async :${port}...`);
    server.start(port);

    // Log available endpoints
    logger.info('📡 Available endpoints:');
    logger.info(`   Dashboard: http://${host}:${port}/`);
    logger.info(`   API: http://${host}:${port}/api/`);
    logger.info(`   Health: http://${host}:${port}/api/health`);
    logger.info(`   WebSocket: ws://${host}:${port}/`);

    // Graceful shutdown
    const gracefulShutdown = async () => {
      logger.info('🛑 Graceful shutdown initiated...');
      await server.stop();
      logger.success('✅ Server stopped successfully');
      process.exit(0);
    };

    process.on('SIGTERM', gracefulShutdown);
    process.on('SIGINT', gracefulShutdown);
  } catch (error) {
    logger.error('❌ Server startup failed:', error);
    process.exit(1);
  }
}

// Export for programmatic usage
{ createStandaloneServer, startServer };

// Direct execution
if (import.meta.url === `file://${process.argv[1]}`) {
  startServer();
}

} // namespace elizaos
