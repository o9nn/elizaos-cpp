#include "elizaos/workgroups.hpp"
#include "elizaos/agentlogger.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <random>
#include <set>
#include <limits>

#include <nlohmann/json.hpp>

namespace elizaos {
namespace {

using json = nlohmann::json;
constexpr int kWorkgroupSnapshotVersion = 1;
constexpr const char* kWorkgroupSnapshotSchema = "elizaos.workgroup";
constexpr const char* kWorkgroupSnapshotTable = "workgroup_state";
constexpr const char* kWorkgroupRoomPrefix = "workgroup:";

std::int64_t workgroupTimeToMilliseconds(std::chrono::system_clock::time_point value) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(value.time_since_epoch()).count();
}

std::chrono::system_clock::time_point workgroupTimeFromMilliseconds(std::int64_t value) {
    return std::chrono::system_clock::time_point(std::chrono::milliseconds(value));
}

bool validRoleValue(int value) {
    return value >= static_cast<int>(WorkgroupRole::LEADER) &&
           value <= static_cast<int>(WorkgroupRole::OBSERVER);
}

bool validStateValue(int value) {
    return value >= static_cast<int>(WorkgroupState::FORMING) &&
           value <= static_cast<int>(WorkgroupState::DISSOLVED);
}

std::string snapshotRoom(const std::string& workgroupId) {
    return std::string(kWorkgroupRoomPrefix) + workgroupId;
}

int counterValueForId(const std::string& id) {
    if (id.rfind("wg_", 0) != 0 || id.size() <= 3) return 0;
    try {
        const long long parsed = std::stoll(id.substr(3));
        if (parsed < 0 || parsed >= std::numeric_limits<int>::max()) return 0;
        return static_cast<int>(parsed) + 1;
    } catch (...) {
        return 0;
    }
}

}  // namespace

// ============================================================================
// Workgroup Implementation
// ============================================================================

Workgroup::Workgroup(const std::string& id, 
                     const WorkgroupConfig& config,
                     const AgentId& creator)
    : id_(id), config_(config), creator_(creator), 
      state_(WorkgroupState::FORMING),
      createdAt_(std::chrono::system_clock::now()) {
    
    // Create dedicated communication channel for this workgroup
    channelId_ = "workgroup_" + id_;
    
    // Add creator as leader
    addMember(creator, WorkgroupRole::LEADER);
    
    AgentLogger logger;
    logger.log("Workgroup created: " + config_.name + " (ID: " + id_ + ")", 
               "Workgroups", "Creation", LogLevel::INFO);
}

bool Workgroup::addMember(const AgentId& agentId, WorkgroupRole role) {
    std::lock_guard<std::mutex> lock(membersMutex_);
    
    // Check if already a member
    if (members_.find(agentId) != members_.end()) {
        return false;
    }
    
    // Check max members limit
    if (static_cast<int>(members_.size()) >= config_.maxMembers) {
        return false;
    }
    
    // Add member
    members_[agentId] = WorkgroupMember(agentId, role);
    
    AgentLogger logger;
    logger.log("Agent " + agentId + " joined workgroup " + config_.name + 
               " as " + workgroups_utils::workgroupRoleToString(role),
               "Workgroups", "Membership", LogLevel::INFO);
    
    return true;
}

bool Workgroup::removeMember(const AgentId& agentId) {
    std::lock_guard<std::mutex> lock(membersMutex_);
    
    auto it = members_.find(agentId);
    if (it == members_.end()) {
        return false;
    }
    
    // Don't allow removing the creator if they're the only leader
    if (agentId == creator_) {
        int leaderCount = 0;
        for (const auto& [id, member] : members_) {
            if (member.role == WorkgroupRole::LEADER) {
                leaderCount++;
            }
        }
        if (leaderCount <= 1) {
            return false; // Must transfer leadership first
        }
    }
    
    members_.erase(it);
    
    AgentLogger logger;
    logger.log("Agent " + agentId + " left workgroup " + config_.name,
               "Workgroups", "Membership", LogLevel::INFO);
    
    return true;
}

bool Workgroup::updateMemberRole(const AgentId& agentId, WorkgroupRole newRole) {
    std::lock_guard<std::mutex> lock(membersMutex_);
    
    auto it = members_.find(agentId);
    if (it == members_.end()) {
        return false;
    }
    
    it->second.role = newRole;
    
    AgentLogger logger;
    logger.log("Agent " + agentId + " role updated to " + 
               workgroups_utils::workgroupRoleToString(newRole) + 
               " in workgroup " + config_.name,
               "Workgroups", "Membership", LogLevel::INFO);
    
    return true;
}

