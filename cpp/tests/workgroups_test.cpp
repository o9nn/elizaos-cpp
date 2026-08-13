// workgroups_test.cpp - End-to-end tests for Workgroup and WorkgroupManager.
#include <gtest/gtest.h>

#include "elizaos/workgroups.hpp"

#include <algorithm>
#include <chrono>
#include <memory>
#include <set>
#include <string>
#include <vector>

using namespace elizaos;

namespace {

WorkgroupConfig makeConfig(const std::string& name = "Autonomy Guild") {
    WorkgroupConfig config;
    config.name = name;
    config.purpose = "Repair and validate autonomous cognitive systems";
    config.maxMembers = 8;
    config.allowSelfJoin = true;
    config.requireApproval = false;
    config.persistState = true;
    config.taskTimeout = std::chrono::seconds(7200);
    return config;
}

bool containsMember(const std::vector<WorkgroupMember>& members, const AgentId& id) {
    return std::any_of(members.begin(), members.end(), [&](const auto& member) {
        return member.agentId == id;
    });
}

}  // namespace

TEST(WorkgroupConfig, DefaultsAreOperationalAndBounded) {
    WorkgroupConfig config;
    EXPECT_EQ(config.maxMembers, 50);
    EXPECT_FALSE(config.allowSelfJoin);
    EXPECT_TRUE(config.requireApproval);
    EXPECT_TRUE(config.persistState);
    EXPECT_GT(config.taskTimeout.count(), 0);
}

TEST(WorkgroupValueTypes, ConstructorsInitializeSafeDefaults) {
    WorkgroupTask task;
    EXPECT_TRUE(task.taskId.empty());
    EXPECT_FALSE(task.completed);
    EXPECT_TRUE(task.result.empty());

    WorkgroupMember member;
    EXPECT_TRUE(member.agentId.empty());
    EXPECT_EQ(member.role, WorkgroupRole::MEMBER);
    EXPECT_EQ(member.tasksCompleted, 0);
    EXPECT_TRUE(member.active);
}

TEST(WorkgroupEnums, RolesAndStatesHaveStableDistinctValues) {
    EXPECT_NE(static_cast<int>(WorkgroupRole::LEADER),
              static_cast<int>(WorkgroupRole::COORDINATOR));
    EXPECT_NE(static_cast<int>(WorkgroupRole::MEMBER),
              static_cast<int>(WorkgroupRole::OBSERVER));
    EXPECT_NE(static_cast<int>(WorkgroupState::FORMING),
              static_cast<int>(WorkgroupState::ACTIVE));
    EXPECT_NE(static_cast<int>(WorkgroupState::COMPLETING),
              static_cast<int>(WorkgroupState::DISSOLVED));
}

TEST(Workgroup, MemberRolesPermissionsAndLeaderInvariantAreEnforced) {
    Workgroup group("wg-test", makeConfig(), "leader");
    EXPECT_TRUE(group.isMember("leader"));
    EXPECT_EQ(group.getMemberRole("leader"), WorkgroupRole::LEADER);
    EXPECT_FALSE(group.removeMember("leader"));
    EXPECT_TRUE(group.hasPermission("leader", "manage_members"));

    ASSERT_TRUE(group.addMember("coordinator", WorkgroupRole::COORDINATOR));
    ASSERT_TRUE(group.addMember("member", WorkgroupRole::MEMBER));
    ASSERT_TRUE(group.addMember("observer", WorkgroupRole::OBSERVER));
    EXPECT_FALSE(group.addMember("member", WorkgroupRole::MEMBER));
    EXPECT_TRUE(group.hasPermission("coordinator", "assign_tasks"));
    EXPECT_FALSE(group.hasPermission("member", "assign_tasks"));
    EXPECT_TRUE(group.hasPermission("member", "complete_tasks"));
    EXPECT_FALSE(group.hasPermission("observer", "complete_tasks"));

    ASSERT_TRUE(group.updateMemberRole("coordinator", WorkgroupRole::LEADER));
    EXPECT_TRUE(group.removeMember("leader"));
    EXPECT_FALSE(group.isMember("leader"));
    EXPECT_EQ(group.getMembersByRole(WorkgroupRole::LEADER),
              (std::vector<AgentId>{"coordinator"}));
}

