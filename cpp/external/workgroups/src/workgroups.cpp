#include "elizaos/workgroups.hpp"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <atomic>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <limits>
#include <set>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace elizaos {
namespace {

using Json = nlohmann::json;
using Clock = std::chrono::system_clock;

constexpr int kWorkgroupSchemaVersion = 1;
constexpr int kManagerSchemaVersion = 1;
constexpr std::size_t kMaxPayloadBytes = 16U * 1024U * 1024U;
constexpr std::size_t kMaxMembers = 10000U;
constexpr std::size_t kMaxTasks = 100000U;
constexpr std::size_t kMaxTextLength = 65536U;
constexpr const char* kAllKey = "all_workgroups";
std::atomic<std::uint64_t> gTemporaryCounter{0};

bool isValidId(const std::string& value) {
    if (value.empty() || value.size() > 128U) return false;
    return std::all_of(value.begin(), value.end(), [](unsigned char ch) {
        return std::isalnum(ch) != 0 || ch == '_' || ch == '-' || ch == '.';
    });
}

bool isValidWorkgroupId(const std::string& value) {
    return value.size() <= 96U && isValidId(value);
}

bool isValidText(const std::string& value, bool allowEmpty = false) {
    if (value.size() > kMaxTextLength) return false;
    if (value.empty()) return allowEmpty;
    return std::any_of(value.begin(), value.end(), [](unsigned char ch) {
        return std::isspace(ch) == 0;
    });
}

bool isValidRole(WorkgroupRole role) {
    switch (role) {
        case WorkgroupRole::LEADER:
        case WorkgroupRole::COORDINATOR:
        case WorkgroupRole::MEMBER:
        case WorkgroupRole::OBSERVER:
            return true;
    }
    return false;
}

bool isValidState(WorkgroupState state) {
    switch (state) {
        case WorkgroupState::FORMING:
        case WorkgroupState::ACTIVE:
        case WorkgroupState::PAUSED:
        case WorkgroupState::COMPLETING:
        case WorkgroupState::DISSOLVED:
            return true;
    }
    return false;
}

bool isValidTaskState(WorkgroupTaskState state) {
    switch (state) {
        case WorkgroupTaskState::PENDING:
        case WorkgroupTaskState::COMPLETED:
        case WorkgroupTaskState::CANCELLED:
            return true;
    }
    return false;
}

std::int64_t toMillis(Clock::time_point value) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               value.time_since_epoch())
        .count();
}

Clock::time_point fromMillis(std::int64_t value) {
    return Clock::time_point(std::chrono::milliseconds(value));
}

bool hasOnlyKeys(const Json& object,
                 std::initializer_list<const char*> allowedKeys) {
    if (!object.is_object()) return false;
    std::set<std::string> allowed;
    for (const char* key : allowedKeys) allowed.emplace(key);
    for (const auto& entry : object.items()) {
        if (allowed.count(entry.key()) == 0U) return false;
    }
    return true;
}

bool parseCounterSuffix(const std::string& id,
                        const std::string& prefix,
                        std::uint64_t& nextCounter) {
    if (id.rfind(prefix, 0U) != 0U || id.size() == prefix.size()) return false;
    const std::string suffix = id.substr(prefix.size());
    if (!std::all_of(suffix.begin(), suffix.end(), [](unsigned char ch) {
            return std::isdigit(ch) != 0;
        })) {
        return false;
    }
    try {
        const auto parsed = std::stoull(suffix);
        if (parsed == std::numeric_limits<unsigned long long>::max()) return false;
        const auto value = static_cast<std::uint64_t>(parsed);
        nextCounter = std::max(nextCounter, value + std::uint64_t{1});
        return true;
    } catch (...) {
        return false;
    }
}

bool validConfig(const WorkgroupConfig& config) {
    return isValidText(config.name) && isValidText(config.purpose) &&
           config.maxMembers > 0 &&
           static_cast<std::size_t>(config.maxMembers) <= kMaxMembers &&
           config.taskTimeout.count() > 0 &&
           config.taskTimeout <= std::chrono::hours(24 * 365);
}

Json configToJson(const WorkgroupConfig& config) {
    return Json{{"allowSelfJoin", config.allowSelfJoin},
                {"maxMembers", config.maxMembers},
                {"name", config.name},
                {"persistState", config.persistState},
                {"purpose", config.purpose},
                {"requireApproval", config.requireApproval},
                {"taskTimeoutSeconds", config.taskTimeout.count()}};
}

bool parseConfig(const Json& value, WorkgroupConfig& config) {
    if (!hasOnlyKeys(value,
                     {"allowSelfJoin", "maxMembers", "name", "persistState",
                      "purpose", "requireApproval", "taskTimeoutSeconds"})) {
        return false;
    }
    try {
        config.allowSelfJoin = value.at("allowSelfJoin").get<bool>();
        config.maxMembers = value.at("maxMembers").get<int>();
        config.name = value.at("name").get<std::string>();
        config.persistState = value.at("persistState").get<bool>();
        config.purpose = value.at("purpose").get<std::string>();
        config.requireApproval = value.at("requireApproval").get<bool>();
        config.taskTimeout = std::chrono::seconds(
            value.at("taskTimeoutSeconds").get<std::int64_t>());
        return validConfig(config);
    } catch (...) {
        return false;
    }
}

bool pathContainsSymlink(const std::filesystem::path& path) {
    std::filesystem::path current;
    for (const auto& component : path.lexically_normal()) {
        current /= component;
        std::error_code error;
        const auto status = std::filesystem::symlink_status(current, error);
        if (!error && std::filesystem::is_symlink(status)) return true;
    }
    return false;
}

std::filesystem::path safePath(const std::string& root,
                               const std::string& key) {
    if (!isValidId(key)) return {};
    const std::filesystem::path rootPath(root);
    if (rootPath.empty() || !rootPath.is_absolute()) return {};
    const std::filesystem::path normalizedRoot = rootPath.lexically_normal();
    if (pathContainsSymlink(normalizedRoot)) return {};
    return normalizedRoot / (key + ".json");
}

std::string roleName(WorkgroupRole role) {
    switch (role) {
        case WorkgroupRole::LEADER: return "Leader";
        case WorkgroupRole::COORDINATOR: return "Coordinator";
        case WorkgroupRole::MEMBER: return "Member";
        case WorkgroupRole::OBSERVER: return "Observer";
    }
    throw std::invalid_argument("invalid workgroup role");
}

