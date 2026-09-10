#include "elizaos/hat.hpp"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <mutex>
#include <stdexcept>
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

constexpr double kWorkloadEpsilon = 1e-9;

bool isValidWorkload(double value) {
    return std::isfinite(value) && value >= 0.0;
}

bool hasTeamMember(const std::vector<TeamMember>& members, const std::string& memberId) {
    return std::any_of(members.begin(), members.end(), [&](const TeamMember& member) {
        return member.id == memberId;
    });
}

TeamMember* findMember(std::vector<TeamMember>& members, const std::string& memberId) {
    const auto it = std::find_if(members.begin(), members.end(), [&](const TeamMember& member) {
        return member.id == memberId;
    });
    return it == members.end() ? nullptr : &*it;
}

bool isWorkloadBearing(TaskStatus status) {
    return status == TaskStatus::ASSIGNED || status == TaskStatus::IN_PROGRESS ||
           status == TaskStatus::BLOCKED;
}

bool isTerminal(TaskStatus status) {
    return status == TaskStatus::COMPLETED || status == TaskStatus::CANCELLED;
}

bool isValidTransition(TaskStatus from, TaskStatus to) {
    if (from == to || isTerminal(from) || to == TaskStatus::PENDING ||
        to == TaskStatus::ASSIGNED) {
        return false;
    }
    switch (from) {
        case TaskStatus::PENDING:
            return to == TaskStatus::CANCELLED;
        case TaskStatus::ASSIGNED:
            return to == TaskStatus::IN_PROGRESS || to == TaskStatus::BLOCKED ||
                   to == TaskStatus::COMPLETED || to == TaskStatus::CANCELLED;
        case TaskStatus::IN_PROGRESS:
        case TaskStatus::BLOCKED:
            return to == TaskStatus::IN_PROGRESS || to == TaskStatus::BLOCKED ||
                   to == TaskStatus::COMPLETED || to == TaskStatus::CANCELLED;
        case TaskStatus::COMPLETED:
        case TaskStatus::CANCELLED:
            return false;
    }
    return false;
}

bool hasCapabilities(const TeamMember& member, const TeamTask& task) {
    return std::all_of(task.requiredCapabilities.begin(), task.requiredCapabilities.end(),
        [&](const std::string& required) { return member.canHandle(required); });
}

bool memberCanHandleTask(const TeamMember& member, const TeamTask& task) {
    if (!member.isAvailable || member.role == TeamRole::OBSERVER ||
        !isValidWorkload(member.workloadCapacity) || member.workloadCapacity <= 0.0 ||
        !isValidWorkload(member.currentWorkload) || !isValidWorkload(task.estimatedEffort) ||
        !hasCapabilities(member, task)) {
        return false;
    }
    return member.currentWorkload + task.estimatedEffort <=
           member.workloadCapacity + kWorkloadEpsilon;
}

std::size_t findBestMemberIndex(const std::vector<TeamMember>& members, const TeamTask& task) {
    std::size_t best = members.size();
    double bestUtilization = 0.0;
    double bestRemaining = 0.0;
    for (std::size_t i = 0; i < members.size(); ++i) {
        const auto& member = members[i];
        if (!memberCanHandleTask(member, task)) {
            continue;
        }
        const double utilization =
            (member.currentWorkload + task.estimatedEffort) / member.workloadCapacity;
        const double remaining =
            member.workloadCapacity - member.currentWorkload - task.estimatedEffort;
        if (best == members.size() || utilization < bestUtilization - kWorkloadEpsilon ||
            (std::abs(utilization - bestUtilization) <= kWorkloadEpsilon &&
             (remaining > bestRemaining + kWorkloadEpsilon ||
              (std::abs(remaining - bestRemaining) <= kWorkloadEpsilon &&
               member.id < members[best].id)))) {
            best = i;
            bestUtilization = utilization;
            bestRemaining = remaining;
        }
    }
    return best;
}

void addWorkload(TeamMember& member, const TeamTask& task) {
    member.currentWorkload += task.estimatedEffort;
}