TEST(Workgroup, TasksTrackAssignmentsCompletionCancellationAndStatistics) {
    Workgroup group("wg-tasks", makeConfig(), "leader");
    ASSERT_TRUE(group.addMember("agent-a", WorkgroupRole::MEMBER));
    ASSERT_TRUE(group.addMember("agent-b", WorkgroupRole::MEMBER));

    const std::string taskId = group.createTask(
        "Repair persistence", "leader", {"agent-a"});
    ASSERT_FALSE(taskId.empty());
    ASSERT_TRUE(group.assignTask(taskId, "agent-b"));
    EXPECT_FALSE(group.assignTask(taskId, "agent-b"));
    ASSERT_EQ(group.getTasksForAgent("agent-a").size(), 1u);
    ASSERT_EQ(group.getTasksForAgent("agent-b").size(), 1u);
    EXPECT_EQ(group.getPendingTasks().size(), 1u);

    ASSERT_TRUE(group.completeTask(taskId, "all tests green"));
    EXPECT_FALSE(group.completeTask(taskId, "duplicate"));
    const auto task = group.getTask(taskId);
    ASSERT_NE(task, nullptr);
    EXPECT_TRUE(task->completed);
    EXPECT_EQ(task->result, "all tests green");
    EXPECT_TRUE(group.getPendingTasks().empty());

    auto members = group.getMembers();
    const auto findMember = [&](const AgentId& id) {
        return std::find_if(members.begin(), members.end(), [&](const auto& member) {
            return member.agentId == id;
        });
    };
    ASSERT_NE(findMember("agent-a"), members.end());
    ASSERT_NE(findMember("agent-b"), members.end());
    EXPECT_EQ(findMember("agent-a")->tasksCompleted, 1);
    EXPECT_EQ(findMember("agent-b")->tasksCompleted, 1);

    const WorkgroupStats stats = group.getStats();
    EXPECT_EQ(stats.totalMembers, 3);
    EXPECT_EQ(stats.totalTasks, 1);
    EXPECT_EQ(stats.completedTasks, 1);
    EXPECT_EQ(stats.pendingTasks, 0);

    EXPECT_TRUE(group.cancelTask(taskId));
    EXPECT_FALSE(group.cancelTask(taskId));
    EXPECT_EQ(group.getStats().totalTasks, 0);
}

TEST(WorkgroupManager, PersistenceRoundTripRestoresCompleteStateAndIndexes) {
    auto memory = std::make_shared<AgentMemoryManager>();
    WorkgroupManager manager(nullptr, memory);
    const std::string id = manager.createWorkgroup(makeConfig(), "leader");
    ASSERT_TRUE(manager.joinWorkgroup(id, "coordinator", WorkgroupRole::COORDINATOR));
    ASSERT_TRUE(manager.joinWorkgroup(id, "worker", WorkgroupRole::MEMBER));

    auto group = manager.getWorkgroup(id);
    ASSERT_NE(group, nullptr);
    group->setState(WorkgroupState::ACTIVE);
    const std::string taskId = group->createTask(
        "Run dual-fork validation", "leader", {"coordinator", "worker"});
    auto task = group->getTask(taskId);
    ASSERT_NE(task, nullptr);
    task->metadata = {{"priority", "critical"}, {"center", "autonomy"}};
    task->dueDate = std::chrono::system_clock::time_point(std::chrono::milliseconds(987654));
    ASSERT_TRUE(group->completeTask(taskId, "validated 109 tests"));

    ASSERT_TRUE(manager.saveWorkgroupState(id));
    const auto snapshots = memory->getAllMemoriesFromTable("workgroup_state");
    ASSERT_EQ(snapshots.size(), 1u);

    WorkgroupManager restored(nullptr, memory);
    ASSERT_TRUE(restored.loadWorkgroupState(id));
    EXPECT_EQ(restored.getTotalWorkgroups(), 1);
    EXPECT_EQ(restored.getActiveWorkgroups(), 1);
    EXPECT_EQ(restored.getWorkgroupsForAgent("leader"), (std::vector<std::string>{id}));
    EXPECT_EQ(restored.getWorkgroupsForAgent("worker"), (std::vector<std::string>{id}));

    auto restoredGroup = restored.getWorkgroup(id);
    ASSERT_NE(restoredGroup, nullptr);
    EXPECT_EQ(restoredGroup->getName(), "Autonomy Guild");
    EXPECT_EQ(restoredGroup->getPurpose(), makeConfig().purpose);
    EXPECT_EQ(restoredGroup->getState(), WorkgroupState::ACTIVE);
    EXPECT_TRUE(restoredGroup->hasPermission("coordinator", "assign_tasks"));
    EXPECT_TRUE(containsMember(restoredGroup->getMembers(), "worker"));

    const auto restoredTask = restoredGroup->getTask(taskId);
    ASSERT_NE(restoredTask, nullptr);
    EXPECT_EQ(restoredTask->metadata, task->metadata);
    EXPECT_EQ(restoredTask->dueDate, task->dueDate);
    EXPECT_TRUE(restoredTask->completed);
    EXPECT_EQ(restoredTask->result, task->result);
    EXPECT_EQ(restoredGroup->getStats().completedTasks, 1);
}

