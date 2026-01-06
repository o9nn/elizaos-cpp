// hat.cpp - HAT (Human-Agent Teaming) protocol for ElizaOS
// Provides interfaces for human-agent collaboration and task coordination

#include "elizaos/hat.hpp"
#include <algorithm>
#include <sstream>
#include <random>
#include <iostream>

namespace elizaos {
namespace hat {

// ==============================================================================
// UTILITY FUNCTIONS
// ==============================================================================

static std::string generateId(const std::string& prefix) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(100000, 999999);
    return prefix + "_" + std::to_string(dis(gen));
}

std::string roleToString(TeamRole role) {
    switch (role) {
        case TeamRole::HUMAN_LEADER: return "HUMAN_LEADER";
        case TeamRole::AGENT_LEADER: return "AGENT_LEADER";
        case TeamRole::HUMAN_MEMBER: return "HUMAN_MEMBER";
        case TeamRole::AGENT_MEMBER: return "AGENT_MEMBER";
        case TeamRole::OBSERVER: return "OBSERVER";
        default: return "UNKNOWN";
    }
}

TeamRole stringToRole(const std::string& str) {
    if (str == "HUMAN_LEADER") return TeamRole::HUMAN_LEADER;
    if (str == "AGENT_LEADER") return TeamRole::AGENT_LEADER;
    if (str == "HUMAN_MEMBER") return TeamRole::HUMAN_MEMBER;
    if (str == "AGENT_MEMBER") return TeamRole::AGENT_MEMBER;
    if (str == "OBSERVER") return TeamRole::OBSERVER;
    return TeamRole::OBSERVER;
}

std::string priorityToString(TaskPriority priority) {
    switch (priority) {
        case TaskPriority::CRITICAL: return "CRITICAL";
        case TaskPriority::HIGH: return "HIGH";
        case TaskPriority::NORMAL: return "NORMAL";
        case TaskPriority::LOW: return "LOW";
        case TaskPriority::BACKGROUND: return "BACKGROUND";
        default: return "NORMAL";
    }
}

TaskPriority stringToPriority(const std::string& str) {
    if (str == "CRITICAL") return TaskPriority::CRITICAL;
    if (str == "HIGH") return TaskPriority::HIGH;
    if (str == "NORMAL") return TaskPriority::NORMAL;
    if (str == "LOW") return TaskPriority::LOW;
    if (str == "BACKGROUND") return TaskPriority::BACKGROUND;
    return TaskPriority::NORMAL;
}

std::string statusToString(TaskStatus status) {
    switch (status) {
        case TaskStatus::PENDING: return "PENDING";
        case TaskStatus::ASSIGNED: return "ASSIGNED";
        case TaskStatus::IN_PROGRESS: return "IN_PROGRESS";
        case TaskStatus::BLOCKED: return "BLOCKED";
        case TaskStatus::COMPLETED: return "COMPLETED";
        case TaskStatus::CANCELLED: return "CANCELLED";
        default: return "PENDING";
    }
}

TaskStatus stringToStatus(const std::string& str) {
    if (str == "PENDING") return TaskStatus::PENDING;
    if (str == "ASSIGNED") return TaskStatus::ASSIGNED;
    if (str == "IN_PROGRESS") return TaskStatus::IN_PROGRESS;
    if (str == "BLOCKED") return TaskStatus::BLOCKED;
    if (str == "COMPLETED") return TaskStatus::COMPLETED;
    if (str == "CANCELLED") return TaskStatus::CANCELLED;
    return TaskStatus::PENDING;
}

void hat_placeholder() {
    // Placeholder function for backwards compatibility
}

// ==============================================================================
// TEAM COORDINATOR IMPLEMENTATION
// ==============================================================================

struct TeamCoordinator::Impl {
    std::unordered_map<std::string, TeamContext> teams;
    std::unordered_map<std::string, TeamTask> tasks;
    std::unordered_map<std::string, TeamMessage> messages;
    std::mutex mutex;
};

TeamCoordinator::TeamCoordinator() : impl_(std::make_unique<Impl>()) {}

TeamCoordinator::~TeamCoordinator() = default;

std::string TeamCoordinator::createTeam(const std::string& name, const std::string& objective) {
    std::lock_guard<std::mutex> lock(impl_->mutex);

    std::string teamId = generateId("team");
    TeamContext context;
    context.teamId = teamId;
    context.teamName = name;
    context.currentObjective = objective;
    context.sessionStart = std::chrono::system_clock::now();

    impl_->teams[teamId] = context;
    return teamId;
}

bool TeamCoordinator::addMember(const std::string& teamId, const TeamMember& member) {
    std::lock_guard<std::mutex> lock(impl_->mutex);

    auto it = impl_->teams.find(teamId);
    if (it == impl_->teams.end()) {
        return false;
    }

    // Check if member already exists
    for (const auto& existing : it->second.members) {
        if (existing.id == member.id) {
            return false;
        }
    }

    it->second.members.push_back(member);
    return true;
}

bool TeamCoordinator::removeMember(const std::string& teamId, const std::string& memberId) {
    std::lock_guard<std::mutex> lock(impl_->mutex);

    auto it = impl_->teams.find(teamId);
    if (it == impl_->teams.end()) {
        return false;
    }

    auto& members = it->second.members;
    auto memberIt = std::find_if(members.begin(), members.end(),
        [&memberId](const TeamMember& m) { return m.id == memberId; });

    if (memberIt == members.end()) {
        return false;
    }

    members.erase(memberIt);
    return true;
}

std::vector<TeamMember> TeamCoordinator::getTeamMembers(const std::string& teamId) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);

    auto it = impl_->teams.find(teamId);
    if (it == impl_->teams.end()) {
        return {};
    }

    return it->second.members;
}

