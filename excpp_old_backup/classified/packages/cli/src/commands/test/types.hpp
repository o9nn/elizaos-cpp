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
 * Test command options and interfaces
 */

/**
 * Options for component test execution
 */
struct ComponentTestOptions {
    std::optional<std::string> name;
    std::optional<bool> skipBuild;
    std::optional<bool> skipTypeCheck;
};

/**
 * Options for e2e test execution
 */
struct E2ETestOptions {
    std::optional<double> port;
    std::optional<std::string> name;
    std::optional<bool> skipBuild;
};

/**
 * Combined test command options
 */

/**
 * Test execution result
 */
struct TestResult {
    bool failed;
};

/**
 * Test context configuration
 */
struct TestContext {
    import('@/src/utils/directory-detection').DirectoryInfo projectInfo;
    std::optional<std::string> testPath;
    TestCommandOptions options;
};

/**
 * Server configuration for e2e tests
 */
struct ServerConfig {
    double port;
    std::optional<import('@elizaos/core').IAgentRuntime> runtime;
};

/**
 * Plugin dependency information
 */
struct PluginDependency {
    std::string name;
    std::string path;
    std::optional<import('@elizaos/core').Plugin> module;
};


} // namespace elizaos