bool Workgroup::isMember(const AgentId& agentId) const {
    std::lock_guard<std::mutex> lock(membersMutex_);
    return members_.find(agentId) != members_.end();
}

WorkgroupRole Workgroup::getMemberRole(const AgentId& agentId) const {
    std::lock_guard<std::mutex> lock(membersMutex_);
    
    auto it = members_.find(agentId);
    if (it == members_.end()) {
        return WorkgroupRole::OBSERVER; // Default for non-members
    }
    
    return it->second.role;
}

std::vector<WorkgroupMember> Workgroup::getMembers() const {
    std::lock_guard<std::mutex> lock(membersMutex_);
    
    std::vector<WorkgroupMember> result;
    result.reserve(members_.size());
    
    for (const auto& [id, member] : members_) {
        result.push_back(member);
    }
    
    return result;
}

std::vector<AgentId> Workgroup::getMembersByRole(WorkgroupRole role) const {
    std::lock_guard<std::mutex> lock(membersMutex_);
    
    std::vector<AgentId> result;
    
    for (const auto& [id, member] : members_) {
        if (member.role == role) {
            result.push_back(id);
        }
    }
    
    return result;
}

std::string Workgroup::createTask(const std::string& description, 
                                 const AgentId& creator,
                                 const std::vector<AgentId>& assignees) {
    std::lock_guard<std::mutex> lock(tasksMutex_);
    
    std::string taskId = generateTaskId();
    auto task = std::make_shared<WorkgroupTask>(taskId, description, creator);
    task->assignedTo = assignees;
    
    tasks_[taskId] = task;
    
    // Update member task assignments
    {
        std::lock_guard<std::mutex> memberLock(membersMutex_);
        for (const auto& agentId : assignees) {
            auto it = members_.find(agentId);
            if (it != members_.end()) {
                it->second.assignedTaskIds.push_back(taskId);
            }
        }
    }
    
    AgentLogger logger;
    logger.log("Task created in workgroup " + config_.name + ": " + description,
               "Workgroups", "Task", LogLevel::INFO);
    
    return taskId;
}

bool Workgroup::assignTask(const std::string& taskId, const AgentId& agentId) {
    std::lock_guard<std::mutex> lock(tasksMutex_);
    
    auto taskIt = tasks_.find(taskId);
    if (taskIt == tasks_.end()) {
        return false;
    }
    
    // Check if already assigned
    auto& assignees = taskIt->second->assignedTo;
    if (std::find(assignees.begin(), assignees.end(), agentId) != assignees.end()) {
        return false; // Already assigned
    }
    
    assignees.push_back(agentId);
    
    // Update member assignment
    {
        std::lock_guard<std::mutex> memberLock(membersMutex_);
        auto memberIt = members_.find(agentId);
        if (memberIt != members_.end()) {
            memberIt->second.assignedTaskIds.push_back(taskId);
        }
    }
    
    return true;
}

bool Workgroup::completeTask(const std::string& taskId, const std::string& result) {
    std::lock_guard<std::mutex> lock(tasksMutex_);
    
    auto it = tasks_.find(taskId);
    if (it == tasks_.end()) {
        return false;
    }
    
    if (it->second->completed) {
        return false; // Already completed
    }
    
    it->second->completed = true;
    it->second->result = result;
    
    // Update member completion count
    {
        std::lock_guard<std::mutex> memberLock(membersMutex_);
        for (const auto& agentId : it->second->assignedTo) {
            auto memberIt = members_.find(agentId);
            if (memberIt != members_.end()) {
                memberIt->second.tasksCompleted++;
            }
        }
    }
    
    AgentLogger logger;
    logger.log("Task completed in workgroup " + config_.name + ": " + taskId,
               "Workgroups", "Task", LogLevel::SUCCESS);
    
    return true;
}