std::string stateName(WorkgroupState state) {
    switch (state) {
        case WorkgroupState::FORMING: return "Forming";
        case WorkgroupState::ACTIVE: return "Active";
        case WorkgroupState::PAUSED: return "Paused";
        case WorkgroupState::COMPLETING: return "Completing";
        case WorkgroupState::DISSOLVED: return "Dissolved";
    }
    throw std::invalid_argument("invalid workgroup state");
}

std::string taskStateName(WorkgroupTaskState state) {
    switch (state) {
        case WorkgroupTaskState::PENDING: return "Pending";
        case WorkgroupTaskState::COMPLETED: return "Completed";
        case WorkgroupTaskState::CANCELLED: return "Cancelled";
    }
    throw std::invalid_argument("invalid workgroup task state");
}

WorkgroupRole parseRole(const std::string& value) {
    if (value == "Leader") return WorkgroupRole::LEADER;
    if (value == "Coordinator") return WorkgroupRole::COORDINATOR;
    if (value == "Member") return WorkgroupRole::MEMBER;
    if (value == "Observer") return WorkgroupRole::OBSERVER;
    throw std::invalid_argument("invalid workgroup role string");
}

WorkgroupState parseState(const std::string& value) {
    if (value == "Forming") return WorkgroupState::FORMING;
    if (value == "Active") return WorkgroupState::ACTIVE;
    if (value == "Paused") return WorkgroupState::PAUSED;
    if (value == "Completing") return WorkgroupState::COMPLETING;
    if (value == "Dissolved") return WorkgroupState::DISSOLVED;
    throw std::invalid_argument("invalid workgroup state string");
}

WorkgroupTaskState parseTaskState(const std::string& value) {
    if (value == "Pending") return WorkgroupTaskState::PENDING;
    if (value == "Completed") return WorkgroupTaskState::COMPLETED;
    if (value == "Cancelled") return WorkgroupTaskState::CANCELLED;
    throw std::invalid_argument("invalid workgroup task state string");
}

template <typename T>
std::vector<std::shared_ptr<WorkgroupTask>> sortedTaskSnapshots(
    const T& tasks,
    const std::function<bool(const WorkgroupTask&)>& include) {
    std::vector<std::shared_ptr<WorkgroupTask>> result;
    for (const auto& entry : tasks) {
        if (include(entry.second)) {
            result.push_back(std::make_shared<WorkgroupTask>(entry.second));
        }
    }
    std::sort(result.begin(), result.end(), [](const auto& left, const auto& right) {
        return left->taskId < right->taskId;
    });
    return result;
}

} // namespace

WorkgroupTask::WorkgroupTask() : WorkgroupTask("", "", "") {}

WorkgroupTask::WorkgroupTask(const std::string& id,
                             const std::string& desc,
                             const std::string& creator)
    : taskId(id),
      description(desc),
      createdBy(creator),
      createdAt(Clock::now()),
      dueDate(),
      completedAt(),
      state(WorkgroupTaskState::PENDING),
      completed(false) {}

WorkgroupMember::WorkgroupMember()
    : WorkgroupMember("", WorkgroupRole::MEMBER) {}

WorkgroupMember::WorkgroupMember(const AgentId& id, WorkgroupRole roleValue)
    : agentId(id),
      role(roleValue),
      joinedAt(Clock::now()),
      tasksCompleted(0),
      active(true) {}

LocalJsonWorkgroupPersistenceAdapter::LocalJsonWorkgroupPersistenceAdapter(
    std::string rootDirectory)
    : rootDirectory_(std::move(rootDirectory)) {}

bool LocalJsonWorkgroupPersistenceAdapter::save(const std::string& key,
                                                const std::string& payload) {
    if (payload.empty() || payload.size() > kMaxPayloadBytes) return false;
    try {
        const Json document = Json::parse(payload);
        if (!document.is_object()) return false;
    } catch (...) {
        return false;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    const auto target = safePath(rootDirectory_, key);
    if (target.empty()) return false;
    std::error_code error;
    std::filesystem::create_directories(target.parent_path(), error);
    if (error || pathContainsSymlink(target.parent_path())) return false;
    const auto targetStatus = std::filesystem::symlink_status(target, error);
    if (!error && std::filesystem::is_symlink(targetStatus)) return false;
    error.clear();
    const auto temporary = target.parent_path() /
        (target.filename().string() + ".tmp." +
         std::to_string(gTemporaryCounter.fetch_add(1U)));
    {
        std::ofstream output(temporary, std::ios::binary | std::ios::trunc);
        if (!output) return false;
        output.write(payload.data(), static_cast<std::streamsize>(payload.size()));
        output.flush();
        if (!output) {
            output.close();
            std::filesystem::remove(temporary, error);
            return false;
        }
        output.close();
        if (!output) {
            std::filesystem::remove(temporary, error);
            return false;
        }
    }
    std::filesystem::rename(temporary, target, error);
    if (error) std::filesystem::remove(temporary, error);
    return !error;
}

bool LocalJsonWorkgroupPersistenceAdapter::load(const std::string& key,
                                                std::string& payload) {
    std::lock_guard<std::mutex> lock(mutex_);
    const auto target = safePath(rootDirectory_, key);
    if (target.empty()) return false;
    std::error_code error;
    const auto status = std::filesystem::symlink_status(target, error);
    if (error || !std::filesystem::is_regular_file(status) ||
        std::filesystem::is_symlink(status)) {
        return false;
    }
    const auto size = std::filesystem::file_size(target, error);
    if (error || size == 0U || size > kMaxPayloadBytes) return false;
    std::ifstream input(target, std::ios::binary);
    if (!input) return false;
    std::string staged(static_cast<std::size_t>(size), '\0');
    input.read(staged.data(), static_cast<std::streamsize>(staged.size()));
    if (!input || input.gcount() != static_cast<std::streamsize>(staged.size())) {
        return false;
    }
    payload = std::move(staged);
    return true;
}

Workgroup::Workgroup(const std::string& id,
                     const WorkgroupConfig& config,
                     const AgentId& creator)
    : id_(id),
      config_(config),
      creator_(creator),
      state_(WorkgroupState::FORMING),
      channelId_("workgroup_" + id),
      createdAt_(Clock::now()),
      taskCounter_(0U) {
    if (!isValidWorkgroupId(id_) || !isValidId(creator_) || !validConfig(config_)) {
        throw std::invalid_argument("invalid workgroup construction data");
    }
    members_.emplace(creator_, WorkgroupMember(creator_, WorkgroupRole::LEADER));
}

bool Workgroup::addMember(const AgentId& agentId, WorkgroupRole role) {
    if (!isValidId(agentId) || !isValidRole(role)) return false;
    std::lock_guard<std::mutex> lock(mutex_);
    if (state_ == WorkgroupState::DISSOLVED ||
        state_ == WorkgroupState::COMPLETING ||
        role == WorkgroupRole::LEADER ||
        members_.count(agentId) != 0U ||
        members_.size() >= static_cast<std::size_t>(config_.maxMembers)) {
        return false;
    }
    members_.emplace(agentId, WorkgroupMember(agentId, role));
    return true;
}

bool Workgroup::removeMember(const AgentId& agentId) {
    if (!isValidId(agentId)) return false;
    std::lock_guard<std::mutex> lock(mutex_);
    if (state_ == WorkgroupState::DISSOLVED) return false;
    const auto member = members_.find(agentId);
    if (member == members_.end()) return false;
    if (member->second.role == WorkgroupRole::LEADER) {
        const auto leaders = std::count_if(
            members_.begin(), members_.end(), [](const auto& entry) {
                return entry.second.role == WorkgroupRole::LEADER;
            });
        if (leaders <= 1) return false;
    }
    for (auto& taskEntry : tasks_) {
        auto& assigned = taskEntry.second.assignedTo;
        assigned.erase(std::remove(assigned.begin(), assigned.end(), agentId),
                       assigned.end());
    }
    members_.erase(member);
    return true;
}

bool Workgroup::updateMemberRole(const AgentId& agentId,
                                 WorkgroupRole newRole) {
    if (!isValidId(agentId) || !isValidRole(newRole)) return false;
    std::lock_guard<std::mutex> lock(mutex_);
    if (state_ == WorkgroupState::DISSOLVED) return false;
    const auto member = members_.find(agentId);
    if (member == members_.end() || member->second.role == newRole) return false;
    if (member->second.role == WorkgroupRole::LEADER &&
        newRole != WorkgroupRole::LEADER) {
        const auto leaders = std::count_if(
            members_.begin(), members_.end(), [](const auto& entry) {
                return entry.second.role == WorkgroupRole::LEADER;
            });
        if (leaders <= 1) return false;
    }
    member->second.role = newRole;
    return true;
}

bool Workgroup::isMember(const AgentId& agentId) const {
    if (!isValidId(agentId)) return false;
    std::lock_guard<std::mutex> lock(mutex_);
    return members_.count(agentId) != 0U;
}

WorkgroupRole Workgroup::getMemberRole(const AgentId& agentId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    const auto member = members_.find(agentId);
    return member == members_.end() ? WorkgroupRole::OBSERVER
                                    : member->second.role;
}

std::vector<WorkgroupMember> Workgroup::getMembers() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<WorkgroupMember> result;
    result.reserve(members_.size());
    for (const auto& entry : members_) result.push_back(entry.second);
    std::sort(result.begin(), result.end(), [](const auto& left, const auto& right) {
        return left.agentId < right.agentId;
    });
    for (auto& member : result) {
        std::sort(member.assignedTaskIds.begin(), member.assignedTaskIds.end());
    }
    return result;
}

