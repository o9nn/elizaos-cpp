#pragma once

#include "elizaos/core.hpp"
#include "elizaos/agentloop.hpp"
#include "elizaos/agentmemory.hpp"
#include "elizaos/agentaction.hpp"
#include "elizaos/agentlogger.hpp"
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace elizaos {

/**
 * @brief SWE-Agent: Autonomous Software Engineer
 * 
 * A fully-featured autonomous software engineer that can solve GitHub issues,
 * write code, run tests, and submit complete solutions.
 * 
 * Key Features:
 * - Autonomous problem solving from GitHub issues
 * - Automatic code generation and testing
 * - Multi-model AI support (OpenAI, Anthropic, etc.)
 * - Parallel execution capabilities
 * - Interactive shell mode for debugging
 */

// Issue representation
struct GitHubIssue {
    std::string repo;
    int issueNumber;
    std::string title;
    std::string description;
    std::vector<std::string> labels;
    std::string status;
};

// Solution result
struct SolutionResult {
    bool success;
    std::string description;
    std::vector<std::string> filesChanged;
    std::vector<std::string> testsRun;
    std::string pullRequestUrl;
    std::string errorMessage;
};

// Code generation context
struct CodeContext {
    std::string language;
    std::string framework;
    std::vector<std::string> dependencies;
    std::unordered_map<std::string, std::string> environment;
};

/**
 * @brief SWE-Agent core class
 * 
 * Autonomous software engineer capable of solving coding problems,
 * generating solutions, and submitting pull requests.
 */
class SWEAgent {
public:
    SWEAgent(const std::string& agentId, const std::string& model);
    ~SWEAgent() = default;
    
    // Core functionality
    SolutionResult solveIssue(const GitHubIssue& issue);
    SolutionResult solveFromDescription(const std::string& description, const CodeContext& context);
    
    // Code generation
    std::string generateCode(const std::string& specification, const CodeContext& context);
    std::vector<std::string> generateTests(const std::string& code, const CodeContext& context);
    bool validateSolution(const std::string& code, const std::vector<std::string>& tests);
    
    // Repository operations
    bool cloneRepository(const std::string& repoUrl, const std::string& targetPath);
    bool applyChanges(const std::vector<std::string>& files);
    bool runTests(const std::string& testCommand);
    bool createPullRequest(const std::string& title, const std::string& description);
    
    // Interactive mode
    void startInteractiveShell();
    void executeCommand(const std::string& command);
    
    // Configuration
    void setModel(const std::string& model);
    void setMaxIterations(int maxIter);
    void setParallelExecutionMode(bool enabled);
    
    // Status and monitoring
    std::string getStatus() const;
    std::vector<SolutionResult> getHistory() const;
    
private:
    std::string agentId_;
    std::string model_;
    int maxIterations_;
    bool parallelMode_;
    std::vector<SolutionResult> solutionHistory_;
    std::shared_ptr<AgentLogger> logger_;
    std::shared_ptr<Memory> memory_;
    
    // Internal methods
    std::string analyzeIssue(const GitHubIssue& issue);
    std::string planSolution(const std::string& analysis);
    bool implementSolution(const std::string& plan, const CodeContext& context);
    bool verifySolution(const std::string& code);
};

/**
 * @brief SWE-Agent Manager for parallel execution
 * 
 * Manages multiple SWE-Agent instances for parallel problem solving.
 */
class SWEAgentManager {
public:
    SWEAgentManager();
    ~SWEAgentManager() = default;
    
    // Agent management
    void addAgent(std::shared_ptr<SWEAgent> agent);
    void removeAgent(const std::string& agentId);
    std::shared_ptr<SWEAgent> getAgent(const std::string& agentId);
    
    // Parallel execution
    std::vector<SolutionResult> solveIssuesParallel(const std::vector<GitHubIssue>& issues);
    void setMaxParallelAgents(int maxAgents);
    
    // Status monitoring
    std::vector<std::string> getActiveAgents() const;
    std::unordered_map<std::string, std::string> getAgentStatuses() const;
    
private:
    std::unordered_map<std::string, std::shared_ptr<SWEAgent>> agents_;
    int maxParallelAgents_;
    mutable std::mutex mutex_;
};

/**
 * @brief SWE-Bench evaluation framework
 * 
 * Evaluates SWE-Agent performance on benchmark datasets.
 */
class SWEBench {
public:
    SWEBench(const std::string& benchmarkPath);
    ~SWEBench() = default;
    
    // Benchmark execution
    void runBenchmark(SWEAgent& agent);
    void runBenchmarkSubset(SWEAgent& agent, int numIssues);
    
    // Results and metrics
    float getSuccessRate() const;
    std::vector<std::string> getFailedTests() const;
    std::string generateReport() const;
    
private:
    std::string benchmarkPath_;
    std::vector<GitHubIssue> testIssues_;
    std::vector<SolutionResult> results_;
};

} // namespace elizaos
