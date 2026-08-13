#include "elizaos/the_org.hpp"
#include "elizaos/agentlogger.hpp"
#include <algorithm>
#include <sstream>
#include <random>
#include <iomanip>
#include <fstream>
#include <cctype>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace fs = std::filesystem;
using json = nlohmann::json;

// Utility functions
namespace the_org_utils {
    
    UUID generateAgentId(AgentRole role) {
        const char* prefix = "agent_";
        switch (role) {
            case AgentRole::COMMUNITY_MANAGER: prefix = "cm_"; break;
            case AgentRole::DEVELOPER_RELATIONS: prefix = "dr_"; break;
            case AgentRole::COMMUNITY_LIAISON: prefix = "cl_"; break;
            case AgentRole::PROJECT_MANAGER: prefix = "pm_"; break;
            case AgentRole::SOCIAL_MEDIA_MANAGER: prefix = "sm_"; break;
        }

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 15);
        std::stringstream ss;
        ss << prefix << std::hex;
        for (int i = 0; i < 32; ++i) {
            ss << dis(gen);
            if (i == 7 || i == 11 || i == 15 || i == 19) ss << "-";
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

bool TheOrgAgent::sendMessage(PlatformType platform, const std::string& channelId,
                              const std::string& message) {
    if (channelId.empty() || !validateMessage(message)) return false;
    const std::string formatted = formatResponse(message, platform);
    {
        std::lock_guard<std::mutex> lock(platformMutex_);
        if (platforms_.find(platform) == platforms_.end()) return false;
        auto& history = platformMessages_[platform][channelId];
        history.push_back(formatted);
        constexpr size_t kMaxChannelHistory = 1000;
        if (history.size() > kMaxChannelHistory) {
            history.erase(history.begin(), history.begin() +
                static_cast<std::ptrdiff_t>(history.size() - kMaxChannelHistory));
        }
    }
    AgentLogger logger;
    logger.log("Agent " + config_.agentName + " sent message to " +
        the_org_utils::platformTypeToString(platform) + " channel " + channelId);
    return true;
}

std::vector<std::string> TheOrgAgent::getRecentMessages(PlatformType platform,
                                                        const std::string& channelId,
                                                        size_t count) {
    std::lock_guard<std::mutex> lock(platformMutex_);
    auto platformIt = platformMessages_.find(platform);
    if (platformIt == platformMessages_.end()) return {};
    auto channelIt = platformIt->second.find(channelId);
    if (channelIt == platformIt->second.end() || count == 0) return {};
    const auto& history = channelIt->second;
    const size_t start = history.size() > count ? history.size() - count : 0;
    return std::vector<std::string>(history.begin() + static_cast<std::ptrdiff_t>(start),
                                    history.end());
}

void TheOrgAgent::sendToAgent(const UUID& agentId, const std::string& message,
                              const std::string& type) {
    if (agentId.empty() || !validateMessage(message)) return;
    auto record = createMemory("Outbound " + (type.empty() ? std::string("message") : type) +
                               " to " + agentId + ": " + message,
                               MemoryType::MESSAGE);
    addMemory(record);
    AgentLogger logger;
    logger.log("Agent " + config_.agentName + " sending to agent " + agentId + ": " + message);
}

std::queue<std::string> TheOrgAgent::getIncomingMessages() {
    std::lock_guard<std::mutex> lock(messageMutex_);
    return incomingMessages_;
}

void TheOrgAgent::processMessage(const std::string& message, const std::string& senderId) {
    if (!validateMessage(message)) return;
    std::lock_guard<std::mutex> lock(messageMutex_);
    incomingMessages_.push(senderId.empty() ? message : "From " + senderId + ": " + message);
}

std::optional<std::string> TheOrgAgent::popIncomingMessage() {
    std::lock_guard<std::mutex> lock(messageMutex_);
    if (incomingMessages_.empty()) return std::nullopt;
    std::string message = std::move(incomingMessages_.front());
    incomingMessages_.pop();
    return message;
}

UUID TheOrgAgent::createTask(const std::string& name, const std::string& description,
                                 int priority) {
    if (name.empty()) return {};
    static std::atomic<std::uint64_t> sequence{0};
    const UUID taskId = config_.agentId + "-task-" +
        std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + "-" +
        std::to_string(sequence.fetch_add(1, std::memory_order_relaxed));
    auto task = std::make_shared<Task>(taskId, name, description);
    task->setPriority(priority);
    {
        std::lock_guard<std::mutex> lock(taskMutex_);
        tasks_[taskId] = task;
    }
    return taskId;
}

bool TheOrgAgent::completeTask(const UUID& taskId) {
    std::lock_guard<std::mutex> lock(taskMutex_);
    auto it = tasks_.find(taskId);
    if (it == tasks_.end() || !it->second) return false;
    const TaskStatus status = it->second->getStatus();
    if (status == TaskStatus::COMPLETED || status == TaskStatus::CANCELLED) return false;
    it->second->setStatus(TaskStatus::COMPLETED);
    it->second->updateTimestamp();
    return true;
}

std::vector<std::shared_ptr<Task>> TheOrgAgent::getPendingTasks() {
    std::vector<std::shared_ptr<Task>> pending;
    std::lock_guard<std::mutex> lock(taskMutex_);
    for (const auto& [_, task] : tasks_) {
        if (task && (task->getStatus() == TaskStatus::PENDING ||
                     task->getStatus() == TaskStatus::RUNNING)) {
            pending.push_back(task);
        }
    }
    std::sort(pending.begin(), pending.end(), [](const auto& left, const auto& right) {
        if (left->getPriority() != right->getPriority()) {
            return left->getPriority() > right->getPriority();
        }
        return left->getCreatedAt() < right->getCreatedAt();
    });
    return pending;
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
    : TheOrgAgent(config, AgentRole::COMMUNITY_MANAGER), currentMetrics_{} {
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
            while (auto message = popIncomingMessage()) {
                addMemory(createMemory("Processed community message: " + *message,
                                       MemoryType::MESSAGE));
                trackUserActivity("system", "message_processed");
            }
            updateCommunityMetrics();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
        const auto replaceAll = [&greeting](const std::string& token, const std::string& value) {
            size_t pos = 0;
            while ((pos = greeting.find(token, pos)) != std::string::npos) {
                greeting.replace(pos, token.size(), value);
                pos += value.size();
            }
        };
        replaceAll("{user}", userName);
        replaceAll("{server}", serverName);
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

void CommunityManagerAgent::trackUserActivity(const std::string& userId,
                                                const std::string& activity) {
    if (userId.empty()) return;
    const Timestamp now = std::chrono::system_clock::now();
    std::lock_guard<std::mutex> lock(activityMutex_);
    userActivity_[userId].push_back(now);
    userActivityDetails_[userId].push_back({now, activity.empty() ? "activity" : activity});
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

std::vector<std::string> CommunityManagerAgent::getTopTopics(std::chrono::hours timeWindow) const {
    const Timestamp cutoff = std::chrono::system_clock::now() - timeWindow;
    std::unordered_map<std::string, size_t> counts;
    {
        std::lock_guard<std::mutex> lock(activityMutex_);
        for (const auto& [_, activities] : userActivityDetails_) {
            for (const auto& [timestamp, activity] : activities) {
                if (timestamp >= cutoff && !activity.empty()) ++counts[activity];
            }
        }
    }
    std::vector<std::pair<std::string, size_t>> ranked(counts.begin(), counts.end());
    std::sort(ranked.begin(), ranked.end(), [](const auto& left, const auto& right) {
        if (left.second != right.second) return left.second > right.second;
        return left.first < right.first;
    });
    std::vector<std::string> topics;
    for (size_t i = 0; i < ranked.size() && i < 5; ++i) topics.push_back(ranked[i].first);
    return topics;
}

void CommunityManagerAgent::initiateConflictResolution(const std::vector<std::string>& userIds,
                                                        const std::string& channelId) {
    if (userIds.empty()) return;
    std::ostringstream record;
    record << "Conflict resolution initiated in " << channelId << " for ";
    for (size_t i = 0; i < userIds.size(); ++i) {
        if (i) record << ", ";
        record << userIds[i];
    }
    addMemory(createMemory(record.str(), MemoryType::CUSTOM));
    AgentLogger logger;
    logger.log(record.str());
}

void CommunityManagerAgent::escalateIssue(const std::string& description,
                                           const std::vector<std::string>& involvedUsers) {
    if (description.empty()) return;
    std::ostringstream record;
    record << "Issue escalated: " << description;
    if (!involvedUsers.empty()) {
        record << " [users=";
        for (size_t i = 0; i < involvedUsers.size(); ++i) {
            if (i) record << ',';
            record << involvedUsers[i];
        }
        record << ']';
    }
    addMemory(createMemory(record.str(), MemoryType::CUSTOM));
    AgentLogger logger;
    logger.log(record.str(), "", "Escalation", LogLevel::WARNING);
}

void CommunityManagerAgent::scheduleEvent(const std::string& eventName,
                                          const std::string& description,
                                          Timestamp scheduledTime) {
    if (eventName.empty()) return;
    CommunityEventRecord event;
    event.id = eventName;
    event.name = eventName;
    event.description = description;
    event.scheduledTime = scheduledTime;
    std::lock_guard<std::mutex> lock(communityEventMutex_);
    communityEvents_[event.id] = std::move(event);
    ++nextEventId_;
}

void CommunityManagerAgent::announceEvent(const std::string& eventId,
                                          const std::vector<std::string>& channelIds) {
    CommunityEventRecord event;
    {
        std::lock_guard<std::mutex> lock(communityEventMutex_);
        auto it = communityEvents_.find(eventId);
        if (it == communityEvents_.end()) return;
        it->second.channels = channelIds;
        event = it->second;
    }
    const std::string announcement = "Event '" + event.name + "': " + event.description;
    std::vector<PlatformType> platforms;
    {
        std::lock_guard<std::mutex> lock(platformMutex_);
        for (const auto& [type, _] : platforms_) platforms.push_back(type);
    }
    bool delivered = false;
    for (const auto type : platforms) {
        for (const auto& channel : channelIds) delivered = sendMessage(type, channel, announcement) || delivered;
    }
    if (!delivered) addMemory(createMemory(announcement, MemoryType::MESSAGE));
}

void CommunityManagerAgent::trackEventParticipation(const std::string& eventId,
                                                     const std::string& userId) {
    if (eventId.empty() || userId.empty()) return;
    std::lock_guard<std::mutex> lock(communityEventMutex_);
    auto it = communityEvents_.find(eventId);
    if (it == communityEvents_.end()) return;
    auto& participants = it->second.participants;
    if (std::find(participants.begin(), participants.end(), userId) == participants.end()) {
        participants.push_back(userId);
    }
}

void CommunityManagerAgent::processNewUserJoin(const std::string& userId,
                                               const std::string& serverId) {
    trackUserActivity(userId, "new_member");
    if (!shouldGreetNewUser(userId)) return;
    const std::string greeting = generateGreeting(userId, serverId);
    std::vector<PlatformType> platforms;
    {
        std::lock_guard<std::mutex> lock(platformMutex_);
        for (const auto& [type, _] : platforms_) platforms.push_back(type);
    }
    bool delivered = false;
    for (const auto type : platforms) delivered = sendMessage(type, greetingChannelId_, greeting) || delivered;
    if (!delivered) addMemory(createMemory(greeting, MemoryType::MESSAGE));
}

void CommunityManagerAgent::processMessageForModeration(const std::string& message, const std::string& userId, const std::string& channelId) {
    evaluateMessage(message, userId, channelId);
}

void CommunityManagerAgent::generateDailyReport() {
    updateCommunityMetrics();
    const CommunityMetrics metrics = generateCommunityMetrics();
    std::ostringstream report;
    report << "Community daily report: members=" << metrics.totalMembers
           << ", active=" << metrics.activeMembers
           << ", new=" << metrics.newMembersToday
           << ", messages=" << metrics.messagesPerDay
           << ", engagement=" << metrics.engagementRate;
    addMemory(createMemory(report.str(), MemoryType::CUSTOM));
}

void CommunityManagerAgent::updateCommunityMetrics() {
    const Timestamp now = std::chrono::system_clock::now();
    const Timestamp cutoff = now - std::chrono::hours(24);
    size_t totalMembers = 0;
    size_t activeMembers = 0;
    size_t newMembers = 0;
    size_t messages = 0;
    {
        std::lock_guard<std::mutex> lock(activityMutex_);
        totalMembers = userActivity_.size();
        for (const auto& [user, timestamps] : userActivity_) {
            (void)user;
            const bool active = std::any_of(timestamps.begin(), timestamps.end(),
                [&](const Timestamp& timestamp) { return timestamp >= cutoff; });
            if (active) ++activeMembers;
            if (!timestamps.empty() && timestamps.front() >= cutoff) ++newMembers;
        }
        for (const auto& [_, activities] : userActivityDetails_) {
            messages += static_cast<size_t>(std::count_if(activities.begin(), activities.end(),
                [&](const auto& activity) {
                    return activity.first >= cutoff && activity.second.find("message") != std::string::npos;
                }));
        }
    }
    std::vector<std::string> activeChannels;
    {
        std::lock_guard<std::mutex> lock(platformMutex_);
        for (const auto& [_, channels] : platformMessages_) {
            for (const auto& [channel, history] : channels) {
                if (!history.empty()) activeChannels.push_back(channel);
            }
        }
    }
    std::sort(activeChannels.begin(), activeChannels.end());
    activeChannels.erase(std::unique(activeChannels.begin(), activeChannels.end()), activeChannels.end());

    std::lock_guard<std::mutex> lock(metricsMutex_);
    currentMetrics_.totalMembers = totalMembers;
    currentMetrics_.activeMembers = activeMembers;
    currentMetrics_.newMembersToday = newMembers;
    currentMetrics_.messagesPerDay = messages;
    currentMetrics_.engagementRate = totalMembers == 0 ? 0.0 :
        static_cast<double>(activeMembers) / static_cast<double>(totalMembers);
    currentMetrics_.topTopics = getTopTopics(std::chrono::hours(24));
    currentMetrics_.mostActiveChannels = std::move(activeChannels);
    currentMetrics_.lastUpdated = now;
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
    processingThread_ = std::thread(&DeveloperRelationsAgent::processLoop, this);
    AgentLogger logger;
    logger.log("Developer Relations Agent started: " + config_.agentName);
}

void DeveloperRelationsAgent::stop() {
    if (!running_) return;
    running_ = false;
    if (processingThread_.joinable()) processingThread_.join();
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
    while (running_) {
        if (!paused_) {
            while (auto message = popIncomingMessage()) {
                addMemory(createMemory("Processed developer-relations request: " + *message,
                                       MemoryType::MESSAGE));
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void DeveloperRelationsAgent::indexDocumentation(const std::string& docPath, const std::string& version) {
    if (docPath.empty()) return;
    DocumentationEntry entry;
    entry.path = docPath;
    entry.version = version.empty() ? "latest" : version;
    entry.lastUpdated = std::chrono::system_clock::now();
    std::ifstream input(docPath);
    if (input) {
        entry.content.assign(std::istreambuf_iterator<char>(input),
                             std::istreambuf_iterator<char>());
    }

    {
        std::lock_guard<std::mutex> lock(docMutex_);
        auto existing = std::find_if(documentationIndex_.begin(), documentationIndex_.end(),
            [&](const DocumentationEntry& candidate) {
                return candidate.path == entry.path && candidate.version == entry.version;
            });
        if (existing == documentationIndex_.end()) documentationIndex_.push_back(std::move(entry));
        else *existing = std::move(entry);
    }
    AgentLogger logger;
    logger.log("Indexed documentation: " + docPath + " (version: " + version + ")");
}

void DeveloperRelationsAgent::addTechnicalKnowledge(const std::string& topic, const std::string& content,
                                                     const std::vector<std::string>& tags) {
    if (topic.empty() || content.empty()) return;
    KnowledgeEntry entry;
    entry.topic = topic;
    entry.content = content;
    entry.tags = tags;
    entry.relatedTopics = tags;
    entry.lastUpdated = std::chrono::system_clock::now();
    entry.relevanceScore = 1.0;
    {
        std::lock_guard<std::mutex> lock(knowledgeMutex_);
        knowledgeBase_[topic] = std::move(entry);
    }
    AgentLogger logger;
    logger.log("Added knowledge: " + topic);
}

std::vector<std::string> DeveloperRelationsAgent::searchDocumentation(const std::string& query) const {
    if (query.empty()) return {};
    const auto lower = [](std::string value) {
        std::transform(value.begin(), value.end(), value.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return value;
    };
    const std::string needle = lower(query);
    std::vector<std::string> results;
    std::lock_guard<std::mutex> lock(docMutex_);
    for (const auto& entry : documentationIndex_) {
        if (lower(entry.path).find(needle) != std::string::npos ||
            lower(entry.content).find(needle) != std::string::npos ||
            lower(entry.version).find(needle) != std::string::npos) {
            results.push_back(entry.path);
        }
    }
    std::sort(results.begin(), results.end());
    return results;
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
    processingThread_ = std::thread(&ProjectManagerAgent::processLoop, this);
}

void ProjectManagerAgent::stop() {
    if (!running_) return;
    running_ = false;
    if (processingThread_.joinable()) processingThread_.join();
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
    while (running_) {
        if (!paused_) {
            while (auto message = popIncomingMessage()) {
                if (message->find("scheduled_tick") != std::string::npos) {
                    const auto projects = getActiveProjects();
                    for (const auto& project : projects) scheduleDailyCheckins(project.id);
                } else {
                    addMemory(createMemory("Processed project-management message: " + *message,
                                           MemoryType::MESSAGE));
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

UUID ProjectManagerAgent::createProject(const std::string& name, const std::string& description,
                                              const std::vector<UUID>& teamMemberIds) {
    if (name.empty()) return {};
    static std::atomic<std::uint64_t> sequence{0};
    Project project;
    project.id = config_.agentId + "-project-" + std::to_string(
        std::chrono::steady_clock::now().time_since_epoch().count()) + "-" +
        std::to_string(sequence.fetch_add(1, std::memory_order_relaxed));
    project.name = name;
    project.description = description;
    project.status = ProjectStatus::PLANNING;
    project.teamMemberIds = teamMemberIds;
    project.createdAt = std::chrono::system_clock::now();
    project.updatedAt = project.createdAt;
    {
        std::lock_guard<std::mutex> lock(projectMutex_);
        projects_[project.id] = project;
    }
    return project.id;
}

UUID ProjectManagerAgent::addTeamMember(const TeamMember& member) {
    static std::atomic<std::uint64_t> sequence{0};
    TeamMember stored = member;
    if (stored.id.empty()) {
        stored.id = config_.agentId + "-member-" + std::to_string(
            std::chrono::steady_clock::now().time_since_epoch().count()) + "-" +
            std::to_string(sequence.fetch_add(1, std::memory_order_relaxed));
    }
    {
        std::lock_guard<std::mutex> lock(teamMutex_);
        teamMembers_[stored.id] = stored;
    }
    return stored.id;
}

void ProjectManagerAgent::recordDailyUpdate(const DailyUpdate& update) {
    if (update.projectId.empty() || update.teamMemberId.empty()) return;
    static std::atomic<std::uint64_t> sequence{0};
    DailyUpdate stored = update;
    if (stored.id.empty()) {
        stored.id = config_.agentId + "-update-" +
            std::to_string(sequence.fetch_add(1, std::memory_order_relaxed));
    }
    if (stored.submittedAt == Timestamp{}) stored.submittedAt = std::chrono::system_clock::now();
    {
        std::lock_guard<std::mutex> lock(updateMutex_);
        dailyUpdates_.push_back(stored);
    }
    {
        std::lock_guard<std::mutex> lock(projectMutex_);
        auto project = projects_.find(stored.projectId);
        if (project != projects_.end()) project->second.updatedAt = stored.submittedAt;
    }
}

std::string ProjectManagerAgent::generateProjectStatusReport(const UUID& projectId) const {
    Project project;
    {
        std::lock_guard<std::mutex> lock(projectMutex_);
        auto it = projects_.find(projectId);
        if (it == projects_.end()) return "Project not found: " + projectId;
        project = it->second;
    }
    const auto statusName = [](ProjectStatus status) {
        switch (status) {
            case ProjectStatus::PLANNING: return "Planning";
            case ProjectStatus::ACTIVE: return "Active";
            case ProjectStatus::ON_HOLD: return "On Hold";
            case ProjectStatus::COMPLETED: return "Completed";
            case ProjectStatus::CANCELLED: return "Cancelled";
        }
        return "Unknown";
    };
    size_t updateCount = 0;
    {
        std::lock_guard<std::mutex> lock(updateMutex_);
        updateCount = static_cast<size_t>(std::count_if(dailyUpdates_.begin(), dailyUpdates_.end(),
            [&](const DailyUpdate& update) { return update.projectId == projectId; }));
    }
    std::ostringstream report;
    report << "Project Status Report: " << project.name << "\n"
           << "Status: " << statusName(project.status) << "\n"
           << "Description: " << project.description << "\n"
           << "Team members: " << project.teamMemberIds.size() << "\n"
           << "Tasks: " << project.taskIds.size() << "\n"
           << "Daily updates: " << updateCount;
    return report.str();
}

std::string ProjectManagerAgent::generateWeeklyReport(const std::vector<UUID>& projectIds) const {
    std::vector<UUID> ids = projectIds;
    if (ids.empty()) {
        std::lock_guard<std::mutex> lock(projectMutex_);
        ids.reserve(projects_.size());
        for (const auto& [id, _] : projects_) ids.push_back(id);
    }
    std::sort(ids.begin(), ids.end());
    std::ostringstream report;
    report << "Weekly Project Report\n";
    for (const auto& id : ids) report << generateProjectStatusReport(id) << "\n";
    return report.str();
}

void ProjectManagerAgent::sendCheckinReminder(const UUID& teamMemberId,
                                                  const UUID& projectId) {
    if (teamMemberId.empty() || projectId.empty()) return;
    const std::string reminder = "Daily check-in reminder for project " + projectId;
    sendToAgent(teamMemberId, reminder, "checkin_reminder");
}

// ============================================================================
// TheOrgManager Implementation
// ============================================================================

TheOrgManager::TheOrgManager() : currentMetrics_{} {
}

TheOrgManager::~TheOrgManager() {
    stopAllAgents();
}

void TheOrgManager::addAgent(std::shared_ptr<TheOrgAgent> agent) {
    if (!agent || agent->getId().empty()) return;
    std::lock_guard<std::mutex> lock(agentMutex_);
    agents_[agent->getId()] = agent;
    roleToAgentMap_[agent->getRole()] = agent->getId();
}

void TheOrgManager::removeAgent(const UUID& agentId) {
    std::shared_ptr<TheOrgAgent> removed;
    {
        std::lock_guard<std::mutex> lock(agentMutex_);
        auto it = agents_.find(agentId);
        if (it == agents_.end()) return;
        removed = it->second;
        roleToAgentMap_.erase(removed->getRole());
        agents_.erase(it);
    }
    if (removed) removed->stop();
}

std::shared_ptr<TheOrgAgent> TheOrgManager::getAgent(const UUID& agentId) const {
    std::lock_guard<std::mutex> lock(agentMutex_);
    auto it = agents_.find(agentId);
    return (it != agents_.end()) ? it->second : nullptr;
}

void TheOrgManager::startAllAgents() {
    const auto agents = getAllAgents();
    for (const auto& agent : agents) if (agent) agent->start();
}

void TheOrgManager::stopAllAgents() {
    const auto agents = getAllAgents();
    for (const auto& agent : agents) if (agent) agent->stop();
}

std::shared_ptr<TheOrgAgent> TheOrgManager::getAgentByRole(AgentRole role) const {
    std::lock_guard<std::mutex> lock(agentMutex_);
    auto mapped = roleToAgentMap_.find(role);
    if (mapped != roleToAgentMap_.end()) {
        auto agent = agents_.find(mapped->second);
        if (agent != agents_.end()) return agent->second;
    }
    for (const auto& [_, agent] : agents_) {
        if (agent && agent->getRole() == role) return agent;
    }
    return nullptr;
}

// Additional TheOrgManager methods
void TheOrgManager::initializeAllAgents(const std::vector<AgentConfig>& configs) {
    auto inferRole = [](const AgentConfig& config) {
        std::string text = config.agentName + " " + config.bio + " " + config.lore;
        std::transform(text.begin(), text.end(), text.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        if (text.find("developer") != std::string::npos) return AgentRole::DEVELOPER_RELATIONS;
        if (text.find("project") != std::string::npos) return AgentRole::PROJECT_MANAGER;
        if (text.find("social") != std::string::npos) return AgentRole::SOCIAL_MEDIA_MANAGER;
        if (text.find("liaison") != std::string::npos) return AgentRole::COMMUNITY_LIAISON;
        return AgentRole::COMMUNITY_MANAGER;
    };

    for (const auto& config : configs) {
        auto existing = getAgent(config.agentId);
        if (!existing) {
            std::shared_ptr<TheOrgAgent> created;
            switch (inferRole(config)) {
                case AgentRole::COMMUNITY_MANAGER:
                    created = std::make_shared<CommunityManagerAgent>(config); break;
                case AgentRole::DEVELOPER_RELATIONS:
                    created = std::make_shared<DeveloperRelationsAgent>(config); break;
                case AgentRole::COMMUNITY_LIAISON:
                    created = std::make_shared<CommunityLiaisonAgent>(config); break;
                case AgentRole::PROJECT_MANAGER:
                    created = std::make_shared<ProjectManagerAgent>(config); break;
                case AgentRole::SOCIAL_MEDIA_MANAGER:
                    created = std::make_shared<SocialMediaManagerAgent>(config); break;
            }
            addAgent(created);
            existing = std::move(created);
        }
        if (existing) existing->initialize();
    }
}

void TheOrgManager::broadcastMessage(const std::string& message, const std::string& senderId,
                                     const std::vector<AgentRole>& targetRoles) {
    if (message.empty()) return;
    const auto agents = getAllAgents();
    for (const auto& agent : agents) {
        if (!agent) continue;
        const bool targeted = targetRoles.empty() ||
            std::find(targetRoles.begin(), targetRoles.end(), agent->getRole()) != targetRoles.end();
        if (targeted && agent->getId() != senderId) agent->processMessage(message, senderId);
    }
}

TheOrgManager::SystemMetrics TheOrgManager::getSystemMetrics() const {
    SystemMetrics metrics{};
    const auto agents = getAllAgents();
    metrics.totalAgents = agents.size();
    for (const auto& agent : agents) {
        if (!agent) continue;
        if (agent->isRunning()) ++metrics.activeAgents;
        const auto pending = agent->getPendingTasks();
        metrics.pendingTasks += pending.size();
        metrics.totalTasks += pending.size();
    }
    metrics.systemLoad = metrics.totalAgents == 0 ? 0.0 :
        static_cast<double>(metrics.activeAgents) / static_cast<double>(metrics.totalAgents);
    metrics.averageResponseTime = std::chrono::milliseconds(0);
    metrics.lastUpdated = std::chrono::system_clock::now();
    return metrics;
}

std::vector<std::shared_ptr<TheOrgAgent>> TheOrgManager::getAllAgents() const {
    std::vector<std::shared_ptr<TheOrgAgent>> result;
    std::lock_guard<std::mutex> lock(agentMutex_);
    result.reserve(agents_.size());
    for (const auto& [_, agent] : agents_) result.push_back(agent);
    std::sort(result.begin(), result.end(), [](const auto& left, const auto& right) {
        return left && right ? left->getId() < right->getId() : static_cast<bool>(left);
    });
    return result;
}

// Additional DeveloperRelationsAgent methods
std::string DeveloperRelationsAgent::retrieveKnowledge(const std::string& topic) const {
    const auto lower = [](std::string value) {
        std::transform(value.begin(), value.end(), value.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return value;
    };
    const std::string needle = lower(topic);
    std::lock_guard<std::mutex> lock(knowledgeMutex_);
    auto exact = knowledgeBase_.find(topic);
    if (exact != knowledgeBase_.end()) return exact->second.content;
    for (const auto& [key, entry] : knowledgeBase_) {
        bool matched = lower(key).find(needle) != std::string::npos ||
                       lower(entry.content).find(needle) != std::string::npos;
        for (const auto& tag : entry.tags) {
            matched = matched || lower(tag).find(needle) != std::string::npos;
        }
        if (matched) return entry.content;
    }
    return "Knowledge not found for topic: " + topic;
}

std::string DeveloperRelationsAgent::generateCodeExample(const std::string& topic, const std::string& language) const {
    std::string normalized = topic;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    if (language == "cpp" && normalized.find("agent") != std::string::npos) {
        return "AgentConfig config;\nconfig.agentName = \"Example\";\n"
               "CommunityManagerAgent agent(config);\nagent.initialize();\n";
    }
    if (language == "cpp" && normalized.find("memory") != std::string::npos) {
        return "auto memory = agent.createMemory(\"important context\", MemoryType::MESSAGE);\n"
               "agent.addMemory(memory);\n";
    }
    return "// " + language + " example for " + topic + "\n";
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
    processingThread_ = std::thread(&CommunityLiaisonAgent::processLoop, this);
    AgentLogger logger;
    logger.log("Community Liaison Agent started: " + config_.agentName);
}

void CommunityLiaisonAgent::stop() {
    if (!running_) return;
    running_ = false;
    if (processingThread_.joinable()) processingThread_.join();
    AgentLogger logger;
    logger.log("Community Liaison Agent stopped: " + config_.agentName);
}

void CommunityLiaisonAgent::pause() { paused_ = true; }
void CommunityLiaisonAgent::resume() { paused_ = false; }
bool CommunityLiaisonAgent::isRunning() const { return running_; }
void CommunityLiaisonAgent::processLoop() {
    while (running_) {
        if (!paused_) {
            while (auto message = popIncomingMessage()) {
                addMemory(createMemory("Processed liaison message: " + *message,
                                       MemoryType::MESSAGE));
            }
            updateTopicTrends();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
}

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

void CommunityLiaisonAgent::trackDiscussion(const UUID& orgId, const std::string& topic,
                                            const std::string& summary,
                                            const std::string& channelId) {
    if (orgId.empty() || topic.empty()) return;
    {
        std::lock_guard<std::mutex> lock(orgMutex_);
        if (organizations_.find(orgId) == organizations_.end()) return;
    }
    DiscussionEntry entry;
    entry.orgId = orgId;
    entry.topic = topic;
    entry.summary = summary;
    entry.channelId = channelId;
    entry.timestamp = std::chrono::system_clock::now();
    entry.engagementLevel = 1.0;
    {
        std::lock_guard<std::mutex> lock(discussionMutex_);
        discussionHistory_.push_back(std::move(entry));
        constexpr size_t kMaxDiscussionHistory = 10000;
        if (discussionHistory_.size() > kMaxDiscussionHistory) {
            discussionHistory_.erase(discussionHistory_.begin(),
                discussionHistory_.begin() + static_cast<std::ptrdiff_t>(
                    discussionHistory_.size() - kMaxDiscussionHistory));
        }
    }
    {
        std::lock_guard<std::mutex> lock(trendMutex_);
        topicOrgRelevance_[topic][orgId] += 1.0;
    }
}

std::vector<ParallelTopic> CommunityLiaisonAgent::identifyParallelTopics(
    std::chrono::hours timeWindow) const {
    const Timestamp cutoff = std::chrono::system_clock::now() - timeWindow;
    struct Aggregate {
        std::vector<UUID> organizations;
        std::vector<std::string> discussions;
        size_t count = 0;
    };
    std::unordered_map<std::string, Aggregate> grouped;
    {
        std::lock_guard<std::mutex> lock(discussionMutex_);
        for (const auto& discussion : discussionHistory_) {
            if (discussion.timestamp < cutoff) continue;
            auto& aggregate = grouped[discussion.topic];
            if (std::find(aggregate.organizations.begin(), aggregate.organizations.end(),
                          discussion.orgId) == aggregate.organizations.end()) {
                aggregate.organizations.push_back(discussion.orgId);
            }
            if (!discussion.summary.empty()) aggregate.discussions.push_back(discussion.summary);
            ++aggregate.count;
        }
    }
    std::vector<ParallelTopic> result;
    for (auto& [topic, aggregate] : grouped) {
        if (aggregate.organizations.size() < 2) continue;
        std::sort(aggregate.organizations.begin(), aggregate.organizations.end());
        ParallelTopic parallel;
        parallel.topic = topic;
        parallel.organizationIds = std::move(aggregate.organizations);
        parallel.recentDiscussions = std::move(aggregate.discussions);
        parallel.potentialSynergies = "Coordinate shared work on " + topic + " across " +
            std::to_string(parallel.organizationIds.size()) + " organizations";
        parallel.relevanceScore = std::min(1.0,
            static_cast<double>(aggregate.count) /
                static_cast<double>(parallel.organizationIds.size() * 2));
        result.push_back(std::move(parallel));
    }
    std::sort(result.begin(), result.end(), [](const auto& left, const auto& right) {
        if (left.relevanceScore != right.relevanceScore)
            return left.relevanceScore > right.relevanceScore;
        return left.topic < right.topic;
    });
    return result;
}

CrossOrgReport CommunityLiaisonAgent::generateDailyReport(
    const std::vector<UUID>& recipientOrgIds) const {
    CrossOrgReport report;
    report.id = config_.agentId + "-daily-" + std::to_string(
        std::chrono::system_clock::now().time_since_epoch().count());
    report.type = ReportType::DAILY;
    report.generatedAt = std::chrono::system_clock::now();
    report.date = the_org_utils::formatTimestamp(report.generatedAt, "%Y-%m-%d");
    report.recipientOrgIds = recipientOrgIds;
    report.content.parallelTopics = identifyParallelTopics(std::chrono::hours(24));
    report.content.overview = std::to_string(report.content.parallelTopics.size()) +
        " parallel topics identified in the last 24 hours";
    for (const auto& topic : report.content.parallelTopics)
        report.content.collaborationOpportunities.push_back(topic.potentialSynergies);
    return report;
}

CrossOrgReport CommunityLiaisonAgent::generateWeeklyReport(
    const std::vector<UUID>& recipientOrgIds) const {
    CrossOrgReport report;
    report.id = config_.agentId + "-weekly-" + std::to_string(
        std::chrono::system_clock::now().time_since_epoch().count());
    report.type = ReportType::WEEKLY;
    report.generatedAt = std::chrono::system_clock::now();
    report.date = the_org_utils::formatTimestamp(report.generatedAt, "%Y-%m-%d");
    report.recipientOrgIds = recipientOrgIds;
    report.content.parallelTopics = identifyParallelTopics(std::chrono::hours(168));
    report.content.overview = std::to_string(report.content.parallelTopics.size()) +
        " parallel topics identified in the last 7 days";
    for (const auto& topic : report.content.parallelTopics)
        report.content.collaborationOpportunities.push_back(topic.potentialSynergies);
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
    processingThread_ = std::thread(&SocialMediaManagerAgent::processLoop, this);
    AgentLogger logger;
    logger.log("Social Media Manager Agent started: " + config_.agentName);
}

void SocialMediaManagerAgent::stop() {
    if (!running_) return;
    running_ = false;
    if (processingThread_.joinable()) processingThread_.join();
    AgentLogger logger;
    logger.log("Social Media Manager Agent stopped: " + config_.agentName);
}

void SocialMediaManagerAgent::pause() { paused_ = true; }
void SocialMediaManagerAgent::resume() { paused_ = false; }
bool SocialMediaManagerAgent::isRunning() const { return running_; }
void SocialMediaManagerAgent::processLoop() {
    while (running_) {
        if (!paused_) {
            while (auto message = popIncomingMessage()) {
                addMemory(createMemory("Processed social-media message: " + *message,
                                       MemoryType::MESSAGE));
            }
            std::vector<UUID> due;
            const Timestamp now = std::chrono::system_clock::now();
            {
                std::lock_guard<std::mutex> lock(contentMutex_);
                for (const auto& [id, content] : content_) {
                    if (content.status == ContentStatus::SCHEDULED &&
                        content.scheduledTime && *content.scheduledTime <= now) {
                        due.push_back(id);
                    }
                }
            }
            for (const auto& id : due) publishContent(id);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

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

    // Return fully initialized default metrics.
    SocialMediaMetrics metrics{};
    metrics.platform = platform;
    metrics.followers = 0;
    metrics.totalPosts = 0;
    metrics.engagementRate = 0.0;
    metrics.lastUpdated = std::chrono::system_clock::now();
    return metrics;
}

std::string SocialMediaManagerAgent::generateAnalyticsReport(std::chrono::hours timeWindow) const {
    std::vector<SocialMediaMetrics> metrics;
    {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        for (const auto& [_, value] : platformMetrics_) metrics.push_back(value);
    }
    std::sort(metrics.begin(), metrics.end(), [](const auto& left, const auto& right) {
        return static_cast<int>(left.platform) < static_cast<int>(right.platform);
    });
    std::ostringstream report;
    report << "Social Media Analytics Report (last " << timeWindow.count() << "h)\n";
    std::uint64_t totalFollowers = 0;
    std::uint64_t totalPosts = 0;
    double weightedEngagement = 0.0;
    for (const auto& value : metrics) {
        totalFollowers += value.followers;
        totalPosts += value.totalPosts;
        weightedEngagement += value.engagementRate * static_cast<double>(value.totalPosts);
        report << "- " << the_org_utils::platformTypeToString(value.platform)
               << ": followers=" << value.followers
               << ", posts=" << value.totalPosts
               << ", engagement=" << value.engagementRate << '\n';
    }
    const double aggregate = totalPosts == 0 ? 0.0 :
        weightedEngagement / static_cast<double>(totalPosts);
    report << "Totals: followers=" << totalFollowers << ", posts=" << totalPosts
           << ", weighted_engagement=" << aggregate;
    return report.str();
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
    if (it != projects_.end() && !teamMemberId.empty() &&
        std::find(it->second.teamMemberIds.begin(), it->second.teamMemberIds.end(), teamMemberId) ==
            it->second.teamMemberIds.end()) {
        it->second.teamMemberIds.push_back(teamMemberId);
        it->second.updatedAt = std::chrono::system_clock::now();
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

void ProjectManagerAgent::scheduleDailyCheckins(const UUID& projectId) {
    std::vector<UUID> members;
    {
        std::lock_guard<std::mutex> lock(projectMutex_);
        auto project = projects_.find(projectId);
        if (project == projects_.end()) return;
        members = project->second.teamMemberIds;
    }
    std::vector<UUID> taskIds;
    taskIds.reserve(members.size());
    for (const auto& memberId : members) {
        const UUID taskId = createTask("Daily check-in: " + memberId,
            "Submit a daily progress update for project " + projectId, 1);
        if (!taskId.empty()) taskIds.push_back(taskId);
    }
    if (!taskIds.empty()) {
        std::lock_guard<std::mutex> lock(projectMutex_);
        auto project = projects_.find(projectId);
        if (project != projects_.end()) {
            project->second.taskIds.insert(project->second.taskIds.end(),
                                           taskIds.begin(), taskIds.end());
            project->second.updatedAt = std::chrono::system_clock::now();
        }
    }
}

std::vector<DailyUpdate> ProjectManagerAgent::getDailyUpdates(const UUID& projectId, const std::string& date) const {
    std::vector<DailyUpdate> result;
    std::lock_guard<std::mutex> lock(updateMutex_);
    for (const auto& update : dailyUpdates_) {
        if ((projectId.empty() || update.projectId == projectId) &&
            (date.empty() || update.date == date)) {
            result.push_back(update);
        }
    }
    return result;
}

std::vector<DailyUpdate> ProjectManagerAgent::getMemberUpdates(const UUID& teamMemberId,
                                                               std::chrono::hours timeWindow) const {
    std::vector<DailyUpdate> result;
    const auto cutoff = std::chrono::system_clock::now() - timeWindow;
    std::lock_guard<std::mutex> lock(updateMutex_);
    for (const auto& update : dailyUpdates_) {
        if (update.teamMemberId == teamMemberId && update.submittedAt >= cutoff) result.push_back(update);
    }
    return result;
}

std::string ProjectManagerAgent::generateTeamProductivityReport(
    const std::vector<UUID>& teamMemberIds, std::chrono::hours timeWindow) const {
    const Timestamp cutoff = std::chrono::system_clock::now() - timeWindow;
    std::unordered_map<UUID, std::vector<DailyUpdate>> grouped;
    {
        std::lock_guard<std::mutex> lock(updateMutex_);
        for (const auto& update : dailyUpdates_) {
            const bool selected = teamMemberIds.empty() ||
                std::find(teamMemberIds.begin(), teamMemberIds.end(), update.teamMemberId) !=
                    teamMemberIds.end();
            if (selected && update.submittedAt >= cutoff) grouped[update.teamMemberId].push_back(update);
        }
    }
    std::vector<UUID> members = teamMemberIds;
    if (members.empty()) {
        for (const auto& [memberId, _] : grouped) members.push_back(memberId);
    }
    std::sort(members.begin(), members.end());
    members.erase(std::unique(members.begin(), members.end()), members.end());
    std::ostringstream report;
    report << "Team Productivity Report (last " << timeWindow.count() << "h)\n";
    for (const auto& memberId : members) {
        const auto found = grouped.find(memberId);
        const size_t updateCount = found == grouped.end() ? 0 : found->second.size();
        size_t accomplishments = 0;
        size_t blockers = 0;
        if (found != grouped.end()) {
            for (const auto& update : found->second) {
                accomplishments += update.accomplishments.size();
                blockers += update.blockers.size();
            }
        }
        report << "- " << memberId << ": updates=" << updateCount
               << ", accomplishments=" << accomplishments
               << ", blockers=" << blockers << '\n';
    }
    return report.str();
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

std::vector<std::string> ProjectManagerAgent::getActiveBlockers(const UUID& projectId) const {
    std::vector<std::string> blockers;
    std::lock_guard<std::mutex> lock(updateMutex_);
    for (const auto& update : dailyUpdates_) {
        if (!projectId.empty() && update.projectId != projectId) continue;
        blockers.insert(blockers.end(), update.blockers.begin(), update.blockers.end());
    }
    std::sort(blockers.begin(), blockers.end());
    blockers.erase(std::unique(blockers.begin(), blockers.end()), blockers.end());
    return blockers;
}

// ============================================================================
// Additional TheOrgManager Methods
// ============================================================================

void TheOrgManager::pauseAllAgents() {
    const auto agents = getAllAgents();
    for (const auto& agent : agents) if (agent) agent->pause();
}

void TheOrgManager::resumeAllAgents() {
    const auto agents = getAllAgents();
    for (const auto& agent : agents) if (agent) agent->resume();
}

void TheOrgManager::sendDirectMessage(const UUID& fromAgentId, const UUID& toAgentId,
                                      const std::string& message) {
    if (message.empty()) return;
    auto agent = getAgent(toAgentId);
    if (agent) agent->processMessage(message, fromAgentId.empty() ? "manager" : fromAgentId);
}

void TheOrgManager::loadConfiguration(const std::string& configPath) {
    if (configPath.empty()) return;
    try {
        std::ifstream input(configPath);
        if (!input) return;
        json document;
        input >> document;
        if (!document.is_object() || document.value("schema", std::string{}) !=
                "elizaos.the_org.configuration" || document.value("version", 0) != 1) {
            return;
        }

        std::unordered_map<std::string, std::string> loadedSettings;
        if (document.contains("settings")) {
            if (!document["settings"].is_object()) return;
            for (const auto& [key, value] : document["settings"].items()) {
                if (!value.is_string()) return;
                loadedSettings[key] = value.get<std::string>();
            }
        }

        std::unordered_map<PlatformType, PlatformConfig> loadedPlatforms;
        if (document.contains("platforms")) {
            if (!document["platforms"].is_array()) return;
            for (const auto& value : document["platforms"]) {
                if (!value.is_object() || !value.contains("type") || !value["type"].is_string()) return;
                PlatformConfig platform{};
                std::string type = value["type"].get<std::string>();
                std::transform(type.begin(), type.end(), type.begin(),
                    [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
                const std::array<std::string, 7> validTypes = {
                    "discord", "telegram", "twitter", "slack", "facebook", "linkedin", "github"};
                if (std::find(validTypes.begin(), validTypes.end(), type) == validTypes.end()) return;
                platform.type = the_org_utils::stringToPlatformType(type);
                platform.applicationId = value.value("application_id", std::string{});
                platform.apiToken = value.value("api_token", std::string{});
                platform.webhookUrl = value.value("webhook_url", std::string{});
                if (value.contains("additional_settings")) {
                    if (!value["additional_settings"].is_object()) return;
                    platform.additionalSettings =
                        value["additional_settings"].get<std::unordered_map<std::string, std::string>>();
                }
                loadedPlatforms[platform.type] = std::move(platform);
            }
        }

        bool loggingEnabled = false;
        std::string loggingPath;
        std::string loggingLevel = "INFO";
        if (document.contains("event_logging")) {
            const auto& logging = document["event_logging"];
            if (!logging.is_object()) return;
            loggingEnabled = logging.value("enabled", false);
            loggingPath = logging.value("path", std::string{});
            loggingLevel = logging.value("level", std::string{"INFO"});
        }

        {
            std::lock_guard<std::mutex> lock(settingsMutex_);
            globalSettings_ = std::move(loadedSettings);
        }
        {
            std::lock_guard<std::mutex> lock(platformMutex_);
            globalPlatforms_ = std::move(loadedPlatforms);
        }
        {
            std::lock_guard<std::mutex> lock(logMutex_);
            eventLoggingEnabled_ = loggingEnabled;
            logPath_ = std::move(loggingPath);
            logLevel_ = std::move(loggingLevel);
        }
    } catch (const std::exception&) {
        // Transactional load: malformed or unreadable data leaves current state unchanged.
    }
}

void TheOrgManager::saveConfiguration(const std::string& configPath) const {
    if (configPath.empty()) return;
    json document = {
        {"schema", "elizaos.the_org.configuration"},
        {"version", 1},
        {"settings", json::object()},
        {"platforms", json::array()},
        {"event_logging", json::object()}
    };
    {
        std::lock_guard<std::mutex> lock(settingsMutex_);
        document["settings"] = globalSettings_;
    }
    {
        std::lock_guard<std::mutex> lock(platformMutex_);
        std::vector<PlatformConfig> platforms;
        platforms.reserve(globalPlatforms_.size());
        for (const auto& [_, platform] : globalPlatforms_) platforms.push_back(platform);
        std::sort(platforms.begin(), platforms.end(), [](const auto& left, const auto& right) {
            return static_cast<int>(left.type) < static_cast<int>(right.type);
        });
        for (const auto& platform : platforms) {
            document["platforms"].push_back({
                {"type", the_org_utils::platformTypeToString(platform.type)},
                {"application_id", platform.applicationId},
                {"api_token", platform.apiToken},
                {"webhook_url", platform.webhookUrl},
                {"additional_settings", platform.additionalSettings}
            });
        }
    }
    {
        std::lock_guard<std::mutex> lock(logMutex_);
        document["event_logging"] = {
            {"enabled", eventLoggingEnabled_}, {"path", logPath_}, {"level", logLevel_}};
    }

    try {
        const fs::path destination(configPath);
        if (!destination.parent_path().empty()) fs::create_directories(destination.parent_path());
        fs::path temporary = destination;
        temporary += ".tmp";
        {
            std::ofstream output(temporary, std::ios::trunc);
            if (!output) return;
            output << document.dump(2) << '\n';
            output.flush();
            if (!output) return;
        }
        std::error_code ec;
        fs::rename(temporary, destination, ec);
        if (ec) {
            fs::remove(destination, ec);
            ec.clear();
            fs::rename(temporary, destination, ec);
        }
        if (ec) fs::remove(temporary, ec);
    } catch (const std::exception&) {
        // Save is best-effort because the legacy API has no error return channel.
    }
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
    const auto agents = getAllAgents();
    for (const auto& agent : agents) {
        if (agent && !agent->isRunning()) {
            AgentLogger logger;
            logger.log("Agent " + agent->getId() + " is not running", "", "Health", LogLevel::WARNING);
        }
    }
}

void TheOrgManager::enableEventLogging(const std::string& logPath) {
    if (logPath.empty()) return;
    try {
        const fs::path path(logPath);
        if (!path.parent_path().empty()) fs::create_directories(path.parent_path());
        std::ofstream probe(path, std::ios::app);
        if (!probe) return;
    } catch (const std::exception&) {
        return;
    }
    std::lock_guard<std::mutex> lock(logMutex_);
    eventLoggingEnabled_ = true;
    logPath_ = logPath;
}

void TheOrgManager::disableEventLogging() {
    std::lock_guard<std::mutex> lock(logMutex_);
    eventLoggingEnabled_ = false;
}

std::vector<std::string> TheOrgManager::getRecentEvents(std::chrono::hours timeWindow) const {
    const Timestamp cutoff = std::chrono::system_clock::now() - timeWindow;
    std::vector<std::string> result;
    std::lock_guard<std::mutex> lock(logMutex_);
    for (const auto& [timestamp, event] : eventHistory_) {
        if (timestamp >= cutoff) result.push_back(event);
    }
    return result;
}

void TheOrgManager::setLogLevel(const std::string& level) {
    std::string normalized = level;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
        [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    static const std::array<std::string, 5> accepted = {"DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"};
    if (std::find(accepted.begin(), accepted.end(), normalized) == accepted.end()) return;
    std::lock_guard<std::mutex> lock(logMutex_);
    logLevel_ = std::move(normalized);
}

// ============================================================================
// Additional DeveloperRelationsAgent Methods
// ============================================================================

std::string DeveloperRelationsAgent::provideAPIReference(const std::string& apiName) const {
    if (apiName.empty()) return "API name is required.";
    const auto matches = searchDocumentation(apiName);
    if (!matches.empty()) {
        std::lock_guard<std::mutex> lock(docMutex_);
        const auto entry = std::find_if(documentationIndex_.begin(), documentationIndex_.end(),
            [&](const DocumentationEntry& value) { return value.path == matches.front(); });
        if (entry != documentationIndex_.end()) {
            std::ostringstream reference;
            reference << "API Reference: " << apiName << "\nSource: " << entry->path
                      << "\nVersion: " << entry->version;
            if (!entry->content.empty()) reference << "\n\n" << entry->content.substr(0, 1200);
            return reference.str();
        }
    }
    return "API Reference: " + apiName +
        "\nNo indexed document matched. Index the public header or documentation file with "
        "indexDocumentation(path, version), then request this reference again.";
}

std::string DeveloperRelationsAgent::diagnoseIssue(const std::string& errorMessage,
                                                    const std::string& context) const {
    if (errorMessage.empty()) return "Diagnosis requires an error message.";
    std::string normalized = errorMessage + " " + context;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    std::string cause = "inspect the first failing boundary and its immediate inputs";
    if (normalized.find("permission") != std::string::npos) cause = "verify filesystem or credential permissions";
    else if (normalized.find("timeout") != std::string::npos) cause = "inspect transport latency, cancellation, and timeout configuration";
    else if (normalized.find("null") != std::string::npos || normalized.find("segmentation") != std::string::npos)
        cause = "capture a sanitizer trace and validate object lifetime before dereference";
    else if (normalized.find("link") != std::string::npos || normalized.find("undefined reference") != std::string::npos)
        cause = "verify target linkage, source registration, and ABI-compatible declarations";
    return "Diagnosis for '" + errorMessage + "': " + cause + ". Context: " + context;
}

std::vector<std::string> DeveloperRelationsAgent::suggestSolutions(
    const std::string& problemDescription) const {
    std::vector<std::string> solutions = {
        "Reproduce the failure with the smallest deterministic input",
        "Inspect the first error rather than downstream cascade failures",
        "Add a regression test before applying the repair"};
    std::string normalized = problemDescription;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    if (normalized.find("build") != std::string::npos || normalized.find("link") != std::string::npos)
        solutions.push_back("Regenerate the build graph and verify the owning target links every required library");
    if (normalized.find("race") != std::string::npos || normalized.find("thread") != std::string::npos)
        solutions.push_back("Run the focused test under ThreadSanitizer and define a single lock-order policy");
    if (normalized.find("memory") != std::string::npos || normalized.find("crash") != std::string::npos)
        solutions.push_back("Run AddressSanitizer and UndefinedBehaviorSanitizer with fail-fast options");
    return solutions;
}

std::string DeveloperRelationsAgent::generateTutorial(const std::string& topic,
                                                       const std::string& difficulty) const {
    if (topic.empty()) return "A tutorial topic is required.";
    std::ostringstream tutorial;
    tutorial << "# " << topic << " Tutorial (" << (difficulty.empty() ? "intermediate" : difficulty)
             << ")\n\n1. Define the observable contract and failure modes.\n"
             << "2. Build the smallest working example.\n"
             << "3. Add deterministic unit and integration assertions.\n"
             << "4. Exercise invalid input, timeout, and persistence paths.\n"
             << "5. Run sanitizers and document the verified behavior.\n";
    return tutorial.str();
}

std::string DeveloperRelationsAgent::reviewCode(const std::string& code,
                                                 const std::string& language) const {
    if (code.empty()) return "Code review: no code supplied.";
    size_t lines = 1;
    size_t todoCount = 0;
    size_t longLines = 0;
    std::istringstream input(code);
    std::string line;
    lines = 0;
    while (std::getline(input, line)) {
        ++lines;
        if (line.size() > 120) ++longLines;
        if (line.find("TODO") != std::string::npos || line.find("FIXME") != std::string::npos) ++todoCount;
    }
    std::ostringstream review;
    review << "Code review (" << (language.empty() ? "unspecified" : language) << "): "
           << lines << " lines, " << todoCount << " unresolved markers, "
           << longLines << " lines over 120 characters. ";
    review << (todoCount == 0 ? "No explicit incomplete markers found. " :
        "Resolve incomplete markers or document their external dependency. ");
    review << "Add tests for invalid inputs, state transitions, and failure recovery.";
    return review.str();
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

    for (const unsigned char raw : content) {
        const char c = static_cast<char>(raw);
        if (c == '#') {
            if (inHashtag && current.size() > 1) hashtags.push_back(current);
            current = "#";
            inHashtag = true;
        } else if (inHashtag) {
            if (std::isalnum(raw) || c == '_') {
                current += c;
            } else {
                if (current.size() > 1) hashtags.push_back(current);
                current.clear();
                inHashtag = false;
            }
        }
    }

    if (inHashtag && current.size() > 1) hashtags.push_back(current);
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
    if (!validateUrl(url)) return {};
    std::string domain;

    size_t protocolEnd = url.find("://");
    size_t start = (protocolEnd != std::string::npos) ? protocolEnd + 3 : 0;

    // Remove www. prefix if present
    if (url.substr(start, 4) == "www.") {
        start += 4;
    }

    // Find end of domain (path or query std::string)
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