bool Workgroup::cancelTask(const std::string& taskId) {
    std::lock_guard<std::mutex> lock(tasksMutex_);
    
    auto it = tasks_.find(taskId);
    if (it == tasks_.end()) {
        return false;
    }
    
    // Remove from member assignments
    {
        std::lock_guard<std::mutex> memberLock(membersMutex_);
        for (const auto& agentId : it->second->assignedTo) {
            auto memberIt = members_.find(agentId);
            if (memberIt != members_.end()) {
                auto& taskIds = memberIt->second.assignedTaskIds;
                taskIds.erase(std::remove(taskIds.begin(), taskIds.end(), taskId), 
                            taskIds.end());
            }
        }
    }
    
    tasks_.erase(it);
    return true;
}

std::shared_ptr<WorkgroupTask> Workgroup::getTask(const std::string& taskId) const {
    std::lock_guard<std::mutex> lock(tasksMutex_);
    
    auto it = tasks_.find(taskId);
    if (it == tasks_.end()) {
        return nullptr;
    }
    
    return it->second;
}

std::vector<std::shared_ptr<WorkgroupTask>> Workgroup::getAllTasks() const {
    std::lock_guard<std::mutex> lock(tasksMutex_);
    
    std::vector<std::shared_ptr<WorkgroupTask>> result;
    result.reserve(tasks_.size());
    
    for (const auto& [id, task] : tasks_) {
        result.push_back(task);
    }
    
    return result;
}

std::vector<std::shared_ptr<WorkgroupTask>> Workgroup::getTasksForAgent(const AgentId& agentId) const {
    std::lock_guard<std::mutex> lock(tasksMutex_);
    
    std::vector<std::shared_ptr<WorkgroupTask>> result;
    
    for (const auto& [id, task] : tasks_) {
        if (std::find(task->assignedTo.begin(), task->assignedTo.end(), agentId) 
            != task->assignedTo.end()) {
            result.push_back(task);
        }
    }
    
    return result;
}

std::vector<std::shared_ptr<WorkgroupTask>> Workgroup::getPendingTasks() const {
    std::lock_guard<std::mutex> lock(tasksMutex_);
    
    std::vector<std::shared_ptr<WorkgroupTask>> result;
    
    for (const auto& [id, task] : tasks_) {
        if (!task->completed) {
            result.push_back(task);
        }
    }
    
    return result;
}

void Workgroup::broadcast(const Message& message) {
    // Broadcast to all members via the workgroup channel
    // This would integrate with CommunicationHub in a real implementation
    AgentLogger logger;
    logger.log("Broadcasting message in workgroup " + config_.name + ": " + message.content,
               "Workgroups", "Communication", LogLevel::INFO);
}

void Workgroup::sendToRole(const Message& message, WorkgroupRole role) {
    (void)message; // Suppress unused warning - would integrate with CommunicationHub
    std::lock_guard<std::mutex> lock(membersMutex_);
    
    for (const auto& [id, member] : members_) {
        if (member.role == role) {
            // Send message to this member
            // Would integrate with CommunicationHub
        }
    }
}

void Workgroup::sendToMember(const Message& message, const AgentId& agentId) {
    (void)message; // Suppress unused warning - would integrate with CommunicationHub
    if (isMember(agentId)) {
        // Send message to specific member
        // Would integrate with CommunicationHub
    }
}

void Workgroup::setState(WorkgroupState newState) {
    state_ = newState;
    
    AgentLogger logger;
    logger.log("Workgroup " + config_.name + " state changed to " + 
               workgroups_utils::workgroupStateToString(newState),
               "Workgroups", "State", LogLevel::INFO);
}

WorkgroupStats Workgroup::getStats() const {
    WorkgroupStats stats;
    
    {
        std::lock_guard<std::mutex> lock(membersMutex_);
        stats.totalMembers = static_cast<int>(members_.size());
        stats.activeMembers = 0;
        for (const auto& [id, member] : members_) {
            if (member.active) {
                stats.activeMembers++;
            }
        }
    }
    
    {
        std::lock_guard<std::mutex> lock(tasksMutex_);
        stats.totalTasks = static_cast<int>(tasks_.size());
        stats.completedTasks = 0;
        stats.pendingTasks = 0;
        
        for (const auto& [id, task] : tasks_) {
            if (task->completed) {
                stats.completedTasks++;
            } else {
                stats.pendingTasks++;
            }
        }
    }
    
    stats.createdAt = createdAt_;
    stats.averageTaskCompletionTime = std::chrono::milliseconds(0); // Would calculate from task history
    
    return stats;
}

