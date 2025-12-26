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
        config_.workingDirectory = "/tmp/sweagent_test";
        config_.maxIterations = 50;
        config_.timeout = 300;
    }

    SWEAgentConfig config_;
};

// ============================================================================
// SWEAgentConfig Tests
// ============================================================================

TEST_F(SWEAgentTest, DefaultSWEAgentConfigValues) {
    SWEAgentConfig defaultConfig;

    EXPECT_EQ(defaultConfig.workingDirectory, ".");
    EXPECT_EQ(defaultConfig.maxIterations, 100);
    EXPECT_EQ(defaultConfig.timeout, 600);
    EXPECT_TRUE(defaultConfig.enableLogging);
    EXPECT_TRUE(defaultConfig.enableUndo);
}

TEST_F(SWEAgentTest, CustomSWEAgentConfigValues) {
    config_.workingDirectory = "/project";
    config_.maxIterations = 25;
    config_.timeout = 120;
    config_.enableUndo = false;

    EXPECT_EQ(config_.workingDirectory, "/project");
    EXPECT_EQ(config_.maxIterations, 25);
    EXPECT_EQ(config_.timeout, 120);
    EXPECT_FALSE(config_.enableUndo);
}

// ============================================================================
// SWETask Tests
// ============================================================================

TEST_F(SWEAgentTest, SWETaskCreation) {
    SWETask task;
    task.id = "task-001";
    task.description = "Fix bug in authentication module";
    task.type = SWETaskType::BUG_FIX;
    task.priority = TaskPriority::HIGH;

    EXPECT_EQ(task.id, "task-001");
    EXPECT_EQ(task.type, SWETaskType::BUG_FIX);
    EXPECT_EQ(task.priority, TaskPriority::HIGH);
}

TEST_F(SWEAgentTest, SWETaskWithIssue) {
    SWETask task;
    task.id = "task-002";
    task.issueUrl = "https://github.com/org/repo/issues/123";
    task.repository = "org/repo";
    task.branch = "fix/auth-bug";

    EXPECT_EQ(task.issueUrl, "https://github.com/org/repo/issues/123");
    EXPECT_EQ(task.repository, "org/repo");
    EXPECT_EQ(task.branch, "fix/auth-bug");
}

TEST_F(SWEAgentTest, SWETaskFiles) {
    SWETask task;
    task.targetFiles = {"src/auth.cpp", "include/auth.hpp", "tests/test_auth.cpp"};

    EXPECT_EQ(task.targetFiles.size(), 3);
    EXPECT_THAT(task.targetFiles, Contains("src/auth.cpp"));
}

// ============================================================================
// SWEAction Tests
// ============================================================================

TEST_F(SWEAgentTest, SWEActionFileEdit) {
    SWEAction action;
    action.type = SWEActionType::EDIT_FILE;
    action.filePath = "src/main.cpp";
    action.content = "int main() { return 0; }";
    action.lineStart = 1;
    action.lineEnd = 10;

    EXPECT_EQ(action.type, SWEActionType::EDIT_FILE);
    EXPECT_EQ(action.filePath, "src/main.cpp");
    EXPECT_EQ(action.lineStart, 1);
    EXPECT_EQ(action.lineEnd, 10);
}

TEST_F(SWEAgentTest, SWEActionCommand) {
    SWEAction action;
    action.type = SWEActionType::RUN_COMMAND;
    action.command = "make test";
    action.workingDir = "/project";

    EXPECT_EQ(action.type, SWEActionType::RUN_COMMAND);
    EXPECT_EQ(action.command, "make test");
}

TEST_F(SWEAgentTest, SWEActionSearch) {
    SWEAction action;
    action.type = SWEActionType::SEARCH_CODE;
    action.searchQuery = "authentication";
    action.searchScope = "*.cpp";

    EXPECT_EQ(action.type, SWEActionType::SEARCH_CODE);
    EXPECT_EQ(action.searchQuery, "authentication");
}

// ============================================================================
// SWEActionResult Tests
// ============================================================================

TEST_F(SWEAgentTest, SWEActionResultSuccess) {
    SWEActionResult result;
    result.success = true;
    result.output = "Tests passed: 42/42";
    result.duration = std::chrono::milliseconds(5000);

    EXPECT_TRUE(result.success);
    EXPECT_NE(result.output.find("42/42"), std::string::npos);
}

TEST_F(SWEAgentTest, SWEActionResultFailure) {
    SWEActionResult result;
    result.success = false;
    result.error = "Compilation failed";
    result.errorCode = 1;
    result.errorOutput = "error: undefined reference to 'foo'";

    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, 1);
    EXPECT_NE(result.errorOutput.find("undefined reference"), std::string::npos);
}

// ============================================================================
// SWEPlan Tests
// ============================================================================

TEST_F(SWEAgentTest, SWEPlanCreation) {
    SWEPlan plan;
    plan.id = "plan-001";
    plan.taskId = "task-001";
    plan.description = "Fix authentication bug by updating token validation";

    EXPECT_EQ(plan.id, "plan-001");
    EXPECT_EQ(plan.taskId, "task-001");
}

