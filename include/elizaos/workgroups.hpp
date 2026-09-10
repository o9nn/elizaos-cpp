#pragma once

#include "elizaos/agentagenda.hpp"
#include "elizaos/agentcomms.hpp"
#include "elizaos/agentmemory.hpp"
#include "elizaos/core.hpp"

#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

class AgentMemoryManager;
class CommunicationHub;
class AgentAgenda;

enum class WorkgroupRole { LEADER, COORDINATOR, MEMBER, OBSERVER };
enum class WorkgroupState { FORMING, ACTIVE, PAUSED, COMPLETING, DISSOLVED };
enum class WorkgroupTaskState { PENDING, COMPLETED, CANCELLED };

struct WorkgroupTask {
    std::string taskId;
    std::string description;
    std::string createdBy;
    std::vector<AgentId> assignedTo;
    std::unordered_map<std::string, std::string> metadata;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point dueDate;
    std::chrono::system_clock::time_point completedAt;
    WorkgroupTaskState state;
    bool completed;
    std::string result;

    WorkgroupTask();
    WorkgroupTask(const std::string& id,
                  const std::string& desc,
                  const std::string& creator);
};

struct WorkgroupMember {
    AgentId agentId;
    WorkgroupRole role;
    std::chrono::system_clock::time_point joinedAt;
    std::vector<std::string> assignedTaskIds;
    int tasksCompleted;
    bool active;

    WorkgroupMember();
    WorkgroupMember(const AgentId& id, WorkgroupRole role);
};

struct WorkgroupStats {
    int totalMembers = 0;
    int activeMembers = 0;
    int totalTasks = 0;
    int completedTasks = 0;
    int pendingTasks = 0;
    std::chrono::system_clock::time_point createdAt{};
    std::chrono::milliseconds averageTaskCompletionTime{0};
};

struct WorkgroupConfig {
    std::string name;
    std::string purpose;
    int maxMembers = 50;
    bool allowSelfJoin = false;
    bool requireApproval = true;
    bool persistState = true;
    std::chrono::seconds taskTimeout = std::chrono::seconds(3600);
};

class WorkgroupPersistenceAdapter {
public:
    virtual ~WorkgroupPersistenceAdapter() = default;
    virtual bool save(const std::string& key, const std::string& payload) = 0;
    virtual bool load(const std::string& key, std::string& payload) = 0;
};

class LocalJsonWorkgroupPersistenceAdapter final
    : public WorkgroupPersistenceAdapter {
public:
    explicit LocalJsonWorkgroupPersistenceAdapter(std::string rootDirectory);
    bool save(const std::string& key, const std::string& payload) override;
    bool load(const std::string& key, std::string& payload) override;

private:
    std::string rootDirectory_;
    mutable std::mutex mutex_;
};

using WorkgroupMessageDispatcher =
    std::function<void(const Message&, const AgentId&, const std::string&)>;

class Workgroup {
public:
    Workgroup(const std::string& id,
              const WorkgroupConfig& config,
              const AgentId& creator);
    ~Workgroup() = default;

    Workgroup(const Workgroup&) = delete;
    Workgroup& operator=(const Workgroup&) = delete;

    bool addMember(const AgentId& agentId, WorkgroupRole role);
    bool removeMember(const AgentId& agentId);
    bool updateMemberRole(const AgentId& agentId, WorkgroupRole newRole);
    bool isMember(const AgentId& agentId) const;
    WorkgroupRole getMemberRole(const AgentId& agentId) const;
    std::vector<WorkgroupMember> getMembers() const;
    std::vector<AgentId> getMembersByRole(WorkgroupRole role) const;

    std::string createTask(const std::string& description,
                           const AgentId& creator,
                           const std::vector<AgentId>& assignees = {});
    bool assignTask(const std::string& taskId, const AgentId& agentId);
    bool completeTask(const std::string& taskId, const std::string& result);
    bool cancelTask(const std::string& taskId);
    std::shared_ptr<WorkgroupTask> getTask(const std::string& taskId) const;
    std::vector<std::shared_ptr<WorkgroupTask>> getAllTasks() const;
    std::vector<std::shared_ptr<WorkgroupTask>> getTasksForAgent(
        const AgentId& agentId) const;
    std::vector<std::shared_ptr<WorkgroupTask>> getPendingTasks() const;

    void setMessageDispatcher(WorkgroupMessageDispatcher dispatcher);
    void broadcast(const Message& message);
    void sendToRole(const Message& message, WorkgroupRole role);
    void sendToMember(const Message& message, const AgentId& agentId);
    std::string getChannelId() const;

