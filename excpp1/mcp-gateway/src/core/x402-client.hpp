#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Client for making outbound x402 payments to downstream APIs
 * Uses x402-axios to automatically handle 402 Payment Required responses
 */
class X402PaymentClient {
public:
    X402PaymentClient(GatewayConfig config, Console = console logger);
    AxiosInstance createServerClient(const std::string& serverId, std::optional<std::string> serverWallet);
    this::axiosInstance;

    try createServerClient(auto config.serverId, auto config.serverWallet);
    void catch(const std::any& error);
    void catch(const std::any& error);
    std::string getWalletAddress();
    bool isEnabled();

private:
    AxiosInstance axiosInstance_;
    GatewayConfig config_;
    Console logger_;
};


} // namespace elizaos
