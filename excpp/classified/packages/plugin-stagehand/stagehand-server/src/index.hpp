#include "logger.js.hpp"
#include "message-handler.js.hpp"
#include "playwright-installer.js.hpp"
#include "session-manager.js.hpp"
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

;
;
;
;
;
;
;

// Load environment variables
config();

const PORT = process.env.STAGEHAND_SERVER_PORT || 3456;
const logger = new Logger();
const playwrightInstaller = new PlaywrightInstaller(logger);

async `);

  // Ensure Playwright is installed before starting the server
  try {
    await playwrightInstaller.ensurePlaywrightInstalled();
  } catch (error) {
    logger.error('Failed to ensure Playwright installation:', error);
    logger.warn(
      'Server will start but Stagehand operations may fail until Playwright is installed'
    );
  }

  // Create WebSocket server
  const wss = new WebSocketServer({ port: Number(PORT) });
  const sessionManager = new SessionManager(logger, playwrightInstaller);
  const messageHandler = new MessageHandler(sessionManager, logger);

  logger.info(`Stagehand server initialization complete`);

  // Handle new connections
  wss.on('connection', (ws) => {
    const clientId = `client-${Date.now()}-${Math.random().toString(36).substring(7)}`;
    logger.info(`New client connected: ${clientId}`);

    // Send welcome message
    ws.send(
      JSON.stringify({
        type: 'connected',
        clientId,
        version: '1.0.0',
      })
    );

    // Handle messages from client
    ws.on('message', async (data) => {
      try {
        const message = JSON.parse(data.toString());
        logger.debug(`Received message from ${clientId}:`, message);

        const response = await messageHandler.handleMessage(message, clientId);

        ws.send(JSON.stringify(response));
      } catch (error) {
        logger.error(`Error handling message from ${clientId}:`, error);
        ws.send(
          JSON.stringify({
            type: 'error',
            error: error instanceof Error ? error.message : 'Unknown error',
            requestId: null,
          })
        );
      }
    });

    // Handle client disconnect
    ws.on('close', () => {
      logger.info(`Client disconnected: ${clientId}`);
      // Clean up sessions for this client
      sessionManager.cleanupClientSessions(clientId);
    });

    // Handle errors
    ws.on('error', (error) => {
      logger.error(`WebSocket error for ${clientId}:`, error);
    });
  });

  // Handle server shutdown
  process.on('SIGINT', async () => {
    logger.info('Shutting down server...');
    await sessionManager.cleanup();
    process.exit(0);
  });

  process.on('SIGTERM', async () => {
    logger.info('Shutting down server...');
    await sessionManager.cleanup();
    process.exit(0);
  });

  logger.info(`Stagehand server listening on port ${PORT}`);
}

// Start the server
startServer().catch((error) => {
  logger.error('Failed to start server:', error);
  process.exit(1);
});

} // namespace elizaos
