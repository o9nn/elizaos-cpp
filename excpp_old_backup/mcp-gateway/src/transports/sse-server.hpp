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
 * SSE Server Transport for MCP Gateway
 * 
 * Provides HTTP/SSE transport with full x402 payment support.
 * This allows the gateway to:
 * - Receive HTTP requests with payment headers
 * - Return HTTP 402 status codes for payment requirements
 * - Validate x402 payments before processing requests
 */

struct SSEServerOptions {
    double port;
    std::optional<std::string> host;
    std::optional<std::string> ssePath;
    std::optional<std::string> messagePath;
    std::optional<PaymentMiddleware> paymentMiddleware;
};

struct Session {
    std::string id;
    ServerResponse response;
    std::unordered_map<std::string, std::string> headers;
    double lastActivity;
};

class SSEServerTransport {
  private httpServer: HTTPServer;
  private sessions = new Map<string, Session>();
  private options: Required<SSEServerOptions>;
  private mcpServer?: Server;
  private messageHandlers = new Map<string, (response: JSONRPCResponse) => void>();
  private logger: Console;

  constructor(options: SSEServerOptions, logger: Console = console) {
    this.logger = logger;
    this.options = {
      host: options.host || 'localhost',
      ssePath: options.ssePath || '/sse',
      messagePath: options.messagePath || '/message',
      ...options
    };

    this.httpServer = new HTTPServer(this.handleRequest.bind(this));

    // Clean up stale sessions every 60 seconds
    setInterval(() => this.cleanupStaleSessions(), 60000);
  }

    // Close all active sessions

    // Enable CORS

    // Set SSE headers

    // Store session

    // Send session ID to client

    // Handle client disconnect

    // Keep-alive ping every 30 seconds

    // Update headers with latest from this request

        // Check if this is a tool call that requires payment

        // Process the message through MCP server
          
          // Send response to client

          // Also send via SSE for streaming

    // Inject headers into request params for payment middleware

    // For now, allow the request to proceed - payment will be checked by the gateway
    // In the future, we could check here and return 402 immediately

    // Inject headers into message for payment middleware access

    // Handle the message through the MCP server's request handlers
    // We need to route based on method

      // Call the appropriate handler
      // This is a simplified version - in production, you'd want to properly route all MCP methods


} // namespace elizaos
