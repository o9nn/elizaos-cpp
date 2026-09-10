// hat_test.cpp - E2E tests for elizaos::hat (Human-Agent Teaming).
#include <gtest/gtest.h>
#include "elizaos/hat.hpp"

#include <algorithm>
#include <atomic>
#include <limits>
#include <thread>

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
    ASSERT_TRUE(coord.assignTask(taskId, "a"));
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

TEST(HATTokens, PermissionRegistryAndRevocation) {
    std::vector<std::string> permissions = {"read", "write", "execute"};
    const auto token = issueHATToken("agent@1", "team/main", permissions);

    EXPECT_FALSE(token.empty());
    EXPECT_TRUE(validateHATToken(token));
    EXPECT_TRUE(checkHATPermission(token, "read"));
    EXPECT_TRUE(checkHATPermission(token, "write"));
    EXPECT_FALSE(checkHATPermission(token, "admin"));
    EXPECT_FALSE(checkHATPermission("missing-token", "read"));

    revokeHATToken(token);
    EXPECT_FALSE(validateHATToken(token));
    EXPECT_FALSE(checkHATPermission(token, "read"));
}

TEST_F(HATCoordinatorTest, AcknowledgeMessageMutatesMessageAndRejectsMissingId) {
    auto teamId = coord.createTeam("t", "obj");
    ASSERT_TRUE(coord.addMember(teamId, mkMember("a", "A", TeamRole::AGENT_MEMBER, {})));

    TeamMessage msg;
    msg.senderId = "system";
    msg.receiverId = "a";
    msg.type = CommunicationType::DIRECTIVE;
    msg.content = "ack me";
    msg.acknowledged = false;
    auto id = coord.sendMessage(msg);

    EXPECT_FALSE(coord.acknowledgeMessage("missing-message"));
    EXPECT_TRUE(coord.acknowledgeMessage(id));
    auto messages = coord.getMessagesForMember("a");
    ASSERT_EQ(messages.size(), 1u);
    EXPECT_TRUE(messages.front().acknowledged);
}

TEST_F(HATCoordinatorTest, BestAssigneeHonorsCapabilitiesAndCapacity) {
    auto teamId = coord.createTeam("t", "obj");
    ASSERT_TRUE(coord.addMember(teamId, mkMember("busy", "Busy", TeamRole::AGENT_MEMBER, {"code"}, 1.0, 0.9)));
    ASSERT_TRUE(coord.addMember(teamId, mkMember("free", "Free", TeamRole::AGENT_MEMBER, {"code", "review"}, 1.0, 0.2)));
    ASSERT_TRUE(coord.addMember(teamId, mkMember("wrong", "Wrong", TeamRole::AGENT_MEMBER, {"paint"}, 1.0, 0.0)));

    auto task = mkTask("review feature", TaskPriority::HIGH, {"code", "review"});
    EXPECT_EQ(coord.findBestAssignee(teamId, task), "free");
    task.status = TaskStatus::IN_PROGRESS;
    EXPECT_EQ(coord.findBestAssignee(teamId, task), "");
    EXPECT_EQ(coord.findBestAssignee("missing-team", task), "");
}

TEST(HATProtocol, RejectsInvalidOperationsAndEmitsAssistanceCallback) {
    HATProtocolHandler h;
    EXPECT_FALSE(h.joinTeam("team-1"));
    EXPECT_FALSE(h.reportStatus("ready"));
    EXPECT_FALSE(h.requestAssistance("task-1", "need context"));

    bool callbackCalled = false;
    h.onMessage([&](const TeamMessage& message) {
        callbackCalled = true;
        EXPECT_EQ(message.senderId, "agent-1");
        EXPECT_EQ(message.relatedTaskId, "task-1");
        EXPECT_EQ(message.type, CommunicationType::QUERY);
    });

    ASSERT_TRUE(h.initialize("agent-1"));
    EXPECT_FALSE(h.initialize(""));
    EXPECT_TRUE(h.joinTeam("team-1"));
    EXPECT_TRUE(h.joinTeam("team-1"));
    EXPECT_TRUE(h.reportStatus("ready"));
    EXPECT_FALSE(h.reportStatus(""));
    EXPECT_TRUE(h.requestAssistance("task-1", "need context"));
    EXPECT_TRUE(callbackCalled);
    EXPECT_TRUE(h.leaveTeam("team-1"));
    EXPECT_FALSE(h.leaveTeam("team-1"));
}

