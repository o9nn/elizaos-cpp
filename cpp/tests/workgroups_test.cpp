#include <gtest/gtest.h>

#include "elizaos/workgroups.hpp"

#include <algorithm>
#include <atomic>
#include <filesystem>
#include <fstream>
#include <future>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

using namespace elizaos;
namespace {

WorkgroupConfig config(std::string name = "Alpha",
                       std::string purpose = "deterministic collaboration") {
    WorkgroupConfig value;
    value.name = std::move(name);
    value.purpose = std::move(purpose);
    value.maxMembers = 128;
    value.taskTimeout = std::chrono::seconds(60);
    return value;
}

class MemoryAdapter final : public WorkgroupPersistenceAdapter {
public:
    bool save(const std::string& key, const std::string& payload) override {
        if (throwOnSave) throw std::runtime_error("save failure");
        {
            std::lock_guard<std::mutex> lock(mutex);
            data[key] = payload;
        }
        if (onSave) onSave();
        return acceptSave;
    }

    bool load(const std::string& key, std::string& payload) override {
        if (throwOnLoad) throw std::runtime_error("load failure");
        {
            std::lock_guard<std::mutex> lock(mutex);
            const auto entry = data.find(key);
            if (entry == data.end()) return false;
            payload = entry->second;
        }
        if (onLoad) onLoad();
        return true;
    }

    std::string get(const std::string& key) const {
        std::lock_guard<std::mutex> lock(mutex);
        const auto entry = data.find(key);
        return entry == data.end() ? std::string{} : entry->second;
    }

    void put(const std::string& key, std::string payload) {
        std::lock_guard<std::mutex> lock(mutex);
        data[key] = std::move(payload);
    }

    mutable std::mutex mutex;
    std::unordered_map<std::string, std::string> data;
    std::function<void()> onSave;
    std::function<void()> onLoad;
    bool acceptSave = true;
    bool throwOnSave = false;
    bool throwOnLoad = false;
};

std::filesystem::path temporaryDirectory() {
    static std::atomic<unsigned long long> counter{0};
    const auto path = std::filesystem::temp_directory_path() /
        ("elizaos_workgroups_" + std::to_string(counter.fetch_add(1)));
    std::filesystem::remove_all(path);
    return path;
}

} // namespace

TEST(WorkgroupData, ConstructorsAndUtilityConversions) {
    WorkgroupTask task;
    EXPECT_EQ(task.state, WorkgroupTaskState::PENDING);
    EXPECT_FALSE(task.completed);
    WorkgroupMember member;
    EXPECT_EQ(member.role, WorkgroupRole::MEMBER);
    EXPECT_TRUE(member.active);
    WorkgroupStats stats;
    EXPECT_EQ(stats.totalMembers, 0);

    EXPECT_EQ(workgroups_utils::workgroupRoleToString(WorkgroupRole::LEADER),
              "Leader");
    EXPECT_EQ(workgroups_utils::stringToWorkgroupRole("Coordinator"),
              WorkgroupRole::COORDINATOR);
    EXPECT_THROW(workgroups_utils::stringToWorkgroupRole("bogus"),
                 std::invalid_argument);
    EXPECT_EQ(workgroups_utils::workgroupStateToString(WorkgroupState::ACTIVE),
              "Active");
    EXPECT_THROW(workgroups_utils::stringToWorkgroupState("bogus"),
                 std::invalid_argument);
    EXPECT_EQ(workgroups_utils::workgroupTaskStateToString(
                  WorkgroupTaskState::CANCELLED),
              "Cancelled");
    EXPECT_THROW(workgroups_utils::stringToWorkgroupTaskState("bogus"),
                 std::invalid_argument);
}

TEST(WorkgroupValidation, ConstructorAndManagerRejectMalformedInput) {
    EXPECT_THROW(Workgroup("../bad", config(), "leader"), std::invalid_argument);
    EXPECT_THROW(Workgroup("wg_ok", config("", "purpose"), "leader"),
                 std::invalid_argument);
    WorkgroupManager manager(nullptr, nullptr);
    EXPECT_TRUE(manager.createWorkgroup(config(), "").empty());
    EXPECT_TRUE(manager.createWorkgroup(config("", "purpose"), "leader").empty());
    EXPECT_FALSE(manager.joinWorkgroup("../bad", "agent", WorkgroupRole::MEMBER));
    EXPECT_EQ(manager.getTotalWorkgroups(), 0);
}

