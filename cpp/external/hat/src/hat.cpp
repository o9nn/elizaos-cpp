#include "elizaos/hat.hpp"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <mutex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {
namespace hat {
namespace {

struct TokenRecord {
    std::string agentId;
    std::string teamId;
    std::unordered_set<std::string> permissions;
    bool revoked = false;
    std::chrono::system_clock::time_point issuedAt;
};

std::mutex& tokenMutex() {
    static std::mutex mutex;
    return mutex;
}

std::unordered_map<std::string, TokenRecord>& tokenRegistry() {
    static std::unordered_map<std::string, TokenRecord> registry;
    return registry;
}

std::atomic<unsigned long long>& tokenCounter() {
    static std::atomic<unsigned long long> counter{1};
    return counter;
}

std::string sanitizeTokenPart(const std::string& input) {
    std::string out;
    out.reserve(input.size());
    for (char c : input) {
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') || c == '-' || c == '_') {
            out.push_back(c);
        } else {
            out.push_back('_');
        }
    }
    return out.empty() ? "unknown" : out;
}

bool hasTeamMember(const std::vector<TeamMember>& members, const std::string& memberId) {
    return std::any_of(members.begin(), members.end(), [&](const TeamMember& member) {
        return member.id == memberId;
    });
}

bool memberCanHandleTask(const TeamMember& member, const TeamTask& task) {
    if (!member.isAvailable || member.availableCapacity() <= 0.0) {
        return false;
    }
    for (const auto& required : task.requiredCapabilities) {
        if (!member.canHandle(required)) {
            return false;
        }
    }
    return true;
}

} // namespace

// Free functions
bool validateHATToken(const std::string& token) {
    if (token.empty()) {
        return false;
    }

    std::lock_guard<std::mutex> lock(tokenMutex());
    auto it = tokenRegistry().find(token);
    return it != tokenRegistry().end() && !it->second.revoked;
}

bool checkHATPermission(const std::string& token, const std::string& permission) {
    if (permission.empty()) {
        return false;
    }

    std::lock_guard<std::mutex> lock(tokenMutex());
    auto it = tokenRegistry().find(token);
    if (it == tokenRegistry().end() || it->second.revoked) {
        return false;
    }
    return it->second.permissions.find(permission) != it->second.permissions.end();
}

void revokeHATToken(const std::string& token) {
    std::lock_guard<std::mutex> lock(tokenMutex());
    auto it = tokenRegistry().find(token);
    if (it != tokenRegistry().end()) {
        it->second.revoked = true;
    }
}

std::string issueHATToken(const std::string& agentId, const std::string& teamId,
                          const std::vector<std::string>& permissions) {
    const auto sequence = tokenCounter().fetch_add(1);
    std::ostringstream token;
    token << "hat_" << sanitizeTokenPart(agentId) << "_" << sanitizeTokenPart(teamId)
          << "_" << sequence;

    TokenRecord record;
    record.agentId = agentId;
    record.teamId = teamId;
    record.issuedAt = std::chrono::system_clock::now();
    record.permissions.insert(permissions.begin(), permissions.end());

    const std::string tokenId = token.str();
    std::lock_guard<std::mutex> lock(tokenMutex());
    tokenRegistry()[tokenId] = std::move(record);
    return tokenId;
}

void hat_placeholder() {
    // Backward-compatible module-link probe. Real functionality is covered by
    // the stateful token registry, TeamCoordinator, and HATProtocolHandler.
}

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
    auto it = impl_->teams.find(teamId);
    if (it == impl_->teams.end() || member.id.empty()) {
        return false;
    }
    if (hasTeamMember(it->second, member.id)) {
        return false;
    }
    it->second.push_back(member);
    return true;
}

bool TeamCoordinator::removeMember(const std::string& teamId, const std::string& memberId) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    auto it = impl_->teams.find(teamId);
    if (it == impl_->teams.end()) {
        return false;
    }
    auto& members = it->second;
    const auto before = members.size();
    members.erase(
        std::remove_if(members.begin(), members.end(),
            [&](const TeamMember& member) { return member.id == memberId; }),
        members.end());
    impl_->member_tasks.erase(memberId);
    return members.size() != before;
}

std::vector<TeamMember> TeamCoordinator::getTeamMembers(const std::string& teamId) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    auto it = impl_->teams.find(teamId);
    return (it != impl_->teams.end()) ? it->second : std::vector<TeamMember>{};
}

