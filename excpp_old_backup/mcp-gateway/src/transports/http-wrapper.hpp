#include ".config/manager.js.hpp"
#include ".core/payment-middleware.js.hpp"
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
 * HTTP Wrapper for MCP Gateway with x402 Support
 * 
 * This creates an HTTP/SSE server that wraps the gateway's stdio interface
 * and adds proper x402 payment support by returning HTTP 402 status codes.
 * 
 * Usage:
 *   bun run src/transports/http-wrapper.ts --config=examples/coingecko-config.yaml --port=8000
 */

struct Session {
    std::string id;
    ServerResponse response;
    ChildProcess process;
    std::string buffer;
};

class HTTPGatewayWrapper {
  private sessions = new Map<string, Session>();
  private paymentMiddleware?: PaymentMiddleware;
  private config: any;
  private logger: Console;
  private port: number;
  private gatewayCommand: string[];

  constructor(configPath: string, port: number, logger: Console = console) {
    this.logger = logger;
    this.port = port;

    // Load gateway config for payment middleware
    this.config = configManager.loadFromFile(configPath);
    
    // Initialize payment middleware (HTTP layer handles payments)
    if (this.config.payment?.enabled) {
      this.paymentMiddleware = new PaymentMiddleware(this.config, logger);
      logger.info('Payment middleware enabled for HTTP wrapper');
    }

    // Gateway subprocess command - use wrapper config (no payment checking in subprocess)
    // The HTTP layer handles all payment verification
    // IMPORTANT: Subprocess must run in STDIO mode, not SSE mode
    
    // Just re-run the same script we're currently running, but in STDIO mode
    // process.argv[1] is the path to the currently executing script (works for both src and build)
    // Use 'bun' directly (not 'bun run') to execute the script without package.json interference
    const currentScript = process.argv[1]!;
    const wrapperConfigPath = configPath.replace('.yaml', '-wrapper.yaml').replace('.json', '-wrapper.json');
    this.gatewayCommand = ['bun', currentScript, '--mode=stdio', `--config=${wrapperConfigPath}`];
    
    logger.info(`Gateway subprocess will use config: ${wrapperConfigPath}`);
    logger.info(`Gateway subprocess command: ${this.gatewayCommand.join(' ')}`);
  }

    // CORS headers

    // Start gateway process for this session

    // Set SSE headers

    // Send endpoint URL (send as plain text, not JSON-stringified)

    // Handle process output

    // Handle client disconnect

    // Keep-alive

        // Check for payment requirement on tool calls
          
            // Return HTTP 402 with x402 headers

        // Forward to gateway

        // In SSE mode, responses come via the SSE stream, not via HTTP responses
        // Just acknowledge receipt of the message

    // Extract server ID from tool name (format: namespace:toolname or just toolname)

      // Generate x402 requirements with full URL for resource field

        // Send all responses via SSE
        // Not JSON, ignore (could be log output)

      // Connection closed

// CLI
std::future<void> main();


} // namespace elizaos