TEST(WorkgroupLifecycle, DeterministicIdsSortedQueriesAndStates) {
    WorkgroupManager manager(nullptr, nullptr);
    const std::string first = manager.createWorkgroup(config("Zulu", "shared beta"),
                                                       "leader_z");
    const std::string second = manager.createWorkgroup(config("Alpha", "shared alpha"),
                                                        "leader_a");
    EXPECT_EQ(first, "wg_000000000000");
    EXPECT_EQ(second, "wg_000000000001");
    EXPECT_EQ(manager.getAllWorkgroupIds(),
              (std::vector<std::string>{first, second}));
    EXPECT_EQ(manager.findWorkgroupsByPurpose("shared"),
              (std::vector<std::string>{first, second}));
    EXPECT_TRUE(manager.findWorkgroupsByPurpose("").empty());

    auto group = manager.getWorkgroup(first);
    ASSERT_NE(group, nullptr);
    EXPECT_EQ(group->getId(), first);
    EXPECT_EQ(group->getName(), "Zulu");
    EXPECT_EQ(group->getPurpose(), "shared beta");
    EXPECT_EQ(group->getChannelId(), "workgroup_" + first);
    EXPECT_EQ(group->getState(), WorkgroupState::FORMING);
    EXPECT_FALSE(group->isActive());
    EXPECT_TRUE(group->transitionTo(WorkgroupState::ACTIVE));
    EXPECT_TRUE(group->isActive());
    EXPECT_EQ(manager.getActiveWorkgroups(), 1);
    EXPECT_FALSE(group->transitionTo(WorkgroupState::FORMING));
    EXPECT_TRUE(group->transitionTo(WorkgroupState::PAUSED));
    group->setState(WorkgroupState::ACTIVE);
    EXPECT_TRUE(group->isActive());
    EXPECT_FALSE(group->transitionTo(WorkgroupState::ACTIVE));
}

TEST(WorkgroupMembers, LeaderRolesDuplicatesCapacityAndLeaveInvariant) {
    WorkgroupConfig limited = config();
    limited.maxMembers = 3;
    WorkgroupManager manager(nullptr, nullptr);
    const auto id = manager.createWorkgroup(limited, "leader");
    auto group = manager.getWorkgroup(id);
    ASSERT_NE(group, nullptr);

    EXPECT_FALSE(group->addMember("second_leader", WorkgroupRole::LEADER));
    EXPECT_TRUE(manager.joinWorkgroup(id, "b", WorkgroupRole::MEMBER));
    EXPECT_TRUE(group->addMember("a", WorkgroupRole::COORDINATOR));
    EXPECT_FALSE(group->addMember("a", WorkgroupRole::MEMBER));
    EXPECT_FALSE(group->addMember("overflow", WorkgroupRole::MEMBER));
    EXPECT_EQ(group->getMembersByRole(WorkgroupRole::COORDINATOR),
              (std::vector<AgentId>{"a"}));
    const auto members = group->getMembers();
    ASSERT_EQ(members.size(), 3U);
    EXPECT_EQ(members[0].agentId, "a");
    EXPECT_EQ(members[1].agentId, "b");
    EXPECT_EQ(members[2].agentId, "leader");

    EXPECT_FALSE(manager.leaveWorkgroup(id, "leader"));
    EXPECT_TRUE(group->updateMemberRole("a", WorkgroupRole::LEADER));
    EXPECT_TRUE(manager.leaveWorkgroup(id, "leader"));
    EXPECT_EQ(manager.findWorkgroupsByMember("leader").size(), 0U);
    EXPECT_FALSE(group->updateMemberRole("a", WorkgroupRole::MEMBER));
    EXPECT_TRUE(group->isMember("a"));
    EXPECT_EQ(group->getMemberRole("missing"), WorkgroupRole::OBSERVER);
    EXPECT_FALSE(group->removeMember("missing"));
}