std::vector<AgentId> Workgroup::getMembersByRole(WorkgroupRole role) const {
    if (!isValidRole(role)) return {};
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<AgentId> result;
    for (const auto& entry : members_) {
        if (entry.second.role == role) result.push_back(entry.first);
    }
    std::sort(result.begin(), result.end());
    return result;
}

std::string Workgroup::createTask(const std::string& description,
                                  const AgentId& creator,
                                  const std::vector<AgentId>& assignees) {
    if (!isValidText(description) || !isValidId(creator)) return {};
    std::lock_guard<std::mutex> lock(mutex_);
    if (state_ != WorkgroupState::ACTIVE ||
        !hasPermissionUnlocked(creator, "assign_tasks")) {
        return {};
    }
    std::set<AgentId> uniqueAssignees;
    for (const auto& assignee : assignees) {
        const auto member = members_.find(assignee);
        if (!isValidId(assignee) || member == members_.end() ||
            member->second.role == WorkgroupRole::OBSERVER ||
            !member->second.active || !uniqueAssignees.insert(assignee).second) {
            return {};
        }
    }
    const std::string taskId = generateTaskIdUnlocked();
    if (taskId.empty()) return {};
    WorkgroupTask task(taskId, description, creator);
    task.assignedTo.assign(uniqueAssignees.begin(), uniqueAssignees.end());
    task.dueDate = task.createdAt + config_.taskTimeout;
    tasks_.emplace(taskId, task);
    for (const auto& assignee : task.assignedTo) {
        members_.at(assignee).assignedTaskIds.push_back(taskId);
    }
    return taskId;
}

bool Workgroup::assignTask(const std::string& taskId, const AgentId& agentId) {
    if (!isValidId(taskId) || !isValidId(agentId)) return false;
    std::lock_guard<std::mutex> lock(mutex_);
    if (state_ != WorkgroupState::ACTIVE) return false;
    const auto task = tasks_.find(taskId);
    const auto member = members_.find(agentId);
    if (task == tasks_.end() || member == members_.end() ||
        task->second.state != WorkgroupTaskState::PENDING ||
        member->second.role == WorkgroupRole::OBSERVER ||
        !member->second.active ||
        std::find(task->second.assignedTo.begin(), task->second.assignedTo.end(),
                  agentId) != task->second.assignedTo.end()) {
        return false;
    }
    task->second.assignedTo.push_back(agentId);
    std::sort(task->second.assignedTo.begin(), task->second.assignedTo.end());
    member->second.assignedTaskIds.push_back(taskId);
    return true;
}

bool Workgroup::completeTask(const std::string& taskId,
                             const std::string& result) {
    if (!isValidId(taskId) || !isValidText(result)) return false;
    std::lock_guard<std::mutex> lock(mutex_);
    if (state_ != WorkgroupState::ACTIVE &&
        state_ != WorkgroupState::COMPLETING) {
        return false;
    }
    const auto task = tasks_.find(taskId);
    if (task == tasks_.end() ||
        task->second.state != WorkgroupTaskState::PENDING) {
        return false;
    }
    task->second.state = WorkgroupTaskState::COMPLETED;
    task->second.completed = true;
    task->second.result = result;
    task->second.completedAt = Clock::now();
    for (const auto& assignee : task->second.assignedTo) {
        auto member = members_.find(assignee);
        if (member != members_.end()) ++member->second.tasksCompleted;
    }
    return true;
}