    void setState(WorkgroupState newState);
    bool transitionTo(WorkgroupState newState);
    WorkgroupState getState() const;
    bool isActive() const;

    std::string getId() const;
    std::string getName() const;
    std::string getPurpose() const;
    WorkgroupStats getStats() const;
    bool hasPermission(const AgentId& agentId,
                       const std::string& action) const;

private:
    friend class WorkgroupManager;

    std::string id_;
    WorkgroupConfig config_;
    AgentId creator_;
    WorkgroupState state_;
    std::string channelId_;
    std::unordered_map<AgentId, WorkgroupMember> members_;
    std::unordered_map<std::string, WorkgroupTask> tasks_;
    std::chrono::system_clock::time_point createdAt_;
    std::uint64_t taskCounter_;
    WorkgroupMessageDispatcher messageDispatcher_;
    mutable std::mutex mutex_;

    bool hasPermissionUnlocked(const AgentId& agentId,
                               const std::string& action) const;
    bool transitionAllowedUnlocked(WorkgroupState newState) const;
    std::string generateTaskIdUnlocked();
    std::string serializeSnapshot() const;
    std::string serializeSnapshotUnlocked() const;
    static std::shared_ptr<Workgroup> deserializeSnapshot(
        const std::string& payload);
};

class WorkgroupManager {
public:
    WorkgroupManager(
        std::shared_ptr<CommunicationHub> commHub,
        std::shared_ptr<AgentMemoryManager> memoryMgr,
        std::shared_ptr<WorkgroupPersistenceAdapter> persistenceAdapter = nullptr);
    ~WorkgroupManager() = default;

    WorkgroupManager(const WorkgroupManager&) = delete;
    WorkgroupManager& operator=(const WorkgroupManager&) = delete;

    std::string createWorkgroup(const WorkgroupConfig& config,
                                const AgentId& creator);
    bool dissolveWorkgroup(const std::string& workgroupId,
                           const AgentId& requestor);
    std::shared_ptr<Workgroup> getWorkgroup(
        const std::string& workgroupId) const;
    std::vector<std::string> getAllWorkgroupIds() const;
    std::vector<std::string> getWorkgroupsForAgent(
        const AgentId& agentId) const;

    bool joinWorkgroup(const std::string& workgroupId,
                       const AgentId& agentId,
                       WorkgroupRole role);
    bool leaveWorkgroup(const std::string& workgroupId,
                        const AgentId& agentId);

    std::vector<std::string> findWorkgroupsByPurpose(
        const std::string& purposeKeyword) const;
    std::vector<std::string> findWorkgroupsByMember(
        const AgentId& agentId) const;

    int getTotalWorkgroups() const;
    int getActiveWorkgroups() const;

    void setPersistenceAdapter(
        std::shared_ptr<WorkgroupPersistenceAdapter> persistenceAdapter);
    bool saveWorkgroupState(const std::string& workgroupId);
    bool loadWorkgroupState(const std::string& workgroupId);
    bool saveAllWorkgroups();
    bool loadAllWorkgroups();

private:
    std::shared_ptr<CommunicationHub> commHub_;
    std::shared_ptr<AgentMemoryManager> memoryMgr_;
    std::shared_ptr<WorkgroupPersistenceAdapter> persistenceAdapter_;
    std::unordered_map<std::string, std::shared_ptr<Workgroup>> workgroups_;
    std::unordered_map<AgentId, std::unordered_set<std::string>> agentWorkgroups_;
    mutable std::mutex mutex_;
    std::uint64_t workgroupCounter_;

    std::string generateWorkgroupIdUnlocked();
    void indexWorkgroupUnlocked(const std::string& workgroupId,
                                const AgentId& agentId);
    void unindexWorkgroupUnlocked(const std::string& workgroupId,
                                  const AgentId& agentId);
    void rebuildIndexUnlocked();
};

namespace workgroups_utils {
std::string workgroupRoleToString(WorkgroupRole role);
WorkgroupRole stringToWorkgroupRole(const std::string& roleStr);
std::string workgroupStateToString(WorkgroupState state);
WorkgroupState stringToWorkgroupState(const std::string& stateStr);
std::string workgroupTaskStateToString(WorkgroupTaskState state);
WorkgroupTaskState stringToWorkgroupTaskState(const std::string& stateStr);
} // namespace workgroups_utils

} // namespace elizaos