TEST(WorkgroupPermissions, RoleMatrixAndDissolution) {
    WorkgroupManager manager(nullptr, nullptr);
    const auto id = manager.createWorkgroup(config(), "leader");
    auto group = manager.getWorkgroup(id);
    ASSERT_TRUE(group->addMember("coord", WorkgroupRole::COORDINATOR));
    ASSERT_TRUE(group->addMember("member", WorkgroupRole::MEMBER));
    ASSERT_TRUE(group->addMember("observer", WorkgroupRole::OBSERVER));

    EXPECT_TRUE(group->hasPermission("leader", "manage_members"));
    EXPECT_TRUE(group->hasPermission("coord", "assign_tasks"));
    EXPECT_TRUE(group->hasPermission("member", "complete_tasks"));
    EXPECT_FALSE(group->hasPermission("observer", "send_messages"));
    EXPECT_TRUE(group->hasPermission("observer", "view"));
    EXPECT_FALSE(group->hasPermission("member", "unknown"));
    EXPECT_FALSE(manager.dissolveWorkgroup(id, "member"));
    EXPECT_TRUE(manager.dissolveWorkgroup(id, "leader"));
    EXPECT_EQ(group->getState(), WorkgroupState::DISSOLVED);
    EXPECT_EQ(manager.getWorkgroup(id), nullptr);
    EXPECT_TRUE(manager.getWorkgroupsForAgent("coord").empty());
    EXPECT_FALSE(group->addMember("late", WorkgroupRole::MEMBER));
    EXPECT_FALSE(group->transitionTo(WorkgroupState::ACTIVE));
    EXPECT_FALSE(group->hasPermission("leader", "view"));
}

TEST(WorkgroupTasks, ValidatesCreatorsAssigneesTransitionsAndDeepSnapshots) {
    Workgroup group("wg_tasks", config(), "leader");
    ASSERT_TRUE(group.addMember("coord", WorkgroupRole::COORDINATOR));
    ASSERT_TRUE(group.addMember("member", WorkgroupRole::MEMBER));
    ASSERT_TRUE(group.addMember("observer", WorkgroupRole::OBSERVER));
    EXPECT_TRUE(group.createTask("too early", "leader").empty());
    ASSERT_TRUE(group.transitionTo(WorkgroupState::ACTIVE));
    EXPECT_TRUE(group.createTask("", "leader").empty());
    EXPECT_TRUE(group.createTask("bad creator", "missing").empty());
    EXPECT_TRUE(group.createTask("observer creator", "observer").empty());
    EXPECT_TRUE(group.createTask("observer assigned", "leader", {"observer"}).empty());
    EXPECT_TRUE(group.createTask("duplicates", "leader", {"member", "member"}).empty());

    const auto first = group.createTask("first", "coord", {"member"});
    const auto second = group.createTask("second", "leader");
    EXPECT_EQ(first, "wg_tasks_task_000000000000");
    EXPECT_EQ(second, "wg_tasks_task_000000000001");
    EXPECT_FALSE(group.assignTask(first, "member"));
    EXPECT_TRUE(group.assignTask(second, "member"));
    EXPECT_FALSE(group.assignTask(second, "observer"));
    EXPECT_FALSE(group.assignTask("missing", "member"));

    auto snapshot = group.getTask(first);
    ASSERT_NE(snapshot, nullptr);
    snapshot->description = "tampered";
    EXPECT_EQ(group.getTask(first)->description, "first");
    const auto all = group.getAllTasks();
    ASSERT_EQ(all.size(), 2U);
    EXPECT_EQ(all[0]->taskId, first);
    EXPECT_EQ(all[1]->taskId, second);
    EXPECT_EQ(group.getTasksForAgent("member").size(), 2U);
    EXPECT_EQ(group.getPendingTasks().size(), 2U);

    EXPECT_FALSE(group.completeTask(first, ""));
    EXPECT_TRUE(group.completeTask(first, "done"));
    EXPECT_FALSE(group.completeTask(first, "again"));
    EXPECT_FALSE(group.cancelTask(first));
    EXPECT_TRUE(group.cancelTask(second));
    EXPECT_FALSE(group.cancelTask(second));
    EXPECT_EQ(group.getPendingTasks().size(), 0U);
    const auto stats = group.getStats();
    EXPECT_EQ(stats.totalTasks, 2);
    EXPECT_EQ(stats.completedTasks, 1);
    EXPECT_EQ(stats.pendingTasks, 0);
    EXPECT_EQ(group.getTask(second)->state, WorkgroupTaskState::CANCELLED);

    EXPECT_TRUE(group.transitionTo(WorkgroupState::COMPLETING));
    EXPECT_TRUE(group.transitionTo(WorkgroupState::DISSOLVED));
    EXPECT_TRUE(group.createTask("late", "leader").empty());
    EXPECT_FALSE(group.assignTask(first, "member"));
}