bool Workgroup::cancelTask(const std::string& taskId) {
    if (!isValidId(taskId)) return false;
    std::lock_guard<std::mutex> lock(mutex_);
    if (state_ == WorkgroupState::DISSOLVED) return false;
    const auto task = tasks_.find(taskId);
    if (task == tasks_.end() ||
        task->second.state != WorkgroupTaskState::PENDING) {
        return false;
    }
    task->second.state = WorkgroupTaskState::CANCELLED;
    task->second.completed = false;
    task->second.result.clear();
    for (const auto& assignee : task->second.assignedTo) {
        auto member = members_.find(assignee);
        if (member != members_.end()) {
            auto& assignedIds = member->second.assignedTaskIds;
            assignedIds.erase(
                std::remove(assignedIds.begin(), assignedIds.end(), taskId),
                assignedIds.end());
        }
    }
    return true;
}

std::shared_ptr<WorkgroupTask> Workgroup::getTask(
    const std::string& taskId) const {
    if (!isValidId(taskId)) return nullptr;
    std::lock_guard<std::mutex> lock(mutex_);
    const auto task = tasks_.find(taskId);
    return task == tasks_.end()
               ? nullptr
               : std::make_shared<WorkgroupTask>(task->second);
}

std::vector<std::shared_ptr<WorkgroupTask>> Workgroup::getAllTasks() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return sortedTaskSnapshots(tasks_, [](const WorkgroupTask&) { return true; });
}

std::vector<std::shared_ptr<WorkgroupTask>> Workgroup::getTasksForAgent(
    const AgentId& agentId) const {
    if (!isValidId(agentId)) return {};
    std::lock_guard<std::mutex> lock(mutex_);
    return sortedTaskSnapshots(tasks_, [&agentId](const WorkgroupTask& task) {
        return std::find(task.assignedTo.begin(), task.assignedTo.end(), agentId) !=
               task.assignedTo.end();
    });
}

std::vector<std::shared_ptr<WorkgroupTask>> Workgroup::getPendingTasks() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return sortedTaskSnapshots(tasks_, [](const WorkgroupTask& task) {
        return task.state == WorkgroupTaskState::PENDING;
    });
}

void Workgroup::setMessageDispatcher(WorkgroupMessageDispatcher dispatcher) {
    std::lock_guard<std::mutex> lock(mutex_);
    messageDispatcher_ = std::move(dispatcher);
}

void Workgroup::broadcast(const Message& message) {
    WorkgroupMessageDispatcher dispatcher;
    std::vector<AgentId> recipients;
    std::string channel;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (state_ == WorkgroupState::DISSOLVED || !messageDispatcher_) return;
        dispatcher = messageDispatcher_;
        channel = channelId_;
        for (const auto& member : members_) {
            if (member.second.active) recipients.push_back(member.first);
        }
    }
    std::sort(recipients.begin(), recipients.end());
    for (const auto& recipient : recipients) {
        try {
            dispatcher(message, recipient, channel);
        } catch (...) {
        }
    }
}

void Workgroup::sendToRole(const Message& message, WorkgroupRole role) {
    if (!isValidRole(role)) return;
    WorkgroupMessageDispatcher dispatcher;
    std::vector<AgentId> recipients;
    std::string channel;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (state_ == WorkgroupState::DISSOLVED || !messageDispatcher_) return;
        dispatcher = messageDispatcher_;
        channel = channelId_;
        for (const auto& member : members_) {
            if (member.second.active && member.second.role == role) {
                recipients.push_back(member.first);
            }
        }
    }
    std::sort(recipients.begin(), recipients.end());
    for (const auto& recipient : recipients) {
        try {
            dispatcher(message, recipient, channel);
        } catch (...) {
        }
    }
}

void Workgroup::sendToMember(const Message& message,
                             const AgentId& agentId) {
    if (!isValidId(agentId)) return;
    WorkgroupMessageDispatcher dispatcher;
    std::string channel;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto member = members_.find(agentId);
        if (state_ == WorkgroupState::DISSOLVED || !messageDispatcher_ ||
            member == members_.end() || !member->second.active) {
            return;
        }
        dispatcher = messageDispatcher_;
        channel = channelId_;
    }
    try {
        dispatcher(message, agentId, channel);
    } catch (...) {
    }
}

std::string Workgroup::getChannelId() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return channelId_;
}

void Workgroup::setState(WorkgroupState newState) {
    static_cast<void>(transitionTo(newState));
}

bool Workgroup::transitionTo(WorkgroupState newState) {
    if (!isValidState(newState)) return false;
    std::lock_guard<std::mutex> lock(mutex_);
    if (!transitionAllowedUnlocked(newState)) return false;
    state_ = newState;
    return true;
}

WorkgroupState Workgroup::getState() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return state_;
}

bool Workgroup::isActive() const {
    return getState() == WorkgroupState::ACTIVE;
}

std::string Workgroup::getId() const {
    return id_;
}

std::string Workgroup::getName() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return config_.name;
}

std::string Workgroup::getPurpose() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return config_.purpose;
}

WorkgroupStats Workgroup::getStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    WorkgroupStats stats;
    stats.totalMembers = static_cast<int>(members_.size());
    stats.activeMembers = static_cast<int>(std::count_if(
        members_.begin(), members_.end(), [](const auto& member) {
            return member.second.active;
        }));
    stats.totalTasks = static_cast<int>(tasks_.size());
    std::int64_t totalCompletionMilliseconds = 0;
    for (const auto& task : tasks_) {
        if (task.second.state == WorkgroupTaskState::COMPLETED) {
            ++stats.completedTasks;
            totalCompletionMilliseconds +=
                std::max<std::int64_t>(0, toMillis(task.second.completedAt) -
                                             toMillis(task.second.createdAt));
        } else if (task.second.state == WorkgroupTaskState::PENDING) {
            ++stats.pendingTasks;
        }
    }
    stats.createdAt = createdAt_;
    if (stats.completedTasks != 0) {
        stats.averageTaskCompletionTime = std::chrono::milliseconds(
            totalCompletionMilliseconds / stats.completedTasks);
    }
    return stats;
}

bool Workgroup::hasPermission(const AgentId& agentId,
                              const std::string& action) const {
    if (!isValidId(agentId)) return false;
    std::lock_guard<std::mutex> lock(mutex_);
    return hasPermissionUnlocked(agentId, action);
}