TEST_F(SWEAgentTest, SWEPlanSteps) {
    SWEPlan plan;

    SWEPlanStep step1;
    step1.order = 1;
    step1.description = "Locate the authentication module";
    step1.status = StepStatus::PENDING;
    plan.steps.push_back(step1);

    SWEPlanStep step2;
    step2.order = 2;
    step2.description = "Identify the bug";
    step2.status = StepStatus::PENDING;
    plan.steps.push_back(step2);

    SWEPlanStep step3;
    step3.order = 3;
    step3.description = "Implement fix";
    step3.status = StepStatus::PENDING;
    plan.steps.push_back(step3);

    EXPECT_EQ(plan.steps.size(), 3);
    EXPECT_EQ(plan.steps[0].order, 1);
    EXPECT_EQ(plan.steps[2].description, "Implement fix");
}

// ============================================================================
// SWEContext Tests
// ============================================================================

TEST_F(SWEAgentTest, SWEContextCreation) {
    SWEContext context;
    context.currentFile = "src/auth.cpp";
    context.currentLine = 42;
    context.recentOutput = "Build succeeded";

    EXPECT_EQ(context.currentFile, "src/auth.cpp");
    EXPECT_EQ(context.currentLine, 42);
}

TEST_F(SWEAgentTest, SWEContextHistory) {
    SWEContext context;

    SWEAction action1;
    action1.type = SWEActionType::READ_FILE;
    action1.filePath = "src/main.cpp";
    context.actionHistory.push_back(action1);

    SWEAction action2;
    action2.type = SWEActionType::EDIT_FILE;
    action2.filePath = "src/main.cpp";
    context.actionHistory.push_back(action2);

    EXPECT_EQ(context.actionHistory.size(), 2);
}

TEST_F(SWEAgentTest, SWEContextOpenFiles) {
    SWEContext context;
    context.openFiles["src/main.cpp"] = "int main() {}";
    context.openFiles["include/header.hpp"] = "#pragma once";

    EXPECT_EQ(context.openFiles.size(), 2);
    EXPECT_TRUE(context.openFiles.count("src/main.cpp") > 0);
}

// ============================================================================
// SWEObservation Tests
// ============================================================================

TEST_F(SWEAgentTest, SWEObservationCreation) {
    SWEObservation obs;
    obs.type = SWEObservationType::FILE_CONTENT;
    obs.content = "Current file content...";
    obs.metadata["lines"] = "100";

    EXPECT_EQ(obs.type, SWEObservationType::FILE_CONTENT);
    EXPECT_FALSE(obs.content.empty());
    EXPECT_EQ(obs.metadata["lines"], "100");
}

TEST_F(SWEAgentTest, SWEObservationError) {
    SWEObservation obs;
    obs.type = SWEObservationType::ERROR;
    obs.content = "File not found: missing.cpp";
    obs.isError = true;

    EXPECT_EQ(obs.type, SWEObservationType::ERROR);
    EXPECT_TRUE(obs.isError);
}

// ============================================================================
// SWESession Tests
// ============================================================================

TEST_F(SWEAgentTest, SWESessionCreation) {
    SWESession session;
    session.id = "session-001";
    session.startTime = std::chrono::system_clock::now();
    session.status = SWESessionStatus::ACTIVE;

    EXPECT_EQ(session.id, "session-001");
    EXPECT_EQ(session.status, SWESessionStatus::ACTIVE);
}

TEST_F(SWEAgentTest, SWESessionMetrics) {
    SWESession session;
    session.metrics.actionsExecuted = 15;
    session.metrics.filesModified = 3;
    session.metrics.linesChanged = 42;
    session.metrics.testsPassed = 10;
    session.metrics.testsFailed = 0;

    EXPECT_EQ(session.metrics.actionsExecuted, 15);
    EXPECT_EQ(session.metrics.filesModified, 3);
    EXPECT_EQ(session.metrics.linesChanged, 42);
    EXPECT_EQ(session.metrics.testsFailed, 0);
}

// ============================================================================
// SWEDiff Tests
// ============================================================================

TEST_F(SWEAgentTest, SWEDiffCreation) {
    SWEDiff diff;
    diff.filePath = "src/auth.cpp";
    diff.oldContent = "int validate() { return 0; }";
    diff.newContent = "int validate() { return validate_token(); }";

    EXPECT_EQ(diff.filePath, "src/auth.cpp");
    EXPECT_NE(diff.oldContent, diff.newContent);
}

TEST_F(SWEAgentTest, SWEDiffHunks) {
    SWEDiff diff;
    diff.filePath = "src/main.cpp";

    DiffHunk hunk;
    hunk.oldStart = 10;
    hunk.oldCount = 5;
    hunk.newStart = 10;
    hunk.newCount = 7;
    hunk.content = "@@ -10,5 +10,7 @@\n-old line\n+new line";
    diff.hunks.push_back(hunk);

    EXPECT_EQ(diff.hunks.size(), 1);
    EXPECT_EQ(diff.hunks[0].oldStart, 10);
}