TEST(WorkgroupStates, CompletingRequiresNoPendingTasksBeforeDissolve) {
    Workgroup group("wg_state", config(), "leader");
    ASSERT_TRUE(group.transitionTo(WorkgroupState::ACTIVE));
    const auto task = group.createTask("pending", "leader");
    ASSERT_FALSE(task.empty());
    ASSERT_TRUE(group.transitionTo(WorkgroupState::COMPLETING));
    EXPECT_FALSE(group.transitionTo(WorkgroupState::DISSOLVED));
    EXPECT_TRUE(group.completeTask(task, "complete"));
    EXPECT_TRUE(group.transitionTo(WorkgroupState::DISSOLVED));
}

TEST(WorkgroupCommunication, StableRecipientsExceptionsAndReentrancy) {
    Workgroup group("wg_messages", config(), "leader");
    ASSERT_TRUE(group.addMember("b", WorkgroupRole::MEMBER));
    ASSERT_TRUE(group.addMember("a", WorkgroupRole::MEMBER));
    std::vector<std::string> recipients;
    std::mutex recipientsMutex;
    group.setMessageDispatcher(
        [&](const Message&, const AgentId& recipient, const std::string& channel) {
            EXPECT_EQ(channel, "workgroup_wg_messages");
            EXPECT_TRUE(group.isMember(recipient));
            std::lock_guard<std::mutex> lock(recipientsMutex);
            recipients.push_back(recipient);
            if (recipient == "a") throw std::runtime_error("expected");
        });
    Message message("m", MessageType::TEXT, "leader", "", "hello");
    group.broadcast(message);
    EXPECT_EQ(recipients,
              (std::vector<std::string>{"a", "b", "leader"}));
    recipients.clear();
    group.sendToRole(message, WorkgroupRole::MEMBER);
    EXPECT_EQ(recipients, (std::vector<std::string>{"a", "b"}));
    recipients.clear();
    group.sendToMember(message, "b");
    EXPECT_EQ(recipients, (std::vector<std::string>{"b"}));
    recipients.clear();
    group.sendToMember(message, "missing");
    EXPECT_TRUE(recipients.empty());
}

TEST(WorkgroupPersistence, NoAdapterRejectsEveryOperation) {
    WorkgroupManager manager(nullptr, nullptr);
    const auto id = manager.createWorkgroup(config(), "leader");
    EXPECT_FALSE(manager.saveWorkgroupState(id));
    EXPECT_FALSE(manager.loadWorkgroupState(id));
    EXPECT_FALSE(manager.saveAllWorkgroups());
    EXPECT_FALSE(manager.loadAllWorkgroups());
}

TEST(WorkgroupPersistence, SingleAndAllRoundTripRestoreIndicesAndCounters) {
    auto adapter = std::make_shared<MemoryAdapter>();
    WorkgroupManager source(nullptr, nullptr, adapter);
    const auto first = source.createWorkgroup(config("First", "round trip"), "lead1");
    const auto second = source.createWorkgroup(config("Second", "round trip"), "lead2");
    auto firstGroup = source.getWorkgroup(first);
    ASSERT_TRUE(firstGroup->addMember("member", WorkgroupRole::MEMBER));
    ASSERT_TRUE(firstGroup->transitionTo(WorkgroupState::ACTIVE));
    const auto task = firstGroup->createTask("persist me", "lead1", {"member"});
    ASSERT_TRUE(firstGroup->completeTask(task, "saved result"));
    ASSERT_TRUE(source.saveWorkgroupState(first));

    WorkgroupManager single(nullptr, nullptr, adapter);
    ASSERT_TRUE(single.loadWorkgroupState(first));
    auto restoredSingle = single.getWorkgroup(first);
    ASSERT_NE(restoredSingle, nullptr);
    EXPECT_EQ(restoredSingle->getTask(task)->result, "saved result");
    EXPECT_EQ(single.findWorkgroupsByMember("member"),
              (std::vector<std::string>{first}));
    EXPECT_EQ(single.createWorkgroup(config(), "newlead"),
              "wg_000000000001");

    ASSERT_TRUE(source.saveAllWorkgroups());
    const auto deterministic = adapter->get("all_workgroups");
    ASSERT_TRUE(source.saveAllWorkgroups());
    EXPECT_EQ(adapter->get("all_workgroups"), deterministic);

    WorkgroupManager restored(nullptr, nullptr, adapter);
    ASSERT_TRUE(restored.loadAllWorkgroups());
    EXPECT_EQ(restored.getAllWorkgroupIds(),
              (std::vector<std::string>{first, second}));
    EXPECT_EQ(restored.getWorkgroupsForAgent("member"),
              (std::vector<std::string>{first}));
    EXPECT_EQ(restored.createWorkgroup(config(), "thirdlead"),
              "wg_000000000002");
    ASSERT_TRUE(restored.loadAllWorkgroups());
    EXPECT_EQ(restored.getTotalWorkgroups(), 2);
}

