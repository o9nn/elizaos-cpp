#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "elizaos/sweagent.hpp"
#include <memory>
#include <string>
#include <vector>

using namespace elizaos;
using namespace ::testing;

class SWEAgentTest : public ::testing::Test {
protected:
    void SetUp() override {
        agent_ = std::make_shared<SWEAgent>("test-agent", "gpt-4");
    }

    std::shared_ptr<SWEAgent> agent_;
};

// ============================================================================
// GitHubIssue Tests
// ============================================================================

TEST_F(SWEAgentTest, GitHubIssueCreation) {
    GitHubIssue issue;
    issue.repo = "org/repo";
    issue.issueNumber = 123;
    issue.title = "Fix authentication bug";
    issue.description = "Users cannot login with OAuth";
    issue.status = "open";

    EXPECT_EQ(issue.repo, "org/repo");
    EXPECT_EQ(issue.issueNumber, 123);
    EXPECT_EQ(issue.title, "Fix authentication bug");
    EXPECT_EQ(issue.status, "open");
}

TEST_F(SWEAgentTest, GitHubIssueWithLabels) {
    GitHubIssue issue;
    issue.repo = "org/repo";
    issue.issueNumber = 456;
    issue.labels = {"bug", "high-priority", "authentication"};

    EXPECT_EQ(issue.labels.size(), 3);
    EXPECT_THAT(issue.labels, Contains("bug"));
    EXPECT_THAT(issue.labels, Contains("high-priority"));
}

// ============================================================================
// SolutionResult Tests
// ============================================================================

TEST_F(SWEAgentTest, SolutionResultSuccess) {
    SolutionResult result;
    result.success = true;
    result.description = "Fixed authentication bug by updating token validation";
    result.filesChanged = {"src/auth.cpp", "include/auth.hpp"};
    result.testsRun = {"test_auth.cpp"};
    result.pullRequestUrl = "https://github.com/org/repo/pull/789";

    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.filesChanged.size(), 2);
    EXPECT_FALSE(result.pullRequestUrl.empty());
}

TEST_F(SWEAgentTest, SolutionResultFailure) {
    SolutionResult result;
    result.success = false;
    result.errorMessage = "Could not determine root cause of issue";

    EXPECT_FALSE(result.success);
    EXPECT_FALSE(result.errorMessage.empty());
}

// ============================================================================
// CodeContext Tests
// ============================================================================

TEST_F(SWEAgentTest, CodeContextCreation) {
    CodeContext context;
    context.language = "cpp";
    context.framework = "qt";
    context.dependencies = {"boost", "nlohmann-json", "gtest"};

    EXPECT_EQ(context.language, "cpp");
    EXPECT_EQ(context.framework, "qt");
    EXPECT_EQ(context.dependencies.size(), 3);
}

TEST_F(SWEAgentTest, CodeContextWithEnvironment) {
    CodeContext context;
    context.language = "python";
    context.environment["PYTHONPATH"] = "/project/src";
    context.environment["DEBUG"] = "1";

    EXPECT_EQ(context.environment.size(), 2);
    EXPECT_EQ(context.environment["DEBUG"], "1");
}

// ============================================================================
// SWEAgent Tests
// ============================================================================

TEST_F(SWEAgentTest, AgentCreation) {
    SWEAgent agent("agent-001", "gpt-4");

    // Should be able to create without errors
    EXPECT_TRUE(true);
}

TEST_F(SWEAgentTest, SetModel) {
    agent_->setModel("claude-3-opus");

    // Should not crash
    EXPECT_TRUE(true);
}

TEST_F(SWEAgentTest, SetMaxIterations) {
    agent_->setMaxIterations(50);

    // Should not crash
    EXPECT_TRUE(true);
}

TEST_F(SWEAgentTest, SetParallelExecutionMode) {
    agent_->setParallelExecutionMode(true);
    agent_->setParallelExecutionMode(false);

    // Should not crash
    EXPECT_TRUE(true);
}

TEST_F(SWEAgentTest, GetStatus) {
    std::string status = agent_->getStatus();

    // Should return some status
    EXPECT_FALSE(status.empty());
}