bool Workgroup::hasPermission(const AgentId& agentId, const std::string& action) const {
    WorkgroupRole role = getMemberRole(agentId);
    
    if (action == "manage_members") {
        return role == WorkgroupRole::LEADER;
    } else if (action == "assign_tasks") {
        return role == WorkgroupRole::LEADER || role == WorkgroupRole::COORDINATOR;
    } else if (action == "complete_tasks") {
        return role != WorkgroupRole::OBSERVER;
    } else if (action == "send_messages") {
        return role != WorkgroupRole::OBSERVER;
    }
    
    return false;
}

bool Workgroup::checkPermission(const AgentId& agentId, WorkgroupRole requiredRole) const {
    WorkgroupRole memberRole = getMemberRole(agentId);
    
    // Role hierarchy: LEADER > COORDINATOR > MEMBER > OBSERVER
    int memberLevel = static_cast<int>(memberRole);
    int requiredLevel = static_cast<int>(requiredRole);
    
    return memberLevel <= requiredLevel;
}

std::string Workgroup::generateTaskId() {
    static std::atomic<int> counter{0};
    std::stringstream ss;
    ss << id_ << "_task_" << counter++;
    return ss.str();
}

// ============================================================================
// WorkgroupManager Implementation
// ============================================================================

WorkgroupManager::WorkgroupManager(std::shared_ptr<CommunicationHub> commHub,
                                 std::shared_ptr<AgentMemoryManager> memoryMgr)
    : commHub_(commHub), memoryMgr_(memoryMgr), workgroupCounter_(0) {
    
    AgentLogger logger;
    logger.log("WorkgroupManager initialized", "Workgroups", "Init", LogLevel::INFO);
}

std::string WorkgroupManager::createWorkgroup(const WorkgroupConfig& config, 
                                             const AgentId& creator) {
    std::lock_guard<std::mutex> lock(workgroupsMutex_);
    
    std::string workgroupId = generateWorkgroupId();
    auto workgroup = std::make_shared<Workgroup>(workgroupId, config, creator);
    
    workgroups_[workgroupId] = workgroup;
    indexWorkgroup(workgroupId, creator);
    
    AgentLogger logger;
    logger.log("Workgroup created: " + config.name + " by " + creator,
               "Workgroups", "Creation", LogLevel::SUCCESS);
    
    return workgroupId;
}

bool WorkgroupManager::dissolveWorkgroup(const std::string& workgroupId, 
                                        const AgentId& requestor) {
    std::lock_guard<std::mutex> lock(workgroupsMutex_);
    
    auto it = workgroups_.find(workgroupId);
    if (it == workgroups_.end()) {
        return false;
    }
    
    // Check permission
    if (!it->second->hasPermission(requestor, "manage_members")) {
        return false;
    }
    
    // Remove from agent index
    auto members = it->second->getMembers();
    for (const auto& member : members) {
        unindexWorkgroup(workgroupId, member.agentId);
    }
    
    // Set state to dissolved
    it->second->setState(WorkgroupState::DISSOLVED);
    
    // Remove from active workgroups
    workgroups_.erase(it);
    
    AgentLogger logger;
    logger.log("Workgroup dissolved: " + workgroupId,
               "Workgroups", "Dissolution", LogLevel::INFO);
    
    return true;
}

std::shared_ptr<Workgroup> WorkgroupManager::getWorkgroup(const std::string& workgroupId) const {
    std::lock_guard<std::mutex> lock(workgroupsMutex_);
    
    auto it = workgroups_.find(workgroupId);
    if (it == workgroups_.end()) {
        return nullptr;
    }
    
    return it->second;
}

std::vector<std::string> WorkgroupManager::getAllWorkgroupIds() const {
    std::lock_guard<std::mutex> lock(workgroupsMutex_);
    
    std::vector<std::string> result;
    result.reserve(workgroups_.size());
    
    for (const auto& [id, workgroup] : workgroups_) {
        result.push_back(id);
    }
    
    return result;
}

std::vector<std::string> WorkgroupManager::getWorkgroupsForAgent(const AgentId& agentId) const {
    std::lock_guard<std::mutex> lock(workgroupsMutex_);
    
    auto it = agentWorkgroups_.find(agentId);
    if (it == agentWorkgroups_.end()) {
        return {};
    }
    
    return std::vector<std::string>(it->second.begin(), it->second.end());
}