bool Workgroup::hasPermissionUnlocked(const AgentId& agentId,
                                      const std::string& action) const {
    if (state_ == WorkgroupState::DISSOLVED) return false;
    const auto member = members_.find(agentId);
    if (member == members_.end() || !member->second.active) return false;
    const WorkgroupRole role = member->second.role;
    if (action == "manage_members" || action == "dissolve") {
        return role == WorkgroupRole::LEADER;
    }
    if (action == "assign_tasks") {
        return role == WorkgroupRole::LEADER ||
               role == WorkgroupRole::COORDINATOR;
    }
    if (action == "complete_tasks" || action == "send_messages") {
        return role != WorkgroupRole::OBSERVER;
    }
    if (action == "view") return true;
    return false;
}

bool Workgroup::transitionAllowedUnlocked(WorkgroupState newState) const {
    if (newState == state_) return false;
    switch (state_) {
        case WorkgroupState::FORMING:
            return newState == WorkgroupState::ACTIVE ||
                   newState == WorkgroupState::DISSOLVED;
        case WorkgroupState::ACTIVE:
            return newState == WorkgroupState::PAUSED ||
                   newState == WorkgroupState::COMPLETING ||
                   newState == WorkgroupState::DISSOLVED;
        case WorkgroupState::PAUSED:
            return newState == WorkgroupState::ACTIVE ||
                   newState == WorkgroupState::COMPLETING ||
                   newState == WorkgroupState::DISSOLVED;
        case WorkgroupState::COMPLETING:
            if (newState == WorkgroupState::ACTIVE) return true;
            if (newState != WorkgroupState::DISSOLVED) return false;
            return std::none_of(tasks_.begin(), tasks_.end(), [](const auto& task) {
                return task.second.state == WorkgroupTaskState::PENDING;
            });
        case WorkgroupState::DISSOLVED:
            return false;
    }
    return false;
}

std::string Workgroup::generateTaskIdUnlocked() {
    while (taskCounter_ != std::numeric_limits<std::uint64_t>::max()) {
        std::ostringstream id;
        id << id_ << "_task_" << std::setfill('0') << std::setw(12)
           << taskCounter_++;
        if (tasks_.count(id.str()) == 0U) return id.str();
    }
    return {};
}

std::string Workgroup::serializeSnapshot() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return serializeSnapshotUnlocked();
}

std::string Workgroup::serializeSnapshotUnlocked() const {
    Json members = Json::array();
    std::vector<AgentId> memberIds;
    memberIds.reserve(members_.size());
    for (const auto& member : members_) memberIds.push_back(member.first);
    std::sort(memberIds.begin(), memberIds.end());
    for (const auto& memberId : memberIds) {
        const auto& member = members_.at(memberId);
        auto assigned = member.assignedTaskIds;
        std::sort(assigned.begin(), assigned.end());
        members.push_back(Json{{"active", member.active},
                               {"agentId", member.agentId},
                               {"assignedTaskIds", assigned},
                               {"joinedAtMs", toMillis(member.joinedAt)},
                               {"role", roleName(member.role)},
                               {"tasksCompleted", member.tasksCompleted}});
    }
    Json tasks = Json::array();
    std::vector<std::string> taskIds;
    taskIds.reserve(tasks_.size());
    for (const auto& task : tasks_) taskIds.push_back(task.first);
    std::sort(taskIds.begin(), taskIds.end());
    for (const auto& taskId : taskIds) {
        const auto& task = tasks_.at(taskId);
        auto assigned = task.assignedTo;
        std::sort(assigned.begin(), assigned.end());
        std::vector<std::pair<std::string, std::string>> metadata(
            task.metadata.begin(), task.metadata.end());
        std::sort(metadata.begin(), metadata.end());
        Json metadataJson = Json::object();
        for (const auto& entry : metadata) metadataJson[entry.first] = entry.second;
        tasks.push_back(Json{{"assignedTo", assigned},
                             {"completed", task.completed},
                             {"completedAtMs", toMillis(task.completedAt)},
                             {"createdAtMs", toMillis(task.createdAt)},
                             {"createdBy", task.createdBy},
                             {"description", task.description},
                             {"dueDateMs", toMillis(task.dueDate)},
                             {"metadata", metadataJson},
                             {"result", task.result},
                             {"state", taskStateName(task.state)},
                             {"taskId", task.taskId}});
    }
    const Json document{{"config", configToJson(config_)},
                        {"createdAtMs", toMillis(createdAt_)},
                        {"creator", creator_},
                        {"id", id_},
                        {"members", members},
                        {"schema", "elizaos.workgroup"},
                        {"state", stateName(state_)},
                        {"taskCounter", taskCounter_},
                        {"tasks", tasks},
                        {"version", kWorkgroupSchemaVersion}};
    return document.dump();
}

