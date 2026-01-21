#include "elizaos/workgroups.hpp"
#include "elizaos/agentlogger.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <random>

namespace elizaos {

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
    (void)workgroupId; // Suppress unused warning - would integrate with AgentMemoryManager
    // Would integrate with AgentMemoryManager for persistence
    // Serialize workgroup state to memory/database
    return true;
}

bool WorkgroupManager::loadWorkgroupState(const std::string& workgroupId) {
    (void)workgroupId; // Suppress unused warning - would integrate with AgentMemoryManager
    // Would integrate with AgentMemoryManager for persistence
    // Deserialize workgroup state from memory/database
    return true;
}

bool WorkgroupManager::saveAllWorkgroups() {
    std::lock_guard<std::mutex> lock(workgroupsMutex_);
    
    for (const auto& [id, workgroup] : workgroups_) {
        saveWorkgroupState(id);
    }
    
    return true;
}

bool WorkgroupManager::loadAllWorkgroups() {
    // Would load all persisted workgroups from storage
    return true;
}

std::string WorkgroupManager::generateWorkgroupId() {
    std::stringstream ss;
    ss << "wg_" << std::setfill('0') << std::setw(6) << workgroupCounter_++;
    return ss.str();
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
