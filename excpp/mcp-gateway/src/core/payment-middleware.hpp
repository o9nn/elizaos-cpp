#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct PaymentVerificationResult {
    bool authorized;
    std::optional<std::string> error;
    std::string amount;
    std::string method;
};

struct PaymentRequirements {
    double x402Version;
    std::string scheme;
    std::string network;
    std::string maxAmountRequired;
    std::string resource;
    std::string description;
    std::optional<std::string> mimeType;
    std::string payTo;
    double maxTimeoutSeconds;
    std::string asset;
    std::string name;
    std::string version;
    std::optional<std::string> error;
};

/**
 * Middleware for handling payment verification for MCP tools
 * Supports both x402 blockchain payments and ELIZA_API_KEY authentication
 */
class PaymentMiddleware {
public:
    PaymentMiddleware(GatewayConfig config, Console logger = console);
    std::future<PaymentVerificationResult> verifyPayment(const std::string& toolName, const std::string& serverId, std::optional<std::unordered_map<std::string, std::string>> headers);
    PaymentVerificationResult verifyApiKey(std::optional<std::unordered_map<std::string, std::string>> headers, std::optional<ToolPricing> pricing);
    std::future<PaymentVerificationResult> verifyX402Payment(std::optional<std::unordered_map<std::string, std::string>> headers, std::optional<ToolPricing> pricing, std::optional<std::string> toolName, std::optional<std::string> requestUrl);
    ToolPricing getToolPricing(const std::string& toolName, const std::string& serverId);
    PaymentRequirements generatePaymentRequiredResponse(const std::string& toolName, const std::string& serverId, std::optional<std::string> requestUrl);
    std::string parseAmountToAtomicUnits(const std::string& amount);
    std::string getUsdcAddress(const std::string& network);
    std::string getServerPaymentMode(const std::string& serverId);
    std::string calculateMarkup(const std::string& downstreamPrice, const std::string& markup);
    std::unordered_map<std::string, std::string> extractPaymentHeaders(std::optional<std::unordered_map<std::string, std::string>> headers);
    void getStats();

private:
    GatewayConfig config_;
    Console logger_;
};

} // namespace elizaos