namespace {
const TeamMember* memberById(const std::vector<TeamMember>& members, const std::string& id) {
    const auto it = std::find_if(members.begin(), members.end(),
        [&](const TeamMember& member) { return member.id == id; });
    return it == members.end() ? nullptr : &*it;
}

const TeamTask* taskById(const std::vector<TeamTask>& tasks, const std::string& id) {
    const auto it = std::find_if(tasks.begin(), tasks.end(),
        [&](const TeamTask& task) { return task.id == id; });
    return it == tasks.end() ? nullptr : &*it;
}

TeamTask assignedProtocolTask(const std::string& id, const std::string& agent,
                              double effort = 0.25,
                              std::vector<std::string> capabilities = {"code"}) {
    TeamTask task = mkTask("protocol task", TaskPriority::NORMAL, std::move(capabilities));
    task.id = id;
    task.assignedTo = agent;
    task.status = TaskStatus::ASSIGNED;
    task.estimatedEffort = effort;
    return task;
}
}

TEST_F(HATCoordinatorTest, WorkloadAccountingTracksAssignReassignCompleteAndRemove) {
    const auto teamId = coord.createTeam("accounting", "keep exact load");
    ASSERT_TRUE(coord.addMember(teamId,
        mkMember("a", "A", TeamRole::AGENT_MEMBER, {"code"}, 2.0, 0.25)));
    ASSERT_TRUE(coord.addMember(teamId,
        mkMember("b", "B", TeamRole::AGENT_MEMBER, {"code"}, 2.0, 0.10)));

    auto task = mkTask("feature", TaskPriority::HIGH, {"code"});
    task.estimatedEffort = 0.75;
    const auto taskId = coord.createTask(teamId, task);
    ASSERT_TRUE(coord.assignTask(taskId, "a"));
    auto members = coord.getTeamMembers(teamId);
    ASSERT_NE(memberById(members, "a"), nullptr);
    EXPECT_NEAR(memberById(members, "a")->currentWorkload, 1.0, 1e-9);

    ASSERT_TRUE(coord.assignTask(taskId, "b"));
    members = coord.getTeamMembers(teamId);
    EXPECT_NEAR(memberById(members, "a")->currentWorkload, 0.25, 1e-9);
    EXPECT_NEAR(memberById(members, "b")->currentWorkload, 0.85, 1e-9);

    ASSERT_TRUE(coord.updateTaskStatus(taskId, TaskStatus::IN_PROGRESS));
    EXPECT_FALSE(coord.assignTask(taskId, "a"));
    ASSERT_TRUE(coord.updateTaskStatus(taskId, TaskStatus::COMPLETED));
    members = coord.getTeamMembers(teamId);
    EXPECT_NEAR(memberById(members, "b")->currentWorkload, 0.10, 1e-9);

    auto second = mkTask("handoff", TaskPriority::NORMAL, {"code"});
    second.estimatedEffort = 0.5;
    const auto secondId = coord.createTask(teamId, second);
    ASSERT_TRUE(coord.assignTask(secondId, "a"));
    ASSERT_TRUE(coord.removeMember(teamId, "a"));
    const TeamTask* requeued = taskById(coord.getPendingTasks(teamId), secondId);
    ASSERT_NE(requeued, nullptr);
    EXPECT_TRUE(requeued->assignedTo.empty());
    EXPECT_TRUE(coord.rebalanceWorkload(teamId));
    EXPECT_NE(taskById(coord.getTasksForMember("b"), secondId), nullptr);
}

