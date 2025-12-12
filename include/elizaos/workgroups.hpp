#pragma once

#include "elizaos/agentcomms.hpp"
#include "elizaos/agentmemory.hpp"
#include "elizaos/agentagenda.hpp"
#include "elizaos/core.hpp"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <chrono>
#include <functional>

namespace elizaos {

/**
 * @brief Workgroups - Multi-Agent Collaboration System
 * 
 * Enables groups of agents to collaborate on shared tasks with role-based
 * permissions, communication channels, and coordinated task distribution.
 */

// Forward declarations
class AgentMemoryManager;
class CommunicationHub;
class AgentAgenda;

/**
 * Agent role within a workgroup
 */
enum class WorkgroupRole {
    LEADER,      // Can manage members, assign tasks, dissolve group
    COORDINATOR, // Can assign tasks and manage communication
    MEMBER,      // Can participate and complete tasks
    OBSERVER     // Read-only access to workgroup activities
};

/**
 * Workgroup state
 */
enum class WorkgroupState {
    FORMING,     // Initial creation, adding members
    ACTIVE,      // Actively working on tasks
    PAUSED,      // Temporarily suspended
    COMPLETING,  // Finishing remaining tasks
    DISSOLVED    // Workgroup ended
};

/**
 * Task assignment within a workgroup
 */
struct WorkgroupTask {
    std::string taskId;
    std::string description;
    std::string createdBy;
    std::vector<AgentId> assignedTo;
    std::unordered_map<std::string, std::string> metadata;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point dueDate;
    bool completed;
    std::string result;
    
    WorkgroupTask() : WorkgroupTask("", "", "") {}
    
    WorkgroupTask(const std::string& id, const std::string& desc, const std::string& creator)
        : taskId(id), description(desc), createdBy(creator), 
          createdAt(std::chrono::system_clock::now()), completed(false) {}
};

/**
 * Member information within a workgroup
 */
struct WorkgroupMember {
    AgentId agentId;
    WorkgroupRole role;
    std::chrono::system_clock::time_point joinedAt;
    std::vector<std::string> assignedTaskIds;
    int tasksCompleted;
    bool active;
    
    WorkgroupMember() : WorkgroupMember("", WorkgroupRole::MEMBER) {}
    
    WorkgroupMember(const AgentId& id, WorkgroupRole r)
        : agentId(id), role(r), 
          joinedAt(std::chrono::system_clock::now()),
          tasksCompleted(0), active(true) {}
};

/**
 * Workgroup statistics
 */
struct WorkgroupStats {
    int totalMembers;
    int activeMembers;
    int totalTasks;
    int completedTasks;
    int pendingTasks;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::milliseconds averageTaskCompletionTime;
};

/**
 * Workgroup configuration
 */
struct WorkgroupConfig {
    std::string name;
    std::string purpose;
    int maxMembers = 50;
    bool allowSelfJoin = false;
    bool requireApproval = true;
    bool persistState = true;
    std::chrono::seconds taskTimeout = std::chrono::seconds(3600); // 1 hour default
};

/**
 * Main Workgroup class representing a collaborative agent group
 */
class Workgroup {
public:
    /**
     * Create a new workgroup
     */
    Workgroup(const std::string& id, 
              const WorkgroupConfig& config,
              const AgentId& creator);
    
    ~Workgroup() = default;
    
    // Member management
    bool addMember(const AgentId& agentId, WorkgroupRole role);
    bool removeMember(const AgentId& agentId);
    bool updateMemberRole(const AgentId& agentId, WorkgroupRole newRole);
    bool isMember(const AgentId& agentId) const;
    WorkgroupRole getMemberRole(const AgentId& agentId) const;
    std::vector<WorkgroupMember> getMembers() const;
    std::vector<AgentId> getMembersByRole(WorkgroupRole role) const;
    
    // Task management
    std::string createTask(const std::string& description, 
                          const AgentId& creator,
                          const std::vector<AgentId>& assignees = {});
    bool assignTask(const std::string& taskId, const AgentId& agentId);
    bool completeTask(const std::string& taskId, const std::string& result);
    bool cancelTask(const std::string& taskId);
    std::shared_ptr<WorkgroupTask> getTask(const std::string& taskId) const;
    std::vector<std::shared_ptr<WorkgroupTask>> getAllTasks() const;
    std::vector<std::shared_ptr<WorkgroupTask>> getTasksForAgent(const AgentId& agentId) const;
    std::vector<std::shared_ptr<WorkgroupTask>> getPendingTasks() const;
    
