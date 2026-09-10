#pragma once

#include "elizaos/core.hpp"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <thread>
#include <mutex>
#include <future>
#include <atomic>
#include <cstddef>

namespace elizaos {

/**
 * Plugin development and deployment automation module
 * Provides CI/CD pipeline integration and automated testing capabilities
 */

// Plugin status enumeration
enum class PluginStatus {
    UNKNOWN,
    LOADING,
    LOADED,
    ACTIVE,
    INACTIVE,
    FAILED,
    UNLOADING
};

// Plugin metadata structure
struct PluginMetadata {
    std::string name;
    std::string version;
    std::string apiVersion = "1.0";
    std::string description;
    std::string author;
    std::vector<std::string> dependencies;
    std::unordered_map<std::string, std::string> config;
    
    PluginMetadata() = default;
    PluginMetadata(const std::string& n, const std::string& v, const std::string& desc)
        : name(n), version(v), description(desc) {}
};

// Plugin interface
class Plugin {
public:
    virtual ~Plugin() = default;
    virtual bool initialize(const PluginMetadata& metadata) = 0;
    virtual bool activate() = 0;
    virtual bool deactivate() = 0;
    virtual bool shutdown() = 0;
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual PluginStatus getStatus() const = 0;
    virtual std::vector<std::string> getDependencies() const = 0;

    // Initialized plugins are healthy before and after activation by default.
    virtual bool healthCheck() const {
        const auto status = getStatus();
        return status == PluginStatus::LOADED || status == PluginStatus::ACTIVE ||
               status == PluginStatus::INACTIVE;
    }
};

// Plugin registry for managing loaded plugins
class PluginRegistry {
public:
    PluginRegistry();
    ~PluginRegistry();
    
    // Plugin management
    bool registerPlugin(std::shared_ptr<Plugin> plugin, const PluginMetadata& metadata);
    bool unregisterPlugin(const std::string& name);
    std::shared_ptr<Plugin> getPlugin(const std::string& name);
    std::vector<std::string> getPluginNames() const;

    // Validated lifecycle transitions; plugin callbacks run outside registry locks.
    bool initializePlugin(const std::string& name);
    bool activatePlugin(const std::string& name);
    bool deactivatePlugin(const std::string& name);
    bool checkPluginHealth(const std::string& name) const;
    
    // Status queries
    PluginStatus getPluginStatus(const std::string& name) const;
    std::vector<std::string> getActivePlugins() const;
    std::vector<std::string> getFailedPlugins() const;
    
    // Dependency management
    bool resolveDependencies(const std::string& pluginName);
    std::vector<std::string> getDependencyChain(const std::string& pluginName) const;
    
private:
    std::unordered_map<std::string, std::shared_ptr<Plugin>> plugins_;
    std::unordered_map<std::string, PluginMetadata> metadata_;
    std::unordered_map<std::string, PluginStatus> states_;
    std::unordered_map<std::string, PluginStatus> stableStates_;
    std::unordered_map<std::string, bool> transitions_;
    mutable std::mutex registryMutex_;
    
    bool checkDependencies(const PluginMetadata& metadata) const;
    bool hasDependents(const std::string& name) const;
};

// CI/CD pipeline integration
class CIPipeline {
public:
    CIPipeline();
    ~CIPipeline();
    
    // Pipeline operations
    std::future<bool> buildPlugin(const std::string& pluginPath);
    std::future<bool> testPlugin(const std::string& pluginName);
    std::future<bool> deployPlugin(const std::string& pluginName, const std::string& target);
    
    // Build configuration
    void setBuildCommand(const std::string& command);
    void setTestCommand(const std::string& command);
    void setDeployCommand(const std::string& command);
    
    // Status monitoring
    struct PipelineStatus {
        std::string pluginName;
        std::string stage;          // "build", "test", "deploy"
        bool inProgress = false;
        bool success = false;
        std::string output;
        std::string error;
    };
    
    PipelineStatus getStatus(const std::string& pluginName) const;
    std::vector<PipelineStatus> getAllStatuses() const;
    
private:
    std::string buildCommand_;
    std::string testCommand_;
    std::string deployCommand_;
    
