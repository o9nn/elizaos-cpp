#pragma once
#include <algorithm>
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
#include "elizaos/logger.hpp"
#include "elizaos/plugin-sql.hpp"
#include "elizaos/types.hpp"
#include "run-bun.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Configuration

struct CreationResult {
    bool success;
    std::string pluginName;
    std::string pluginPath;
    std::optional<std::runtime_error> error;
};

struct PluginSpecification {
    std::string name;
    std::string description;
    std::vector<std::string> features;
    std::optional<std::vector<std::string>> actions;
    std::optional<std::vector<std::string>> providers;
    std::optional<std::vector<std::string>> evaluators;
    std::optional<std::vector<std::string>> services;
};

struct ValidationResult {
    bool production_ready;
    std::optional<std::string> revision_instructions;
};

struct CreatorOptions {
    std::optional<bool> skipTests;
    std::optional<bool> skipValidation;
    std::optional<bool> skipPrompts;
    std::optional<PluginSpecification> spec;
};

class PluginCreator {
public:
    PluginCreator(CreatorOptions options = {});
    void registerCleanupHandlers();
    std::future<void> initializeAnthropic();
    std::future<CreationResult> create(std::optional<PluginSpecification> pluginSpec);
    std::future<PluginSpecification> collectPluginSpecification();
    std::future<void> createFromTemplate(const std::string& pluginName);
    std::future<void> createPluginStructureManually(const std::string& pluginName);
    std::future<std::string> generateDetailedSpecification(PluginSpecification spec);
    std::future<void> createSpecificationDocument(PluginSpecification spec, const std::string& detailedSpec);
    std::future<bool> runGenerationWithValidation();
    std::future<bool> runBuildLoop();
    std::future<bool> runTestLoop();
    std::future<bool> runProductionValidationLoop();
    std::future<void> runClaudeCode();
    std::future<void> runClaudeCodeWithContext(const std::string& context);
    std::future<void> runClaudeCodeWithPrompt(const std::string& prompt);
    std::future<std::string> getBuildErrors();
    std::future<std::string> getTestErrors();
    std::future<ValidationResult> validateProductionReadiness();
    std::future<std::string> getAllPluginFiles();
    std::future<std::string> copyToCWD();
    std::future<void> checkDiskSpace();
    std::future<double> getAvailableDiskSpace();


} // namespace elizaos