std::string TeamCoordinator::createTask(const std::string& teamId, const TeamTask& task) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (impl_->teams.find(teamId) == impl_->teams.end()) {
        return "";
    }
    std::string id = "task_" + std::to_string(impl_->next_task_id++);
    TeamTask t = task;
    t.id = id;
    t.status = task.status;
    if (t.createdAt.time_since_epoch().count() == 0) {
        t.createdAt = std::chrono::system_clock::now();
    }
    t.updatedAt = std::chrono::system_clock::now();
    impl_->team_tasks[teamId].push_back(t);
    return id;
}

bool TeamCoordinator::assignTask(const std::string& taskId, const std::string& memberId) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    for (auto& [teamId, tasks] : impl_->team_tasks) {
        auto teamIt = impl_->teams.find(teamId);
        if (teamIt == impl_->teams.end() || !hasTeamMember(teamIt->second, memberId)) {
            continue;
        }
        for (auto& task : tasks) {
            if (task.id == taskId) {
                task.assignedTo = memberId;
                task.status = TaskStatus::IN_PROGRESS;
                task.updatedAt = std::chrono::system_clock::now();
                auto& memberQueue = impl_->member_tasks[memberId];
                auto existing = std::find_if(memberQueue.begin(), memberQueue.end(), [&](const TeamTask& queued) {
                    return queued.id == taskId;
                });
                if (existing == memberQueue.end()) {
                    memberQueue.push_back(task);
                } else {
                    *existing = task;
                }
                return true;
            }
        }
    }
    return false;
}

bool TeamCoordinator::updateTaskStatus(const std::string& taskId, TaskStatus status) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    for (auto& [teamId, tasks] : impl_->team_tasks) {
        (void)teamId;
        for (auto& task : tasks) {
            if (task.id == taskId) {
                task.status = status;
                task.updatedAt = std::chrono::system_clock::now();
                if (!task.assignedTo.empty()) {
                    auto& memberQueue = impl_->member_tasks[task.assignedTo];
                    auto existing = std::find_if(memberQueue.begin(), memberQueue.end(), [&](const TeamTask& queued) {
                        return queued.id == taskId;
                    });
                    if (existing != memberQueue.end()) {
                        *existing = task;
                    }
                }
                return true;
            }
        }
    }
    return false;
}

std::vector<TeamTask> TeamCoordinator::getTasksForMember(const std::string& memberId) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    auto it = impl_->member_tasks.find(memberId);
    return (it != impl_->member_tasks.end()) ? it->second : std::vector<TeamTask>{};
}

std::vector<TeamTask> TeamCoordinator::getPendingTasks(const std::string& teamId) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
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
    if (msg.timestamp.time_since_epoch().count() == 0) {
        msg.timestamp = std::chrono::system_clock::now();
    }
    msg.acknowledged = message.acknowledged;
    impl_->member_messages[message.receiverId].push_back(msg);
    return id;
}

bool TeamCoordinator::acknowledgeMessage(const std::string& messageId) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    for (auto& [memberId, messages] : impl_->member_messages) {
        (void)memberId;
        for (auto& message : messages) {
            if (message.id == messageId) {
                message.acknowledged = true;
                return true;
            }
        }
    }
    return false;
}

std::vector<TeamMessage> TeamCoordinator::getMessagesForMember(const std::string& memberId) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    auto it = impl_->member_messages.find(memberId);
    return (it != impl_->member_messages.end()) ? it->second : std::vector<TeamMessage>{};
}

TeamContext TeamCoordinator::getTeamContext(const std::string& teamId) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    TeamContext ctx;
    ctx.teamId = teamId;
    auto it = impl_->teams.find(teamId);
    if (it != impl_->teams.end()) {
        ctx.members = it->second;
        for (const auto& member : it->second) {
            auto msgIt = impl_->member_messages.find(member.id);
            if (msgIt != impl_->member_messages.end()) {
                ctx.recentMessages.insert(ctx.recentMessages.end(), msgIt->second.begin(), msgIt->second.end());
            }
        }
    }
    auto mit = impl_->team_meta.find(teamId);
    if (mit != impl_->team_meta.end()) {
        ctx.teamName = mit->second.name;
        ctx.currentObjective = mit->second.objective;
        ctx.sessionStart = mit->second.sessionStart;
    }
    auto tit = impl_->team_tasks.find(teamId);
    if (tit != impl_->team_tasks.end()) {
        ctx.activeTasks = tit->second;
    }
    return ctx;
}