bool WorkgroupManager::joinWorkgroup(const std::string& workgroupId, 
                                    const AgentId& agentId, 
                                    WorkgroupRole role) {
    auto workgroup = getWorkgroup(workgroupId);
    if (!workgroup) {
        return false;
    }
    
    if (workgroup->addMember(agentId, role)) {
        std::lock_guard<std::mutex> lock(workgroupsMutex_);
        indexWorkgroup(workgroupId, agentId);
        return true;
    }
    
    return false;
}

bool WorkgroupManager::leaveWorkgroup(const std::string& workgroupId, 
                                     const AgentId& agentId) {
    auto workgroup = getWorkgroup(workgroupId);
    if (!workgroup) {
        return false;
    }
    
    if (workgroup->removeMember(agentId)) {
        std::lock_guard<std::mutex> lock(workgroupsMutex_);
        unindexWorkgroup(workgroupId, agentId);
        return true;
    }
    
    return false;
}

std::vector<std::string> WorkgroupManager::findWorkgroupsByPurpose(const std::string& purposeKeyword) const {
    std::lock_guard<std::mutex> lock(workgroupsMutex_);
    
    std::vector<std::string> result;
    
    for (const auto& [id, workgroup] : workgroups_) {
        if (workgroup->getPurpose().find(purposeKeyword) != std::string::npos) {
            result.push_back(id);
        }
    }
    
    return result;
}

std::vector<std::string> WorkgroupManager::findWorkgroupsByMember(const AgentId& agentId) const {
    return getWorkgroupsForAgent(agentId);
}

int WorkgroupManager::getTotalWorkgroups() const {
    std::lock_guard<std::mutex> lock(workgroupsMutex_);
    return static_cast<int>(workgroups_.size());
}

int WorkgroupManager::getActiveWorkgroups() const {
    std::lock_guard<std::mutex> lock(workgroupsMutex_);
    
    int count = 0;
    for (const auto& [id, workgroup] : workgroups_) {
        if (workgroup->isActive()) {
            count++;
        }
    }
    
    return count;
}

bool WorkgroupManager::saveWorkgroupState(const std::string& workgroupId) {
    if (!memoryMgr_ || workgroupId.empty()) return false;
    std::shared_ptr<Workgroup> workgroup;
    {
        std::lock_guard<std::mutex> lock(workgroupsMutex_);
        auto it = workgroups_.find(workgroupId);
        if (it == workgroups_.end() || !it->second || !it->second->config_.persistState) {
            return false;
        }
        workgroup = it->second;
    }

    try {
        const std::string data = serializeWorkgroup(*workgroup);
        const std::string room = snapshotRoom(workgroupId);
        const auto previous = memoryMgr_->getAllMemoriesFromTable(kWorkgroupSnapshotTable);
        std::vector<UUID> previousIds;
        for (const auto& memory : previous) {
            if (memory && memory->getRoomId() == room) previousIds.push_back(memory->getId());
        }

        const UUID snapshotId = generateUUID();
        auto snapshot = std::make_shared<Memory>(snapshotId, data, workgroupId, "workgroup-manager");
        snapshot->setRoomId(room);
        const UUID storedId = memoryMgr_->createMemory(snapshot, kWorkgroupSnapshotTable, false);
        if (storedId != snapshotId || !memoryMgr_->getMemoryById(snapshotId)) return false;
        for (const auto& id : previousIds) memoryMgr_->deleteMemory(id);
        return true;
    } catch (...) {
        return false;
    }
}

bool WorkgroupManager::loadWorkgroupState(const std::string& workgroupId) {
    if (!memoryMgr_ || workgroupId.empty()) return false;
    try {
        const std::string room = snapshotRoom(workgroupId);
        std::shared_ptr<Memory> newest;
        for (const auto& memory : memoryMgr_->getAllMemoriesFromTable(kWorkgroupSnapshotTable)) {
            if (!memory || memory->getRoomId() != room) continue;
            if (!newest || memory->getCreatedAt() > newest->getCreatedAt()) newest = memory;
        }
        if (!newest) return false;
        auto loaded = deserializeWorkgroup(newest->getContent());
        if (!loaded || loaded->id_ != workgroupId) return false;

        std::lock_guard<std::mutex> lock(workgroupsMutex_);
        auto existing = workgroups_.find(workgroupId);
        if (existing != workgroups_.end() && existing->second) {
            for (const auto& member : existing->second->getMembers()) {
                unindexWorkgroup(workgroupId, member.agentId);
            }
        }
        workgroups_[workgroupId] = loaded;
        for (const auto& member : loaded->getMembers()) indexWorkgroup(workgroupId, member.agentId);
        workgroupCounter_ = std::max(workgroupCounter_, counterValueForId(workgroupId));
        return true;
    } catch (...) {
        return false;
    }
}

