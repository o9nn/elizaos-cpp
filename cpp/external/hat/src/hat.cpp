// hat.cpp - Human-Agent Teaming protocol for ElizaOS
#include "elizaos/hat.hpp"
#include <string>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <functional>

namespace elizaos {
namespace hat {

// Free functions
bool validateHATToken(const std::string& token) {
    return !token.empty() && token.length() >= 8;
}

bool checkHATPermission(const std::string& token, const std::string& permission) {
    return validateHATToken(token);
}

void revokeHATToken(const std::string& token) {
    // Token revocation
}

std::string issueHATToken(const std::string& agentId, const std::string& teamId,
                          const std::vector<std::string>& permissions) {
    return "hat_" + agentId + "_" + teamId;
}

void hat_placeholder() {}

// ==============================================================================
// TeamCoordinator Implementation
// ==============================================================================
struct TeamCoordinator::Impl {
    struct TeamMeta {
        std::string name;
        std::string objective;
        std::chrono::system_clock::time_point sessionStart;
    };
    std::unordered_map<std::string, TeamMeta> team_meta;
    std::unordered_map<std::string, std::vector<TeamMember>> teams;
    std::unordered_map<std::string, std::vector<TeamTask>> team_tasks;
    std::unordered_map<std::string, std::vector<TeamTask>> member_tasks;
    std::unordered_map<std::string, std::vector<TeamMessage>> member_messages;
    std::mutex mutex;
    int next_team_id = 1;
    int next_task_id = 1;
    int next_msg_id = 1;
};

TeamCoordinator::TeamCoordinator() : impl_(std::make_unique<Impl>()) {}
TeamCoordinator::~TeamCoordinator() = default;

std::string TeamCoordinator::createTeam(const std::string& name, const std::string& objective) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    std::string id = "team_" + std::to_string(impl_->next_team_id++);
    impl_->teams[id] = {};
    Impl::TeamMeta meta;
    meta.name = name;
    meta.objective = objective;
    meta.sessionStart = std::chrono::system_clock::now();
    impl_->team_meta[id] = std::move(meta);
    return id;
}

bool TeamCoordinator::addMember(const std::string& teamId, const TeamMember& member) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->teams[teamId].push_back(member);
    return true;
}

bool TeamCoordinator::removeMember(const std::string& teamId, const std::string& memberId) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    auto it = impl_->teams.find(teamId);
    if (it != impl_->teams.end()) {
        auto& members = it->second;
        members.erase(
            std::remove_if(members.begin(), members.end(),
                [&](const TeamMember& m) { return m.id == memberId; }),
            members.end());
        return true;
    }
    return false;
}

std::vector<TeamMember> TeamCoordinator::getTeamMembers(const std::string& teamId) const {
    auto it = impl_->teams.find(teamId);
    return (it != impl_->teams.end()) ? it->second : std::vector<TeamMember>{};
}

std::string TeamCoordinator::createTask(const std::string& teamId, const TeamTask& task) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    std::string id = "task_" + std::to_string(impl_->next_task_id++);
    TeamTask t = task;
    t.id = id;
    impl_->team_tasks[teamId].push_back(t);
    return id;
}

bool TeamCoordinator::assignTask(const std::string& taskId, const std::string& memberId) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    for (auto& [teamId, tasks] : impl_->team_tasks) {
        for (auto& task : tasks) {
            if (task.id == taskId) {
                task.assignedTo = memberId;
                task.status = TaskStatus::IN_PROGRESS;
                impl_->member_tasks[memberId].push_back(task);
                return true;
            }
        }
    }
    return false;
}

bool TeamCoordinator::updateTaskStatus(const std::string& taskId, TaskStatus status) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    for (auto& [teamId, tasks] : impl_->team_tasks) {
        for (auto& task : tasks) {
            if (task.id == taskId) {
                task.status = status;
                return true;
            }
        }
    }
    return false;
}

std::vector<TeamTask> TeamCoordinator::getTasksForMember(const std::string& memberId) const {
    auto it = impl_->member_tasks.find(memberId);
    return (it != impl_->member_tasks.end()) ? it->second : std::vector<TeamTask>{};
}

std::vector<TeamTask> TeamCoordinator::getPendingTasks(const std::string& teamId) const {
    std::vector<TeamTask> pending;
    auto it = impl_->team_tasks.find(teamId);
    if (it != impl_->team_tasks.end()) {
        for (const auto& task : it->second) {
            if (task.status == TaskStatus::PENDING) {
                pending.push_back(task);
            }
        }
    }
    return pending;
}

std::string TeamCoordinator::sendMessage(const TeamMessage& message) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    std::string id = "msg_" + std::to_string(impl_->next_msg_id++);
    TeamMessage msg = message;
    msg.id = id;
    impl_->member_messages[message.receiverId].push_back(msg);
    return id;
}

bool TeamCoordinator::acknowledgeMessage(const std::string& messageId) {
    return true;
}

std::vector<TeamMessage> TeamCoordinator::getMessagesForMember(const std::string& memberId) const {
    auto it = impl_->member_messages.find(memberId);
    return (it != impl_->member_messages.end()) ? it->second : std::vector<TeamMessage>{};
}

TeamContext TeamCoordinator::getTeamContext(const std::string& teamId) const {
    TeamContext ctx;
    ctx.teamId = teamId;
    auto it = impl_->teams.find(teamId);
    if (it != impl_->teams.end()) {
        ctx.members = it->second;
    }
    auto mit = impl_->team_meta.find(teamId);
    if (mit != impl_->team_meta.end()) {
        ctx.teamName         = mit->second.name;
        ctx.currentObjective = mit->second.objective;
        ctx.sessionStart     = mit->second.sessionStart;
    }
    auto tit = impl_->team_tasks.find(teamId);
    if (tit != impl_->team_tasks.end()) {
        ctx.activeTasks = tit->second;
    }
    return ctx;
}

