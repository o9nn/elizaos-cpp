#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"
#include "migration-guide-loader.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct SimpleMigrationResult {
    bool success;
    std::string repoPath;
    double duration;
    double messageCount;
    std::optional<std::runtime_error> error;
    std::optional<std::vector<std::string>> guidesUsed;
};

/**
 * SimpleMigrationAgent uses a class extending EventTarget rather than functional
 * patterns because EventTarget is a native browser/Bun API that requires class inheritance.
 * This is an intentional architectural decision to leverage Bun's native capabilities
 * instead of Node.js EventEmitter for better compatibility.
 *
 * NOTE: Unlike standard EventEmitter, this implementation prevents duplicate handler
 * registration. This is an intentional design choice to prevent memory leaks and
 * unintended multiple executions of the same handler.
 */
class SimpleMigrationAgent {
public:
    SimpleMigrationAgent(const std::string& repoPath, std::any options = {});
    bool emit(const std::string& event, std::optional<std::any> data);
    void removeAllListeners(std::optional<std::string> event);
    double listenerCount(const std::string& event);
    bool isImportantUpdate(const std::string& text);
    std::string formatProgressUpdate(const std::string& text);
    void startSpinner(const std::string& message);
    void stopSpinner(std::optional<std::string> completionMessage);
    void updateTokenTracking(std::optional<std::any> usage);
    std::string formatTokenDisplay();
    std::string getSimplifiedToolName(const std::string& toolName);
    std::future<SimpleMigrationResult> migrate();
    void abort();
    std::string getMigrationHelp(const std::string& issue);
    std::string searchGuides(const std::string& query, double limit = 3);
    std::string getFullMigrationContext();


} // namespace elizaos