std::shared_ptr<Workgroup> Workgroup::deserializeSnapshot(
    const std::string& payload) {
    if (payload.empty() || payload.size() > kMaxPayloadBytes) return nullptr;
    try {
        const Json document = Json::parse(payload);
        if (!hasOnlyKeys(document,
                         {"config", "createdAtMs", "creator", "id", "members",
                          "schema", "state", "taskCounter", "tasks", "version"}) ||
            document.at("schema").get<std::string>() != "elizaos.workgroup" ||
            document.at("version").get<int>() != kWorkgroupSchemaVersion) {
            return nullptr;
        }
        WorkgroupConfig config;
        if (!parseConfig(document.at("config"), config)) return nullptr;
        const std::string id = document.at("id").get<std::string>();
        const AgentId creator = document.at("creator").get<std::string>();
        const WorkgroupState state =
            parseState(document.at("state").get<std::string>());
        if (!isValidWorkgroupId(id) || !isValidId(creator) || !isValidState(state)) {
            return nullptr;
        }
        const Json& memberJson = document.at("members");
        const Json& taskJson = document.at("tasks");
        if (!memberJson.is_array() || memberJson.empty() ||
            memberJson.size() > kMaxMembers || !taskJson.is_array() ||
            taskJson.size() > kMaxTasks) {
            return nullptr;
        }
        auto restored = std::make_shared<Workgroup>(id, config, creator);
        restored->members_.clear();
        restored->tasks_.clear();
        const auto createdAtMs = document.at("createdAtMs").get<std::int64_t>();
        if (createdAtMs < 0) return nullptr;
        restored->createdAt_ = fromMillis(createdAtMs);
        restored->state_ = state;
        restored->taskCounter_ =
            document.at("taskCounter").get<std::uint64_t>();
        std::size_t leaderCount = 0U;
        for (const auto& value : memberJson) {
            if (!hasOnlyKeys(value,
                             {"active", "agentId", "assignedTaskIds", "joinedAtMs",
                              "role", "tasksCompleted"})) {
                return nullptr;
            }
            WorkgroupMember member;
            member.active = value.at("active").get<bool>();
            member.agentId = value.at("agentId").get<std::string>();
            member.assignedTaskIds =
                value.at("assignedTaskIds").get<std::vector<std::string>>();
            const auto joinedAtMs = value.at("joinedAtMs").get<std::int64_t>();
            if (joinedAtMs < 0) return nullptr;
            member.joinedAt = fromMillis(joinedAtMs);
            member.role = parseRole(value.at("role").get<std::string>());
            member.tasksCompleted = value.at("tasksCompleted").get<int>();
            if (!isValidId(member.agentId) || !isValidRole(member.role) ||
                !member.active || member.tasksCompleted < 0 ||
                !restored->members_.emplace(member.agentId, member).second) {
                return nullptr;
            }
            if (member.role == WorkgroupRole::LEADER) ++leaderCount;
        }
        if (leaderCount == 0U) {
            return nullptr;
        }
        std::unordered_map<AgentId, std::vector<std::string>> expectedAssignments;
        std::uint64_t nextTaskCounter = 0U;
        for (const auto& value : taskJson) {
            if (!hasOnlyKeys(value,
                             {"assignedTo", "completed", "completedAtMs",
                              "createdAtMs", "createdBy", "description", "dueDateMs",
                              "metadata", "result", "state", "taskId"})) {
                return nullptr;
            }
            WorkgroupTask task;
            task.assignedTo =
                value.at("assignedTo").get<std::vector<AgentId>>();
            task.completed = value.at("completed").get<bool>();
            task.completedAt =
                fromMillis(value.at("completedAtMs").get<std::int64_t>());
            task.createdAt =
                fromMillis(value.at("createdAtMs").get<std::int64_t>());
            task.createdBy = value.at("createdBy").get<std::string>();
            task.description = value.at("description").get<std::string>();
            task.dueDate = fromMillis(value.at("dueDateMs").get<std::int64_t>());
            task.metadata = value.at("metadata")
                                .get<std::unordered_map<std::string, std::string>>();
            task.result = value.at("result").get<std::string>();
            task.state = parseTaskState(value.at("state").get<std::string>());
            task.taskId = value.at("taskId").get<std::string>();
            const bool completionFieldsValid =
                task.state == WorkgroupTaskState::COMPLETED
                    ? isValidText(task.result) && task.completedAt >= task.createdAt
                    : task.result.empty() && toMillis(task.completedAt) == 0;
            const bool timestampsValid = toMillis(task.createdAt) >= 0 &&
                                         task.dueDate >= task.createdAt;
            const bool metadataValid = std::all_of(
                task.metadata.begin(), task.metadata.end(), [](const auto& entry) {
                    return isValidText(entry.first) &&
                           isValidText(entry.second, true);
                });
            if (!isValidId(task.taskId) || !isValidText(task.description) ||
                !isValidId(task.createdBy) || !timestampsValid ||
                !isValidTaskState(task.state) ||
                task.completed != (task.state == WorkgroupTaskState::COMPLETED) ||
                !completionFieldsValid || task.metadata.size() > 1024U ||
                !metadataValid ||
                !restored->tasks_.emplace(task.taskId, task).second) {
                return nullptr;
            }
            std::set<AgentId> unique;
            for (const auto& assignee : task.assignedTo) {
                const auto member = restored->members_.find(assignee);
                if (member == restored->members_.end() ||
                    member->second.role == WorkgroupRole::OBSERVER ||
                    !unique.insert(assignee).second) {
                    return nullptr;
                }
                if (task.state != WorkgroupTaskState::CANCELLED) {
                    expectedAssignments[assignee].push_back(task.taskId);
                }
            }
            if (!parseCounterSuffix(task.taskId, id + "_task_", nextTaskCounter)) {
                return nullptr;
            }
        }
        if ((!restored->tasks_.empty() && state == WorkgroupState::FORMING) ||
            restored->taskCounter_ < nextTaskCounter) {
            return nullptr;
        }
        std::unordered_map<AgentId, int> expectedCompletions;
        for (const auto& task : restored->tasks_) {
            if (task.second.state == WorkgroupTaskState::COMPLETED) {
                for (const auto& assignee : task.second.assignedTo) {
                    ++expectedCompletions[assignee];
                }
            }
        }
        for (auto& member : restored->members_) {
            auto actual = member.second.assignedTaskIds;
            auto expected = expectedAssignments[member.first];
            std::sort(actual.begin(), actual.end());
            std::sort(expected.begin(), expected.end());
            if (actual != expected ||
                std::adjacent_find(actual.begin(), actual.end()) != actual.end() ||
                member.second.tasksCompleted != expectedCompletions[member.first]) {
                return nullptr;
            }
        }
        return restored;
    } catch (...) {
        return nullptr;
    }
}

WorkgroupManager::WorkgroupManager(
    std::shared_ptr<CommunicationHub> commHub,
    std::shared_ptr<AgentMemoryManager> memoryMgr,
    std::shared_ptr<WorkgroupPersistenceAdapter> persistenceAdapter)
    : commHub_(std::move(commHub)),
      memoryMgr_(std::move(memoryMgr)),
      persistenceAdapter_(std::move(persistenceAdapter)),
      workgroupCounter_(0U) {}

std::string WorkgroupManager::createWorkgroup(const WorkgroupConfig& config,
                                              const AgentId& creator) {
    if (!validConfig(config) || !isValidId(creator)) return {};
    std::lock_guard<std::mutex> lock(mutex_);
    const std::string id = generateWorkgroupIdUnlocked();
    if (id.empty()) return {};
    std::shared_ptr<Workgroup> workgroup;
    try {
        workgroup = std::make_shared<Workgroup>(id, config, creator);
    } catch (...) {
        return {};
    }
    workgroups_.emplace(id, workgroup);
    indexWorkgroupUnlocked(id, creator);
    return id;
}

bool WorkgroupManager::dissolveWorkgroup(const std::string& workgroupId,
                                         const AgentId& requestor) {
    if (!isValidId(workgroupId) || !isValidId(requestor)) return false;
    std::lock_guard<std::mutex> managerLock(mutex_);
    const auto entry = workgroups_.find(workgroupId);
    if (entry == workgroups_.end()) return false;
    const auto& workgroup = entry->second;
    std::lock_guard<std::mutex> workgroupLock(workgroup->mutex_);
    if (!workgroup->hasPermissionUnlocked(requestor, "dissolve") ||
        !workgroup->transitionAllowedUnlocked(WorkgroupState::DISSOLVED)) {
        return false;
    }
    workgroup->state_ = WorkgroupState::DISSOLVED;
    for (const auto& member : workgroup->members_) {
        unindexWorkgroupUnlocked(workgroupId, member.first);
    }
    workgroups_.erase(entry);
    return true;
}

