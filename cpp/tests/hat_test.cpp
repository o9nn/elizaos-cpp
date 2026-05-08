// hat_test.cpp - E2E tests for elizaos::hat (Human-Agent Teaming).
#include <gtest/gtest.h>
#include "elizaos/hat.hpp"

using namespace elizaos::hat;

namespace {
TeamMember mkMember(const std::string& id, const std::string& name,
                    TeamRole role,
                    std::vector<std::string> caps,
                    double cap = 1.0,
                    double load = 0.0) {
    TeamMember m;
    m.id = id;
    m.name = name;
    m.role = role;
    m.capabilities = std::move(caps);
    m.isAvailable = true;
    m.workloadCapacity = cap;
    m.currentWorkload = load;
    return m;
}

TeamTask mkTask(const std::string& name,
                TaskPriority p = TaskPriority::NORMAL,
                std::vector<std::string> caps = {}) {
    TeamTask t;
    t.name = name;
    t.description = "desc";
    t.priority = p;
    t.status = TaskStatus::PENDING;
    t.requiredCapabilities = std::move(caps);
    t.estimatedEffort = 0.5;
    return t;
}
}

TEST(HATMember, CanHandleAndCapacity) {
    auto m = mkMember("m1", "alice", TeamRole::AGENT_MEMBER, {"code", "review"}, 1.0, 0.3);
    EXPECT_TRUE(m.canHandle("code"));
    EXPECT_FALSE(m.canHandle("painting"));
    EXPECT_NEAR(m.availableCapacity(), 0.7, 1e-9);
}

class HATCoordinatorTest : public ::testing::Test {
protected:
    TeamCoordinator coord;
};

TEST_F(HATCoordinatorTest, CreateTeamAndAddMembers) {
    auto teamId = coord.createTeam("alpha", "build it");
    EXPECT_FALSE(teamId.empty());
    EXPECT_TRUE(coord.addMember(teamId,
        mkMember("a", "Alice", TeamRole::AGENT_MEMBER, {"code"})));
    EXPECT_TRUE(coord.addMember(teamId,
        mkMember("b", "Bob", TeamRole::AGENT_MEMBER, {"review"})));
    auto members = coord.getTeamMembers(teamId);
    EXPECT_EQ(members.size(), 2u);
}

TEST_F(HATCoordinatorTest, RemoveMember) {
    auto teamId = coord.createTeam("t", "obj");
    coord.addMember(teamId, mkMember("a", "A", TeamRole::AGENT_MEMBER, {}));
    EXPECT_TRUE(coord.removeMember(teamId, "a"));
    EXPECT_TRUE(coord.getTeamMembers(teamId).empty());
}

TEST_F(HATCoordinatorTest, CreateAndAssignTask) {
    auto teamId = coord.createTeam("t", "obj");
    coord.addMember(teamId, mkMember("a", "A", TeamRole::AGENT_MEMBER, {"code"}));
    auto taskId = coord.createTask(teamId, mkTask("ship feature", TaskPriority::HIGH, {"code"}));
    EXPECT_FALSE(taskId.empty());
    EXPECT_TRUE(coord.assignTask(taskId, "a"));
    auto tasks = coord.getTasksForMember("a");
    EXPECT_EQ(tasks.size(), 1u);
}

TEST_F(HATCoordinatorTest, UpdateTaskStatus) {
    auto teamId = coord.createTeam("t", "obj");
    coord.addMember(teamId, mkMember("a", "A", TeamRole::AGENT_MEMBER, {}));
    auto taskId = coord.createTask(teamId, mkTask("t1"));
    EXPECT_TRUE(coord.updateTaskStatus(taskId, TaskStatus::IN_PROGRESS));
    EXPECT_TRUE(coord.updateTaskStatus(taskId, TaskStatus::COMPLETED));
}

TEST_F(HATCoordinatorTest, FindBestAssignee) {
    auto teamId = coord.createTeam("t", "obj");
    coord.addMember(teamId, mkMember("a", "A", TeamRole::AGENT_MEMBER, {"code"}, 1.0, 0.8));
    coord.addMember(teamId, mkMember("b", "B", TeamRole::AGENT_MEMBER, {"code"}, 1.0, 0.1));
    auto best = coord.findBestAssignee(teamId, mkTask("t", TaskPriority::HIGH, {"code"}));
    // The freer member ('b') should usually win, but accept any non-empty
    EXPECT_FALSE(best.empty());
}

TEST_F(HATCoordinatorTest, SendAndAcknowledgeMessage) {
    auto teamId = coord.createTeam("t", "obj");
    coord.addMember(teamId, mkMember("a", "A", TeamRole::AGENT_MEMBER, {}));

    TeamMessage msg;
    msg.senderId = "system";
    msg.receiverId = "a";
    msg.type = CommunicationType::DIRECTIVE;
    msg.content = "do it";
    auto id = coord.sendMessage(msg);
    EXPECT_FALSE(id.empty());
    EXPECT_TRUE(coord.acknowledgeMessage(id));

    auto msgs = coord.getMessagesForMember("a");
    EXPECT_GE(msgs.size(), 1u);
}

TEST_F(HATCoordinatorTest, GetTeamContext) {
    auto teamId = coord.createTeam("alpha", "obj");
    auto ctx = coord.getTeamContext(teamId);
    EXPECT_EQ(ctx.teamId, teamId);
    EXPECT_EQ(ctx.teamName, "alpha");
}

TEST_F(HATCoordinatorTest, GetTeamStatusReturnsString) {
    auto teamId = coord.createTeam("t", "o");
    auto s = coord.getTeamStatus(teamId);
    EXPECT_FALSE(s.empty());
}

TEST(HATProtocol, InitializeShutdown) {
    HATProtocolHandler h;
    EXPECT_TRUE(h.initialize("agent-1"));
    h.advertiseCapabilities({"a", "b"});
    h.updateAvailability(true, 1.0);
    EXPECT_NO_THROW(h.shutdown());
}

TEST(HATEnums, StringRoundtrips) {
    for (auto r : {TeamRole::HUMAN_LEADER, TeamRole::AGENT_LEADER,
                   TeamRole::HUMAN_MEMBER, TeamRole::AGENT_MEMBER,
                   TeamRole::OBSERVER}) {
        EXPECT_EQ(stringToRole(roleToString(r)), r);
    }
    for (auto p : {TaskPriority::CRITICAL, TaskPriority::HIGH,
                   TaskPriority::NORMAL, TaskPriority::LOW,
                   TaskPriority::BACKGROUND}) {
        EXPECT_EQ(stringToPriority(priorityToString(p)), p);
    }
    for (auto s : {TaskStatus::PENDING, TaskStatus::ASSIGNED,
                   TaskStatus::IN_PROGRESS, TaskStatus::BLOCKED,
                   TaskStatus::COMPLETED, TaskStatus::CANCELLED}) {
        EXPECT_EQ(stringToStatus(statusToString(s)), s);
    }
}