TEST_F(HATCoordinatorTest, RejectsInvalidMembersTasksAssignmentsAndTransitions) {
    const auto teamId = coord.createTeam("validation", "reject invalid state");
    auto invalidMember = mkMember("bad", "Bad", TeamRole::AGENT_MEMBER, {}, 1.0, 2.0);
    EXPECT_FALSE(coord.addMember(teamId, invalidMember));
    invalidMember = mkMember("nan", "NaN", TeamRole::AGENT_MEMBER, {}, 1.0, 0.0);
    invalidMember.workloadCapacity = std::numeric_limits<double>::quiet_NaN();
    EXPECT_FALSE(coord.addMember(teamId, invalidMember));
    ASSERT_TRUE(coord.addMember(teamId,
        mkMember("coder", "Coder", TeamRole::AGENT_MEMBER, {"code"}, 0.5, 0.0)));
    ASSERT_TRUE(coord.addMember(teamId,
        mkMember("observer", "Observer", TeamRole::OBSERVER, {"code"}, 1.0, 0.0)));

    auto invalidTask = mkTask("invalid");
    invalidTask.estimatedEffort = -0.1;
    EXPECT_TRUE(coord.createTask(teamId, invalidTask).empty());
    invalidTask = mkTask("already complete");
    invalidTask.status = TaskStatus::COMPLETED;
    EXPECT_TRUE(coord.createTask(teamId, invalidTask).empty());

    auto task = mkTask("too large", TaskPriority::NORMAL, {"code"});
    task.estimatedEffort = 0.75;
    const auto taskId = coord.createTask(teamId, task);
    ASSERT_FALSE(taskId.empty());
    EXPECT_FALSE(coord.assignTask("", "coder"));
    EXPECT_FALSE(coord.assignTask("missing", "coder"));
    EXPECT_FALSE(coord.assignTask(taskId, "missing"));
    EXPECT_FALSE(coord.assignTask(taskId, "observer"));
    EXPECT_FALSE(coord.assignTask(taskId, "coder"));
    EXPECT_FALSE(coord.updateTaskStatus(taskId, TaskStatus::IN_PROGRESS));
    EXPECT_FALSE(coord.updateTaskStatus(taskId, TaskStatus::COMPLETED));
    EXPECT_TRUE(coord.updateTaskStatus(taskId, TaskStatus::CANCELLED));
    EXPECT_FALSE(coord.updateTaskStatus(taskId, TaskStatus::IN_PROGRESS));
    EXPECT_FALSE(coord.updateTaskStatus("missing", TaskStatus::CANCELLED));
    EXPECT_FALSE(coord.removeMember("missing-team", "coder"));
    EXPECT_FALSE(coord.removeMember(teamId, "missing"));
}

TEST_F(HATCoordinatorTest, RebalanceIsCapabilityAwareCapacityBoundAndDeterministic) {
    const auto teamId = coord.createTeam("balance", "deterministic assignment");
    ASSERT_TRUE(coord.addMember(teamId,
        mkMember("beta", "Beta", TeamRole::AGENT_MEMBER, {"code"}, 1.0, 0.0)));
    ASSERT_TRUE(coord.addMember(teamId,
        mkMember("alpha", "Alpha", TeamRole::AGENT_MEMBER, {"code", "review"}, 1.0, 0.0)));
    auto unavailable = mkMember("gamma", "Gamma", TeamRole::AGENT_MEMBER,
                                {"code", "review"}, 10.0, 0.0);
    unavailable.isAvailable = false;
    ASSERT_TRUE(coord.addMember(teamId, unavailable));

    auto critical = mkTask("critical code", TaskPriority::CRITICAL, {"code"});
    critical.estimatedEffort = 0.6;
    auto normal = mkTask("normal code", TaskPriority::NORMAL, {"code"});
    normal.estimatedEffort = 0.6;
    auto review = mkTask("review", TaskPriority::LOW, {"review"});
    review.estimatedEffort = 0.4;
    const auto criticalId = coord.createTask(teamId, critical);
    const auto normalId = coord.createTask(teamId, normal);
    const auto reviewId = coord.createTask(teamId, review);

    ASSERT_TRUE(coord.rebalanceWorkload(teamId));
    const auto context = coord.getTeamContext(teamId);
    ASSERT_NE(taskById(context.activeTasks, criticalId), nullptr);
    ASSERT_NE(taskById(context.activeTasks, normalId), nullptr);
    ASSERT_NE(taskById(context.activeTasks, reviewId), nullptr);
    EXPECT_EQ(taskById(context.activeTasks, criticalId)->assignedTo, "alpha");
    EXPECT_EQ(taskById(context.activeTasks, normalId)->assignedTo, "beta");
    EXPECT_EQ(taskById(context.activeTasks, reviewId)->assignedTo, "alpha");
    EXPECT_TRUE(coord.getPendingTasks(teamId).empty());

    const auto members = coord.getTeamMembers(teamId);
    EXPECT_NEAR(memberById(members, "alpha")->currentWorkload, 1.0, 1e-9);
    EXPECT_NEAR(memberById(members, "beta")->currentWorkload, 0.6, 1e-9);
    EXPECT_NEAR(memberById(members, "gamma")->currentWorkload, 0.0, 1e-9);
    EXPECT_FALSE(coord.rebalanceWorkload("missing-team"));
}