void releaseWorkload(TeamMember& member, const TeamTask& task) {
    member.currentWorkload = std::max(0.0, member.currentWorkload - task.estimatedEffort);
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
    token << "HAT_" << sanitizeTokenPart(agentId) << "_" << sanitizeTokenPart(teamId)
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
    // Preserve the historical link probe, but make it an actual invariant check
    // rather than a silent no-op. Behavioral coverage still targets the stateful
    // coordinator and protocol handler directly.
    if (stringToRole(roleToString(TeamRole::AGENT_MEMBER)) != TeamRole::AGENT_MEMBER ||
        stringToPriority(priorityToString(TaskPriority::HIGH)) != TaskPriority::HIGH ||
        stringToStatus(statusToString(TaskStatus::IN_PROGRESS)) != TaskStatus::IN_PROGRESS) {
        throw std::logic_error("HAT enum/string conversion self-check failed");
    }
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
    if (it == impl_->teams.end() || member.id.empty() ||
        !isValidWorkload(member.workloadCapacity) ||
        !isValidWorkload(member.currentWorkload) ||
        member.currentWorkload > member.workloadCapacity + kWorkloadEpsilon) {
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
    auto teamIt = impl_->teams.find(teamId);
    if (teamIt == impl_->teams.end() || memberId.empty()) {
        return false;
    }
    auto memberIt = std::find_if(teamIt->second.begin(), teamIt->second.end(),
        [&](const TeamMember& member) { return member.id == memberId; });
    if (memberIt == teamIt->second.end()) {
        return false;
    }

    const auto now = std::chrono::system_clock::now();
    auto taskIt = impl_->team_tasks.find(teamId);
    if (taskIt != impl_->team_tasks.end()) {
        for (auto& task : taskIt->second) {
            if (task.assignedTo != memberId) {
                continue;
            }
            if (isWorkloadBearing(task.status)) {
                releaseWorkload(*memberIt, task);
                task.status = TaskStatus::PENDING;
                task.assignedTo.clear();
                task.updatedAt = now;
            }
        }
    }
    teamIt->second.erase(memberIt);
    return true;
}

std::vector<TeamMember> TeamCoordinator::getTeamMembers(const std::string& teamId) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    auto it = impl_->teams.find(teamId);
    return (it != impl_->teams.end()) ? it->second : std::vector<TeamMember>{};
}

std::string TeamCoordinator::createTask(const std::string& teamId, const TeamTask& task) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (impl_->teams.find(teamId) == impl_->teams.end() ||
        task.status != TaskStatus::PENDING || !task.assignedTo.empty() ||
        !isValidWorkload(task.estimatedEffort) || !isValidWorkload(task.actualEffort)) {
        return "";
    }
    std::string id = "task_" + std::to_string(impl_->next_task_id++);
    TeamTask stored = task;
    stored.id = id;
    const auto now = std::chrono::system_clock::now();
    if (stored.createdAt.time_since_epoch().count() == 0) {
        stored.createdAt = now;
    }
    stored.updatedAt = now;
    impl_->team_tasks[teamId].push_back(std::move(stored));
    return id;
}

bool TeamCoordinator::assignTask(const std::string& taskId, const std::string& memberId) {
    if (taskId.empty() || memberId.empty()) {
        return false;
    }
    std::lock_guard<std::mutex> lock(impl_->mutex);
    for (auto& teamEntry : impl_->team_tasks) {
        auto& teamId = teamEntry.first;
        auto& tasks = teamEntry.second;
        auto taskIt = std::find_if(tasks.begin(), tasks.end(),
            [&](const TeamTask& task) { return task.id == taskId; });
        if (taskIt == tasks.end()) {
            continue;
        }
        if (taskIt->status != TaskStatus::PENDING && taskIt->status != TaskStatus::ASSIGNED) {
            return false;
        }
        auto membersIt = impl_->teams.find(teamId);
        if (membersIt == impl_->teams.end()) {
            return false;
        }
        TeamMember* assignee = findMember(membersIt->second, memberId);
        if (!assignee) {
            return false;
        }
        if (taskIt->status == TaskStatus::ASSIGNED && taskIt->assignedTo == memberId) {
            return assignee->isAvailable && assignee->role != TeamRole::OBSERVER &&
                   hasCapabilities(*assignee, *taskIt) &&
                   assignee->currentWorkload <= assignee->workloadCapacity + kWorkloadEpsilon;
        }
        if (!memberCanHandleTask(*assignee, *taskIt)) {
            return false;
        }
        if (taskIt->status == TaskStatus::ASSIGNED && !taskIt->assignedTo.empty()) {
            TeamMember* previous = findMember(membersIt->second, taskIt->assignedTo);
            if (!previous) {
                return false;
            }
            releaseWorkload(*previous, *taskIt);
        }
        taskIt->assignedTo = memberId;
        taskIt->status = TaskStatus::ASSIGNED;
        taskIt->updatedAt = std::chrono::system_clock::now();
        addWorkload(*assignee, *taskIt);
        return true;
    }
    return false;
}