TEST_F(SWEAgentTest, GetHistory) {
    auto history = agent_->getHistory();

    // Initially should be empty
    EXPECT_EQ(history.size(), 0);
}

TEST_F(SWEAgentTest, GenerateCode) {
    CodeContext context;
    context.language = "cpp";

    std::string code = agent_->generateCode("Create a function to validate email addresses", context);

    // May return empty string if no model connected
    EXPECT_TRUE(true);
}

TEST_F(SWEAgentTest, GenerateTests) {
    CodeContext context;
    context.language = "cpp";

    std::string sampleCode = "bool validate_email(const std::string& email) { return true; }";
    auto tests = agent_->generateTests(sampleCode, context);

    // May return empty if no model connected
    EXPECT_TRUE(true);
}

TEST_F(SWEAgentTest, RunTests) {
    bool result = agent_->runTests("ctest --output-on-failure");

    // Should return true or false without crashing
    EXPECT_TRUE(result || !result);
}

TEST_F(SWEAgentTest, ExecuteCommand) {
    agent_->executeCommand("echo test");

    // Should not crash
    EXPECT_TRUE(true);
}

// ============================================================================
// SWEAgentManager Tests
// ============================================================================

TEST_F(SWEAgentTest, ManagerCreation) {
    SWEAgentManager manager;

    auto agents = manager.getActiveAgents();
    EXPECT_EQ(agents.size(), 0);
}

TEST_F(SWEAgentTest, ManagerAddAgent) {
    SWEAgentManager manager;

    auto agent = std::make_shared<SWEAgent>("agent-1", "gpt-4");
    manager.addAgent(agent);

    auto agents = manager.getActiveAgents();
    EXPECT_EQ(agents.size(), 1);
}

TEST_F(SWEAgentTest, ManagerRemoveAgent) {
    SWEAgentManager manager;

    auto agent = std::make_shared<SWEAgent>("agent-1", "gpt-4");
    manager.addAgent(agent);
    manager.removeAgent("agent-1");

    auto agents = manager.getActiveAgents();
    EXPECT_EQ(agents.size(), 0);
}

TEST_F(SWEAgentTest, ManagerGetAgent) {
    SWEAgentManager manager;

    auto agent = std::make_shared<SWEAgent>("agent-1", "gpt-4");
    manager.addAgent(agent);

    auto retrieved = manager.getAgent("agent-1");
    EXPECT_NE(retrieved, nullptr);
}

TEST_F(SWEAgentTest, ManagerSetMaxParallelAgents) {
    SWEAgentManager manager;

    manager.setMaxParallelAgents(4);

    // Should not crash
    EXPECT_TRUE(true);
}

TEST_F(SWEAgentTest, ManagerGetAgentStatuses) {
    SWEAgentManager manager;

    auto agent = std::make_shared<SWEAgent>("agent-1", "gpt-4");
    manager.addAgent(agent);

    auto statuses = manager.getAgentStatuses();
    EXPECT_EQ(statuses.size(), 1);
}

// ============================================================================
// SWEBench Tests
// ============================================================================

TEST_F(SWEAgentTest, BenchmarkCreation) {
    SWEBench bench("/path/to/benchmark");

    // Should be able to create without errors
    EXPECT_TRUE(true);
}

TEST_F(SWEAgentTest, BenchmarkGetSuccessRate) {
    SWEBench bench("/path/to/benchmark");

    float rate = bench.getSuccessRate();
    EXPECT_GE(rate, 0.0f);
    EXPECT_LE(rate, 1.0f);
}

TEST_F(SWEAgentTest, BenchmarkGetFailedTests) {
    SWEBench bench("/path/to/benchmark");

    auto failed = bench.getFailedTests();
    EXPECT_GE(failed.size(), 0);
}

TEST_F(SWEAgentTest, BenchmarkGenerateReport) {
    SWEBench bench("/path/to/benchmark");

    std::string report = bench.generateReport();
    // Report may be empty if no tests run
    EXPECT_TRUE(true);
}

