#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".emoji-handler.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Initialize tiktoken encoder

// Configuration

struct MigrationResult {
    bool success;
    std::string branchName;
    std::string repoPath;
    std::optional<Error> error;
};

struct ProductionValidationResult {
    bool production_ready;
    std::optional<std::string> revision_instructions;
};

struct MigratorOptions {
    std::optional<bool> skipTests;
    std::optional<bool> skipValidation;
};

class PluginMigrator {
public:
    PluginMigrator(MigratorOptions options = {});
    void registerCleanupHandlers();
    std::future<void> initializeAnthropic();
    std::future<MigrationResult> migrate(const std::string& input);
    std::future<bool> runMigrationWithTestLoop();
    std::future<bool> runProductionValidationLoop();
    std::future<std::string> getTestErrors();
    std::future<void> trackChangedFiles(const std::string& initialCommit);
    std::future<ProductionValidationResult> validateProductionReadiness();
    std::future<std::string> getChangedFilesContent();
    std::future<void> runClaudeCodeWithContext(const std::string& context);
    std::future<void> runClaudeCodeWithPrompt(const std::string& prompt);
    std::future<void> handleInput(const std::string& input);
    std::future<std::string> analyzeRepository();
    std::future<std::string> generateMigrationStrategy(const std::string& context);
    std::future<void> createMigrationInstructions(const std::string& specificStrategy);
    std::future<void> createBranch();
    std::future<void> runClaudeCode();
    std::future<void> checkDiskSpace();
    std::future<double> getAvailableDiskSpace();
    std::future<void> createLockFile();
    std::future<void> removeLockFile();


} // namespace elizaos