bool TeamCoordinator::updateTaskStatus(const std::string& taskId, TaskStatus status) {
    if (taskId.empty()) {
        return false;
    }
    std::lock_guard<std::mutex> lock(impl_->mutex);
    for (auto& teamEntry : impl_->team_tasks) {
        auto& teamId = teamEntry.first;
        auto& tasks = teamEntry.second;
        for (auto& task : tasks) {
            if (task.id != taskId) {
                continue;
            }
            if (!isValidTransition(task.status, status)) {
                return false;
            }
            if (isWorkloadBearing(task.status) && isTerminal(status)) {
                auto membersIt = impl_->teams.find(teamId);
                if (membersIt == impl_->teams.end()) {
                    return false;
                }
                TeamMember* assignee = findMember(membersIt->second, task.assignedTo);
                if (!assignee) {
                    return false;
                }
                releaseWorkload(*assignee, task);
            }
            task.status = status;
            task.updatedAt = std::chrono::system_clock::now();
            return true;
        }
    }
    return false;
}

std::vector<TeamTask> TeamCoordinator::getTasksForMember(const std::string& memberId) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    std::vector<TeamTask> result;
    if (memberId.empty()) {
        return result;
    }
    for (const auto& teamEntry : impl_->team_tasks) {
        for (const auto& task : teamEntry.second) {
            if (task.assignedTo == memberId) {
                result.push_back(task);
            }
        }
    }
    std::sort(result.begin(), result.end(),
        [](const TeamTask& lhs, const TeamTask& rhs) { return lhs.id < rhs.id; });
    return result;
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
    if (message.receiverId.empty()) {
        return "";
    }
    std::lock_guard<std::mutex> lock(impl_->mutex);
    std::string id = "msg_" + std::to_string(impl_->next_msg_id++);
    TeamMessage stored = message;
    stored.id = id;
    if (stored.timestamp.time_since_epoch().count() == 0) {
        stored.timestamp = std::chrono::system_clock::now();
    }
    stored.acknowledged = false;
    impl_->member_messages[message.receiverId].push_back(std::move(stored));
    return id;
}