std::string TeamCoordinator::getTeamStatus(const std::string& teamId) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    auto it = impl_->teams.find(teamId);
    if (it == impl_->teams.end()) return "unknown";
    const auto taskIt = impl_->team_tasks.find(teamId);
    if (taskIt != impl_->team_tasks.end()) {
        for (const auto& task : taskIt->second) {
            if (task.status == TaskStatus::BLOCKED) {
                return "blocked";
            }
        }
    }
    return it->second.empty() ? "idle" : "active";
}

std::string TeamCoordinator::findBestAssignee(const std::string& teamId, const TeamTask& task) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    auto it = impl_->teams.find(teamId);
    if (it == impl_->teams.end()) {
        return "";
    }

    const TeamMember* best = nullptr;
    for (const auto& member : it->second) {
        if (!memberCanHandleTask(member, task)) {
            continue;
        }
        if (!best || member.availableCapacity() > best->availableCapacity()) {
            best = &member;
        }
    }
    return best ? best->id : "";
}

bool TeamCoordinator::rebalanceWorkload(const std::string& teamId) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->teams.find(teamId) != impl_->teams.end();
}

// ==============================================================================
// HATProtocolHandler Implementation
// ==============================================================================
struct HATProtocolHandler::Impl {
    std::string agentId;
    std::vector<std::string> joinedTeams;
    std::vector<std::string> capabilities;
    std::string lastStatus;
    std::vector<std::pair<std::string, std::string>> assistanceRequests;
    bool available = true;
    double capacity = 1.0;
    MessageCallback messageCallback;
    TaskCallback taskAssignedCallback;
    TaskCallback taskCompletedCallback;
};

HATProtocolHandler::HATProtocolHandler() : impl_(std::make_unique<Impl>()) {}
HATProtocolHandler::~HATProtocolHandler() = default;

bool HATProtocolHandler::initialize(const std::string& agentId) {
    if (agentId.empty()) {
        return false;
    }
    impl_->agentId = agentId;
    return true;
}

void HATProtocolHandler::shutdown() {
    impl_->joinedTeams.clear();
    impl_->capabilities.clear();
    impl_->assistanceRequests.clear();
    impl_->lastStatus.clear();
    impl_->available = false;
    impl_->capacity = 0.0;
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
    if (impl_->agentId.empty() || teamId.empty()) {
        return false;
    }
    if (std::find(impl_->joinedTeams.begin(), impl_->joinedTeams.end(), teamId) == impl_->joinedTeams.end()) {
        impl_->joinedTeams.push_back(teamId);
    }
    return true;
}

bool HATProtocolHandler::leaveTeam(const std::string& teamId) {
    auto& teams = impl_->joinedTeams;
    const auto before = teams.size();
    teams.erase(std::remove(teams.begin(), teams.end(), teamId), teams.end());
    return teams.size() != before;
}

bool HATProtocolHandler::reportStatus(const std::string& status) {
    if (impl_->agentId.empty() || status.empty()) {
        return false;
    }
    impl_->lastStatus = status;
    return true;
}

bool HATProtocolHandler::requestAssistance(const std::string& taskId, const std::string& reason) {
    if (impl_->agentId.empty() || taskId.empty() || reason.empty()) {
        return false;
    }
    impl_->assistanceRequests.emplace_back(taskId, reason);
    if (impl_->messageCallback) {
        TeamMessage msg;
        msg.senderId = impl_->agentId;
        msg.receiverId = "team";
        msg.type = CommunicationType::QUERY;
        msg.content = reason;
        msg.relatedTaskId = taskId;
        msg.timestamp = std::chrono::system_clock::now();
        msg.acknowledged = false;
        impl_->messageCallback(msg);
    }
    return true;
}

void HATProtocolHandler::advertiseCapabilities(const std::vector<std::string>& capabilities) {
    impl_->capabilities = capabilities;
}

void HATProtocolHandler::updateAvailability(bool available, double capacity) {
    impl_->available = available;
    if (capacity < 0.0) {
        impl_->capacity = 0.0;
    } else if (capacity > 1.0) {
        impl_->capacity = 1.0;
    } else {
        impl_->capacity = capacity;
    }
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
