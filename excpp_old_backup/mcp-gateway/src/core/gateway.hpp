#include ".types/index.hpp"
#include "payment-middleware.hpp"
#include "registry.hpp"
#include "server-manager.hpp"
#include "x402-client.hpp"
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



class GatewayServer {
  private server: Server;
  private serverManager: ServerManager;
  private registry: GatewayRegistry;
  private paymentMiddleware?: PaymentMiddleware;
  private x402Client?: X402PaymentClient;
  private config: GatewayConfig;
  private logger: Console;
  private healthCheckInterval?: NodeJS.Timeout;

  constructor(config: GatewayConfig, logger: Console = console) {
    this.config = config;
    this.logger = logger;
    this.serverManager = new ServerManager(logger);
    this.registry = new GatewayRegistry(config, logger);

    // Initialize payment middleware if enabled
    if (config.payment?.enabled) {
      this.paymentMiddleware = new PaymentMiddleware(config, logger);
      this.logger.info('Payment middleware enabled');
      const stats = this.paymentMiddleware.getStats();
      this.logger.info(`Payment config: network=${stats.network}, recipient=${stats.recipient}, apiKeys=${stats.apiKeys}`);
    }

    // Initialize x402 payment client for outbound payments
    if (config.payment?.outboundWallet) {
      try {
        this.x402Client = new X402PaymentClient(config, logger);
        this.logger.info(`X402 payment client enabled for outbound payments: ${this.x402Client.getWalletAddress()}`);
      } catch (error) {
        this.logger.error(`Failed to initialize x402 payment client: ${error}`);
      }
    }

    this.server = new Server(
      {
        name: config.name,
        version: config.version,
        description: config.description
      },
      {
        capabilities: {
          tools: {},
          resources: {},
          prompts: {}
        }
      }
    );

    this.setupHandlers();
  }

      // Payment verification
          // Extract headers from request metadata (MCP clients can pass custom metadata)

            // Generate 402 Payment Required response

          // Log successful payment


} // namespace elizaos
