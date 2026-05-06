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


/**
 * Payment E2E Test Suite for Eliza MCP Gateway
 *
 * This test suite validates all payment gating features:
 * - Payment configuration loading
 * - x402 blockchain payment verification
 * - ELIZA API key authentication
 * - Tiered pricing strategies
 * - Free tool access
 * - 402 Payment Required responses
 * - Backward compatibility (payment disabled)
 */

struct TestResult {
    std::string name;
    bool passed;
    std::optional<std::string> error;
    double duration;
    std::optional<std::string> output;
};

class PaymentE2ETestRunner {
public:
    std::future<void> runAllTests();
    std::future<void> testPaymentConfigLoading();
    std::future<void> testFreeToolAccess();
    std::future<void> testApiKeyAuthentication();
    std::future<void> testTieredPricing();
    std::future<void> testDefaultPricing();
    std::future<void> testPaymentDisabled();
    std::future<void> testInvalidScenarios();
    $ log();
    std::future<std::string> runGatewayWithTimeout(const std::string& configPath, double timeoutMs = 8000);
    void printResults();
};

// Run tests if this file is executed directly

} // namespace elizaos
