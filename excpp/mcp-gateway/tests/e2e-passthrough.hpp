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
 * E2E tests for x402 passthrough payment functionality
 * Tests: passthrough, markup, and absorb payment modes
 */

class PassthroughE2ETestRunner {
public:
    std::future<void> runAllTests();
    std::future<void> testPassthroughConfig();
    std::future<void> testMarkupCalculation();
    std::future<void> testAbsorbMode();
    std::future<void> testHybridMode();
    std::future<std::string> runGatewayWithTimeout(const std::string& configPath, double timeoutMs);
    void printResults();
};

// Run tests

} // namespace elizaos
