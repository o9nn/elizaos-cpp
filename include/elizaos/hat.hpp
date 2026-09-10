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
    TeamRole role = TeamRole::OBSERVER;
    std::vector<std::string> capabilities;
    bool isAvailable = false;
    double workloadCapacity = 0.0;
    double currentWorkload = 0.0;

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
    TaskPriority priority = TaskPriority::NORMAL;
    TaskStatus status = TaskStatus::PENDING;
    std::string assignedTo;
    std::vector<std::string> requiredCapabilities;
    std::chrono::system_clock::time_point deadline;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point updatedAt;
    std::vector<std::string> dependencies;
    double estimatedEffort = 0.0;
    double actualEffort = 0.0;
    std::unordered_map<std::string, std::string> metadata;
};

struct TeamMessage {
    std::string id;
    std::string senderId;
    std::string receiverId;
    CommunicationType type = CommunicationType::STATUS_UPDATE;
    std::string content;
    std::chrono::system_clock::time_point timestamp;
    bool acknowledged = false;
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
    // Reassigns pending/assigned work across available, capable team members.
    // In-progress and blocked tasks retain their current owners; terminal tasks
    // never consume workload capacity.
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

    // Task-event ingress for the local HAT protocol. Assignments are accepted
    // only for this initialized agent on a joined team. Completion updates the
    // accepted task before invoking the corresponding callback. Callbacks are
    // always invoked without holding the handler's internal lock, so they may
    // safely call back into the handler.
    bool receiveTaskAssignment(const std::string& teamId, const TeamTask& task);
    bool completeTask(const std::string& taskId, double actualEffort = 0.0);

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

// Backward-compatible module-link probe. It performs a lightweight invariant
// self-check and throws std::logic_error only if the compiled HAT enum/string
// conversion contract is internally inconsistent.
void hat_placeholder();

// Token lifecycle helpers. Tokens are in-process capabilities used by the
// local HAT coordinator tests and demos; they are not cryptographic bearer
// tokens for network use.
std::string issueHATToken(const std::string& agentId, const std::string& teamId,
                          const std::vector<std::string>& permissions);
bool validateHATToken(const std::string& token);
bool checkHATPermission(const std::string& token, const std::string& permission);
void revokeHATToken(const std::string& token);

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
