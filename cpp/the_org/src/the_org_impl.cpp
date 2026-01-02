#include "elizaos/the_org.hpp"
#include "elizaos/agentlogger.hpp"
#include <algorithm>
#include <sstream>
#include <random>
#include <iomanip>

namespace elizaos {

// Utility functions
namespace the_org_utils {
    
    UUID generateAgentId(AgentRole /* role */) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 15);
        
        std::stringstream ss;
        ss << std::hex;
        for (int i = 0; i < 32; ++i) {
            ss << dis(gen);
            if (i == 7 || i == 11 || i == 15 || i == 19) {
                ss << "-";
            }
        }
        return ss.str();
    }
    
    std::string agentRoleToString(AgentRole role) {
        switch (role) {
            case AgentRole::COMMUNITY_MANAGER: return "Community Manager";
            case AgentRole::DEVELOPER_RELATIONS: return "Developer Relations";
            case AgentRole::COMMUNITY_LIAISON: return "Community Liaison";
            case AgentRole::PROJECT_MANAGER: return "Project Manager";
            case AgentRole::SOCIAL_MEDIA_MANAGER: return "Social Media Manager";
            default: return "Unknown";
        }
    }
    
    std::string platformTypeToString(PlatformType type) {
        switch (type) {
            case PlatformType::DISCORD: return "Discord";
            case PlatformType::TELEGRAM: return "Telegram";
            case PlatformType::TWITTER: return "Twitter";
            case PlatformType::SLACK: return "Slack";
            case PlatformType::FACEBOOK: return "Facebook";
            case PlatformType::LINKEDIN: return "LinkedIn";
            case PlatformType::GITHUB: return "GitHub";
            default: return "Unknown";
        }
    }
}

// ============================================================================
// TheOrgAgent Base Class Implementation
// ============================================================================

TheOrgAgent::TheOrgAgent(const AgentConfig& config, AgentRole role)
    : config_(config), role_(role), state_(config) {
}

std::shared_ptr<Memory> TheOrgAgent::createMemory(const std::string& content, MemoryType /* type */) {
    UUID memId = config_.agentId + "-mem-" + std::to_string(memoryStore_.size());
    auto memory = std::make_shared<Memory>(memId, content, config_.agentId, config_.agentId);
    return memory;
}

void TheOrgAgent::addMemory(std::shared_ptr<Memory> memory) {
    std::lock_guard<std::mutex> lock(memoryMutex_);
    memoryStore_.push_back(memory);
}

std::vector<std::shared_ptr<Memory>> TheOrgAgent::searchMemories(const std::string& query, size_t maxResults) {
    std::lock_guard<std::mutex> lock(memoryMutex_);
    std::vector<std::shared_ptr<Memory>> results;
    
    for (const auto& memory : memoryStore_) {
        if (memory->getContent().find(query) != std::string::npos) {
            results.push_back(memory);
            if (results.size() >= maxResults) break;
        }
    }
    
    return results;
}

void TheOrgAgent::addPlatform(const PlatformConfig& platform) {
    std::lock_guard<std::mutex> lock(platformMutex_);
    platforms_[platform.type] = platform;
}

void TheOrgAgent::removePlatform(PlatformType type) {
    std::lock_guard<std::mutex> lock(platformMutex_);
    platforms_.erase(type);
}

bool TheOrgAgent::sendMessage(PlatformType platform, const std::string& channelId, const std::string& /* message */) {
    std::lock_guard<std::mutex> lock(platformMutex_);
    
    if (platforms_.find(platform) == platforms_.end()) {
        return false;
    }
    
    // Mock implementation - in real system would use platform API
    AgentLogger logger;
    logger.log("Agent " + config_.agentName + " sending message to " + 
        the_org_utils::platformTypeToString(platform) + " channel " + channelId);
    
    return true;
}

std::vector<std::string> TheOrgAgent::getRecentMessages(PlatformType /* platform */, const std::string& /* channelId */, size_t /* count */) {
    // Mock implementation
    return std::vector<std::string>();
}

void TheOrgAgent::sendToAgent(const UUID& agentId, const std::string& message, const std::string& /* type */) {
    // Mock implementation - in real system would use message broker
    AgentLogger logger;
    logger.log("Agent " + config_.agentName + " sending to agent " + agentId + ": " + message);
}

std::queue<std::string> TheOrgAgent::getIncomingMessages() {
    std::lock_guard<std::mutex> lock(messageMutex_);
    return incomingMessages_;
}

void TheOrgAgent::processMessage(const std::string& message, const std::string& /* senderId */) {
    std::lock_guard<std::mutex> lock(messageMutex_);
    incomingMessages_.push(message);
}

UUID TheOrgAgent::createTask(const std::string& /* name */, const std::string& /* description */, int /* priority */) {
    // Mock implementation
    return config_.agentId + "-task-" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
}

bool TheOrgAgent::completeTask(const UUID& /* taskId */) {
    // Mock implementation
    return true;
}

std::vector<std::shared_ptr<Task>> TheOrgAgent::getPendingTasks() {
    // Mock implementation
    return std::vector<std::shared_ptr<Task>>();
}