bool TeamCoordinator::acknowledgeMessage(const std::string& messageId) {
    if (messageId.empty()) {
        return false;
    }
    std::lock_guard<std::mutex> lock(impl_->mutex);
    for (auto& memberEntry : impl_->member_messages) {
        for (auto& message : memberEntry.second) {
            if (message.id == messageId) {
                if (message.acknowledged) {
                    return false;
                }
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
    TeamContext context;
    context.teamId = teamId;
    auto memberIt = impl_->teams.find(teamId);
    if (memberIt != impl_->teams.end()) {
        context.members = memberIt->second;
        for (const auto& member : memberIt->second) {
            auto messageIt = impl_->member_messages.find(member.id);
            if (messageIt != impl_->member_messages.end()) {
                context.recentMessages.insert(context.recentMessages.end(),
                    messageIt->second.begin(), messageIt->second.end());
            }
        }
    }
    auto metaIt = impl_->team_meta.find(teamId);
    if (metaIt != impl_->team_meta.end()) {
        context.teamName = metaIt->second.name;
        context.currentObjective = metaIt->second.objective;
        context.sessionStart = metaIt->second.sessionStart;
    }
    auto taskIt = impl_->team_tasks.find(teamId);
    if (taskIt != impl_->team_tasks.end()) {
        context.activeTasks = taskIt->second;
    }
    return context;
}

std::string TeamCoordinator::getTeamStatus(const std::string& teamId) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    auto it = impl_->teams.find(teamId);
    if (it == impl_->teams.end()) {
        return "unknown";
    }
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

std::string TeamCoordinator::findBestAssignee(const std::string& teamId,
                                               const TeamTask& task) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    auto it = impl_->teams.find(teamId);
    if (it == impl_->teams.end() ||
        (task.status != TaskStatus::PENDING && task.status != TaskStatus::ASSIGNED) ||
        !isValidWorkload(task.estimatedEffort)) {
        return "";
    }
    auto candidates = it->second;
    if (task.status == TaskStatus::ASSIGNED) {
        TeamMember* current = findMember(candidates, task.assignedTo);
        if (!current) {
            return "";
        }
        releaseWorkload(*current, task);
    }
    const std::size_t best = findBestMemberIndex(candidates, task);
    return best == candidates.size() ? "" : candidates[best].id;
}

bool TeamCoordinator::rebalanceWorkload(const std::string& teamId) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    auto membersIt = impl_->teams.find(teamId);
    if (membersIt == impl_->teams.end()) {
        return false;
    }
    auto tasksIt = impl_->team_tasks.find(teamId);
    if (tasksIt == impl_->team_tasks.end()) {
        return true;
    }

    auto& members = membersIt->second;
    auto& tasks = tasksIt->second;
    const auto now = std::chrono::system_clock::now();
    std::vector<TeamTask*> assignable;
    assignable.reserve(tasks.size());

    for (auto& task : tasks) {
        if (task.status == TaskStatus::ASSIGNED) {
            if (TeamMember* previous = findMember(members, task.assignedTo)) {
                releaseWorkload(*previous, task);
            }
            task.status = TaskStatus::PENDING;
            task.assignedTo.clear();
            assignable.push_back(&task);
        } else if (task.status == TaskStatus::PENDING) {
            task.assignedTo.clear();
            assignable.push_back(&task);
        }
    }

    std::sort(assignable.begin(), assignable.end(), [](const TeamTask* lhs, const TeamTask* rhs) {
        if (lhs->priority != rhs->priority) {
            return static_cast<int>(lhs->priority) < static_cast<int>(rhs->priority);
        }
        const bool lhsHasDeadline = lhs->deadline.time_since_epoch().count() != 0;
        const bool rhsHasDeadline = rhs->deadline.time_since_epoch().count() != 0;
        if (lhsHasDeadline != rhsHasDeadline) {
            return lhsHasDeadline;
        }
        if (lhsHasDeadline && lhs->deadline != rhs->deadline) {
            return lhs->deadline < rhs->deadline;
        }
        if (lhs->createdAt != rhs->createdAt) {
            return lhs->createdAt < rhs->createdAt;
        }
        return lhs->id < rhs->id;
    });

    for (TeamTask* task : assignable) {
        if (!isValidWorkload(task->estimatedEffort)) {
            continue;
        }
        const std::size_t best = findBestMemberIndex(members, *task);
        if (best == members.size()) {
            task->updatedAt = now;
            continue;
        }
        task->assignedTo = members[best].id;
        task->status = TaskStatus::ASSIGNED;
        task->updatedAt = now;
        addWorkload(members[best], *task);
    }
    return true;
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
    std::unordered_map<std::string, TeamTask> tasks;
    std::unordered_map<std::string, std::string> taskTeams;
    bool available = true;
    double capacity = 1.0;
    double currentWorkload = 0.0;
    MessageCallback messageCallback;
    TaskCallback taskAssignedCallback;
    TaskCallback taskCompletedCallback;
    std::mutex mutex;
};

HATProtocolHandler::HATProtocolHandler() : impl_(std::make_unique<Impl>()) {}
HATProtocolHandler::~HATProtocolHandler() = default;

bool HATProtocolHandler::initialize(const std::string& agentId) {
    if (agentId.empty()) {
        return false;
    }
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->agentId.empty()) {
        return false;
    }
    impl_->agentId = agentId;
    impl_->joinedTeams.clear();
    impl_->tasks.clear();
    impl_->taskTeams.clear();
    impl_->assistanceRequests.clear();
    impl_->lastStatus.clear();
    impl_->available = true;
    impl_->capacity = 1.0;
    impl_->currentWorkload = 0.0;
    return true;
}

void HATProtocolHandler::shutdown() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->agentId.clear();
    impl_->joinedTeams.clear();
    impl_->capabilities.clear();
    impl_->tasks.clear();
    impl_->taskTeams.clear();
    impl_->assistanceRequests.clear();
    impl_->lastStatus.clear();
    impl_->available = false;
    impl_->capacity = 0.0;
    impl_->currentWorkload = 0.0;
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
    if (teamId.empty()) {
        return false;
    }
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (impl_->agentId.empty()) {
        return false;
    }
    if (std::find(impl_->joinedTeams.begin(), impl_->joinedTeams.end(), teamId) ==
        impl_->joinedTeams.end()) {
        impl_->joinedTeams.push_back(teamId);
    }
    return true;
}

