#include "elizaos/autonomous_starter.hpp"
#include "elizaos/agentlogger.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <string>
#include <thread>

using namespace elizaos;

namespace {

bool memoryContains(const State& state, const std::string& needle) {
    for (const auto& memory : state.getRecentMessages()) {
        if (memory && memory->getContent().find(needle) != std::string::npos) {
            return true;
        }
    }
    return false;
}

} // namespace

class AutonomousStarterTest : public ::testing::Test {
protected:
    void SetUp() override {
        if (!globalLogger) {
            globalLogger = std::make_shared<AgentLogger>();
        }

        agent = createAutolizaAgent();
        ASSERT_NE(agent, nullptr);
    }

    void TearDown() override {
        if (agent) {
            agent->stop();
        }
    }

    std::shared_ptr<AutonomousStarter> agent;
};

TEST_F(AutonomousStarterTest, AgentCreation) {
    EXPECT_FALSE(agent->isRunning());
    EXPECT_EQ(agent->getConfig().agentName, "Autoliza");
    EXPECT_FALSE(agent->getConfig().agentId.empty());
    EXPECT_FALSE(agent->getConfig().bio.empty());
    EXPECT_FALSE(agent->getCurrentWorkingDirectory().empty());
}

TEST_F(AutonomousStarterTest, StartStopCreatesLifecycleMemories) {
    EXPECT_FALSE(agent->isRunning());

    agent->start();
    EXPECT_TRUE(agent->isRunning());
    EXPECT_TRUE(memoryContains(agent->getState(), "Awakening"));

    agent->stop();
    EXPECT_FALSE(agent->isRunning());
    EXPECT_TRUE(memoryContains(agent->getState(), "Shutdown"));
}

TEST_F(AutonomousStarterTest, ShellCommandExecutionCapturesSuccessAndFailure) {
    agent->start();

    auto result = agent->executeShellCommand("echo 'Hello World'");
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.exitCode, 0);
    EXPECT_NE(result.output.find("Hello World"), std::string::npos);
    EXPECT_TRUE(result.error.empty());

    result = agent->executeShellCommand("nonexistentcommand12345");
    EXPECT_FALSE(result.success);
    EXPECT_NE(result.exitCode, 0);
    EXPECT_FALSE(result.error.empty());
}

TEST_F(AutonomousStarterTest, EmptyCommandsAndForbiddenCommandsAreRejected) {
    agent->start();

    auto result = agent->executeShellCommand("   ");
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.exitCode, -1);
    EXPECT_NE(result.error.find("empty"), std::string::npos);

    result = agent->executeShellCommand("rm -rf /");
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.exitCode, -1);
    EXPECT_FALSE(result.error.empty());
    EXPECT_NE(result.error.find("forbidden"), std::string::npos);
    EXPECT_TRUE(memoryContains(agent->getState(), "Command rejected"));
}

TEST_F(AutonomousStarterTest, ShellAccessControlAppliesToDirectAndTaskCommands) {
    agent->start();

    auto result = agent->executeShellCommand("pwd");
    EXPECT_TRUE(result.success);

    agent->enableShellAccess(false);
    result = agent->executeShellCommand("pwd");
    EXPECT_FALSE(result.success);
    EXPECT_NE(result.error.find("disabled"), std::string::npos);

    UUID rejectedTaskId = agent->executeShellCommandAsTask("echo should-not-run");
    EXPECT_TRUE(rejectedTaskId.empty());

    agent->enableShellAccess(true);
    result = agent->executeShellCommand("pwd");
    EXPECT_TRUE(result.success);
}

TEST_F(AutonomousStarterTest, WorkingDirectoryTrackingAndCdAreStateful) {
    agent->start();

    const std::filesystem::path tmp = std::filesystem::temp_directory_path();
    auto result = agent->executeShellCommand("cd " + tmp.string());
    EXPECT_TRUE(result.success) << result.error;
    EXPECT_EQ(agent->getCurrentWorkingDirectory(), std::filesystem::weakly_canonical(tmp).string());

    result = agent->executeShellCommand("pwd");
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.output.find(agent->getCurrentWorkingDirectory()), std::string::npos);

    result = agent->executeShellCommand("cd /definitely/not/a/real/elizaos/path");
    EXPECT_FALSE(result.success);
    EXPECT_NE(result.error.find("no such directory"), std::string::npos);
}

TEST_F(AutonomousStarterTest, AutonomousLoopProducesPerceptionReasoningAndActionMemories) {
    agent->start();
    agent->setLoopInterval(std::chrono::milliseconds(200));

    EXPECT_FALSE(agent->isAutonomousLoopRunning());

    agent->startAutonomousLoop();
    EXPECT_TRUE(agent->isAutonomousLoopRunning());

    std::this_thread::sleep_for(std::chrono::milliseconds(900));

    agent->stopAutonomousLoop();
    EXPECT_FALSE(agent->isAutonomousLoopRunning());

    EXPECT_TRUE(memoryContains(agent->getState(), "perception"));
    EXPECT_TRUE(memoryContains(agent->getState(), "reasoning"));
    EXPECT_TRUE(memoryContains(agent->getState(), "action"));
}

TEST_F(AutonomousStarterTest, LoopIntervalRejectsInvalidValues) {
    agent->start();

    agent->setLoopInterval(std::chrono::milliseconds(500));
    EXPECT_EQ(agent->getLoopInterval().count(), 500);

    agent->setLoopInterval(std::chrono::milliseconds(0));
    EXPECT_EQ(agent->getLoopInterval().count(), 500);

    agent->setLoopInterval(std::chrono::milliseconds(-10));
    EXPECT_EQ(agent->getLoopInterval().count(), 500);
}

TEST_F(AutonomousStarterTest, TaskBasedExecutionRecordsCompletionMemory) {
    agent->start();

    UUID taskId = agent->executeShellCommandAsTask("echo 'Task Test'");
    EXPECT_FALSE(taskId.empty());

    std::this_thread::sleep_for(std::chrono::milliseconds(1500));

    EXPECT_TRUE(memoryContains(agent->getState(), "Task scheduled"));
    EXPECT_TRUE(memoryContains(agent->getState(), "Task completed"));
    EXPECT_TRUE(memoryContains(agent->getState(), "Task Test"));
}

TEST_F(AutonomousStarterTest, CommandExecutionCreatesInspectableMemory) {
    agent->start();

    const auto& state = agent->getState();
    size_t initialMemoryCount = state.getRecentMessages().size();

    agent->executeShellCommand("echo 'Memory Test'");

    EXPECT_GT(state.getRecentMessages().size(), initialMemoryCount);
    EXPECT_TRUE(memoryContains(state, "echo 'Memory Test'"));
    EXPECT_TRUE(memoryContains(state, "Exit code: 0"));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