    std::unordered_map<std::string, PipelineStatus> statuses_;
    mutable std::mutex statusMutex_;
    
    bool executeCommand(const std::string& command, std::string& output, std::string& error);
    bool runStage(const std::string& pluginName, const std::string& stage,
                  const std::string& command);
};

// Automated testing framework for plugins
class PluginTester {
public:
    class StopToken {
    public:
        StopToken() = default;
        bool stopRequested() const noexcept;

    private:
        explicit StopToken(std::shared_ptr<std::atomic<bool>> stopRequested);
        std::shared_ptr<std::atomic<bool>> stopRequested_;
        friend class PluginTester;
    };

    using CooperativeTestFunction = std::function<bool(const StopToken&)>;

    PluginTester();
    ~PluginTester();
    
    // Test execution
    struct TestResult {
        std::string testName;
        bool passed;
        std::string message;
        double executionTime;
        bool timedOut = false;
        bool admissionRejected = false;
        
        TestResult(const std::string& name = "", bool success = false, 
                  const std::string& msg = "", double time = 0.0)
            : testName(name), passed(success), message(msg), executionTime(time) {}
    };
    
    // Test suite management
    void addTestCase(const std::string& testName, std::function<bool()> testFunc);
    void addTestCase(const std::string& testName, CooperativeTestFunction testFunc);
    void removeTestCase(const std::string& testName);
    std::vector<TestResult> runTests(const std::string& pluginName);
    std::vector<TestResult> runAllTests();
    
    // Test configuration
    void setTimeout(int seconds);
    void setVerbose(bool verbose);
    void setMaxPendingTests(std::size_t maxPendingTests);
    
private:
    struct ExecutorState;
    std::unordered_map<std::string, CooperativeTestFunction> testCases_;
    int timeoutSeconds_;
    bool verbose_;
    std::size_t maxPendingTests_;
    mutable std::mutex testMutex_;
    std::shared_ptr<ExecutorState> executor_;
    
    TestResult executeTest(const std::string& testName, CooperativeTestFunction testFunc);
};

// Development workflow automation
class WorkflowAutomation {
public:
    WorkflowAutomation();
    ~WorkflowAutomation();
    
    // Workflow actions
    bool createPluginTemplate(const std::string& pluginName, const std::string& outputPath);
    bool generatePluginDocs(const std::string& pluginPath);
    bool validatePluginStructure(const std::string& pluginPath);
    bool packagePlugin(const std::string& pluginPath, const std::string& outputPath);
    
    // Code generation
    bool generateInterface(const std::string& interfaceName, const std::string& outputPath);
    bool generateTestSkeleton(const std::string& pluginName, const std::string& outputPath);
    
    // Template management
    void setTemplateDirectory(const std::string& path);
    std::vector<std::string> getAvailableTemplates() const;
    
private:
    std::string templateDirectory_;
    
    bool copyTemplate(const std::string& templateName, const std::string& destination);
    bool replaceTokens(const std::string& filePath, const std::unordered_map<std::string, std::string>& tokens);
};

// Main automation manager
class PluginsAutomation {
public:
    PluginsAutomation();
    ~PluginsAutomation();
    
    // Component access
    PluginRegistry& getRegistry() { return registry_; }
    CIPipeline& getPipeline() { return pipeline_; }
    PluginTester& getTester() { return tester_; }
    WorkflowAutomation& getWorkflow() { return workflow_; }
    
    // Integrated operations
    bool automatedPluginSetup(const std::string& pluginName, const std::string& templateName);
    bool automatedBuildAndTest(const std::string& pluginPath);
    bool automatedDeployment(const std::string& pluginName, const std::string& target);
    
    // Configuration
    void loadConfiguration(const std::string& configPath);
    void saveConfiguration(const std::string& configPath);
    
private:
    PluginRegistry registry_;
    CIPipeline pipeline_;
    PluginTester tester_;
    WorkflowAutomation workflow_;
    
    std::unordered_map<std::string, std::string> config_;
    mutable std::mutex configMutex_;
};

// Global automation instance
extern std::shared_ptr<PluginsAutomation> globalPluginAutomation;

} // namespace elizaos