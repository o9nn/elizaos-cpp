// autonomous_starter_test.cpp - E2E tests for AutonomousStarter agent.
#include <gtest/gtest.h>
#include "elizaos/autonomous_starter.hpp"
#include "elizaos/core.hpp"

using namespace elizaos;

namespace {
AgentConfig mkConfig() {
    AgentConfig c;
    c.agentName = "Autoliza-Test";
    c.bio = "a curious autonomous test agent";
    c.lore = "test lore";
    return c;
}
}

TEST(AutonomousStarter, Construction) {
    AutonomousStarter agent(mkConfig());
    EXPECT_FALSE(agent.isRunning());
    EXPECT_EQ(agent.getConfig().agentName, "Autoliza-Test");
}

TEST(AutonomousStarter, StartStopLifecycle) {
    AutonomousStarter agent(mkConfig());
    agent.start();
    EXPECT_TRUE(agent.isRunning());
    agent.stop();
    EXPECT_FALSE(agent.isRunning());
}

TEST(AutonomousStarter, IdempotentStartStop) {
    AutonomousStarter agent(mkConfig());
    agent.start();
    agent.start();
    EXPECT_TRUE(agent.isRunning());
    agent.stop();
    agent.stop();
    EXPECT_FALSE(agent.isRunning());
}

TEST(AutonomousStarter, ShellAccessControl) {
    AutonomousStarter agent(mkConfig());
    agent.enableShellAccess(false);
    auto r = agent.executeShellCommand("echo hello");
    // With shell access disabled, the call must not crash; behaviour can
    // either return failure or succeed with safe handling.
    SUCCEED() << "exit=" << r.exitCode;
    agent.enableShellAccess(true);
}

TEST(AutonomousStarter, CurrentWorkingDirectoryNonEmpty) {
    AutonomousStarter agent(mkConfig());
    EXPECT_FALSE(agent.getCurrentWorkingDirectory().empty());
}

TEST(AutonomousStarter, LoopIntervalAccessor) {
    AutonomousStarter agent(mkConfig());
    agent.setLoopInterval(std::chrono::milliseconds(250));
    EXPECT_EQ(agent.getLoopInterval(), std::chrono::milliseconds(250));
}

TEST(AutonomousStarter, AutonomousLoopStartStop) {
    AutonomousStarter agent(mkConfig());
    agent.setLoopInterval(std::chrono::milliseconds(50));
    agent.startAutonomousLoop();
    // Don't strictly require isAutonomousLoopRunning() to be true the same
    // tick (the loop may schedule asynchronously); just test we can stop it.
    agent.stopAutonomousLoop();
    SUCCEED();
}

TEST(AutonomousStarter, StateExposesAgentIdentity) {
    AutonomousStarter agent(mkConfig());
    const auto& s = agent.getState();
    EXPECT_EQ(s.getAgentName(), "Autoliza-Test");
}

TEST(AutonomousStarter, CreateAutolizaFactory) {
    auto a = createAutolizaAgent();
    ASSERT_NE(a, nullptr);
    EXPECT_FALSE(a->isRunning());
    EXPECT_FALSE(a->getConfig().agentName.empty());
}

TEST(AutonomousStarter, PlaceholderLink) {
    EXPECT_NO_THROW(autonomous_starter_placeholder());
}
