#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "AuthenticationService.js.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * CLI Authentication Commands for ElizaOS Services
 * Provides elizaos auth commands for key management
 */

struct CLICommand {
    std::string name;
    std::string description;
    std::optional<std::vector<CLIOption>> options;
};

struct CLIOption {
    std::string name;
    std::string description;
    std::optional<bool> required;
    std::variant<'string', 'boolean'> type;
};

/**
 * CLI Authentication Commands
 */
class CLIAuthCommands {
public:
    CLIAuthCommands(IAgentRuntime runtime);
    std::vector<CLICommand> getCommands();
    std::future<void> handleAuthStatus();
    std::future<void> handleAuthTest();
    std::future<void> handleAuthValidate(const std::any& args);
    std::future<void> handleTestKeys();
    std::future<void> handleClearCache();
    std::future<void> handleSetup();
    std::string getStatusIcon(const std::string& status);

private:
    AuthenticationService authService_;
};

/**
 * Export CLI command registration function
 */
std::vector<CLICommand> registerAuthCommands(IAgentRuntime runtime);

} // namespace elizaos