std::string TeamCoordinator::createTask(const std::string& teamId, const TeamTask& task) {
    std::lock_guard<std::mutex> lock(impl_->mutex);

    auto it = impl_->teams.find(teamId);
    if (it == impl_->teams.end()) {
        return "";
    }

    TeamTask newTask = task;
    newTask.id = generateId("task");
    newTask.status = TaskStatus::PENDING;
    newTask.createdAt = std::chrono::system_clock::now();
    newTask.updatedAt = newTask.createdAt;

    impl_->tasks[newTask.id] = newTask;
    it->second.activeTasks.push_back(newTask);

    return newTask.id;
}

bool TeamCoordinator::assignTask(const std::string& taskId, const std::string& memberId) {
    std::lock_guard<std::mutex> lock(impl_->mutex);

    auto it = impl_->tasks.find(taskId);
    if (it == impl_->tasks.end()) {
        return false;
    }

    it->second.assignedTo = memberId;
    it->second.status = TaskStatus::ASSIGNED;
    it->second.updatedAt = std::chrono::system_clock::now();

    return true;
}

bool TeamCoordinator::updateTaskStatus(const std::string& taskId, TaskStatus status) {
    std::lock_guard<std::mutex> lock(impl_->mutex);

    auto it = impl_->tasks.find(taskId);
    if (it == impl_->tasks.end()) {
        return false;
    }

    it->second.status = status;
    it->second.updatedAt = std::chrono::system_clock::now();

    return true;
}

std::vector<TeamTask> TeamCoordinator::getTasksForMember(const std::string& memberId) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);

    std::vector<TeamTask> result;
    for (const auto& pair : impl_->tasks) {
        if (pair.second.assignedTo == memberId) {
            result.push_back(pair.second);
        }
    }

    return result;
}

std::vector<TeamTask> TeamCoordinator::getPendingTasks(const std::string& teamId) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);

    auto it = impl_->teams.find(teamId);
    if (it == impl_->teams.end()) {
        return {};
    }

    std::vector<TeamTask> result;
    for (const auto& task : it->second.activeTasks) {
        if (task.status == TaskStatus::PENDING) {
            result.push_back(task);
        }
    }

    return result;
}

std::string TeamCoordinator::sendMessage(const TeamMessage& message) {
    std::lock_guard<std::mutex> lock(impl_->mutex);

    TeamMessage newMessage = message;
    newMessage.id = generateId("msg");
    newMessage.timestamp = std::chrono::system_clock::now();
    newMessage.acknowledged = false;

    impl_->messages[newMessage.id] = newMessage;

    return newMessage.id;
}

bool TeamCoordinator::acknowledgeMessage(const std::string& messageId) {
    std::lock_guard<std::mutex> lock(impl_->mutex);

    auto it = impl_->messages.find(messageId);
    if (it == impl_->messages.end()) {
        return false;
    }

    it->second.acknowledged = true;
    return true;
}

std::vector<TeamMessage> TeamCoordinator::getMessagesForMember(const std::string& memberId) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);

    std::vector<TeamMessage> result;
    for (const auto& pair : impl_->messages) {
        if (pair.second.receiverId == memberId) {
            result.push_back(pair.second);
        }
    }

    return result;
}

TeamContext TeamCoordinator::getTeamContext(const std::string& teamId) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);

    auto it = impl_->teams.find(teamId);
    if (it == impl_->teams.end()) {
        return {};
    }

    return it->second;
}

