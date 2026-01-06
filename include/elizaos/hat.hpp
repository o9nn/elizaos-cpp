// hat.hpp - HAT (Human-Agent Teaming) protocol for ElizaOS
// Provides interfaces for human-agent collaboration and task coordination

#ifndef ELIZAOS_HAT_HPP
#define ELIZAOS_HAT_HPP

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>
#include <unordered_map>
#include <mutex>

namespace elizaos {
namespace hat {

// ==============================================================================
// ENUMS
// ==============================================================================

enum class TeamRole {
    HUMAN_LEADER,
    AGENT_LEADER,
    HUMAN_MEMBER,
    AGENT_MEMBER,
    OBSERVER
};

enum class TaskPriority {
    CRITICAL,
    HIGH,
    NORMAL,
    LOW,
    BACKGROUND
};

enum class TaskStatus {
    PENDING,
    ASSIGNED,
    IN_PROGRESS,
    BLOCKED,
    COMPLETED,
    CANCELLED
};

enum class CommunicationType {
    DIRECTIVE,
    QUERY,
    RESPONSE,
    STATUS_UPDATE,
    ALERT,
    ACKNOWLEDGMENT
};

// ==============================================================================
// DATA STRUCTURES
// ==============================================================================

struct TeamMember {
    std::string id;
    std::string name;
    TeamRole role;
    std::vector<std::string> capabilities;
    bool isAvailable;
    double workloadCapacity;
    double currentWorkload;

    bool canHandle(const std::string& capability) const {
        for (const auto& cap : capabilities) {
            if (cap == capability) return true;
        }
        return false;
    }

    double availableCapacity() const {
        return workloadCapacity - currentWorkload;
    }
};

struct TeamTask {
    std::string id;
    std::string name;
    std::string description;
    TaskPriority priority;
    TaskStatus status;
    std::string assignedTo;
    std::vector<std::string> requiredCapabilities;
    std::chrono::system_clock::time_point deadline;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point updatedAt;
    std::vector<std::string> dependencies;
    double estimatedEffort;
    double actualEffort;
    std::unordered_map<std::string, std::string> metadata;
};

struct TeamMessage {
    std::string id;
    std::string senderId;
    std::string receiverId;
    CommunicationType type;
    std::string content;
    std::chrono::system_clock::time_point timestamp;
    bool acknowledged;
    std::string relatedTaskId;
};

struct TeamContext {
    std::string teamId;
    std::string teamName;
    std::string currentObjective;
    std::vector<TeamMember> members;
    std::vector<TeamTask> activeTasks;
    std::vector<TeamMessage> recentMessages;
    std::chrono::system_clock::time_point sessionStart;
};

// ==============================================================================
// TEAM COORDINATOR
// ==============================================================================

class TeamCoordinator {
public:
    TeamCoordinator();
    ~TeamCoordinator();

    // Team management
    std::string createTeam(const std::string& name, const std::string& objective);
    bool addMember(const std::string& teamId, const TeamMember& member);
    bool removeMember(const std::string& teamId, const std::string& memberId);
    std::vector<TeamMember> getTeamMembers(const std::string& teamId) const;

    // Task management
    std::string createTask(const std::string& teamId, const TeamTask& task);
    bool assignTask(const std::string& taskId, const std::string& memberId);
    bool updateTaskStatus(const std::string& taskId, TaskStatus status);
    std::vector<TeamTask> getTasksForMember(const std::string& memberId) const;
    std::vector<TeamTask> getPendingTasks(const std::string& teamId) const;

    // Communication
    std::string sendMessage(const TeamMessage& message);
    bool acknowledgeMessage(const std::string& messageId);
    std::vector<TeamMessage> getMessagesForMember(const std::string& memberId) const;

    // Context and status
    TeamContext getTeamContext(const std::string& teamId) const;
    std::string getTeamStatus(const std::string& teamId) const;

    // Workload balancing
    std::string findBestAssignee(const std::string& teamId, const TeamTask& task) const;
    bool rebalanceWorkload(const std::string& teamId);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

// ==============================================================================
// HAT PROTOCOL HANDLER
// ==============================================================================

class HATProtocolHandler {
public:
    using MessageCallback = std::function<void(const TeamMessage&)>;
    using TaskCallback = std::function<void(const TeamTask&)>;

    HATProtocolHandler();
    ~HATProtocolHandler();

    // Protocol initialization
    bool initialize(const std::string& agentId);
    void shutdown();

    // Message handling
    void onMessage(MessageCallback callback);
    void onTaskAssigned(TaskCallback callback);
    void onTaskCompleted(TaskCallback callback);

    // Protocol operations
    bool joinTeam(const std::string& teamId);
    bool leaveTeam(const std::string& teamId);
    bool reportStatus(const std::string& status);
    bool requestAssistance(const std::string& taskId, const std::string& reason);

    // Capability advertisement
    void advertiseCapabilities(const std::vector<std::string>& capabilities);
    void updateAvailability(bool available, double capacity);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

// ==============================================================================
// FREE FUNCTIONS
// ==============================================================================

// Placeholder function for library linkage
void hat_placeholder();

// Utility functions
std::string roleToString(TeamRole role);
TeamRole stringToRole(const std::string& str);
std::string priorityToString(TaskPriority priority);
TaskPriority stringToPriority(const std::string& str);
std::string statusToString(TaskStatus status);
TaskStatus stringToStatus(const std::string& str);

} // namespace hat
} // namespace elizaos

#endif // ELIZAOS_HAT_HPP