TEST(WorkgroupManager, RepeatedSaveReplacesSnapshotAndLoadRevertsLaterMutation) {
    auto memory = std::make_shared<AgentMemoryManager>();
    WorkgroupManager manager(nullptr, memory);
    const std::string id = manager.createWorkgroup(makeConfig(), "leader");
    auto group = manager.getWorkgroup(id);
    ASSERT_NE(group, nullptr);
    group->setState(WorkgroupState::ACTIVE);
    ASSERT_TRUE(manager.saveWorkgroupState(id));
    const UUID firstSnapshot = memory->getAllMemoriesFromTable("workgroup_state").front()->getId();

    ASSERT_TRUE(manager.joinWorkgroup(id, "persisted-member", WorkgroupRole::MEMBER));
    ASSERT_TRUE(manager.saveWorkgroupState(id));
    auto snapshots = memory->getAllMemoriesFromTable("workgroup_state");
    ASSERT_EQ(snapshots.size(), 1u);
    EXPECT_NE(snapshots.front()->getId(), firstSnapshot);

    ASSERT_TRUE(manager.joinWorkgroup(id, "transient-member", WorkgroupRole::OBSERVER));
    group->setState(WorkgroupState::PAUSED);
    ASSERT_TRUE(manager.loadWorkgroupState(id));
    auto restored = manager.getWorkgroup(id);
    ASSERT_NE(restored, nullptr);
    EXPECT_TRUE(restored->isMember("persisted-member"));
    EXPECT_FALSE(restored->isMember("transient-member"));
    EXPECT_EQ(restored->getState(), WorkgroupState::ACTIVE);
}

TEST(WorkgroupManager, SaveAndLoadAllAreTransactionalAndRecoverIdCounter) {
    auto memory = std::make_shared<AgentMemoryManager>();
    WorkgroupManager source(nullptr, memory);
    const std::string first = source.createWorkgroup(makeConfig("First"), "leader-a");
    const std::string second = source.createWorkgroup(makeConfig("Second"), "leader-b");
    ASSERT_TRUE(source.joinWorkgroup(second, "shared-agent", WorkgroupRole::MEMBER));
    ASSERT_TRUE(source.saveAllWorkgroups());
    EXPECT_EQ(memory->getAllMemoriesFromTable("workgroup_state").size(), 2u);

    WorkgroupManager restored(nullptr, memory);
    ASSERT_TRUE(restored.loadAllWorkgroups());
    EXPECT_EQ(restored.getTotalWorkgroups(), 2);
    const auto restoredIds = restored.getAllWorkgroupIds();
    EXPECT_EQ(std::set<std::string>(restoredIds.begin(), restoredIds.end()),
              (std::set<std::string>{first, second}));
    EXPECT_EQ(restored.getWorkgroupsForAgent("shared-agent"),
              (std::vector<std::string>{second}));

    const std::string next = restored.createWorkgroup(makeConfig("Third"), "leader-c");
    EXPECT_NE(next, first);
    EXPECT_NE(next, second);
    EXPECT_EQ(next, "wg_000002");
}

TEST(WorkgroupManager, CorruptLoadAllRollsBackAndInvalidStorageFailsTruthfully) {
    auto memory = std::make_shared<AgentMemoryManager>();
    WorkgroupManager source(nullptr, memory);
    source.createWorkgroup(makeConfig("Persisted"), "persisted-leader");
    ASSERT_TRUE(source.saveAllWorkgroups());
    auto snapshots = memory->getAllMemoriesFromTable("workgroup_state");
    ASSERT_EQ(snapshots.size(), 1u);
    snapshots.front()->setContent("{corrupted");

    WorkgroupManager live(nullptr, memory);
    const std::string liveId = live.createWorkgroup(makeConfig("Live"), "live-leader");
    EXPECT_FALSE(live.loadAllWorkgroups());
    EXPECT_EQ(live.getTotalWorkgroups(), 1);
    EXPECT_NE(live.getWorkgroup(liveId), nullptr);
    EXPECT_TRUE(live.getWorkgroup(liveId)->isMember("live-leader"));

    WorkgroupManager withoutMemory(nullptr, nullptr);
    EXPECT_FALSE(withoutMemory.saveWorkgroupState("missing"));
    EXPECT_FALSE(withoutMemory.loadWorkgroupState("missing"));
    EXPECT_FALSE(withoutMemory.saveAllWorkgroups());
    EXPECT_FALSE(withoutMemory.loadAllWorkgroups());

    auto emptyMemory = std::make_shared<AgentMemoryManager>();
    WorkgroupManager empty(nullptr, emptyMemory);
    EXPECT_FALSE(empty.loadAllWorkgroups());
    EXPECT_FALSE(empty.loadWorkgroupState("missing"));
}

TEST(WorkgroupManager, NonPersistentGroupsAreIntentionallySkipped) {
    auto memory = std::make_shared<AgentMemoryManager>();
    WorkgroupManager manager(nullptr, memory);
    auto config = makeConfig("Ephemeral");
    config.persistState = false;
    const std::string id = manager.createWorkgroup(config, "leader");
    EXPECT_FALSE(manager.saveWorkgroupState(id));
    EXPECT_TRUE(manager.saveAllWorkgroups());
    EXPECT_TRUE(memory->getAllMemoriesFromTable("workgroup_state").empty());
}