bool WorkgroupManager::saveAllWorkgroups() {
    if (!memoryMgr_) return false;
    std::vector<std::string> persistentIds;
    {
        std::lock_guard<std::mutex> lock(workgroupsMutex_);
        for (const auto& [id, workgroup] : workgroups_) {
            if (workgroup && workgroup->config_.persistState) persistentIds.push_back(id);
        }
    }
    std::sort(persistentIds.begin(), persistentIds.end());
    bool success = true;
    for (const auto& id : persistentIds) success = saveWorkgroupState(id) && success;
    return success;
}

bool WorkgroupManager::loadAllWorkgroups() {
    if (!memoryMgr_) return false;
    try {
        std::unordered_map<std::string, std::shared_ptr<Memory>> newestById;
        for (const auto& memory : memoryMgr_->getAllMemoriesFromTable(kWorkgroupSnapshotTable)) {
            if (!memory || memory->getRoomId().rfind(kWorkgroupRoomPrefix, 0) != 0 ||
                memory->getEntityId().empty()) continue;
            auto& newest = newestById[memory->getEntityId()];
            if (!newest || memory->getCreatedAt() > newest->getCreatedAt()) newest = memory;
        }
        if (newestById.empty()) return false;

        std::unordered_map<std::string, std::shared_ptr<Workgroup>> loadedGroups;
        std::unordered_map<AgentId, std::unordered_set<std::string>> loadedIndex;
        int loadedCounter = 0;
        for (const auto& [id, memory] : newestById) {
            auto workgroup = deserializeWorkgroup(memory->getContent());
            if (!workgroup || workgroup->id_ != id || !loadedGroups.emplace(id, workgroup).second) {
                return false;
            }
            for (const auto& member : workgroup->getMembers()) {
                loadedIndex[member.agentId].insert(id);
            }
            loadedCounter = std::max(loadedCounter, counterValueForId(id));
        }

        std::lock_guard<std::mutex> lock(workgroupsMutex_);
        workgroups_.swap(loadedGroups);
        agentWorkgroups_.swap(loadedIndex);
        workgroupCounter_ = std::max(workgroupCounter_, loadedCounter);
        return true;
    } catch (...) {
        return false;
    }
}

std::string WorkgroupManager::generateWorkgroupId() {
    std::stringstream ss;
    ss << "wg_" << std::setfill('0') << std::setw(6) << workgroupCounter_++;
    return ss.str();
}

std::string WorkgroupManager::serializeWorkgroup(const Workgroup& workgroup) const {
    std::scoped_lock lock(workgroup.membersMutex_, workgroup.tasksMutex_);
    json members = json::array();
    std::vector<AgentId> memberIds;
    memberIds.reserve(workgroup.members_.size());
    for (const auto& [agentId, _] : workgroup.members_) memberIds.push_back(agentId);
    std::sort(memberIds.begin(), memberIds.end());
    for (const auto& agentId : memberIds) {
        const auto& member = workgroup.members_.at(agentId);
        members.push_back({
            {"agent_id", member.agentId}, {"role", static_cast<int>(member.role)},
            {"joined_at_ms", workgroupTimeToMilliseconds(member.joinedAt)},
            {"assigned_task_ids", member.assignedTaskIds},
            {"tasks_completed", member.tasksCompleted}, {"active", member.active}
        });
    }

    json tasks = json::array();
    std::vector<std::string> taskIds;
    taskIds.reserve(workgroup.tasks_.size());
    for (const auto& [taskId, _] : workgroup.tasks_) taskIds.push_back(taskId);
    std::sort(taskIds.begin(), taskIds.end());
    for (const auto& taskId : taskIds) {
        const auto& task = workgroup.tasks_.at(taskId);
        if (!task) throw std::invalid_argument("workgroup contains a null task");
        tasks.push_back({
            {"task_id", task->taskId}, {"description", task->description},
            {"created_by", task->createdBy}, {"assigned_to", task->assignedTo},
            {"metadata", task->metadata},
            {"created_at_ms", workgroupTimeToMilliseconds(task->createdAt)},
            {"due_date_ms", workgroupTimeToMilliseconds(task->dueDate)},
            {"completed", task->completed}, {"result", task->result}
        });
    }

    const json document = {
        {"schema", kWorkgroupSnapshotSchema}, {"version", kWorkgroupSnapshotVersion},
        {"saved_at_ms", workgroupTimeToMilliseconds(std::chrono::system_clock::now())},
        {"workgroup", {
            {"id", workgroup.id_}, {"creator", workgroup.creator_},
            {"state", static_cast<int>(workgroup.state_)}, {"channel_id", workgroup.channelId_},
            {"created_at_ms", workgroupTimeToMilliseconds(workgroup.createdAt_)},
            {"config", {
                {"name", workgroup.config_.name}, {"purpose", workgroup.config_.purpose},
                {"max_members", workgroup.config_.maxMembers},
                {"allow_self_join", workgroup.config_.allowSelfJoin},
                {"require_approval", workgroup.config_.requireApproval},
                {"persist_state", workgroup.config_.persistState},
                {"task_timeout_seconds", workgroup.config_.taskTimeout.count()}
            }},
            {"members", std::move(members)}, {"tasks", std::move(tasks)}
        }}
    };
    return document.dump();
}

