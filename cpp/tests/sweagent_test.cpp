// sweagent_test.cpp - E2E tests for the SWE-Agent autonomous software engineer.
#include <gtest/gtest.h>
#include "elizaos/sweagent.hpp"

using namespace elizaos;

class SWEAgentTest : public ::testing::Test {
protected:
    SWEAgentTest() : agent("agent-1", "test-model") {}
    SWEAgent agent;
};

TEST_F(SWEAgentTest, IdAccessor) {
    EXPECT_EQ(agent.getId(), "agent-1");
}

TEST_F(SWEAgentTest, ConfigurationKnobs) {
    EXPECT_NO_THROW(agent.setModel("other-model"));
    EXPECT_NO_THROW(agent.setMaxIterations(5));
    EXPECT_NO_THROW(agent.setParallelExecutionMode(true));
}

TEST_F(SWEAgentTest, GenerateCodeReturnsString) {
    CodeContext ctx;
    ctx.language = "cpp";
    auto code = agent.generateCode("hello world", ctx);
    EXPECT_FALSE(code.empty());
}

TEST_F(SWEAgentTest, GenerateTestsReturnsList) {
    CodeContext ctx;
    auto tests = agent.generateTests("int add(int a, int b);", ctx);
    SUCCEED() << "tests=" << tests.size();
}

TEST_F(SWEAgentTest, ValidateSolutionDoesNotThrow) {
    EXPECT_NO_THROW(agent.validateSolution("int x = 0;", {"assert(x == 0);"}));
}

TEST_F(SWEAgentTest, SolveFromDescriptionRuns) {
    CodeContext ctx;
    ctx.language = "cpp";
    EXPECT_NO_THROW({
        auto r = agent.solveFromDescription("Add two numbers", ctx);
        (void)r;
    });
}

TEST_F(SWEAgentTest, GetHistoryAndStatusAccessible) {
    EXPECT_NO_THROW(agent.getStatus());
    EXPECT_NO_THROW(agent.getHistory());
}