    // Communication
    void broadcast(const Message& message);
    void sendToRole(const Message& message, WorkgroupRole role);
    void sendToMember(const Message& message, const AgentId& agentId);
    std::string getChannelId() const { return channelId_; }
    
    // State management
    void setState(WorkgroupState newState);
    WorkgroupState getState() const { return state_; }
    bool isActive() const { return state_ == WorkgroupState::ACTIVE; }
    
    // Information
    std::string getId() const { return id_; }
    std::string getName() const { return config_.name; }
    std::string getPurpose() const { return config_.purpose; }
    WorkgroupStats getStats() const;
    
    // Permissions
    bool hasPermission(const AgentId& agentId, const std::string& action) const;
    
private:
    std::string id_;
    WorkgroupConfig config_;
    AgentId creator_;
    WorkgroupState state_;
    std::string channelId_;
    
    std::unordered_map<AgentId, WorkgroupMember> members_;
    std::unordered_map<std::string, std::shared_ptr<WorkgroupTask>> tasks_;
    
    mutable std::mutex membersMutex_;
    mutable std::mutex tasksMutex_;
    
    std::chrono::system_clock::time_point createdAt_;
    
    // Helper methods
    bool checkPermission(const AgentId& agentId, WorkgroupRole requiredRole) const;
    std::string generateTaskId();
};

/**
 * Workgroup Manager - Central management of all workgroups
 */
class WorkgroupManager {
public:
    WorkgroupManager(std::shared_ptr<CommunicationHub> commHub,
                    std::shared_ptr<AgentMemoryManager> memoryMgr);
    
    ~WorkgroupManager() = default;
    
    // Workgroup lifecycle
    std::string createWorkgroup(const WorkgroupConfig& config, const AgentId& creator);
    bool dissolveWorkgroup(const std::string& workgroupId, const AgentId& requestor);
    std::shared_ptr<Workgroup> getWorkgroup(const std::string& workgroupId) const;
    std::vector<std::string> getAllWorkgroupIds() const;
    std::vector<std::string> getWorkgroupsForAgent(const AgentId& agentId) const;
    
    // Membership operations
    bool joinWorkgroup(const std::string& workgroupId, const AgentId& agentId, WorkgroupRole role);
    bool leaveWorkgroup(const std::string& workgroupId, const AgentId& agentId);
    
    // Discovery
    std::vector<std::string> findWorkgroupsByPurpose(const std::string& purposeKeyword) const;
    std::vector<std::string> findWorkgroupsByMember(const AgentId& agentId) const;
    
    // Statistics
    int getTotalWorkgroups() const;
    int getActiveWorkgroups() const;
    
    // Persistence
    bool saveWorkgroupState(const std::string& workgroupId);
    bool loadWorkgroupState(const std::string& workgroupId);
    bool saveAllWorkgroups();
    bool loadAllWorkgroups();
    
private:
    std::shared_ptr<CommunicationHub> commHub_;
    std::shared_ptr<AgentMemoryManager> memoryMgr_;
    
    std::unordered_map<std::string, std::shared_ptr<Workgroup>> workgroups_;
    std::unordered_map<AgentId, std::unordered_set<std::string>> agentWorkgroups_;
    
    mutable std::mutex workgroupsMutex_;
    
    int workgroupCounter_;
    
    // Helper methods
    std::string generateWorkgroupId();
    void indexWorkgroup(const std::string& workgroupId, const AgentId& agentId);
    void unindexWorkgroup(const std::string& workgroupId, const AgentId& agentId);
};

/**
 * Utility functions
 */
namespace workgroups_utils {
    std::string workgroupRoleToString(WorkgroupRole role);
    WorkgroupRole stringToWorkgroupRole(const std::string& roleStr);
    std::string workgroupStateToString(WorkgroupState state);
    WorkgroupState stringToWorkgroupState(const std::string& stateStr);
}

} // namespace elizaos