void TheOrgAgent::updateConfig(const std::unordered_map<std::string, std::string>& settings) {
    std::lock_guard<std::mutex> lock(settingsMutex_);
    for (const auto& [key, value] : settings) {
        settings_[key] = value;
    }
}

std::string TheOrgAgent::getConfigValue(const std::string& key) const {
    std::lock_guard<std::mutex> lock(settingsMutex_);
    auto it = settings_.find(key);
    return (it != settings_.end()) ? it->second : "";
}

bool TheOrgAgent::validateMessage(const std::string& message) const {
    return !message.empty() && message.length() < 10000;
}

std::string TheOrgAgent::formatResponse(const std::string& response, PlatformType /* platform */) const {
    // Basic formatting - could be extended per platform
    return response;
}

// ============================================================================
// CommunityManagerAgent Implementation
// ============================================================================

CommunityManagerAgent::CommunityManagerAgent(const AgentConfig& config)
    : TheOrgAgent(config, AgentRole::COMMUNITY_MANAGER) {
}

void CommunityManagerAgent::initialize() {
    AgentLogger logger;
    logger.log("Initializing Community Manager Agent: " + config_.agentName);
    
    // Initialize default moderation rules
    moderationRules_["spam"] = {ModerationAction::WARNING, "Spam content detected"};
    moderationRules_["toxic"] = {ModerationAction::TIMEOUT, "Toxic behavior"};
    
    // Initialize metrics
    currentMetrics_ = CommunityMetrics{};
    currentMetrics_.lastUpdated = std::chrono::system_clock::now();
}

void CommunityManagerAgent::start() {
    if (running_) return;
    
    running_ = true;
    paused_ = false;
    
    processingThread_ = std::thread(&CommunityManagerAgent::processLoop, this);
    
    AgentLogger logger;
    logger.log("Community Manager Agent started: " + config_.agentName);
}

void CommunityManagerAgent::stop() {
    if (!running_) return;
    
    running_ = false;
    
    if (processingThread_.joinable()) {
        processingThread_.join();
    }
    
    AgentLogger logger;
    logger.log("Community Manager Agent stopped: " + config_.agentName);
}

void CommunityManagerAgent::pause() {
    paused_ = true;
}

void CommunityManagerAgent::resume() {
    paused_ = false;
}

bool CommunityManagerAgent::isRunning() const {
    return running_;
}