std::shared_ptr<Workgroup> WorkgroupManager::deserializeWorkgroup(const std::string& data) const {
    const json document = json::parse(data);
    if (!document.is_object() || document.value("schema", "") != kWorkgroupSnapshotSchema ||
        document.value("version", 0) != kWorkgroupSnapshotVersion) return nullptr;
    const auto& value = document.at("workgroup");
    const std::string id = value.at("id").get<std::string>();
    const AgentId creator = value.at("creator").get<AgentId>();
    const int stateValue = value.at("state").get<int>();
    if (id.empty() || creator.empty() || !validStateValue(stateValue)) return nullptr;

    const auto& configValue = value.at("config");
    WorkgroupConfig config;
    config.name = configValue.at("name").get<std::string>();
    config.purpose = configValue.at("purpose").get<std::string>();
    config.maxMembers = configValue.at("max_members").get<int>();
    config.allowSelfJoin = configValue.at("allow_self_join").get<bool>();
    config.requireApproval = configValue.at("require_approval").get<bool>();
    config.persistState = configValue.at("persist_state").get<bool>();
    const auto timeoutSeconds = configValue.at("task_timeout_seconds").get<long long>();
    if (config.name.empty() || config.maxMembers <= 0 || timeoutSeconds <= 0) return nullptr;
    config.taskTimeout = std::chrono::seconds(timeoutSeconds);

    const auto& memberValues = value.at("members");
    const auto& taskValues = value.at("tasks");
    if (!memberValues.is_array() || !taskValues.is_array()) return nullptr;

    auto workgroup = std::make_shared<Workgroup>(id, config, creator);
    workgroup->state_ = static_cast<WorkgroupState>(stateValue);
    workgroup->channelId_ = value.at("channel_id").get<std::string>();
    workgroup->createdAt_ = workgroupTimeFromMilliseconds(
        value.at("created_at_ms").get<std::int64_t>());
    workgroup->members_.clear();
    workgroup->tasks_.clear();

    for (const auto& memberValue : memberValues) {
        const AgentId agentId = memberValue.at("agent_id").get<AgentId>();
        const int roleValue = memberValue.at("role").get<int>();
        if (agentId.empty() || !validRoleValue(roleValue)) return nullptr;
        WorkgroupMember member(agentId, static_cast<WorkgroupRole>(roleValue));
        member.joinedAt = workgroupTimeFromMilliseconds(
            memberValue.at("joined_at_ms").get<std::int64_t>());
        member.assignedTaskIds = memberValue.at("assigned_task_ids").get<std::vector<std::string>>();
        member.tasksCompleted = memberValue.at("tasks_completed").get<int>();
        member.active = memberValue.at("active").get<bool>();
        if (member.tasksCompleted < 0 || !workgroup->members_.emplace(agentId, member).second) {
            return nullptr;
        }
    }
    auto creatorIt = workgroup->members_.find(creator);
    if (creatorIt == workgroup->members_.end() || creatorIt->second.role != WorkgroupRole::LEADER ||
        static_cast<int>(workgroup->members_.size()) > config.maxMembers) return nullptr;

    for (const auto& taskValue : taskValues) {
        const std::string taskId = taskValue.at("task_id").get<std::string>();
        const std::string description = taskValue.at("description").get<std::string>();
        const AgentId createdBy = taskValue.at("created_by").get<AgentId>();
        if (taskId.empty() || description.empty() || createdBy.empty()) return nullptr;
        auto task = std::make_shared<WorkgroupTask>(taskId, description, createdBy);
        task->assignedTo = taskValue.at("assigned_to").get<std::vector<AgentId>>();
        task->metadata = taskValue.at("metadata").get<std::unordered_map<std::string, std::string>>();
        task->createdAt = workgroupTimeFromMilliseconds(
            taskValue.at("created_at_ms").get<std::int64_t>());
        task->dueDate = workgroupTimeFromMilliseconds(
            taskValue.at("due_date_ms").get<std::int64_t>());
        task->completed = taskValue.at("completed").get<bool>();
        task->result = taskValue.at("result").get<std::string>();
        std::set<AgentId> uniqueAssignees;
        for (const auto& assignee : task->assignedTo) {
            if (workgroup->members_.count(assignee) == 0 || !uniqueAssignees.insert(assignee).second) {
                return nullptr;
            }
        }
        if (!workgroup->tasks_.emplace(taskId, task).second) return nullptr;
    }

    for (const auto& [agentId, member] : workgroup->members_) {
        std::set<std::string> assignedIds;
        for (const auto& taskId : member.assignedTaskIds) {
            auto taskIt = workgroup->tasks_.find(taskId);
            if (taskIt == workgroup->tasks_.end() || !assignedIds.insert(taskId).second ||
                std::find(taskIt->second->assignedTo.begin(), taskIt->second->assignedTo.end(), agentId) ==
                    taskIt->second->assignedTo.end()) {
                return nullptr;
            }
        }
        for (const auto& [taskId, task] : workgroup->tasks_) {
            const bool taskAssignsMember = std::find(task->assignedTo.begin(), task->assignedTo.end(), agentId) !=
                                           task->assignedTo.end();
            if (taskAssignsMember != (assignedIds.count(taskId) != 0)) return nullptr;
        }
    }
    return workgroup;
}