TEST(WorkgroupPersistence, MalformedLoadRollsBackAndExceptionsFail) {
    auto adapter = std::make_shared<MemoryAdapter>();
    WorkgroupManager manager(nullptr, nullptr, adapter);
    const auto id = manager.createWorkgroup(config(), "leader");
    ASSERT_TRUE(manager.saveAllWorkgroups());
    const auto good = adapter->get("all_workgroups");
    adapter->put("all_workgroups", "{bad json");
    EXPECT_FALSE(manager.loadAllWorkgroups());
    EXPECT_EQ(manager.getAllWorkgroupIds(), (std::vector<std::string>{id}));
    adapter->put("all_workgroups", good.substr(0, good.size() / 2U));
    EXPECT_FALSE(manager.loadAllWorkgroups());
    EXPECT_EQ(manager.getAllWorkgroupIds(), (std::vector<std::string>{id}));
    adapter->put(id, "{\"version\":999}");
    EXPECT_FALSE(manager.loadWorkgroupState(id));
    EXPECT_EQ(manager.getWorkgroup(id)->getName(), "Alpha");
    adapter->throwOnSave = true;
    EXPECT_FALSE(manager.saveAllWorkgroups());
    adapter->throwOnSave = false;
    adapter->throwOnLoad = true;
    EXPECT_FALSE(manager.loadAllWorkgroups());
}

TEST(WorkgroupPersistence, AdapterCallbacksRunOutsideLocksAndMayReenter) {
    auto adapter = std::make_shared<MemoryAdapter>();
    WorkgroupManager manager(nullptr, nullptr, adapter);
    const auto id = manager.createWorkgroup(config(), "leader");
    adapter->onSave = [&] { EXPECT_EQ(manager.getTotalWorkgroups(), 1); };
    EXPECT_TRUE(manager.saveWorkgroupState(id));
    adapter->onLoad = [&] { EXPECT_EQ(manager.getAllWorkgroupIds().size(), 1U); };
    EXPECT_TRUE(manager.loadWorkgroupState(id));
}

TEST(LocalJsonWorkgroupPersistenceAdapter, AtomicPathSafeRoundTrip) {
    const auto directory = temporaryDirectory();
    LocalJsonWorkgroupPersistenceAdapter adapter(directory.string());
    EXPECT_TRUE(adapter.save("safe_key", "{\"value\":1}"));
    std::string value;
    EXPECT_TRUE(adapter.load("safe_key", value));
    EXPECT_EQ(value, "{\"value\":1}");
    EXPECT_FALSE(adapter.save("../escape", "x"));
    EXPECT_FALSE(adapter.load("../escape", value));
    EXPECT_FALSE(adapter.save("safe_key", ""));
    EXPECT_FALSE(std::filesystem::exists(directory / "../escape.json"));
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        EXPECT_EQ(entry.path().extension(), ".json");
    }
    std::filesystem::remove_all(directory);
}

TEST(WorkgroupConcurrency, JoinLeaveTaskAndQueryStress) {
    WorkgroupManager manager(nullptr, nullptr);
    const auto id = manager.createWorkgroup(config(), "leader");
    auto group = manager.getWorkgroup(id);
    ASSERT_TRUE(group->transitionTo(WorkgroupState::ACTIVE));
    constexpr int kAgents = 24;
    std::vector<std::future<void>> futures;
    for (int i = 0; i < kAgents; ++i) {
        futures.push_back(std::async(std::launch::async, [&, i] {
            const auto agent = "agent_" + std::to_string(i);
            if (manager.joinWorkgroup(id, agent, WorkgroupRole::MEMBER)) {
                const auto task = group->createTask("task " + std::to_string(i),
                                                    "leader", {agent});
                if (!task.empty() && (i % 2 == 0)) {
                    EXPECT_TRUE(group->completeTask(task, "done"));
                }
                static_cast<void>(group->getTask(task));
                static_cast<void>(group->getAllTasks());
                static_cast<void>(group->getPendingTasks());
                static_cast<void>(group->getMembers());
                static_cast<void>(manager.findWorkgroupsByPurpose("collaboration"));
                static_cast<void>(manager.getActiveWorkgroups());
                EXPECT_TRUE(manager.leaveWorkgroup(id, agent));
            }
        }));
    }
    for (auto& future : futures) {
        EXPECT_EQ(future.wait_for(std::chrono::seconds(10)),
                  std::future_status::ready);
        future.get();
    }
    EXPECT_EQ(group->getMembers().size(), 1U);
    EXPECT_EQ(manager.getWorkgroupsForAgent("agent_0").size(), 0U);
    for (const auto& task : group->getAllTasks()) {
        EXPECT_TRUE(task->assignedTo.empty());
    }
}