std::shared_ptr<Workgroup> WorkgroupManager::getWorkgroup(
    const std::string& workgroupId) const {
    if (!isValidId(workgroupId)) return nullptr;
    std::lock_guard<std::mutex> lock(mutex_);
    const auto entry = workgroups_.find(workgroupId);
    return entry == workgroups_.end() ? nullptr : entry->second;
}

std::vector<std::string> WorkgroupManager::getAllWorkgroupIds() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> ids;
    ids.reserve(workgroups_.size());
    for (const auto& entry : workgroups_) ids.push_back(entry.first);
    std::sort(ids.begin(), ids.end());
    return ids;
}

std::vector<std::string> WorkgroupManager::getWorkgroupsForAgent(
    const AgentId& agentId) const {
    if (!isValidId(agentId)) return {};
    std::lock_guard<std::mutex> managerLock(mutex_);
    std::vector<std::string> ids;
    for (const auto& entry : workgroups_) {
        std::lock_guard<std::mutex> workgroupLock(entry.second->mutex_);
        if (entry.second->members_.count(agentId) != 0U) ids.push_back(entry.first);
    }
    std::sort(ids.begin(), ids.end());
    return ids;
}

bool WorkgroupManager::joinWorkgroup(const std::string& workgroupId,
                                     const AgentId& agentId,
                                     WorkgroupRole role) {
    if (!isValidId(workgroupId) || !isValidId(agentId) || !isValidRole(role)) {
        return false;
    }
    std::lock_guard<std::mutex> managerLock(mutex_);
    const auto entry = workgroups_.find(workgroupId);
    if (entry == workgroups_.end()) return false;
    const auto& workgroup = entry->second;
    std::lock_guard<std::mutex> workgroupLock(workgroup->mutex_);
    if (workgroup->state_ == WorkgroupState::DISSOLVED ||
        workgroup->state_ == WorkgroupState::COMPLETING ||
        role == WorkgroupRole::LEADER ||
        workgroup->members_.count(agentId) != 0U ||
        workgroup->members_.size() >=
            static_cast<std::size_t>(workgroup->config_.maxMembers)) {
        return false;
    }
    workgroup->members_.emplace(agentId, WorkgroupMember(agentId, role));
    indexWorkgroupUnlocked(workgroupId, agentId);
    return true;
}

bool WorkgroupManager::leaveWorkgroup(const std::string& workgroupId,
                                      const AgentId& agentId) {
    if (!isValidId(workgroupId) || !isValidId(agentId)) return false;
    std::lock_guard<std::mutex> managerLock(mutex_);
    const auto entry = workgroups_.find(workgroupId);
    if (entry == workgroups_.end()) return false;
    const auto& workgroup = entry->second;
    std::lock_guard<std::mutex> workgroupLock(workgroup->mutex_);
    if (workgroup->state_ == WorkgroupState::DISSOLVED) return false;
    const auto member = workgroup->members_.find(agentId);
    if (member == workgroup->members_.end()) return false;
    if (member->second.role == WorkgroupRole::LEADER) {
        const auto leaders = std::count_if(
            workgroup->members_.begin(), workgroup->members_.end(),
            [](const auto& value) {
                return value.second.role == WorkgroupRole::LEADER;
            });
        if (leaders <= 1) return false;
    }
    for (auto& task : workgroup->tasks_) {
        auto& assigned = task.second.assignedTo;
        assigned.erase(std::remove(assigned.begin(), assigned.end(), agentId),
                       assigned.end());
    }
    workgroup->members_.erase(member);
    unindexWorkgroupUnlocked(workgroupId, agentId);
    return true;
}

std::vector<std::string> WorkgroupManager::findWorkgroupsByPurpose(
    const std::string& purposeKeyword) const {
    if (!isValidText(purposeKeyword)) return {};
    std::lock_guard<std::mutex> managerLock(mutex_);
    std::vector<std::string> result;
    for (const auto& entry : workgroups_) {
        std::lock_guard<std::mutex> workgroupLock(entry.second->mutex_);
        if (entry.second->config_.purpose.find(purposeKeyword) !=
            std::string::npos) {
            result.push_back(entry.first);
        }
    }
    std::sort(result.begin(), result.end());
    return result;
}

std::vector<std::string> WorkgroupManager::findWorkgroupsByMember(
    const AgentId& agentId) const {
    return getWorkgroupsForAgent(agentId);
}

int WorkgroupManager::getTotalWorkgroups() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return static_cast<int>(workgroups_.size());
}

int WorkgroupManager::getActiveWorkgroups() const {
    std::lock_guard<std::mutex> managerLock(mutex_);
    int count = 0;
    for (const auto& entry : workgroups_) {
        std::lock_guard<std::mutex> workgroupLock(entry.second->mutex_);
        if (entry.second->state_ == WorkgroupState::ACTIVE) ++count;
    }
    return count;
}

void WorkgroupManager::setPersistenceAdapter(
    std::shared_ptr<WorkgroupPersistenceAdapter> persistenceAdapter) {
    std::lock_guard<std::mutex> lock(mutex_);
    persistenceAdapter_ = std::move(persistenceAdapter);
}

bool WorkgroupManager::saveWorkgroupState(const std::string& workgroupId) {
    if (!isValidId(workgroupId)) return false;
    std::shared_ptr<WorkgroupPersistenceAdapter> adapter;
    std::shared_ptr<Workgroup> workgroup;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        adapter = persistenceAdapter_;
        const auto entry = workgroups_.find(workgroupId);
        if (!adapter || entry == workgroups_.end()) return false;
        workgroup = entry->second;
    }
    try {
        return adapter->save(workgroupId, workgroup->serializeSnapshot());
    } catch (...) {
        return false;
    }
}