void WorkgroupManager::indexWorkgroup(const std::string& workgroupId, const AgentId& agentId) {
    agentWorkgroups_[agentId].insert(workgroupId);
}

void WorkgroupManager::unindexWorkgroup(const std::string& workgroupId, const AgentId& agentId) {
    auto it = agentWorkgroups_.find(agentId);
    if (it != agentWorkgroups_.end()) {
        it->second.erase(workgroupId);
        if (it->second.empty()) {
            agentWorkgroups_.erase(it);
        }
    }
}

// ============================================================================
// Utility Functions
// ============================================================================

namespace workgroups_utils {

std::string workgroupRoleToString(WorkgroupRole role) {
    switch (role) {
        case WorkgroupRole::LEADER: return "Leader";
        case WorkgroupRole::COORDINATOR: return "Coordinator";
        case WorkgroupRole::MEMBER: return "Member";
        case WorkgroupRole::OBSERVER: return "Observer";
        default: return "Unknown";
    }
}

WorkgroupRole stringToWorkgroupRole(const std::string& roleStr) {
    if (roleStr == "Leader") return WorkgroupRole::LEADER;
    if (roleStr == "Coordinator") return WorkgroupRole::COORDINATOR;
    if (roleStr == "Member") return WorkgroupRole::MEMBER;
    if (roleStr == "Observer") return WorkgroupRole::OBSERVER;
    return WorkgroupRole::MEMBER; // Default
}

std::string workgroupStateToString(WorkgroupState state) {
    switch (state) {
        case WorkgroupState::FORMING: return "Forming";
        case WorkgroupState::ACTIVE: return "Active";
        case WorkgroupState::PAUSED: return "Paused";
        case WorkgroupState::COMPLETING: return "Completing";
        case WorkgroupState::DISSOLVED: return "Dissolved";
        default: return "Unknown";
    }
}

WorkgroupState stringToWorkgroupState(const std::string& stateStr) {
    if (stateStr == "Forming") return WorkgroupState::FORMING;
    if (stateStr == "Active") return WorkgroupState::ACTIVE;
    if (stateStr == "Paused") return WorkgroupState::PAUSED;
    if (stateStr == "Completing") return WorkgroupState::COMPLETING;
    if (stateStr == "Dissolved") return WorkgroupState::DISSOLVED;
    return WorkgroupState::FORMING; // Default
}

} // namespace workgroups_utils

} // namespace elizaos
