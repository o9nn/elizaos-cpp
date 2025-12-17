#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".core/payment-middleware.js.hpp"

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
    double lastActivity;
};

class SSEServerTransport {
public:
    SSEServerTransport(SSEServerOptions options, Console = console logger);
    std::future<void> start();
    std::future<void> close();
    void setMCPServer(Server server);
    void handleRequest(IncomingMessage req, ServerResponse res);
    void handleSSE(IncomingMessage req, ServerResponse res);
    std::future<void> handleMessage(IncomingMessage req, ServerResponse res);
    std::future<bool> checkPaymentForToolCall(JSONRPCRequest message, Record<string headers, auto string>, ServerResponse res);
    std::future<JSONRPCResponse> processMessage(JSONRPCRequest message, Record<string headers, auto string>);
    void sendSSE(ServerResponse res, const std::string& event, unknown data);
    std::unordered_map<std::string, std::string> extractHeaders(IncomingMessage req);
    std::string generateSessionId();
    void cleanupStaleSessions();

private:
    HTTPServer httpServer_;
    Required<SSEServerOptions> options_;
    Console logger_;
};


} // namespace elizaos