bool WorkgroupManager::loadWorkgroupState(const std::string& workgroupId) {
    if (!isValidId(workgroupId)) return false;
    std::shared_ptr<WorkgroupPersistenceAdapter> adapter;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        adapter = persistenceAdapter_;
    }
    if (!adapter) return false;
    std::string payload;
    try {
        if (!adapter->load(workgroupId, payload)) return false;
    } catch (...) {
        return false;
    }
    auto restored = Workgroup::deserializeSnapshot(payload);
    if (!restored || restored->getId() != workgroupId ||
        restored->getState() == WorkgroupState::DISSOLVED) {
        return false;
    }
    std::uint64_t nextCounter = 0U;
    if (!parseCounterSuffix(workgroupId, "wg_", nextCounter)) return false;
    try {
        std::lock_guard<std::mutex> lock(mutex_);
        auto stagedWorkgroups = workgroups_;
        stagedWorkgroups[workgroupId] = restored;
        std::unordered_map<AgentId, std::unordered_set<std::string>> stagedIndex;
        for (const auto& entry : stagedWorkgroups) {
            std::lock_guard<std::mutex> workgroupLock(entry.second->mutex_);
            for (const auto& member : entry.second->members_) {
                stagedIndex[member.first].insert(entry.first);
            }
        }
        workgroups_.swap(stagedWorkgroups);
        agentWorkgroups_.swap(stagedIndex);
        workgroupCounter_ = std::max(workgroupCounter_, nextCounter);
        return true;
    } catch (...) {
        return false;
    }
}

bool WorkgroupManager::saveAllWorkgroups() {
    std::shared_ptr<WorkgroupPersistenceAdapter> adapter;
    std::string payload;
    try {
        std::lock_guard<std::mutex> managerLock(mutex_);
        adapter = persistenceAdapter_;
        if (!adapter) return false;
        std::vector<std::pair<std::string, std::shared_ptr<Workgroup>>> snapshot(
            workgroups_.begin(), workgroups_.end());
        std::sort(snapshot.begin(), snapshot.end(), [](const auto& left,
                                                       const auto& right) {
            return left.first < right.first;
        });
        std::vector<std::unique_lock<std::mutex>> workgroupLocks;
        workgroupLocks.reserve(snapshot.size());
        for (const auto& entry : snapshot) {
            workgroupLocks.emplace_back(entry.second->mutex_);
        }
        Json groups = Json::array();
        for (const auto& entry : snapshot) {
            groups.push_back(Json::parse(entry.second->serializeSnapshotUnlocked()));
        }
        const Json document{{"counter", workgroupCounter_},
                            {"schema", "elizaos.workgroup_manager"},
                            {"version", kManagerSchemaVersion},
                            {"workgroups", groups}};
        payload = document.dump();
    } catch (...) {
        return false;
    }
    try {
        return adapter->save(kAllKey, payload);
    } catch (...) {
        return false;
    }
}

bool WorkgroupManager::loadAllWorkgroups() {
    std::shared_ptr<WorkgroupPersistenceAdapter> adapter;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        adapter = persistenceAdapter_;
    }
    if (!adapter) return false;
    std::string payload;
    try {
        if (!adapter->load(kAllKey, payload) || payload.empty() ||
            payload.size() > kMaxPayloadBytes) {
            return false;
        }
        const Json document = Json::parse(payload);
        if (!hasOnlyKeys(document,
                         {"counter", "schema", "version", "workgroups"}) ||
            document.at("schema").get<std::string>() !=
                "elizaos.workgroup_manager" ||
            document.at("version").get<int>() != kManagerSchemaVersion ||
            !document.at("workgroups").is_array() ||
            document.at("workgroups").size() > kMaxMembers) {
            return false;
        }
        std::uint64_t counter = document.at("counter").get<std::uint64_t>();
        std::unordered_map<std::string, std::shared_ptr<Workgroup>> staged;
        std::unordered_map<AgentId, std::unordered_set<std::string>> stagedIndex;
        std::uint64_t minimumCounter = 0U;
        for (const auto& value : document.at("workgroups")) {
            auto workgroup = Workgroup::deserializeSnapshot(value.dump());
            if (!workgroup || workgroup->getState() == WorkgroupState::DISSOLVED ||
                !staged.emplace(workgroup->getId(), workgroup).second ||
                !parseCounterSuffix(workgroup->getId(), "wg_", minimumCounter)) {
                return false;
            }
            for (const auto& member : workgroup->getMembers()) {
                stagedIndex[member.agentId].insert(workgroup->getId());
            }
        }
        if (counter < minimumCounter) return false;
        std::lock_guard<std::mutex> lock(mutex_);
        workgroups_.swap(staged);
        agentWorkgroups_.swap(stagedIndex);
        workgroupCounter_ = counter;
        return true;
    } catch (...) {
        return false;
    }
}

std::string WorkgroupManager::generateWorkgroupIdUnlocked() {
    while (workgroupCounter_ != std::numeric_limits<std::uint64_t>::max()) {
        std::ostringstream id;
        id << "wg_" << std::setfill('0') << std::setw(12)
           << workgroupCounter_++;
        if (workgroups_.count(id.str()) == 0U) return id.str();
    }
    return {};
}

void WorkgroupManager::indexWorkgroupUnlocked(const std::string& workgroupId,
                                              const AgentId& agentId) {
    agentWorkgroups_[agentId].insert(workgroupId);
}

void WorkgroupManager::unindexWorkgroupUnlocked(const std::string& workgroupId,
                                                const AgentId& agentId) {
    const auto entry = agentWorkgroups_.find(agentId);
    if (entry == agentWorkgroups_.end()) return;
    entry->second.erase(workgroupId);
    if (entry->second.empty()) agentWorkgroups_.erase(entry);
}

void WorkgroupManager::rebuildIndexUnlocked() {
    agentWorkgroups_.clear();
    for (const auto& workgroup : workgroups_) {
        std::lock_guard<std::mutex> lock(workgroup.second->mutex_);
        for (const auto& member : workgroup.second->members_) {
            indexWorkgroupUnlocked(workgroup.first, member.first);
        }
    }
}

namespace workgroups_utils {

std::string workgroupRoleToString(WorkgroupRole role) {
    try {
        return roleName(role);
    } catch (...) {
        return "Unknown";
    }
}

WorkgroupRole stringToWorkgroupRole(const std::string& roleStr) {
    return parseRole(roleStr);
}

std::string workgroupStateToString(WorkgroupState state) {
    try {
        return stateName(state);
    } catch (...) {
        return "Unknown";
    }
}

WorkgroupState stringToWorkgroupState(const std::string& stateStr) {
    return parseState(stateStr);
}

std::string workgroupTaskStateToString(WorkgroupTaskState state) {
    try {
        return taskStateName(state);
    } catch (...) {
        return "Unknown";
    }
}

WorkgroupTaskState stringToWorkgroupTaskState(const std::string& stateStr) {
    return parseTaskState(stateStr);
}

} // namespace workgroups_utils
} // namespace elizaos
