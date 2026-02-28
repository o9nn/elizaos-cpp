#include <gtest/gtest.h>
#include "elizaos/hat.hpp"
#include <thread>
#include <chrono>
#include <vector>

using namespace elizaos::hat;

class HATTest : public ::testing::Test {
protected:
    void SetUp() override {
        coordinator = std::make_unique<TeamCoordinator>();
    }
    void TearDown() override {
        coordinator.reset();
    }
    std::unique_ptr<TeamCoordinator> coordinator;
};

TEST_F(HATTest, CanCreateCoordinator) {
    EXPECT_TRUE(coordinator != nullptr);
}

TEST_F(HATTest, TeamMemberStructWorks) {
    TeamMember member;
    member.id = "agent-1";
    member.name = "Test Agent";
    member.role = TeamRole::AGENT_MEMBER;
    member.capabilities = {"coding", "testing"};
    member.isAvailable = true;
    
    EXPECT_EQ(member.id, "agent-1");
    EXPECT_TRUE(member.canHandle("coding"));
    EXPECT_FALSE(member.canHandle("flying"));
}

TEST_F(HATTest, TeamTaskStructWorks) {
    TeamTask task;
    task.id = "task-1";
    task.name = "Test Task";
    task.description = "A test task";
    task.priority = TaskPriority::NORMAL;
    task.status = TaskStatus::PENDING;
    
    EXPECT_EQ(task.id, "task-1");
    EXPECT_TRUE(task.priority == TaskPriority::NORMAL);
}

TEST_F(HATTest, CanCreateProtocolHandler) {
    HATProtocolHandler handler;
    SUCCEED();
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return testing::RUN_ALL_TESTS();
}