std::string TeamCoordinator::getTeamStatus(const std::string& teamId) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);

    auto it = impl_->teams.find(teamId);
    if (it == impl_->teams.end()) {
        return "Team not found";
    }

    std::ostringstream status;
    status << "Team: " << it->second.teamName << "\n";
    status << "Objective: " << it->second.currentObjective << "\n";
    status << "Members: " << it->second.members.size() << "\n";
    status << "Active Tasks: " << it->second.activeTasks.size() << "\n";

    return status.str();
}

std::string TeamCoordinator::findBestAssignee(const std::string& teamId, const TeamTask& task) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);

    auto it = impl_->teams.find(teamId);
    if (it == impl_->teams.end()) {
        return "";
    }

    std::string bestMember;
    double bestScore = -1;

    for (const auto& member : it->second.members) {
        if (!member.isAvailable) continue;

        // Check if member has required capabilities
        bool hasAllCapabilities = true;
        for (const auto& cap : task.requiredCapabilities) {
            if (!member.canHandle(cap)) {
                hasAllCapabilities = false;
                break;
            }
        }

        if (!hasAllCapabilities) continue;

        // Score based on available capacity
        double score = member.availableCapacity();
        if (score > bestScore) {
            bestScore = score;
            bestMember = member.id;
        }
    }

    return bestMember;
}

bool TeamCoordinator::rebalanceWorkload(const std::string& teamId) {
    std::lock_guard<std::mutex> lock(impl_->mutex);

    auto it = impl_->teams.find(teamId);
    if (it == impl_->teams.end()) {
        return false;
    }

    // Simple rebalancing: reassign tasks from overloaded members
    for (auto& task : it->second.activeTasks) {
        if (task.status == TaskStatus::PENDING && task.assignedTo.empty()) {
            TeamTask taskCopy = task;
            std::string newAssignee = findBestAssignee(teamId, taskCopy);
            if (!newAssignee.empty()) {
                task.assignedTo = newAssignee;
                task.status = TaskStatus::ASSIGNED;
                task.updatedAt = std::chrono::system_clock::now();
            }
        }
    }

    return true;
}

// ==============================================================================
// HAT PROTOCOL HANDLER IMPLEMENTATION
// ==============================================================================

struct HATProtocolHandler::Impl {
    std::string agentId;
    std::vector<std::string> capabilities;
    bool available = true;
    double capacity = 1.0;
    std::vector<std::string> joinedTeams;
    MessageCallback messageCallback;
    TaskCallback taskAssignedCallback;
    TaskCallback taskCompletedCallback;
    std::mutex mutex;
};

HATProtocolHandler::HATProtocolHandler() : impl_(std::make_unique<Impl>()) {}

HATProtocolHandler::~HATProtocolHandler() = default;

bool HATProtocolHandler::initialize(const std::string& agentId) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->agentId = agentId;
    return true;
}

void HATProtocolHandler::shutdown() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->joinedTeams.clear();
}

void HATProtocolHandler::onMessage(MessageCallback callback) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->messageCallback = std::move(callback);
}

void HATProtocolHandler::onTaskAssigned(TaskCallback callback) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->taskAssignedCallback = std::move(callback);
}

void HATProtocolHandler::onTaskCompleted(TaskCallback callback) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->taskCompletedCallback = std::move(callback);
}

bool HATProtocolHandler::joinTeam(const std::string& teamId) {
    std::lock_guard<std::mutex> lock(impl_->mutex);

    auto it = std::find(impl_->joinedTeams.begin(), impl_->joinedTeams.end(), teamId);
    if (it != impl_->joinedTeams.end()) {
        return false; // Already joined
    }

    impl_->joinedTeams.push_back(teamId);
    return true;
}

bool HATProtocolHandler::leaveTeam(const std::string& teamId) {
    std::lock_guard<std::mutex> lock(impl_->mutex);

    auto it = std::find(impl_->joinedTeams.begin(), impl_->joinedTeams.end(), teamId);
    if (it == impl_->joinedTeams.end()) {
        return false; // Not in team
    }

    impl_->joinedTeams.erase(it);
    return true;
}

bool HATProtocolHandler::reportStatus(const std::string& status) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    std::cout << "[" << impl_->agentId << "] Status: " << status << std::endl;
    return true;
}

bool HATProtocolHandler::requestAssistance(const std::string& taskId, const std::string& reason) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    std::cout << "[" << impl_->agentId << "] Requesting assistance for task "
              << taskId << ": " << reason << std::endl;
    return true;
}

void HATProtocolHandler::advertiseCapabilities(const std::vector<std::string>& capabilities) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->capabilities = capabilities;
}

void HATProtocolHandler::updateAvailability(bool available, double capacity) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->available = available;
    impl_->capacity = capacity;
}

} // namespace hat
} // namespace elizaos
