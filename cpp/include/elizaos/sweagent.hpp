#pragma once

/**
 * ElizaOS C++ - SWEAgent Module
 *
 * Software Engineering Agent: solves GitHub issues, generates code,
 * creates tests, and manages multi-agent parallel solving.
 */

#include "elizaos.hpp"
#include "agentlogger.hpp"
#include "agentmemory.hpp"
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace elizaos {

// ============================================================================
// Data structures
// ============================================================================

struct GitHubIssue {
    std::string              repo;
    int                      issueNumber = 0;
    std::string              title;
    std::string              description;
    std::vector<std::string> labels;
    std::string              status      = "open";
};

struct CodeContext {
    std::string              language    = "cpp";
    std::string              repo;
    std::vector<std::string> files;
    std::string              buildSystem = "cmake";
    std::string              testFramework = "gtest";
};

struct SolutionResult {
    bool                     success = false;
    std::string              description;
    std::vector<std::string> filesChanged;
    std::string              patchContent;
    std::string              testResults;
    std::vector<std::string> warnings;
    std::string              errorMessage;
};

struct SWEBenchResult {
    std::string taskId;
    bool        solved   = false;
    double      score    = 0.0;
    std::string patch;
    double      durationMs = 0.0;
};

// ============================================================================
// SWEAgent
// ============================================================================

class SWEAgent {
public:
    SWEAgent(const std::string& agentId, const std::string& model = "default");

    // Primary API
    SolutionResult  solveIssue(const GitHubIssue& issue);
    SolutionResult  solveFromDescription(const std::string& description, const CodeContext& context);

    // Code generation
    std::string              generateCode(const std::string& specification, const CodeContext& context);
    std::vector<std::string> generateTests(const std::string& code, const CodeContext& context);
    bool                     validateSolution(const std::string& code, const std::vector<std::string>& tests);

    // Repository operations
    bool cloneRepository(const std::string& repoUrl, const std::string& targetPath);
    bool applyChanges(const std::vector<std::string>& files);
    bool runTests(const std::string& testCommand);
    bool createPullRequest(const std::string& title, const std::string& description);

    // Interactive shell
    void startInteractiveShell();
    void executeCommand(const std::string& command);

    // Configuration
    void        setModel(const std::string& model);
    void        setMaxIterations(int maxIter);
    void        setParallelExecutionMode(bool enabled);
    std::string getStatus() const;

    // History
    std::vector<SolutionResult> getHistory() const;

private:
    std::string    analyzeIssue(const GitHubIssue& issue);
    std::string    planSolution(const std::string& analysis);
    bool           implementSolution(const std::string& plan, const CodeContext& context);
    bool           verifySolution(const std::string& code);

    std::string  agentId_;
    std::string  model_;
    int          maxIterations_    = 10;
    bool         parallelMode_     = false;

    std::vector<SolutionResult>         history_;
    std::shared_ptr<AgentMemoryManager> memory_;
    std::shared_ptr<AgentLogger>        logger_;
    mutable std::mutex                  agentMutex_;
};

// ============================================================================
// SWEAgentManager
// ============================================================================

class SWEAgentManager {
public:
    SWEAgentManager();

    void                   addAgent(std::shared_ptr<SWEAgent> agent);
    void                   removeAgent(const std::string& agentId);
    std::shared_ptr<SWEAgent> getAgent(const std::string& agentId);

    std::vector<SolutionResult> solveIssuesParallel(const std::vector<GitHubIssue>& issues);

    void                     setMaxParallelAgents(int maxAgents);
    std::vector<std::string> getActiveAgents() const;

private:
    std::unordered_map<std::string, std::shared_ptr<SWEAgent>> agents_;
    int          maxParallelAgents_ = 4;
    mutable std::mutex managerMutex_;
};

} // namespace elizaos