TEST(WorkgroupValidation, RejectsIdsThatCannotProduceValidTaskIds) {
    WorkgroupManager manager(nullptr, nullptr);
    EXPECT_EQ(manager.getWorkgroup("../bad"), nullptr);
    const std::string oversized(97U, 'a');
    EXPECT_THROW(Workgroup(oversized, config(), "leader"), std::invalid_argument);
}

TEST(WorkgroupPersistence, SetterEnablesPersistenceAndRejectsInvalidJsonWrites) {
    WorkgroupManager manager(nullptr, nullptr);
    const auto id = manager.createWorkgroup(config(), "leader");
    auto adapter = std::make_shared<MemoryAdapter>();
    manager.setPersistenceAdapter(adapter);
    EXPECT_TRUE(manager.saveWorkgroupState(id));

    const auto directory = temporaryDirectory();
    LocalJsonWorkgroupPersistenceAdapter fileAdapter(directory.string());
    EXPECT_FALSE(fileAdapter.save("bad", "not json"));
    EXPECT_FALSE(fileAdapter.save("bad", "[]"));
    EXPECT_FALSE(fileAdapter.save("bad", ""));
    std::filesystem::remove_all(directory);
}

TEST(WorkgroupConcurrency, ConcurrentSaveMutationQueryAndDissolve) {
    auto adapter = std::make_shared<MemoryAdapter>();
    WorkgroupManager manager(nullptr, nullptr, adapter);
    const auto id = manager.createWorkgroup(config(), "leader");
    auto group = manager.getWorkgroup(id);
    ASSERT_TRUE(group->transitionTo(WorkgroupState::ACTIVE));
    std::atomic<bool> start{false};
    auto mutator = std::async(std::launch::async, [&] {
        while (!start.load()) std::this_thread::yield();
        for (int i = 0; i < 100; ++i) {
            const auto agent = "save_agent_" + std::to_string(i);
            if (manager.joinWorkgroup(id, agent, WorkgroupRole::MEMBER)) {
                const auto task = group->createTask("save task", "leader", {agent});
                if (!task.empty()) static_cast<void>(group->completeTask(task, "done"));
                static_cast<void>(manager.leaveWorkgroup(id, agent));
            }
        }
    });
    auto saver = std::async(std::launch::async, [&] {
        while (!start.load()) std::this_thread::yield();
        for (int i = 0; i < 100; ++i) {
            EXPECT_TRUE(manager.saveAllWorkgroups());
            static_cast<void>(manager.getAllWorkgroupIds());
            static_cast<void>(manager.findWorkgroupsByMember("leader"));
        }
    });
    start.store(true);
    EXPECT_EQ(mutator.wait_for(std::chrono::seconds(10)), std::future_status::ready);
    EXPECT_EQ(saver.wait_for(std::chrono::seconds(10)), std::future_status::ready);
    mutator.get();
    saver.get();
    EXPECT_TRUE(manager.dissolveWorkgroup(id, "leader"));
    EXPECT_FALSE(manager.joinWorkgroup(id, "late", WorkgroupRole::MEMBER));
    EXPECT_FALSE(manager.leaveWorkgroup(id, "leader"));
    EXPECT_TRUE(manager.findWorkgroupsByMember("leader").empty());
}

TEST(WorkgroupLifecycle, RepeatedConstructUseAndDestroy) {
    for (int iteration = 0; iteration < 200; ++iteration) {
        WorkgroupManager manager(nullptr, nullptr);
        const auto id = manager.createWorkgroup(config(), "leader");
        auto group = manager.getWorkgroup(id);
        ASSERT_NE(group, nullptr);
        ASSERT_TRUE(group->transitionTo(WorkgroupState::ACTIVE));
        const auto task = group->createTask("short lifecycle", "leader");
        ASSERT_FALSE(task.empty());
        ASSERT_TRUE(group->completeTask(task, "done"));
        ASSERT_TRUE(manager.dissolveWorkgroup(id, "leader"));
    }
}
