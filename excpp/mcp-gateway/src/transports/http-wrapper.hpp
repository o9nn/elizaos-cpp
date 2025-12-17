#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".config/manager.js.hpp"
#include ".core/payment-middleware.js.hpp"

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
public:
    HTTPGatewayWrapper(const std::string& configPath, double port, Console logger = console);
    void start();
    void handleRequest(IncomingMessage req, ServerResponse res);
    void handleSSE(IncomingMessage req, ServerResponse res);
    std::future<void> handleMessage(IncomingMessage req, ServerResponse res);
    Promise< checkPayment(const std::any& message, const std::unordered_map<std::string, std::string>& headers);
    void processGatewayOutput(Session session);
    void sendSSE(ServerResponse res, const std::string& event, const std::any& data);
    std::unordered_map<std::string, std::string> extractHeaders(IncomingMessage req);
    std::string generateSessionId();

private:
    std::any config_;
    Console logger_;
    double port_;
    std::vector<std::string> gatewayCommand_;
};

// CLI
std::future<void> main();


} // namespace elizaos