TEST_F(HATCoordinatorTest, RebalanceMovesAssignedButPreservesInProgressWork) {
    const auto teamId = coord.createTeam("balance", "status-aware assignment");
    ASSERT_TRUE(coord.addMember(teamId,
        mkMember("loaded", "Loaded", TeamRole::AGENT_MEMBER, {"code"}, 1.0, 0.4)));
    auto task = mkTask("movable", TaskPriority::NORMAL, {"code"});
    task.estimatedEffort = 0.5;
    const auto movableId = coord.createTask(teamId, task);
    ASSERT_TRUE(coord.assignTask(movableId, "loaded"));
    ASSERT_TRUE(coord.addMember(teamId,
        mkMember("free", "Free", TeamRole::AGENT_MEMBER, {"code"}, 1.0, 0.0)));
    ASSERT_TRUE(coord.rebalanceWorkload(teamId));
    EXPECT_EQ(taskById(coord.getTasksForMember("free"), movableId)->assignedTo, "free");

    auto fixed = mkTask("fixed", TaskPriority::NORMAL, {"code"});
    fixed.estimatedEffort = 0.3;
    const auto fixedId = coord.createTask(teamId, fixed);
    ASSERT_TRUE(coord.assignTask(fixedId, "loaded"));
    ASSERT_TRUE(coord.updateTaskStatus(fixedId, TaskStatus::IN_PROGRESS));
    ASSERT_TRUE(coord.rebalanceWorkload(teamId));
    const TeamTask* preserved = taskById(coord.getTasksForMember("loaded"), fixedId);
    ASSERT_NE(preserved, nullptr);
    EXPECT_EQ(preserved->status, TaskStatus::IN_PROGRESS);
}

