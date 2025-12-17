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
#include ".utils/plugin-templates.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Extend the ServiceTypeRegistry to add our custom service type
  struct ServiceTypeRegistry {
    "plugin_creation" PLUGIN_CREATION;
};

struct PluginSpecification {
    std::string name;
    std::string description;
    std::optional<std::string> version;
    std::string name;
    std::string description;
    std::string name;
    std::string description;
    std::string name;
    std::string description;
    std::optional<std::vector<std::string>> methods;
    std::string name;
    std::string description;
    std::optional<std::vector<std::string>> triggers;
    std::string name;
    std::string description;
    bool required;
    bool sensitive;
};

struct PluginCreationJob {
    std::string id;
    PluginSpecification specification;
    std::variant<"pending", "running", "completed", "failed", "cancelled"> status;
    std::string currentPhase;
    double progress;
    std::vector<std::string> logs;
    std::optional<std::string> error;
    std::optional<std::string> result;
    std::string outputPath;
    Date startedAt;
    std::optional<Date> completedAt;
    double currentIteration;
    double maxIterations;
    double passed;
    double failed;
    double duration;
    std::optional<double> validationScore;
    std::optional<std::any> childProcess;
    double iteration;
    std::string phase;
    std::string error;
    Date timestamp;
};

class PluginCreationService {
public:
    PluginCreationService(std::optional<IAgentRuntime> runtime);
    std::future<void> stop();
    std::future<PluginCreationService> start(IAgentRuntime runtime);
    std::future<void> initialize(IAgentRuntime runtime);
    std::future<std::string> createPlugin(PluginSpecification specification, std::optional<std::string> apiKey);
    std::vector<PluginCreationJob> getAllJobs();
    std::optional<PluginCreationJob> getJobStatus(const std::string& jobId);
    void cancelJob(const std::string& jobId);
    void logToJob(const std::string& jobId, const std::string& message);
    std::string getDataDir();
    std::future<void> runCreationProcess(PluginCreationJob job);
    std::future<bool> runSingleIteration(PluginCreationJob job);
    std::future<void> setupPluginWorkspace(PluginCreationJob job);
    std::future<void> generatePluginCode(PluginCreationJob job);
    std::string generateInitialPrompt(PluginSpecification spec);
    std::string generateIterationPrompt(PluginCreationJob job, const std::vector<std::any>& errors);
    std::future<void> writeGeneratedCode(PluginCreationJob job, const std::string& responseText);
    std::future<bool> buildPlugin(PluginCreationJob job);
    std::future<bool> lintPlugin(PluginCreationJob job);
    std::future<bool> testPlugin(PluginCreationJob job);
    std::future<bool> validatePlugin(PluginCreationJob job);
    Promise< runCommand(PluginCreationJob job, const std::string& command, const std::vector<std::string>& args, const std::string& description);
    void if(auto outputSize < maxOutputSize);
    std::any parseTestResults(const std::string& output);
    Promise<Array< collectCodeFiles(const std::string& dirPath);
    void for(auto const entry of entries);
    std::future<void> prepareNextIteration(PluginCreationJob job);
    std::future<void> notifyPluginReady(PluginCreationJob job);
    std::future<void> ensureWorkspaceDirs();
    PluginCreationJob getJob(const std::string& jobId);
    std::vector<PluginCreationJob> listJobs();
    bool isValidPluginName(const std::string& name);
    std::string sanitizePluginName(const std::string& name);
    bool checkRateLimit();
    void cleanupOldJobs();
};


} // namespace elizaos