bool HATProtocolHandler::leaveTeam(const std::string& teamId) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    for (const auto& entry : impl_->taskTeams) {
        const auto taskIt = impl_->tasks.find(entry.first);
        if (entry.second == teamId && taskIt != impl_->tasks.end() &&
            !isTerminal(taskIt->second.status)) {
            return false;
        }
    }
    auto& teams = impl_->joinedTeams;
    const auto before = teams.size();
    teams.erase(std::remove(teams.begin(), teams.end(), teamId), teams.end());
    return teams.size() != before;
}

bool HATProtocolHandler::reportStatus(const std::string& status) {
    if (status.empty()) {
        return false;
    }
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (impl_->agentId.empty()) {
        return false;
    }
    impl_->lastStatus = status;
    return true;
}

bool HATProtocolHandler::requestAssistance(const std::string& taskId,
                                           const std::string& reason) {
    if (taskId.empty() || reason.empty()) {
        return false;
    }
    MessageCallback callback;
    TeamMessage message;
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        if (impl_->agentId.empty()) {
            return false;
        }
        impl_->assistanceRequests.emplace_back(taskId, reason);
        callback = impl_->messageCallback;
        message.senderId = impl_->agentId;
        message.receiverId = "team";
        message.type = CommunicationType::QUERY;
        message.content = reason;
        message.relatedTaskId = taskId;
        message.timestamp = std::chrono::system_clock::now();
        message.acknowledged = false;
    }
    if (callback) {
        callback(message);
    }
    return true;
}

bool HATProtocolHandler::receiveTaskAssignment(const std::string& teamId,
                                               const TeamTask& task) {
    if (teamId.empty() || task.id.empty() || task.status != TaskStatus::ASSIGNED ||
        !isValidWorkload(task.estimatedEffort) || !isValidWorkload(task.actualEffort)) {
        return false;
    }
    TaskCallback callback;
    TeamTask accepted;
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        if (impl_->agentId.empty() || task.assignedTo != impl_->agentId ||
            !impl_->available || impl_->capacity <= 0.0 ||
            std::find(impl_->joinedTeams.begin(), impl_->joinedTeams.end(), teamId) ==
                impl_->joinedTeams.end() ||
            impl_->tasks.find(task.id) != impl_->tasks.end() ||
            impl_->currentWorkload + task.estimatedEffort >
                impl_->capacity + kWorkloadEpsilon) {
            return false;
        }
        if (!std::all_of(task.requiredCapabilities.begin(), task.requiredCapabilities.end(),
                [&](const std::string& required) {
                    return std::find(impl_->capabilities.begin(), impl_->capabilities.end(), required) !=
                           impl_->capabilities.end();
                })) {
            return false;
        }
        accepted = task;
        accepted.updatedAt = std::chrono::system_clock::now();
        impl_->tasks.emplace(accepted.id, accepted);
        impl_->taskTeams.emplace(accepted.id, teamId);
        impl_->currentWorkload += accepted.estimatedEffort;
        callback = impl_->taskAssignedCallback;
    }
    if (callback) {
        callback(accepted);
    }
    return true;
}

bool HATProtocolHandler::completeTask(const std::string& taskId, double actualEffort) {
    if (taskId.empty() || !isValidWorkload(actualEffort)) {
        return false;
    }
    TaskCallback callback;
    TeamTask completed;
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        if (impl_->agentId.empty()) {
            return false;
        }
        auto taskIt = impl_->tasks.find(taskId);
        if (taskIt == impl_->tasks.end() || isTerminal(taskIt->second.status)) {
            return false;
        }
        taskIt->second.status = TaskStatus::COMPLETED;
        taskIt->second.actualEffort = actualEffort;
        taskIt->second.updatedAt = std::chrono::system_clock::now();
        impl_->currentWorkload =
            std::max(0.0, impl_->currentWorkload - taskIt->second.estimatedEffort);
        completed = taskIt->second;
        callback = impl_->taskCompletedCallback;
    }
    if (callback) {
        callback(completed);
    }
    return true;
}

void HATProtocolHandler::advertiseCapabilities(
    const std::vector<std::string>& capabilities) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->capabilities = capabilities;
}

void HATProtocolHandler::updateAvailability(bool available, double capacity) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->available = available;
    if (!std::isfinite(capacity) || capacity < 0.0) {
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