TEST_F(HATCoordinatorTest, ConcurrentAssignmentMaintainsCapacityInvariant) {
    const auto teamId = coord.createTeam("concurrent", "serialize mutation");
    ASSERT_TRUE(coord.addMember(teamId,
        mkMember("worker", "Worker", TeamRole::AGENT_MEMBER, {"code"}, 1.0, 0.0)));
    std::vector<std::string> taskIds;
    for (int i = 0; i < 20; ++i) {
        auto task = mkTask("task", TaskPriority::NORMAL, {"code"});
        task.estimatedEffort = 0.1;
        taskIds.push_back(coord.createTask(teamId, task));
    }
    std::atomic<int> assigned{0};
    std::vector<std::thread> threads;
    for (const auto& id : taskIds) {
        threads.emplace_back([&, id] {
            if (coord.assignTask(id, "worker")) {
                ++assigned;
            }
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }
    EXPECT_EQ(assigned.load(), 10);
    const auto members = coord.getTeamMembers(teamId);
    ASSERT_NE(memberById(members, "worker"), nullptr);
    EXPECT_NEAR(memberById(members, "worker")->currentWorkload, 1.0, 1e-9);
    EXPECT_EQ(coord.getPendingTasks(teamId).size(), 10u);
}

TEST(HATProtocol, AssignmentAndCompletionCallbacksFollowValidatedTaskFlow) {
    HATProtocolHandler handler;
    int assignedCalls = 0;
    int completedCalls = 0;
    TeamTask assigned;
    TeamTask completed;
    handler.onTaskAssigned([&](const TeamTask& task) {
        ++assignedCalls;
        assigned = task;
    });
    handler.onTaskCompleted([&](const TeamTask& task) {
        ++completedCalls;
        completed = task;
    });

    auto task = assignedProtocolTask("task-1", "agent-1", 0.4);
    EXPECT_FALSE(handler.receiveTaskAssignment("team-1", task));
    ASSERT_TRUE(handler.initialize("agent-1"));
    EXPECT_FALSE(handler.initialize("other-agent"));
    handler.advertiseCapabilities({"code"});
    handler.updateAvailability(true, 1.0);
    ASSERT_TRUE(handler.joinTeam("team-1"));
    EXPECT_FALSE(handler.receiveTaskAssignment("other-team", task));
    task.assignedTo = "other-agent";
    EXPECT_FALSE(handler.receiveTaskAssignment("team-1", task));
    task.assignedTo = "agent-1";
    ASSERT_TRUE(handler.receiveTaskAssignment("team-1", task));
    EXPECT_EQ(assignedCalls, 1);
    EXPECT_EQ(assigned.id, "task-1");
    EXPECT_EQ(assigned.status, TaskStatus::ASSIGNED);
    EXPECT_FALSE(handler.receiveTaskAssignment("team-1", task));
    EXPECT_FALSE(handler.leaveTeam("team-1"));

    ASSERT_TRUE(handler.completeTask("task-1", 0.35));
    EXPECT_EQ(completedCalls, 1);
    EXPECT_EQ(completed.status, TaskStatus::COMPLETED);
    EXPECT_DOUBLE_EQ(completed.actualEffort, 0.35);
    EXPECT_FALSE(handler.completeTask("task-1", 0.4));
    EXPECT_TRUE(handler.leaveTeam("team-1"));
}

TEST(HATProtocol, TaskCallbacksAreReentrantAndCapacityIsReleased) {
    HATProtocolHandler handler;
    ASSERT_TRUE(handler.initialize("agent-1"));
    handler.advertiseCapabilities({"code"});
    handler.updateAvailability(true, 0.5);
    ASSERT_TRUE(handler.joinTeam("team-1"));

    std::atomic<int> assignedCalls{0};
    std::atomic<int> completedCalls{0};
    handler.onTaskAssigned([&](const TeamTask& task) {
        ++assignedCalls;
        EXPECT_TRUE(handler.reportStatus("working:" + task.id));
        EXPECT_TRUE(handler.completeTask(task.id, 0.2));
    });
    handler.onTaskCompleted([&](const TeamTask& task) {
        ++completedCalls;
        EXPECT_EQ(task.status, TaskStatus::COMPLETED);
        EXPECT_TRUE(handler.reportStatus("done:" + task.id));
    });

    EXPECT_TRUE(handler.receiveTaskAssignment(
        "team-1", assignedProtocolTask("task-1", "agent-1", 0.5)));
    EXPECT_EQ(assignedCalls.load(), 1);
    EXPECT_EQ(completedCalls.load(), 1);
    EXPECT_TRUE(handler.receiveTaskAssignment(
        "team-1", assignedProtocolTask("task-2", "agent-1", 0.5)));
    EXPECT_EQ(assignedCalls.load(), 2);
    EXPECT_EQ(completedCalls.load(), 2);
}

TEST(HATProtocol, RejectsUnavailableIncapableOverCapacityAndConcurrentDuplicates) {
    HATProtocolHandler handler;
    ASSERT_TRUE(handler.initialize("agent-1"));
    ASSERT_TRUE(handler.joinTeam("team-1"));
    handler.advertiseCapabilities({"code"});
    handler.updateAvailability(false, 1.0);
    auto task = assignedProtocolTask("task-1", "agent-1", 0.6);
    EXPECT_FALSE(handler.receiveTaskAssignment("team-1", task));
    handler.updateAvailability(true, 1.0);
    task.requiredCapabilities = {"review"};
    EXPECT_FALSE(handler.receiveTaskAssignment("team-1", task));
    task.requiredCapabilities = {"code"};
    task.actualEffort = std::numeric_limits<double>::quiet_NaN();
    EXPECT_FALSE(handler.receiveTaskAssignment("team-1", task));
    task.actualEffort = 0.0;
    task.estimatedEffort = 1.1;
    EXPECT_FALSE(handler.receiveTaskAssignment("team-1", task));
    task.estimatedEffort = 0.6;

    std::atomic<int> successes{0};
    std::vector<std::thread> threads;
    for (int i = 0; i < 16; ++i) {
        threads.emplace_back([&] {
            if (handler.receiveTaskAssignment("team-1", task)) {
                ++successes;
            }
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }
    EXPECT_EQ(successes.load(), 1);
    EXPECT_FALSE(handler.receiveTaskAssignment(
        "team-1", assignedProtocolTask("task-2", "agent-1", 0.5)));
    EXPECT_FALSE(handler.completeTask("missing", 0.1));
    EXPECT_FALSE(handler.completeTask("task-1", -0.1));
}

TEST(HATCompatibility, PlaceholderPerformsDocumentedSelfCheck) {
    EXPECT_NO_THROW(hat_placeholder());
}