std::string TeamCoordinator::getTeamStatus(const std::string& teamId) const {
    auto it = impl_->teams.find(teamId);
    if (it == impl_->teams.end()) return "unknown";
    return "active";
}

std::string TeamCoordinator::findBestAssignee(const std::string& teamId, const TeamTask& task) const {
    auto it = impl_->teams.find(teamId);
    if (it != impl_->teams.end()) {
        for (const auto& member : it->second) {
            if (member.isAvailable) return member.id;
        }
    }
    return "";
}

bool TeamCoordinator::rebalanceWorkload(const std::string& teamId) {
    return true;
}

// ==============================================================================
// HATProtocolHandler Implementation
// ==============================================================================
struct HATProtocolHandler::Impl {
    std::string agentId;
    std::vector<std::string> joinedTeams;
    std::vector<std::string> capabilities;
    bool available = true;
    double capacity = 1.0;
    MessageCallback messageCallback;
    TaskCallback taskAssignedCallback;
    TaskCallback taskCompletedCallback;
};

HATProtocolHandler::HATProtocolHandler() : impl_(std::make_unique<Impl>()) {}
HATProtocolHandler::~HATProtocolHandler() = default;

bool HATProtocolHandler::initialize(const std::string& agentId) {
    impl_->agentId = agentId;
    return true;
}

void HATProtocolHandler::shutdown() {
    impl_->joinedTeams.clear();
}

void HATProtocolHandler::onMessage(MessageCallback callback) {
    impl_->messageCallback = std::move(callback);
}

void HATProtocolHandler::onTaskAssigned(TaskCallback callback) {
    impl_->taskAssignedCallback = std::move(callback);
}

void HATProtocolHandler::onTaskCompleted(TaskCallback callback) {
    impl_->taskCompletedCallback = std::move(callback);
}

bool HATProtocolHandler::joinTeam(const std::string& teamId) {
    impl_->joinedTeams.push_back(teamId);
    return true;
}

bool HATProtocolHandler::leaveTeam(const std::string& teamId) {
    auto& teams = impl_->joinedTeams;
    teams.erase(std::remove(teams.begin(), teams.end(), teamId), teams.end());
    return true;
}

bool HATProtocolHandler::reportStatus(const std::string& status) {
    return true;
}

bool HATProtocolHandler::requestAssistance(const std::string& taskId, const std::string& reason) {
    return true;
}

void HATProtocolHandler::advertiseCapabilities(const std::vector<std::string>& capabilities) {
    impl_->capabilities = capabilities;
}

void HATProtocolHandler::updateAvailability(bool available, double capacity) {
    impl_->available = available;
    impl_->capacity = capacity;
}

// ==============================================================================
// FREE UTILITY FUNCTIONS
// ==============================================================================

std::string roleToString(TeamRole role) {
    switch (role) {
        case TeamRole::HUMAN_LEADER: return "HUMAN_LEADER";
        case TeamRole::AGENT_LEADER: return "AGENT_LEADER";
        case TeamRole::HUMAN_MEMBER: return "HUMAN_MEMBER";
        case TeamRole::AGENT_MEMBER: return "AGENT_MEMBER";
        case TeamRole::OBSERVER:     return "OBSERVER";
    }
    return "OBSERVER";
}

TeamRole stringToRole(const std::string& s) {
    if (s == "HUMAN_LEADER") return TeamRole::HUMAN_LEADER;
    if (s == "AGENT_LEADER") return TeamRole::AGENT_LEADER;
    if (s == "HUMAN_MEMBER") return TeamRole::HUMAN_MEMBER;
    if (s == "AGENT_MEMBER") return TeamRole::AGENT_MEMBER;
    return TeamRole::OBSERVER;
}

std::string priorityToString(TaskPriority p) {
    switch (p) {
        case TaskPriority::CRITICAL:   return "CRITICAL";
        case TaskPriority::HIGH:       return "HIGH";
        case TaskPriority::NORMAL:     return "NORMAL";
        case TaskPriority::LOW:        return "LOW";
        case TaskPriority::BACKGROUND: return "BACKGROUND";
    }
    return "NORMAL";
}

TaskPriority stringToPriority(const std::string& s) {
    if (s == "CRITICAL")   return TaskPriority::CRITICAL;
    if (s == "HIGH")       return TaskPriority::HIGH;
    if (s == "LOW")        return TaskPriority::LOW;
    if (s == "BACKGROUND") return TaskPriority::BACKGROUND;
    return TaskPriority::NORMAL;
}

std::string statusToString(TaskStatus st) {
    switch (st) {
        case TaskStatus::PENDING:     return "PENDING";
        case TaskStatus::ASSIGNED:    return "ASSIGNED";
        case TaskStatus::IN_PROGRESS: return "IN_PROGRESS";
        case TaskStatus::BLOCKED:     return "BLOCKED";
        case TaskStatus::COMPLETED:   return "COMPLETED";
        case TaskStatus::CANCELLED:   return "CANCELLED";
    }
    return "PENDING";
}

TaskStatus stringToStatus(const std::string& s) {
    if (s == "ASSIGNED")    return TaskStatus::ASSIGNED;
    if (s == "IN_PROGRESS") return TaskStatus::IN_PROGRESS;
    if (s == "BLOCKED")     return TaskStatus::BLOCKED;
    if (s == "COMPLETED")   return TaskStatus::COMPLETED;
    if (s == "CANCELLED")   return TaskStatus::CANCELLED;
    return TaskStatus::PENDING;
}

} // namespace hat
} // namespace elizaos