void CommunityManagerAgent::processLoop() {
    while (running_) {
        if (!paused_) {
            // Process incoming messages
            auto messages = getIncomingMessages();
            
            // Update metrics periodically
            updateCommunityMetrics();
        }
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void CommunityManagerAgent::enableNewUserGreeting(const std::string& channelId, const std::string& greetingMessage) {
    greetingEnabled_ = true;
    greetingChannelId_ = channelId;
    customGreetingMessage_ = greetingMessage;
}

void CommunityManagerAgent::disableNewUserGreeting() {
    greetingEnabled_ = false;
}

bool CommunityManagerAgent::shouldGreetNewUser(const std::string& /* userId */) const {
    return greetingEnabled_;
}

std::string CommunityManagerAgent::generateGreeting(const std::string& userName, const std::string& serverName) const {
    if (!customGreetingMessage_.empty()) {
        std::string greeting = customGreetingMessage_;
        size_t pos = greeting.find("{user}");
        if (pos != std::string::npos) {
            greeting.replace(pos, 6, userName);
        }
        return greeting;
    }
    
    std::vector<std::string> greetings = {
        "Welcome to " + serverName + ", " + userName + "! 🎉",
        "Hey " + userName + "! Great to have you here! 👋",
        "Welcome aboard, " + userName + "! Feel free to introduce yourself! 🌟",
        userName + " has joined the party! Welcome! 🎊"
    };
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, greetings.size() - 1);
    
    return greetings[dis(gen)];
}

void CommunityManagerAgent::addModerationRule(const std::string& rule, ModerationAction action, const std::string& reason) {
    std::lock_guard<std::mutex> lock(rulesMutex_);
    moderationRules_[rule] = {action, reason};
}

void CommunityManagerAgent::removeModerationRule(const std::string& rule) {
    std::lock_guard<std::mutex> lock(rulesMutex_);
    moderationRules_.erase(rule);
}

bool CommunityManagerAgent::evaluateMessage(const std::string& message, const std::string& userId, const std::string& /* channelId */) {
    std::lock_guard<std::mutex> lock(rulesMutex_);
    
    std::string lowerMessage = message;
    std::transform(lowerMessage.begin(), lowerMessage.end(), lowerMessage.begin(), ::tolower);
    
    for (const auto& [rule, actionInfo] : moderationRules_) {
        if (lowerMessage.find(rule) != std::string::npos) {
            applyModerationAction(userId, actionInfo.first, actionInfo.second);
            return false;
        }
    }
    
    return true;
}

void CommunityManagerAgent::applyModerationAction(const std::string& userId, ModerationAction action, 
                                                   const std::string& reason, std::optional<std::chrono::seconds> duration) {
    ModerationEvent event;
    event.id = config_.agentId + "-mod-" + std::to_string(moderationHistory_.size());
    event.userId = userId;
    event.moderatorId = config_.agentId;
    event.action = action;
    event.reason = reason;
    event.duration = duration;
    event.timestamp = std::chrono::system_clock::now();
    
    moderationHistory_.push_back(event);
    
    AgentLogger logger;
    logger.log("Moderation action applied to user " + userId + ": " + reason, "", "Moderation", LogLevel::WARNING);
}

CommunityMetrics CommunityManagerAgent::generateCommunityMetrics() const {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    return currentMetrics_;
}

void CommunityManagerAgent::trackUserActivity(const std::string& userId, const std::string& /* activity */) {
    std::lock_guard<std::mutex> lock(activityMutex_);
    userActivity_[userId].push_back(std::chrono::system_clock::now());
}

std::vector<std::string> CommunityManagerAgent::identifyActiveUsers(std::chrono::hours timeWindow) const {
    std::lock_guard<std::mutex> lock(activityMutex_);
    std::vector<std::string> activeUsers;
    
    auto now = std::chrono::system_clock::now();
    auto cutoff = now - timeWindow;
    
    for (const auto& [userId, activities] : userActivity_) {
        for (const auto& timestamp : activities) {
            if (timestamp >= cutoff) {
                activeUsers.push_back(userId);
                break;
            }
        }
    }
    
    return activeUsers;
}

std::vector<std::string> CommunityManagerAgent::getTopTopics(std::chrono::hours /* timeWindow */) const {
    // Mock implementation - would analyze message content in real system
    return {"ElizaOS Development", "Agent Architecture", "Community Events"};
}

void CommunityManagerAgent::initiateConflictResolution(const std::vector<std::string>& /* userIds */, const std::string& /* channelId */) {
    // Mock implementation
    AgentLogger logger;
    logger.log("Initiating conflict resolution");
}

void CommunityManagerAgent::escalateIssue(const std::string& description, const std::vector<std::string>& /* involvedUsers */) {
    // Mock implementation
    AgentLogger logger;
    logger.log("Issue escalated: " + description, "", "Escalation", LogLevel::WARNING);
}

void CommunityManagerAgent::scheduleEvent(const std::string& /* eventName */, const std::string& /* description */, Timestamp /* scheduledTime */) {
    // Mock implementation
}

void CommunityManagerAgent::announceEvent(const std::string& /* eventId */, const std::vector<std::string>& /* channelIds */) {
    // Mock implementation
}

void CommunityManagerAgent::trackEventParticipation(const std::string& /* eventId */, const std::string& /* userId */) {
    // Mock implementation
}

void CommunityManagerAgent::processNewUserJoin(const std::string& /* userId */, const std::string& /* serverId */) {
    // Mock implementation
}

void CommunityManagerAgent::processMessageForModeration(const std::string& message, const std::string& userId, const std::string& channelId) {
    evaluateMessage(message, userId, channelId);
}

void CommunityManagerAgent::generateDailyReport() {
    // Mock implementation
}

void CommunityManagerAgent::updateCommunityMetrics() {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    
    currentMetrics_.totalMembers = 100;  // Mock data
    currentMetrics_.activeMembers = identifyActiveUsers(std::chrono::hours(24)).size();
    currentMetrics_.newMembersToday = 5;
    currentMetrics_.messagesPerDay = 250;
    currentMetrics_.engagementRate = 0.45;
    currentMetrics_.topTopics = getTopTopics(std::chrono::hours(24));
    currentMetrics_.lastUpdated = std::chrono::system_clock::now();
}

// ============================================================================
// DeveloperRelationsAgent Implementation
// ============================================================================

DeveloperRelationsAgent::DeveloperRelationsAgent(const AgentConfig& config)
    : TheOrgAgent(config, AgentRole::DEVELOPER_RELATIONS) {
}

void DeveloperRelationsAgent::initialize() {
    AgentLogger logger;
    logger.log("Initializing Developer Relations Agent: " + config_.agentName);
}

void DeveloperRelationsAgent::start() {
    if (running_) return;
    running_ = true;
    paused_ = false;
    
    AgentLogger logger;
    logger.log("Developer Relations Agent started: " + config_.agentName);
}

void DeveloperRelationsAgent::stop() {
    if (!running_) return;
    running_ = false;
    
    AgentLogger logger;
    logger.log("Developer Relations Agent stopped: " + config_.agentName);
}

void DeveloperRelationsAgent::pause() {
    paused_ = true;
}

void DeveloperRelationsAgent::resume() {
    paused_ = false;
}

bool DeveloperRelationsAgent::isRunning() const {
    return running_;
}

void DeveloperRelationsAgent::processLoop() {
    // Mock implementation
}

void DeveloperRelationsAgent::indexDocumentation(const std::string& docPath, const std::string& version) {
    AgentLogger logger;
    logger.log("Indexed documentation: " + docPath + " (version: " + version + ")");
}

void DeveloperRelationsAgent::addTechnicalKnowledge(const std::string& topic, const std::string& /* content */, 
                                                     const std::vector<std::string>& /* tags */) {
    AgentLogger logger;
    logger.log("Added knowledge: " + topic);
}

std::vector<std::string> DeveloperRelationsAgent::searchDocumentation(const std::string& /* query */) const {
    // Mock implementation
    return {"/docs/getting-started.md", "/docs/agent-development.md"};
}

// ============================================================================
// ProjectManagerAgent Implementation
// ============================================================================

ProjectManagerAgent::ProjectManagerAgent(const AgentConfig& config)
    : TheOrgAgent(config, AgentRole::PROJECT_MANAGER) {
}

void ProjectManagerAgent::initialize() {
    AgentLogger logger;
    logger.log("Initializing Project Manager Agent: " + config_.agentName);
}

void ProjectManagerAgent::start() {
    if (running_) return;
    running_ = true;
    paused_ = false;
}

void ProjectManagerAgent::stop() {
    if (!running_) return;
    running_ = false;
}

void ProjectManagerAgent::pause() {
    paused_ = true;
}

void ProjectManagerAgent::resume() {
    paused_ = false;
}

bool ProjectManagerAgent::isRunning() const {
    return running_;
}

void ProjectManagerAgent::processLoop() {
    // Mock implementation
}

UUID ProjectManagerAgent::createProject(const std::string& /* name */, const std::string& /* description */, const std::vector<UUID>& /* teamMemberIds */) {
    return config_.agentId + "-project-" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
}

UUID ProjectManagerAgent::addTeamMember(const TeamMember& /* member */) {
    // Mock implementation
    return config_.agentId + "-member-" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
}

void ProjectManagerAgent::recordDailyUpdate(const DailyUpdate& /* update */) {
    // Mock implementation
}

std::string ProjectManagerAgent::generateProjectStatusReport(const UUID& /* projectId */) const {
    return "Project Status Report: Mock implementation";
}

std::string ProjectManagerAgent::generateWeeklyReport(const std::vector<UUID>& /* projectIds */) const {
    return "Weekly Report: Mock implementation";
}

void ProjectManagerAgent::sendCheckinReminder(const UUID& /* teamMemberId */, const UUID& /* projectId */) {
    // Mock implementation
}

// ============================================================================
// TheOrgManager Implementation
// ============================================================================

TheOrgManager::TheOrgManager() {
}

TheOrgManager::~TheOrgManager() {
    stopAllAgents();
}

void TheOrgManager::addAgent(std::shared_ptr<TheOrgAgent> agent) {
    agents_[agent->getId()] = agent;
}

void TheOrgManager::removeAgent(const UUID& agentId) {
    auto it = agents_.find(agentId);
    if (it != agents_.end()) {
        it->second->stop();
        agents_.erase(it);
    }
}

std::shared_ptr<TheOrgAgent> TheOrgManager::getAgent(const UUID& agentId) const {
    auto it = agents_.find(agentId);
    return (it != agents_.end()) ? it->second : nullptr;
}

void TheOrgManager::startAllAgents() {
    for (auto& [id, agent] : agents_) {
        agent->start();
    }
}

void TheOrgManager::stopAllAgents() {
    for (auto& [id, agent] : agents_) {
        agent->stop();
    }
}

std::shared_ptr<TheOrgAgent> TheOrgManager::getAgentByRole(AgentRole role) const {
    for (const auto& [id, agent] : agents_) {
        if (agent->getRole() == role) {
            return agent;
        }
    }
    return nullptr;
}

// Additional TheOrgManager methods
void TheOrgManager::initializeAllAgents(const std::vector<AgentConfig>& /* configs */) {
    // Mock implementation
}

void TheOrgManager::broadcastMessage(const std::string& /* message */, const std::string& /* senderId */, 
                                     const std::vector<AgentRole>& /* targetRoles */) {
    // Mock implementation
}

TheOrgManager::SystemMetrics TheOrgManager::getSystemMetrics() const {
    SystemMetrics metrics;
    metrics.totalAgents = agents_.size();
    metrics.activeAgents = 0;
    for (const auto& [id, agent] : agents_) {
        if (agent->isRunning()) {
            metrics.activeAgents++;
        }
    }
    metrics.totalTasks = 0;
    metrics.pendingTasks = 0;
    metrics.systemLoad = 0.0;
    metrics.averageResponseTime = std::chrono::milliseconds(0);
    metrics.lastUpdated = std::chrono::system_clock::now();
    return metrics;
}

std::vector<std::shared_ptr<TheOrgAgent>> TheOrgManager::getAllAgents() const {
    std::vector<std::shared_ptr<TheOrgAgent>> result;
    for (const auto& [id, agent] : agents_) {
        result.push_back(agent);
    }
    return result;
}

// Additional DeveloperRelationsAgent methods
std::string DeveloperRelationsAgent::retrieveKnowledge(const std::string& topic) const {
    return "Knowledge about " + topic + ": Mock implementation";
}

std::string DeveloperRelationsAgent::generateCodeExample(const std::string& topic, const std::string& language) const {
    return "// Code example for " + topic + " in " + language + "\n// Mock implementation\n";
}

// Additional utility functions
namespace the_org_utils {
    std::string formatTimestamp(Timestamp timestamp, const std::string& /* format */) {
        auto time_t = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::ctime(&time_t);
        std::string result = ss.str();
        // Remove trailing newline
        if (!result.empty() && result.back() == '\n') {
            result.pop_back();
        }
        return result;
    }
}

// ============================================================================
// CommunityLiaisonAgent Implementation
// ============================================================================

CommunityLiaisonAgent::CommunityLiaisonAgent(const AgentConfig& config)
    : TheOrgAgent(config, AgentRole::COMMUNITY_LIAISON) {
}

void CommunityLiaisonAgent::initialize() {
    AgentLogger logger;
    logger.log("Initializing Community Liaison Agent: " + config_.agentName);
}

void CommunityLiaisonAgent::start() {
    if (running_) return;
    running_ = true;
    paused_ = false;

    AgentLogger logger;
    logger.log("Community Liaison Agent started: " + config_.agentName);
}

void CommunityLiaisonAgent::stop() {
    if (!running_) return;
    running_ = false;

    AgentLogger logger;
    logger.log("Community Liaison Agent stopped: " + config_.agentName);
}

void CommunityLiaisonAgent::pause() { paused_ = true; }
void CommunityLiaisonAgent::resume() { paused_ = false; }
bool CommunityLiaisonAgent::isRunning() const { return running_; }
void CommunityLiaisonAgent::processLoop() {}

void CommunityLiaisonAgent::addOrganization(const OrganizationConfig& org) {
    std::lock_guard<std::mutex> lock(orgMutex_);
    organizations_[org.id] = org;
}

void CommunityLiaisonAgent::removeOrganization(const UUID& orgId) {
    std::lock_guard<std::mutex> lock(orgMutex_);
    organizations_.erase(orgId);
}

std::vector<OrganizationConfig> CommunityLiaisonAgent::getMonitoredOrganizations() const {
    std::lock_guard<std::mutex> lock(orgMutex_);
    std::vector<OrganizationConfig> result;
    for (const auto& [id, org] : organizations_) {
        result.push_back(org);
    }
    return result;
}

void CommunityLiaisonAgent::trackDiscussion(const UUID& /* orgId */, const std::string& /* topic */,
                                            const std::string& /* summary */, const std::string& /* channelId */) {
    // Mock implementation
}

std::vector<ParallelTopic> CommunityLiaisonAgent::identifyParallelTopics(std::chrono::hours /* timeWindow */) const {
    return {}; // Mock
}

CrossOrgReport CommunityLiaisonAgent::generateDailyReport(const std::vector<UUID>& /* recipientOrgIds */) const {
    CrossOrgReport report;
    report.type = ReportType::DAILY;
    report.generatedAt = std::chrono::system_clock::now();
    return report;
}

CrossOrgReport CommunityLiaisonAgent::generateWeeklyReport(const std::vector<UUID>& /* recipientOrgIds */) const {
    CrossOrgReport report;
    report.type = ReportType::WEEKLY;
    report.generatedAt = std::chrono::system_clock::now();
    return report;
}

// ============================================================================
// SocialMediaManagerAgent Implementation
// ============================================================================

SocialMediaManagerAgent::SocialMediaManagerAgent(const AgentConfig& config)
    : TheOrgAgent(config, AgentRole::SOCIAL_MEDIA_MANAGER) {
}

void SocialMediaManagerAgent::initialize() {
    AgentLogger logger;
    logger.log("Initializing Social Media Manager Agent: " + config_.agentName);
}

void SocialMediaManagerAgent::start() {
    if (running_) return;
    running_ = true;
    paused_ = false;

    AgentLogger logger;
    logger.log("Social Media Manager Agent started: " + config_.agentName);
}

void SocialMediaManagerAgent::stop() {
    if (!running_) return;
    running_ = false;

    AgentLogger logger;
    logger.log("Social Media Manager Agent stopped: " + config_.agentName);
}

void SocialMediaManagerAgent::pause() { paused_ = true; }
void SocialMediaManagerAgent::resume() { paused_ = false; }
bool SocialMediaManagerAgent::isRunning() const { return running_; }
void SocialMediaManagerAgent::processLoop() {}

UUID SocialMediaManagerAgent::createContent(ContentType type, const std::string& title, const std::string& contentText,
                                            const std::vector<PlatformType>& targetPlatforms) {
    std::lock_guard<std::mutex> lock(contentMutex_);

    SocialMediaContent content;
    content.id = config_.agentId + "-content-" + std::to_string(content_.size());
    content.type = type;
    content.title = title;
    content.content = contentText;
    content.targetPlatforms = targetPlatforms;
    content.status = ContentStatus::DRAFT;
    content.createdAt = std::chrono::system_clock::now();
    content.updatedAt = content.createdAt;

    content_[content.id] = content;
    return content.id;
}

void SocialMediaManagerAgent::scheduleContent(const UUID& contentId, Timestamp publishTime) {
    std::lock_guard<std::mutex> lock(contentMutex_);
    auto it = content_.find(contentId);
    if (it != content_.end()) {
        it->second.scheduledTime = publishTime;
        it->second.status = ContentStatus::SCHEDULED;
        it->second.updatedAt = std::chrono::system_clock::now();
    }
}

void SocialMediaManagerAgent::publishContent(const UUID& contentId) {
    std::lock_guard<std::mutex> lock(contentMutex_);
    auto it = content_.find(contentId);
    if (it != content_.end()) {
        it->second.status = ContentStatus::PUBLISHED;
        it->second.updatedAt = std::chrono::system_clock::now();

        AgentLogger logger;
        logger.log("Published content: " + it->second.title);
    }
}

std::string SocialMediaManagerAgent::generateContent(const std::string& topic, ContentType /* type */,
                                                     PlatformType platform, const std::string& tone) const {
    std::string platformName = the_org_utils::platformTypeToString(platform);
    return "Generated " + tone + " content about " + topic + " for " + platformName;
}

std::vector<std::string> SocialMediaManagerAgent::suggestHashtags(const std::string& /* content */, PlatformType /* platform */) const {
    return {"#ElizaOS", "#AI", "#Agents", "#OpenSource"};
}

SocialMediaManagerAgent::SocialMediaMetrics SocialMediaManagerAgent::getPlatformMetrics(PlatformType platform) const {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    auto it = platformMetrics_.find(platform);
    if (it != platformMetrics_.end()) {
        return it->second;
    }

    // Return default metrics
    SocialMediaMetrics metrics;
    metrics.platform = platform;
    metrics.followers = 0;
    metrics.totalPosts = 0;
    metrics.engagementRate = 0.0;
    metrics.lastUpdated = std::chrono::system_clock::now();
    return metrics;
}

std::string SocialMediaManagerAgent::generateAnalyticsReport(std::chrono::hours /* timeWindow */) const {
    return "Social Media Analytics Report: Mock implementation";
}

// ============================================================================
// Additional ProjectManagerAgent Methods
// ============================================================================

void ProjectManagerAgent::updateProject(const UUID& projectId, const Project& updatedProject) {
    std::lock_guard<std::mutex> lock(projectMutex_);
    projects_[projectId] = updatedProject;
}

void ProjectManagerAgent::addTeamMemberToProject(const UUID& projectId, const UUID& teamMemberId) {
    std::lock_guard<std::mutex> lock(projectMutex_);
    auto it = projects_.find(projectId);
    if (it != projects_.end()) {
        it->second.teamMemberIds.push_back(teamMemberId);
    }
}

void ProjectManagerAgent::removeTeamMemberFromProject(const UUID& projectId, const UUID& teamMemberId) {
    std::lock_guard<std::mutex> lock(projectMutex_);
    auto it = projects_.find(projectId);
    if (it != projects_.end()) {
        auto& members = it->second.teamMemberIds;
        members.erase(std::remove(members.begin(), members.end(), teamMemberId), members.end());
    }
}

std::vector<Project> ProjectManagerAgent::getActiveProjects() const {
    std::lock_guard<std::mutex> lock(projectMutex_);
    std::vector<Project> result;
    for (const auto& [id, project] : projects_) {
        if (project.status == ProjectStatus::ACTIVE || project.status == ProjectStatus::PLANNING) {
            result.push_back(project);
        }
    }
    return result;
}

std::optional<Project> ProjectManagerAgent::getProject(const UUID& projectId) const {
    std::lock_guard<std::mutex> lock(projectMutex_);
    auto it = projects_.find(projectId);
    if (it != projects_.end()) {
        return it->second;
    }
    return std::nullopt;
}

void ProjectManagerAgent::updateTeamMember(const UUID& memberId, const TeamMember& updatedMember) {
    std::lock_guard<std::mutex> lock(teamMutex_);
    teamMembers_[memberId] = updatedMember;
}

void ProjectManagerAgent::removeTeamMember(const UUID& memberId) {
    std::lock_guard<std::mutex> lock(teamMutex_);
    teamMembers_.erase(memberId);
}

std::optional<TeamMember> ProjectManagerAgent::getTeamMember(const UUID& memberId) const {
    std::lock_guard<std::mutex> lock(teamMutex_);
    auto it = teamMembers_.find(memberId);
    if (it != teamMembers_.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<TeamMember> ProjectManagerAgent::getAllTeamMembers() const {
    std::lock_guard<std::mutex> lock(teamMutex_);
    std::vector<TeamMember> result;
    for (const auto& [id, member] : teamMembers_) {
        result.push_back(member);
    }
    return result;
}

void ProjectManagerAgent::scheduleDailyCheckins(const UUID& /* projectId */) {
    // Mock implementation
}

std::vector<DailyUpdate> ProjectManagerAgent::getDailyUpdates(const UUID& /* projectId */, const std::string& /* date */) const {
    return dailyUpdates_;
}

std::vector<DailyUpdate> ProjectManagerAgent::getMemberUpdates(const UUID& /* teamMemberId */, std::chrono::hours /* timeWindow */) const {
    return {};
}

std::string ProjectManagerAgent::generateTeamProductivityReport(const std::vector<UUID>& /* teamMemberIds */,
                                                                 std::chrono::hours /* timeWindow */) const {
    return "Team Productivity Report: Mock implementation";
}

void ProjectManagerAgent::distributeReport(const std::string& report, const std::vector<std::string>& channelIds) {
    for (const auto& channelId : channelIds) {
        AgentLogger logger;
        logger.log("Distributing report to channel: " + channelId);
    }
    (void)report; // Used for logging
}

void ProjectManagerAgent::linkTaskToProject(const UUID& projectId, const UUID& taskId) {
    std::lock_guard<std::mutex> lock(projectMutex_);
    auto it = projects_.find(projectId);
    if (it != projects_.end()) {
        it->second.taskIds.push_back(taskId);
    }
}

void ProjectManagerAgent::reportBlocker(const UUID& /* projectId */, const UUID& /* teamMemberId */,
                                         const std::string& description) {
    AgentLogger logger;
    logger.log("Blocker reported: " + description, "", "Project", LogLevel::WARNING);
}

void ProjectManagerAgent::resolveBlocker(const UUID& /* blockerId */, const std::string& resolution) {
    AgentLogger logger;
    logger.log("Blocker resolved: " + resolution);
}

std::vector<std::string> ProjectManagerAgent::getActiveBlockers(const UUID& /* projectId */) const {
    return {};
}

// ============================================================================
// Additional TheOrgManager Methods
// ============================================================================

void TheOrgManager::pauseAllAgents() {
    for (auto& [id, agent] : agents_) {
        agent->pause();
    }
}

void TheOrgManager::resumeAllAgents() {
    for (auto& [id, agent] : agents_) {
        agent->resume();
    }
}

void TheOrgManager::sendDirectMessage(const UUID& /* fromAgentId */, const UUID& toAgentId, const std::string& message) {
    auto agent = getAgent(toAgentId);
    if (agent) {
        agent->processMessage(message, "manager");
    }
}

void TheOrgManager::loadConfiguration(const std::string& /* configPath */) {
    // Mock implementation
}

void TheOrgManager::saveConfiguration(const std::string& /* configPath */) const {
    // Mock implementation
}

void TheOrgManager::updateGlobalSetting(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(settingsMutex_);
    globalSettings_[key] = value;
}

std::string TheOrgManager::getGlobalSetting(const std::string& key) const {
    std::lock_guard<std::mutex> lock(settingsMutex_);
    auto it = globalSettings_.find(key);
    return (it != globalSettings_.end()) ? it->second : "";
}

std::string TheOrgManager::generateHealthReport() const {
    std::ostringstream report;
    report << "=== TheOrg Health Report ===\n";

    auto metrics = getSystemMetrics();
    report << "Total Agents: " << metrics.totalAgents << "\n";
    report << "Active Agents: " << metrics.activeAgents << "\n";
    report << "Total Tasks: " << metrics.totalTasks << "\n";
    report << "Pending Tasks: " << metrics.pendingTasks << "\n";
    report << "System Load: " << metrics.systemLoad << "\n";

    return report.str();
}

void TheOrgManager::performHealthCheck() {
    for (const auto& [id, agent] : agents_) {
        if (!agent->isRunning()) {
            AgentLogger logger;
            logger.log("Agent " + id + " is not running", "", "Health", LogLevel::WARNING);
        }
    }
}

void TheOrgManager::enableEventLogging(const std::string& logPath) {
    eventLoggingEnabled_ = true;
    logPath_ = logPath;
}

void TheOrgManager::disableEventLogging() {
    eventLoggingEnabled_ = false;
}

std::vector<std::string> TheOrgManager::getRecentEvents(std::chrono::hours /* timeWindow */) const {
    std::lock_guard<std::mutex> lock(logMutex_);
    return eventLog_;
}

void TheOrgManager::setLogLevel(const std::string& level) {
    logLevel_ = level;
}

// ============================================================================
// Additional DeveloperRelationsAgent Methods
// ============================================================================

std::string DeveloperRelationsAgent::provideAPIReference(const std::string& apiName) const {
    return "API Reference for " + apiName + ": Mock implementation";
}

std::string DeveloperRelationsAgent::diagnoseIssue(const std::string& errorMessage, const std::string& /* context */) const {
    return "Diagnosis for '" + errorMessage + "': Check logs, verify configuration";
}

std::vector<std::string> DeveloperRelationsAgent::suggestSolutions(const std::string& /* problemDescription */) const {
    return {"Check documentation", "Restart the service", "Verify dependencies"};
}

std::string DeveloperRelationsAgent::generateTutorial(const std::string& topic, const std::string& difficulty) const {
    return "# " + topic + " Tutorial (" + difficulty + ")\n\nMock tutorial content...";
}

std::string DeveloperRelationsAgent::reviewCode(const std::string& /* code */, const std::string& /* language */) const {
    return "Code review: Looks good! Consider adding more comments.";
}

std::vector<std::string> DeveloperRelationsAgent::getRelatedTopics(const std::string& /* topic */) const {
    return {"Agent Development", "Plugin System", "Memory Management"};
}

std::string DeveloperRelationsAgent::generateOnboardingGuide(const std::string& project, const std::string& role) const {
    return "# Onboarding Guide for " + project + "\n\nRole: " + role + "\n\nMock content...";
}

void DeveloperRelationsAgent::updateKnowledgeBase(const std::string& topic, const std::string& /* updatedContent */) {
    AgentLogger logger;
    logger.log("Updated knowledge base: " + topic);
}

void DeveloperRelationsAgent::hostTechnicalSession(const std::string& topic, const std::string& /* channelId */, Timestamp /* scheduledTime */) {
    AgentLogger logger;
    logger.log("Technical session scheduled: " + topic);
}

void DeveloperRelationsAgent::answerTechnicalQuestion(const std::string& question, const std::string& channelId,
                                                       const std::string& /* userId */) {
    sendMessage(PlatformType::DISCORD, channelId, "Answering: " + question);
}

// ============================================================================
// Additional the_org_utils Functions
// ============================================================================

namespace the_org_utils {

PlatformType stringToPlatformType(const std::string& str) {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if (lower == "discord") return PlatformType::DISCORD;
    if (lower == "telegram") return PlatformType::TELEGRAM;
    if (lower == "twitter") return PlatformType::TWITTER;
    if (lower == "slack") return PlatformType::SLACK;
    if (lower == "facebook") return PlatformType::FACEBOOK;
    if (lower == "linkedin") return PlatformType::LINKEDIN;
    if (lower == "github") return PlatformType::GITHUB;
    return PlatformType::DISCORD; // Default
}

AgentRole stringToAgentRole(const std::string& str) {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if (lower == "community_manager" || lower == "community manager") return AgentRole::COMMUNITY_MANAGER;
    if (lower == "developer_relations" || lower == "developer relations") return AgentRole::DEVELOPER_RELATIONS;
    if (lower == "community_liaison" || lower == "community liaison") return AgentRole::COMMUNITY_LIAISON;
    if (lower == "project_manager" || lower == "project manager") return AgentRole::PROJECT_MANAGER;
    if (lower == "social_media_manager" || lower == "social media manager") return AgentRole::SOCIAL_MEDIA_MANAGER;
    return AgentRole::COMMUNITY_MANAGER; // Default
}

std::vector<std::string> parseHashtags(const std::string& content) {
    std::vector<std::string> hashtags;
    std::string current;
    bool inHashtag = false;

    for (size_t i = 0; i < content.length(); ++i) {
        char c = content[i];
        if (c == '#') {
            if (!current.empty() && inHashtag) {
                hashtags.push_back(current);
            }
            current.clear();
            inHashtag = true;
        } else if (inHashtag) {
            if (std::isalnum(c) || c == '_') {
                current += c;
            } else {
                if (!current.empty()) {
                    hashtags.push_back(current);
                }
                current.clear();
                inHashtag = false;
            }
        }
    }

    if (!current.empty() && inHashtag) {
        hashtags.push_back(current);
    }

    return hashtags;
}

double calculateSimilarity(const std::vector<std::string>& list1, const std::vector<std::string>& list2) {
    if (list1.empty() && list2.empty()) return 1.0;
    if (list1.empty() || list2.empty()) return 0.0;

    size_t matches = 0;
    for (const auto& item1 : list1) {
        for (const auto& item2 : list2) {
            if (item1 == item2) {
                matches++;
                break;
            }
        }
    }

    size_t totalUnique = list1.size() + list2.size() - matches;
    return static_cast<double>(matches) / static_cast<double>(totalUnique);
}

std::string sanitizeForPlatform(const std::string& content, PlatformType platform) {
    std::string result = content;

    switch (platform) {
        case PlatformType::TWITTER:
            // Twitter has 280 character limit
            if (result.length() > 280) {
                result = result.substr(0, 277) + "...";
            }
            break;
        case PlatformType::DISCORD:
            // Discord has 2000 character limit
            if (result.length() > 2000) {
                result = result.substr(0, 1997) + "...";
            }
            break;
        default:
            // No special sanitization needed
            break;
    }

    return result;
}

bool validateUrl(const std::string& url) {
    if (url.empty()) return false;

    // Basic URL validation
    if (url.find("http://") == 0 || url.find("https://") == 0) {
        // Check for basic URL structure
        size_t slashPos = url.find("://");
        if (slashPos != std::string::npos && slashPos + 3 < url.length()) {
            std::string rest = url.substr(slashPos + 3);
            // Must have at least a domain
            if (!rest.empty() && rest.find('.') != std::string::npos) {
                return true;
            }
        }
    }

    return false;
}

std::string extractDomain(const std::string& url) {
    std::string domain;

    size_t protocolEnd = url.find("://");
    size_t start = (protocolEnd != std::string::npos) ? protocolEnd + 3 : 0;

    // Remove www. prefix if present
    if (url.substr(start, 4) == "www.") {
        start += 4;
    }

    // Find end of domain (path or query string)
    size_t end = url.find('/', start);
    if (end == std::string::npos) {
        end = url.find('?', start);
    }

    if (end != std::string::npos) {
        domain = url.substr(start, end - start);
    } else {
        domain = url.substr(start);
    }

    return domain;
}

std::vector<std::string> splitText(const std::string& text, size_t maxLength, const std::string& delimiter) {
    std::vector<std::string> parts;

    if (text.length() <= maxLength) {
        parts.push_back(text);
        return parts;
    }

    std::istringstream iss(text);
    std::string word;
    std::string current;

    while (iss >> word) {
        if (current.empty()) {
            current = word;
        } else if (current.length() + delimiter.length() + word.length() <= maxLength) {
            current += delimiter + word;
        } else {
            parts.push_back(current);
            current = word;
        }
    }

    if (!current.empty()) {
        parts.push_back(current);
    }

    return parts;
}

std::string joinText(const std::vector<std::string>& parts, const std::string& separator) {
    if (parts.empty()) return "";

    std::ostringstream oss;
    for (size_t i = 0; i < parts.size(); ++i) {
        if (i > 0) {
            oss << separator;
        }
        oss << parts[i];
    }

    return oss.str();
}

} // namespace the_org_utils

} // namespace elizaos
