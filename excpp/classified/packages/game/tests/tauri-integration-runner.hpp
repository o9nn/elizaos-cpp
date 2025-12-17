#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


/**
 * Tauri Integration Test Runner
 *
 * This script validates that Tauri IPC actually works with the Eliza container
 * by running real API calls and verifying responses.
 */

struct TestResult {
    std::string name;
    bool passed;
    std::optional<std::string> error;
    std::optional<double> duration;
};

class TauriIntegrationRunner {
public:
    std::future<void> runAll();
    std::future<void> buildTauriApp();
    std::future<void> startElizaServer();
    std::future<void> startTauriApp();
    std::future<void> runIntegrationTests();
    $ log();
    std::future<void> generateReport();
    std::future<void> cleanup();
};

// Run the tests if this script is executed directly


} // namespace elizaos
