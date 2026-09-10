#include "elizaos/the_org.hpp"
#include "elizaos/agentlogger.hpp"
#include <algorithm>
#include <sstream>
#include <random>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <unordered_set>
#include <cctype>
#include <nlohmann/json.hpp>

namespace elizaos {

// Utility functions
namespace the_org_utils {
    
    UUID generateAgentId(AgentRole role) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 15);

        const char* prefix = "agent_";
        switch (role) {
            case AgentRole::COMMUNITY_MANAGER: prefix = "cm_"; break;
            case AgentRole::DEVELOPER_RELATIONS: prefix = "dr_"; break;
            case AgentRole::COMMUNITY_LIAISON: prefix = "cl_"; break;
            case AgentRole::PROJECT_MANAGER: prefix = "pm_"; break;
            case AgentRole::SOCIAL_MEDIA_MANAGER: prefix = "sm_"; break;
        }

        std::stringstream ss;
        ss << prefix << std::hex;
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
    if (content.empty()) return nullptr;
    UUID memId = config_.agentId + "-mem-" + std::to_string(sequence_.fetch_add(1));
    return std::make_shared<Memory>(memId, content, config_.agentId, config_.agentId);
}

void TheOrgAgent::addMemory(std::shared_ptr<Memory> memory) {
    if (!memory) return;
    std::lock_guard<std::mutex> lock(memoryMutex_);
    memoryStore_.push_back(std::move(memory));
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

void TheOrgAgent::setPlatformAdapter(PlatformType type, PlatformAdapter adapter) {
    std::lock_guard<std::mutex> lock(platformMutex_);
    if (adapter) platformAdapters_[type] = std::move(adapter);
    else platformAdapters_.erase(type);
}

void TheOrgAgent::clearPlatformAdapter(PlatformType type) {
    std::lock_guard<std::mutex> lock(platformMutex_);
    platformAdapters_.erase(type);
}

bool TheOrgAgent::sendMessage(PlatformType platform, const std::string& channelId, const std::string& message) {
    if (channelId.empty() || !validateMessage(message)) return false;
    PlatformConfig config;
    PlatformAdapter adapter;
    {
        std::lock_guard<std::mutex> lock(platformMutex_);
        auto pit = platforms_.find(platform);
        auto ait = platformAdapters_.find(platform);
        if (pit == platforms_.end() || ait == platformAdapters_.end() || !ait->second) return false;
        config = pit->second;
        adapter = ait->second;
    }
    bool accepted = false;
    try { accepted = adapter(config, channelId, formatResponse(message, platform)); }
    catch (...) { accepted = false; }
    if (!accepted) return false;
    {
        std::lock_guard<std::mutex> lock(platformMutex_);
        auto& messages = channelMessages_[platform][channelId];
        messages.push_back(message);
        constexpr std::size_t capacity = 1000;
        if (messages.size() > capacity) messages.erase(messages.begin(), messages.begin() + (messages.size() - capacity));
    }
    return true;
}

std::vector<std::string> TheOrgAgent::getRecentMessages(PlatformType platform, const std::string& channelId, size_t count) {
    if (channelId.empty() || count == 0) return {};
    std::lock_guard<std::mutex> lock(platformMutex_);
    auto pit = channelMessages_.find(platform);
    if (pit == channelMessages_.end()) return {};
    auto cit = pit->second.find(channelId);
    if (cit == pit->second.end()) return {};
    const auto& stored = cit->second;
    const auto begin = stored.size() > count ? stored.size() - count : 0;
    return {stored.begin() + static_cast<std::ptrdiff_t>(begin), stored.end()};
}

void TheOrgAgent::sendToAgent(const UUID& agentId, const std::string& message, const std::string& type) {
    DeliveryEvidence evidence{agentId, message, type, std::chrono::system_clock::now(), false};
    if (!agentId.empty() && !type.empty() && validateMessage(message)) {
        std::lock_guard<std::mutex> lock(messageMutex_);
        incomingMessages_.push("To " + agentId + " [" + type + "]: " + message);
        evidence.accepted = true;
        deliveryEvidence_.push_back(evidence);
        while (deliveryEvidence_.size() > 1000) deliveryEvidence_.erase(deliveryEvidence_.begin());
        lifecycleCv_.notify_all();
        return;
    }
    std::lock_guard<std::mutex> lock(messageMutex_);
    deliveryEvidence_.push_back(std::move(evidence));
}

std::vector<TheOrgAgent::DeliveryEvidence> TheOrgAgent::getDeliveryEvidence() const {
    std::lock_guard<std::mutex> lock(messageMutex_);
    return deliveryEvidence_;
}

std::queue<std::string> TheOrgAgent::getIncomingMessages() {
    std::lock_guard<std::mutex> lock(messageMutex_);
    return incomingMessages_;
}

void TheOrgAgent::processMessage(const std::string& message, const std::string& senderId) {
    if (!validateMessage(message)) return;
    {
        std::lock_guard<std::mutex> lock(messageMutex_);
        incomingMessages_.push(senderId.empty() ? message : "From " + senderId + ": " + message);
    }
    lifecycleCv_.notify_all();
}

std::vector<std::string> TheOrgAgent::drainIncomingMessages() {
    std::lock_guard<std::mutex> lock(messageMutex_);
    std::vector<std::string> drained;
    while (!incomingMessages_.empty()) {
        drained.push_back(std::move(incomingMessages_.front()));
        incomingMessages_.pop();
    }
    return drained;
}

void TheOrgAgent::waitForWork(std::chrono::milliseconds duration) {
    std::unique_lock<std::mutex> lock(lifecycleMutex_);
    lifecycleCv_.wait_for(lock, duration, [this] { return !running_.load(); });
}

UUID TheOrgAgent::createTask(const std::string& name, const std::string& description, int priority) {
    if (name.empty() || name.size() > 512 || description.size() > 10000 || priority < -1000 || priority > 1000) return {};
    const UUID id = config_.agentId + "-task-" + std::to_string(sequence_.fetch_add(1));
    auto task = std::make_shared<Task>(id, name, description);
    task->setPriority(priority);
    std::lock_guard<std::mutex> lock(taskMutex_);
    tasks_[id] = task;
    return id;
}

bool TheOrgAgent::completeTask(const UUID& taskId) {
    if (taskId.empty()) return false;
    std::lock_guard<std::mutex> lock(taskMutex_);
    auto it = tasks_.find(taskId);
    if (it == tasks_.end() || !it->second) return false;
    const auto status = it->second->getStatus();
    if (status != TaskStatus::PENDING && status != TaskStatus::RUNNING) return false;
    it->second->setStatus(TaskStatus::COMPLETED);
    it->second->updateTimestamp();
    return true;
}

std::vector<std::shared_ptr<Task>> TheOrgAgent::getPendingTasks() {
    std::lock_guard<std::mutex> lock(taskMutex_);
    std::vector<std::shared_ptr<Task>> pending;
    for (const auto& [id, task] : tasks_) if (task && task->getStatus() == TaskStatus::PENDING) pending.push_back(task);
    std::sort(pending.begin(), pending.end(), [](const auto& a, const auto& b) {
        if (a->getPriority() != b->getPriority()) return a->getPriority() > b->getPriority();
        return a->getId() < b->getId();
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
    : TheOrgAgent(config, AgentRole::COMMUNITY_MANAGER) {}
CommunityManagerAgent::~CommunityManagerAgent() { stop(); }

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
    {
        std::lock_guard<std::mutex> lock(threadMutex_);
        if (running_) return;
        if (processingThread_.joinable()) processingThread_.join();
        running_ = true;
        paused_ = false;
        processingThread_ = std::thread(&CommunityManagerAgent::processLoop, this);
    }
    AgentLogger logger;
    logger.log("Community Manager Agent started: " + config_.agentName);
}


void CommunityManagerAgent::stop() {
    {
        std::lock_guard<std::mutex> lock(threadMutex_);
        if (!running_ && !processingThread_.joinable()) return;
        running_ = false;
        lifecycleCv_.notify_all();
        if (processingThread_.joinable() && processingThread_.get_id() != std::this_thread::get_id())
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
            auto messages = drainIncomingMessages();
            for (const auto& message : messages) {
                trackUserActivity("queue", "message:" + message);
                noteProcessedWork();
            }
            updateCommunityMetrics();
        }
        waitForWork();
    }
}

void CommunityManagerAgent::enableNewUserGreeting(const std::string& channelId, const std::string& greetingMessage) {
    std::lock_guard<std::mutex> lock(communityStateMutex_);
    greetingEnabled_ = !channelId.empty();
    greetingChannelId_ = channelId;
    customGreetingMessage_ = greetingMessage;
}

void CommunityManagerAgent::disableNewUserGreeting() {
    std::lock_guard<std::mutex> lock(communityStateMutex_);
    greetingEnabled_ = false;
}

bool CommunityManagerAgent::shouldGreetNewUser(const std::string& userId) const {
    if (userId.empty()) return false;
    std::lock_guard<std::mutex> lock(communityStateMutex_);
    return greetingEnabled_ && greetedUsers_.count(userId) == 0;
}

std::string CommunityManagerAgent::generateGreeting(const std::string& userName, const std::string& serverName) const {
    std::string customGreeting;
    {
        std::lock_guard<std::mutex> lock(communityStateMutex_);
        customGreeting = customGreetingMessage_;
    }
    if (!customGreeting.empty()) {
        std::string greeting = std::move(customGreeting);
        auto replaceAll = [&greeting](const std::string& token, const std::string& value) {
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
    std::string lowerMessage = message;
    std::transform(lowerMessage.begin(), lowerMessage.end(), lowerMessage.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    std::optional<std::pair<ModerationAction, std::string>> match;
    {
        std::lock_guard<std::mutex> lock(rulesMutex_);
        for (const auto& [rule, actionInfo] : moderationRules_) {
            if (!rule.empty() && lowerMessage.find(rule) != std::string::npos) {
                match = actionInfo;
                break;
            }
        }
    }
    if (!match) return true;
    applyModerationAction(userId, match->first, match->second);
    return false;
}

void CommunityManagerAgent::applyModerationAction(const std::string& userId, ModerationAction action, 
                                                   const std::string& reason, std::optional<std::chrono::seconds> duration) {
    if (userId.empty() || reason.empty() || (duration && duration->count() <= 0)) return;
    ModerationEvent event;
    event.id = config_.agentId + "-mod-" + std::to_string(sequence_.fetch_add(1));
    event.userId = userId;
    event.moderatorId = config_.agentId;
    event.action = action;
    event.reason = reason;
    event.duration = duration;
    event.timestamp = std::chrono::system_clock::now();
    
    {
        std::lock_guard<std::mutex> lock(communityStateMutex_);
        moderationHistory_.push_back(event);
    }
    
    AgentLogger logger;
    logger.log("Moderation action applied to user " + userId + ": " + reason, "", "Moderation", LogLevel::WARNING);
}

CommunityMetrics CommunityManagerAgent::generateCommunityMetrics() const {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    return currentMetrics_;
}

void CommunityManagerAgent::trackUserActivity(const std::string& userId, const std::string& activity) {
    if (userId.empty() || activity.empty()) return;
    std::lock_guard<std::mutex> lock(activityMutex_);
    userActivity_[userId].emplace_back(std::chrono::system_clock::now(), activity);
}

std::vector<std::string> CommunityManagerAgent::identifyActiveUsers(std::chrono::hours timeWindow) const {
    std::lock_guard<std::mutex> lock(activityMutex_);
    std::vector<std::string> activeUsers;
    
    auto now = std::chrono::system_clock::now();
    auto cutoff = now - timeWindow;
    
    for (const auto& [userId, activities] : userActivity_) {
        for (const auto& activity : activities) {
            if (activity.first >= cutoff) {
                activeUsers.push_back(userId);
                break;
            }
        }
    }
    
    return activeUsers;
}

std::vector<std::string> CommunityManagerAgent::getTopTopics(std::chrono::hours timeWindow) const {
    const auto cutoff = std::chrono::system_clock::now() - timeWindow;
    std::unordered_map<std::string, std::size_t> counts;
    static const std::unordered_set<std::string> stop = {"from","this","that","with","have","your","message","queue","sent","user","channel"};
    std::lock_guard<std::mutex> lock(activityMutex_);
    for (const auto& [user, activities] : userActivity_) for (const auto& [time, activity] : activities) {
        if (time < cutoff) continue;
        std::string word;
        for (unsigned char c : activity) {
            if (std::isalnum(c) || c == '_') word.push_back(static_cast<char>(std::tolower(c)));
            else if (word.size() >= 4) { if (!stop.count(word)) ++counts[word]; word.clear(); } else word.clear();
        }
        if (word.size() >= 4 && !stop.count(word)) ++counts[word];
    }
    std::vector<std::pair<std::string,std::size_t>> ranked(counts.begin(), counts.end());
    std::sort(ranked.begin(), ranked.end(), [](const auto& a,const auto& b){ return a.second != b.second ? a.second > b.second : a.first < b.first; });
    std::vector<std::string> result;
    for (const auto& [topic,count] : ranked) { result.push_back(topic); if (result.size()==10) break; }
    return result;
}

void CommunityManagerAgent::initiateConflictResolution(const std::vector<std::string>& userIds, const std::string& channelId) {
    if (channelId.empty() || userIds.size() < 2 || std::any_of(userIds.begin(), userIds.end(), [](const auto& id){ return id.empty(); })) return;
    std::vector<std::string> sorted = userIds;
    std::sort(sorted.begin(), sorted.end()); sorted.erase(std::unique(sorted.begin(), sorted.end()), sorted.end());
    if (sorted.size() < 2) return;
    std::lock_guard<std::mutex> lock(communityStateMutex_);
    conflictCases_.push_back(channelId + ":" + the_org_utils::joinText(sorted, ","));
}

void CommunityManagerAgent::escalateIssue(const std::string& description, const std::vector<std::string>& involvedUsers) {
    if (description.empty()) return;
    std::vector<std::string> users = involvedUsers;
    std::sort(users.begin(), users.end()); users.erase(std::unique(users.begin(), users.end()), users.end());
    std::lock_guard<std::mutex> lock(communityStateMutex_);
    escalations_.push_back(description + (users.empty() ? "" : " [" + the_org_utils::joinText(users, ",") + "]"));
}

void CommunityManagerAgent::scheduleEvent(const std::string& eventName, const std::string& description, Timestamp scheduledTime) {
    if (eventName.empty() || scheduledTime == Timestamp{}) return;
    CommunityEventRecord event;
    event.id = config_.agentId + "-event-" + std::to_string(sequence_.fetch_add(1));
    event.name = eventName; event.description = description; event.scheduledTime = scheduledTime;
    std::lock_guard<std::mutex> lock(communityStateMutex_);
    communityEvents_[event.id] = std::move(event);
}

void CommunityManagerAgent::announceEvent(const std::string& eventId, const std::vector<std::string>& channelIds) {
    if (eventId.empty()) return;
    CommunityEventRecord snapshot;
    {
        std::lock_guard<std::mutex> lock(communityStateMutex_);
        auto it = communityEvents_.find(eventId); if (it == communityEvents_.end()) return;
        snapshot = it->second;
    }
    std::vector<std::string> accepted;
    for (const auto& channel : channelIds) if (sendMessage(PlatformType::DISCORD, channel, "Event: " + snapshot.name + " - " + snapshot.description)) accepted.push_back(channel);
    std::lock_guard<std::mutex> lock(communityStateMutex_);
    auto it = communityEvents_.find(eventId); if (it != communityEvents_.end()) it->second.announcedChannels.insert(it->second.announcedChannels.end(), accepted.begin(), accepted.end());
}

void CommunityManagerAgent::trackEventParticipation(const std::string& eventId, const std::string& userId) {
    if (eventId.empty() || userId.empty()) return;
    std::lock_guard<std::mutex> lock(communityStateMutex_);
    auto it = communityEvents_.find(eventId); if (it != communityEvents_.end()) it->second.participants.insert(userId);
}

void CommunityManagerAgent::recordNewUser(const std::string& userId, const std::string& serverId) { processNewUserJoin(userId, serverId); }
void CommunityManagerAgent::recordCommunityMessage(const std::string& message, const std::string& userId, const std::string& channelId) {
    if (!validateMessage(message) || userId.empty() || channelId.empty()) return;
    trackUserActivity(userId, "message " + channelId + " " + message);
    processMessageForModeration(message, userId, channelId);
}

void CommunityManagerAgent::processNewUserJoin(const std::string& userId, const std::string& serverId) {
    if (userId.empty()) return;
    bool shouldSend = false;
    std::string greetingChannel;
    {
        std::lock_guard<std::mutex> lock(communityStateMutex_);
        knownUsers_.insert(userId);
        shouldSend = greetingEnabled_ && greetedUsers_.insert(userId).second;
        greetingChannel = greetingChannelId_;
    }
    trackUserActivity(userId, "joined " + serverId);
    if (shouldSend) sendMessage(PlatformType::DISCORD, greetingChannel, generateGreeting(userId, serverId));
}

void CommunityManagerAgent::processMessageForModeration(const std::string& message, const std::string& userId, const std::string& channelId) {
    evaluateMessage(message, userId, channelId);
}

void CommunityManagerAgent::generateDailyReport() {
    updateCommunityMetrics();
    const auto metrics = generateCommunityMetrics();
    std::ostringstream out;
    out << "members=" << metrics.totalMembers << ", active=" << metrics.activeMembers
        << ", new=" << metrics.newMembersToday << ", messages=" << metrics.messagesPerDay;
    std::lock_guard<std::mutex> lock(communityStateMutex_);
    dailyReports_.push_back(out.str());
}

std::string CommunityManagerAgent::generateDailyCommunityReport() {
    generateDailyReport();
    std::lock_guard<std::mutex> lock(communityStateMutex_);
    return dailyReports_.empty() ? std::string{} : dailyReports_.back();
}

void CommunityManagerAgent::updateCommunityMetrics() {
    const auto now = std::chrono::system_clock::now();
    const auto cutoff = now - std::chrono::hours(24);
    std::size_t messages = 0, newMembers = 0;
    std::unordered_map<std::string,std::size_t> channels;
    {
        std::lock_guard<std::mutex> lock(activityMutex_);
        for (const auto& [user, activities] : userActivity_) for (const auto& [time, activity] : activities) if (time >= cutoff) {
            if (activity.rfind("message ",0)==0) { ++messages; auto p=activity.find(' ',8); ++channels[activity.substr(8,p==std::string::npos?std::string::npos:p-8)]; }
            if (activity.rfind("joined ",0)==0) ++newMembers;
        }
    }
    std::size_t totalMembers;
    { std::lock_guard<std::mutex> lock(communityStateMutex_); totalMembers = knownUsers_.size(); }
    auto active = identifyActiveUsers(std::chrono::hours(24));
    auto topics = getTopTopics(std::chrono::hours(24));
    std::vector<std::pair<std::string,std::size_t>> ranked(channels.begin(),channels.end());
    std::sort(ranked.begin(),ranked.end(),[](const auto&a,const auto&b){return a.second!=b.second?a.second>b.second:a.first<b.first;});
    CommunityMetrics next;
    next.totalMembers=totalMembers; next.activeMembers=active.size(); next.newMembersToday=newMembers; next.messagesPerDay=messages;
    next.engagementRate=totalMembers?static_cast<double>(active.size())/totalMembers:0.0; next.topTopics=std::move(topics); next.lastUpdated=now;
    for (const auto& [channel,count]:ranked) { next.mostActiveChannels.push_back(channel); if(next.mostActiveChannels.size()==10) break; }
    std::lock_guard<std::mutex> lock(metricsMutex_); currentMetrics_=std::move(next);
}

std::vector<ModerationEvent> CommunityManagerAgent::getModerationHistory() const { std::lock_guard<std::mutex> lock(communityStateMutex_); return moderationHistory_; }
std::vector<std::string> CommunityManagerAgent::getEscalations() const { std::lock_guard<std::mutex> lock(communityStateMutex_); return escalations_; }
std::vector<std::string> CommunityManagerAgent::getConflictCases() const { std::lock_guard<std::mutex> lock(communityStateMutex_); return conflictCases_; }
std::vector<CommunityEventRecord> CommunityManagerAgent::getScheduledEvents() const { std::lock_guard<std::mutex> lock(communityStateMutex_); std::vector<CommunityEventRecord> out; for(const auto& [id,e]:communityEvents_) out.push_back(e); std::sort(out.begin(),out.end(),[](const auto&a,const auto&b){return a.id<b.id;}); return out; }
std::vector<std::string> CommunityManagerAgent::getDailyReports() const { std::lock_guard<std::mutex> lock(communityStateMutex_); return dailyReports_; }

// ============================================================================
// DeveloperRelationsAgent Implementation
// ============================================================================

DeveloperRelationsAgent::DeveloperRelationsAgent(const AgentConfig& config)
    : TheOrgAgent(config, AgentRole::DEVELOPER_RELATIONS) {}
DeveloperRelationsAgent::~DeveloperRelationsAgent() { stop(); }

void DeveloperRelationsAgent::initialize() {
    AgentLogger logger;
    logger.log("Initializing Developer Relations Agent: " + config_.agentName);
}

void DeveloperRelationsAgent::start() {
    {
        std::lock_guard<std::mutex> lock(threadMutex_);
        if (running_) return;
        if (processingThread_.joinable()) processingThread_.join();
        running_ = true;
        paused_ = false;
        processingThread_ = std::thread(&DeveloperRelationsAgent::processLoop, this);
    }
    AgentLogger logger;
    logger.log("Developer Relations Agent started: " + config_.agentName);
}


void DeveloperRelationsAgent::stop() {
    {
        std::lock_guard<std::mutex> lock(threadMutex_);
        if (!running_ && !processingThread_.joinable()) return;
        running_ = false;
        lifecycleCv_.notify_all();
        if (processingThread_.joinable() && processingThread_.get_id() != std::this_thread::get_id())
            processingThread_.join();
    }
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
            auto messages = drainIncomingMessages();
            for (const auto& message : messages) { processQuestion(message, "queue", "general"); noteProcessedWork(); }
        }
        waitForWork();
    }
}

void DeveloperRelationsAgent::indexDocumentation(const std::string& docPath, const std::string& version) {
    DocumentationEntry entry;
    entry.path = docPath;
    entry.version = version;
    entry.lastUpdated = std::chrono::system_clock::now();
    {
        std::lock_guard<std::mutex> lock(docMutex_);
        auto it = std::find_if(documentationIndex_.begin(), documentationIndex_.end(),
            [&docPath](const DocumentationEntry& existing) {
                return existing.path == docPath;
            });
        if (it == documentationIndex_.end()) documentationIndex_.push_back(std::move(entry));
        else *it = std::move(entry);
    }
    AgentLogger logger;
    logger.log("Indexed documentation: " + docPath + " (version: " + version + ")");
}

void DeveloperRelationsAgent::addTechnicalKnowledge(const std::string& topic, const std::string& content,
                                                     const std::vector<std::string>& tags) {
    KnowledgeEntry entry;
    entry.topic = topic;
    entry.content = content;
    entry.tags = tags;
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
    auto lower = [](std::string value) {
        std::transform(value.begin(), value.end(), value.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return value;
    };
    const std::string needle = lower(query);
    std::lock_guard<std::mutex> lock(docMutex_);
    std::vector<std::string> results;
    for (const auto& entry : documentationIndex_) {
        if (needle.empty() || lower(entry.path).find(needle) != std::string::npos ||
            lower(entry.content).find(needle) != std::string::npos ||
            lower(entry.version).find(needle) != std::string::npos ||
            std::any_of(entry.tags.begin(), entry.tags.end(), [&](const std::string& tag) {
                return lower(tag).find(needle) != std::string::npos;
            })) {
            results.push_back(entry.path);
        }
    }
    return results;
}

// ============================================================================
// ProjectManagerAgent Implementation
// ============================================================================

ProjectManagerAgent::ProjectManagerAgent(const AgentConfig& config)
    : TheOrgAgent(config, AgentRole::PROJECT_MANAGER) {}
ProjectManagerAgent::~ProjectManagerAgent() { stop(); }

void ProjectManagerAgent::initialize() {
    AgentLogger logger;
    logger.log("Initializing Project Manager Agent: " + config_.agentName);
}

void ProjectManagerAgent::start() {
    {
        std::lock_guard<std::mutex> lock(threadMutex_);
        if (running_) return;
        if (processingThread_.joinable()) processingThread_.join();
        running_ = true;
        paused_ = false;
        processingThread_ = std::thread(&ProjectManagerAgent::processLoop, this);
    }
}


void ProjectManagerAgent::stop() {
    {
        std::lock_guard<std::mutex> lock(threadMutex_);
        if (!running_ && !processingThread_.joinable()) return;
        running_ = false;
        lifecycleCv_.notify_all();
        if (processingThread_.joinable() && processingThread_.get_id() != std::this_thread::get_id())
            processingThread_.join();
    }
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
            auto messages = drainIncomingMessages();
            noteProcessedWork(messages.size());
            processCheckinResponses();
        }
        waitForWork();
    }
}

UUID ProjectManagerAgent::createProject(const std::string& name, const std::string& description,
                                        const std::vector<UUID>& teamMemberIds) {
    const auto now = std::chrono::system_clock::now();
    Project project;
    project.id = config_.agentId + "-project-" +
                 std::to_string(now.time_since_epoch().count());
    project.name = name;
    project.description = description;
    project.status = ProjectStatus::PLANNING;
    project.teamMemberIds = teamMemberIds;
    project.createdAt = now;
    project.updatedAt = now;
    {
        std::lock_guard<std::mutex> lock(projectMutex_);
        projects_[project.id] = project;
    }
    return project.id;
}

UUID ProjectManagerAgent::addTeamMember(const TeamMember& member) {
    TeamMember stored = member;
    if (stored.id.empty()) {
        stored.id = config_.agentId + "-member-" +
                    std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    }
    {
        std::lock_guard<std::mutex> lock(teamMutex_);
        teamMembers_[stored.id] = stored;
    }
    return stored.id;
}

void ProjectManagerAgent::recordDailyUpdate(const DailyUpdate& update) {
    DailyUpdate stored = update;
    const auto now = std::chrono::system_clock::now();
    if (stored.id.empty()) {
        stored.id = config_.agentId + "-update-" +
                    std::to_string(now.time_since_epoch().count());
    }
    if (stored.submittedAt == Timestamp{}) stored.submittedAt = now;
    std::lock_guard<std::mutex> lock(updateMutex_);
    dailyUpdates_.push_back(std::move(stored));
}

std::string ProjectManagerAgent::generateProjectStatusReport(const UUID& projectId) const {
    const auto project = getProject(projectId);
    if (!project) return "Project not found: " + projectId;

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
    const auto updates = getDailyUpdates(projectId);
    std::ostringstream report;
    report << "Project Status Report: " << project->name << "\n"
           << "Status: " << statusName(project->status) << "\n"
           << "Description: " << project->description << "\n"
           << "Team members: " << project->teamMemberIds.size() << "\n"
           << "Tasks: " << project->taskIds.size() << "\n"
           << "Daily updates: " << updates.size();
    return report.str();
}

std::string ProjectManagerAgent::generateWeeklyReport(const std::vector<UUID>& projectIds) const {
    std::vector<UUID> selected = projectIds;
    if (selected.empty()) {
        for (const auto& project : getActiveProjects()) selected.push_back(project.id);
    }
    std::ostringstream report;
    report << "Weekly Project Report";
    for (const auto& id : selected) report << "\n\n" << generateProjectStatusReport(id);
    return report.str();
}

void ProjectManagerAgent::sendCheckinReminder(const UUID& teamMemberId, const UUID& projectId) {
    if (teamMemberId.empty() || projectId.empty()) return;
    if (!getTeamMember(teamMemberId) || !getProject(projectId)) return;
    std::lock_guard<std::mutex> lock(checkinMutex_);
    checkinReminders_.push_back(teamMemberId + ":" + projectId);
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
    if (!agent || agent->getId().empty()) return;
    std::lock_guard<std::mutex> lock(agentMutex_);
    agents_[agent->getId()] = agent;
    roleToAgentMap_[agent->getRole()] = agent->getId();
}

void TheOrgManager::removeAgent(const UUID& agentId) {
    std::shared_ptr<TheOrgAgent> removed;
    {
        std::lock_guard<std::mutex> lock(agentMutex_);
        auto it = agents_.find(agentId); if (it == agents_.end()) return;
        removed=it->second; roleToAgentMap_.erase(removed->getRole()); agents_.erase(it);
    }
    removed->stop();
}

std::shared_ptr<TheOrgAgent> TheOrgManager::getAgent(const UUID& agentId) const {
    std::lock_guard<std::mutex> lock(agentMutex_);
    auto it = agents_.find(agentId);
    return (it != agents_.end()) ? it->second : nullptr;
}

void TheOrgManager::startAllAgents() {
    {
        std::unique_lock<std::mutex> operationLock(operationMutex_);
        operationCv_.wait(operationLock, [this] { return !lifecycleOperationInProgress_; });
        lifecycleOperationInProgress_ = true;
    }
    {
        std::lock_guard<std::mutex> lock(lifecycleMutex_);
        if (!running_) {
            running_ = true;
            coordinationThread_ = std::thread(&TheOrgManager::coordinationLoop, this);
        }
    }
    auto snapshot = getAllAgents();
    for (auto& agent : snapshot) agent->start();
    {
        std::lock_guard<std::mutex> operationLock(operationMutex_);
        lifecycleOperationInProgress_ = false;
    }
    operationCv_.notify_all();
}

void TheOrgManager::stopAllAgents() {
    {
        std::unique_lock<std::mutex> operationLock(operationMutex_);
        operationCv_.wait(operationLock, [this] { return !lifecycleOperationInProgress_; });
        lifecycleOperationInProgress_ = true;
    }
    std::thread coordinator;
    {
        std::lock_guard<std::mutex> lock(lifecycleMutex_);
        running_ = false;
        coordinator = std::move(coordinationThread_);
    }
    lifecycleCv_.notify_all();
    if (coordinator.joinable()) coordinator.join();
    auto snapshot = getAllAgents();
    for (auto& agent : snapshot) agent->stop();
    {
        std::lock_guard<std::mutex> operationLock(operationMutex_);
        lifecycleOperationInProgress_ = false;
    }
    operationCv_.notify_all();
}

std::shared_ptr<TheOrgAgent> TheOrgManager::getAgentByRole(AgentRole role) const {
    std::lock_guard<std::mutex> lock(agentMutex_);
    auto rit=roleToAgentMap_.find(role); if(rit==roleToAgentMap_.end()) return nullptr;
    auto it=agents_.find(rit->second); return it==agents_.end()?nullptr:it->second;
}

// Additional TheOrgManager methods
void TheOrgManager::initializeAllAgents(const std::vector<AgentConfig>& /* configs */) {
    auto snapshot=getAllAgents(); for(auto& agent:snapshot) agent->initialize();
}

void TheOrgManager::broadcastMessage(const std::string& message, const std::string& senderId,
                                     const std::vector<AgentRole>& targetRoles) {
    auto snapshot=getAllAgents();
    for (const auto& agent : snapshot) {
        if (!targetRoles.empty() &&
            std::find(targetRoles.begin(), targetRoles.end(), agent->getRole()) == targetRoles.end()) {
            continue;
        }
        agent->processMessage(message, senderId);
    }
}

TheOrgManager::SystemMetrics TheOrgManager::getSystemMetrics() const {
    SystemMetrics metrics;
    auto snapshot=getAllAgents();
    metrics.totalAgents = snapshot.size();
    for (const auto& agent : snapshot) {
        if (agent->isRunning()) metrics.activeAgents++;
        const auto pending=agent->getPendingTasks(); metrics.pendingTasks+=pending.size(); metrics.totalTasks+=pending.size();
    }
    metrics.systemLoad = 0.0;
    metrics.averageResponseTime = std::chrono::milliseconds(0);
    metrics.lastUpdated = std::chrono::system_clock::now();
    return metrics;
}

std::vector<std::shared_ptr<TheOrgAgent>> TheOrgManager::getAllAgents() const {
    std::lock_guard<std::mutex> lock(agentMutex_);
    std::vector<std::shared_ptr<TheOrgAgent>> result;
    for (const auto& [id, agent] : agents_) result.push_back(agent);
    std::sort(result.begin(),result.end(),[](const auto&a,const auto&b){return a->getId()<b->getId();});
    return result;
}

// Additional DeveloperRelationsAgent methods
std::string DeveloperRelationsAgent::retrieveKnowledge(const std::string& topic) const {
    auto lower = [](std::string value) {
        std::transform(value.begin(), value.end(), value.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return value;
    };
    const std::string needle = lower(topic);
    std::lock_guard<std::mutex> lock(knowledgeMutex_);
    auto exact = knowledgeBase_.find(topic);
    if (exact != knowledgeBase_.end()) return exact->second.content;
    for (const auto& [key, entry] : knowledgeBase_) {
        if (lower(key).find(needle) != std::string::npos ||
            lower(entry.content).find(needle) != std::string::npos ||
            std::any_of(entry.tags.begin(), entry.tags.end(), [&](const std::string& tag) {
                return lower(tag).find(needle) != std::string::npos;
            })) {
            return entry.content;
        }
    }
    return "Knowledge not found for topic: " + topic;
}

std::string DeveloperRelationsAgent::generateCodeExample(const std::string& topic,
                                                          const std::string& language) const {
    if (language == "cpp" && topic == "agent-creation") {
        return "AgentConfig config;\nconfig.agentName = \"MyAgent\";\n";
    }
    if (language == "cpp" && topic == "memory-management") {
        return "auto memory = agent.createMemory(\"content\", MemoryType::MESSAGE);\n"
               "agent.addMemory(memory);\n";
    }
    return "// Code example for " + topic + " in " + language + "\n";
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
    : TheOrgAgent(config, AgentRole::COMMUNITY_LIAISON) {}
CommunityLiaisonAgent::~CommunityLiaisonAgent() { stop(); }

void CommunityLiaisonAgent::initialize() {
    AgentLogger logger;
    logger.log("Initializing Community Liaison Agent: " + config_.agentName);
}

void CommunityLiaisonAgent::start() {
    {
        std::lock_guard<std::mutex> lock(threadMutex_);
        if (running_) return;
        if (processingThread_.joinable()) processingThread_.join();
        running_ = true;
        paused_ = false;
        processingThread_ = std::thread(&CommunityLiaisonAgent::processLoop, this);
    }
    AgentLogger logger;
    logger.log("Community Liaison Agent started: " + config_.agentName);
}


void CommunityLiaisonAgent::stop() {
    {
        std::lock_guard<std::mutex> lock(threadMutex_);
        if (!running_ && !processingThread_.joinable()) return;
        running_ = false;
        lifecycleCv_.notify_all();
        if (processingThread_.joinable() && processingThread_.get_id() != std::this_thread::get_id())
            processingThread_.join();
    }
    AgentLogger logger;
    logger.log("Community Liaison Agent stopped: " + config_.agentName);
}


void CommunityLiaisonAgent::pause() { paused_ = true; }
void CommunityLiaisonAgent::resume() { paused_ = false; }
bool CommunityLiaisonAgent::isRunning() const { return running_; }
void CommunityLiaisonAgent::processLoop() { while(running_) { if(!paused_) { auto messages=drainIncomingMessages(); noteProcessedWork(messages.size()); updateTopicTrends(); } waitForWork(); } }

void CommunityLiaisonAgent::addOrganization(const OrganizationConfig& org) {
    if(org.id.empty() || org.name.empty()) return;
    std::lock_guard<std::mutex> lock(orgMutex_); organizations_[org.id] = org;
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
                                            const std::string& summary, const std::string& channelId) {
    if(orgId.empty()||topic.empty()||summary.empty()||channelId.empty()) return;
    { std::lock_guard<std::mutex> lock(orgMutex_); if(!organizations_.count(orgId)) return; }
    DiscussionEntry entry{orgId,topic,summary,channelId,std::chrono::system_clock::now(),{},1.0};
    std::lock_guard<std::mutex> lock(discussionMutex_); discussionHistory_.push_back(std::move(entry));
}

std::vector<ParallelTopic> CommunityLiaisonAgent::identifyParallelTopics(std::chrono::hours timeWindow) const {
    const auto cutoff=std::chrono::system_clock::now()-timeWindow;
    std::unordered_map<std::string,ParallelTopic> grouped;
    { std::lock_guard<std::mutex> lock(discussionMutex_); for(const auto& d:discussionHistory_) if(d.timestamp>=cutoff){auto& p=grouped[d.topic];p.topic=d.topic;p.recentDiscussions.push_back(d.summary);if(std::find(p.organizationIds.begin(),p.organizationIds.end(),d.orgId)==p.organizationIds.end())p.organizationIds.push_back(d.orgId);}}
    std::vector<ParallelTopic> out; for(auto& [topic,p]:grouped) if(p.organizationIds.size()>1){p.relevanceScore=std::min(1.0,static_cast<double>(p.organizationIds.size())/5.0+static_cast<double>(p.recentDiscussions.size())/20.0);p.potentialSynergies="Shared discussion across "+std::to_string(p.organizationIds.size())+" organizations";out.push_back(std::move(p));}
    std::sort(out.begin(),out.end(),[](const auto&a,const auto&b){return a.relevanceScore!=b.relevanceScore?a.relevanceScore>b.relevanceScore:a.topic<b.topic;}); return out;
}

CrossOrgReport CommunityLiaisonAgent::generateDailyReport(const std::vector<UUID>& recipientOrgIds) const {
    CrossOrgReport report; report.id=config_.agentId+"-report-"+std::to_string(sequence_.fetch_add(1)); report.type=ReportType::DAILY; report.generatedAt=std::chrono::system_clock::now(); report.recipientOrgIds=recipientOrgIds; report.content.parallelTopics=identifyParallelTopics(std::chrono::hours(24)); report.content.overview="Daily cross-organization report with "+std::to_string(report.content.parallelTopics.size())+" parallel topics."; return report;
}

CrossOrgReport CommunityLiaisonAgent::generateWeeklyReport(const std::vector<UUID>& recipientOrgIds) const {
    CrossOrgReport report; report.id=config_.agentId+"-report-"+std::to_string(sequence_.fetch_add(1)); report.type=ReportType::WEEKLY; report.generatedAt=std::chrono::system_clock::now(); report.recipientOrgIds=recipientOrgIds; report.content.parallelTopics=identifyParallelTopics(std::chrono::hours(168)); report.content.overview="Weekly cross-organization report with "+std::to_string(report.content.parallelTopics.size())+" parallel topics."; return report;
}

// ============================================================================
// SocialMediaManagerAgent Implementation
// ============================================================================

SocialMediaManagerAgent::SocialMediaManagerAgent(const AgentConfig& config)
    : TheOrgAgent(config, AgentRole::SOCIAL_MEDIA_MANAGER) {}
SocialMediaManagerAgent::~SocialMediaManagerAgent() { stop(); }

void SocialMediaManagerAgent::initialize() {
    AgentLogger logger;
    logger.log("Initializing Social Media Manager Agent: " + config_.agentName);
}

void SocialMediaManagerAgent::start() {
    {
        std::lock_guard<std::mutex> lock(threadMutex_);
        if (running_) return;
        if (processingThread_.joinable()) processingThread_.join();
        running_ = true;
        paused_ = false;
        processingThread_ = std::thread(&SocialMediaManagerAgent::processLoop, this);
    }
    AgentLogger logger;
    logger.log("Social Media Manager Agent started: " + config_.agentName);
}


void SocialMediaManagerAgent::stop() {
    {
        std::lock_guard<std::mutex> lock(threadMutex_);
        if (!running_ && !processingThread_.joinable()) return;
        running_ = false;
        lifecycleCv_.notify_all();
        if (processingThread_.joinable() && processingThread_.get_id() != std::this_thread::get_id())
            processingThread_.join();
    }
    AgentLogger logger;
    logger.log("Social Media Manager Agent stopped: " + config_.agentName);
}


void SocialMediaManagerAgent::pause() { paused_ = true; }
void SocialMediaManagerAgent::resume() { paused_ = false; }
bool SocialMediaManagerAgent::isRunning() const { return running_; }
void SocialMediaManagerAgent::processLoop() { while(running_) { if(!paused_) { auto messages=drainIncomingMessages(); noteProcessedWork(messages.size()); publishScheduledContent(); monitorEngagement(); } waitForWork(); } }

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
    std::vector<PlatformType> targets;
    { std::lock_guard<std::mutex> lock(contentMutex_); auto it=content_.find(contentId); if(it==content_.end()) return; targets=it->second.targetPlatforms; }
    bool all=!targets.empty(); for(auto platform:targets) all=tryPublishContentToPlatform(contentId,platform)&&all;
    std::lock_guard<std::mutex> lock(contentMutex_); auto it=content_.find(contentId); if(it!=content_.end()){it->second.status=all?ContentStatus::PUBLISHED:ContentStatus::FAILED;it->second.updatedAt=std::chrono::system_clock::now();}
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

std::string SocialMediaManagerAgent::generateAnalyticsReport(std::chrono::hours timeWindow) const {
    std::ostringstream report;
    report << "Social Media Analytics Report\n" << analyzeContentPerformance(timeWindow);
    std::lock_guard<std::mutex> lock(metricsMutex_);
    for (const auto& [platform, metrics] : platformMetrics_) {
        report << "\n" << the_org_utils::platformTypeToString(platform)
               << ": posts=" << metrics.totalPosts
               << ", followers=" << metrics.followers
               << ", engagement=" << metrics.engagementRate;
    }
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

void ProjectManagerAgent::scheduleDailyCheckins(const UUID& projectId) {
    if (!getProject(projectId)) return;
    std::lock_guard<std::mutex> lock(checkinMutex_);
    checkinProjects_.insert(projectId);
}

std::vector<DailyUpdate> ProjectManagerAgent::getDailyUpdates(const UUID& projectId,
                                                               const std::string& date) const {
    std::lock_guard<std::mutex> lock(updateMutex_);
    std::vector<DailyUpdate> result;
    for (const auto& update : dailyUpdates_) {
        if (update.projectId == projectId && (date.empty() || update.date == date)) {
            result.push_back(update);
        }
    }
    return result;
}

std::vector<DailyUpdate> ProjectManagerAgent::getMemberUpdates(const UUID& teamMemberId,
                                                               std::chrono::hours timeWindow) const {
    const auto cutoff = std::chrono::system_clock::now() - timeWindow;
    std::lock_guard<std::mutex> lock(updateMutex_);
    std::vector<DailyUpdate> result;
    for (const auto& update : dailyUpdates_) {
        if (update.teamMemberId == teamMemberId && update.submittedAt >= cutoff) {
            result.push_back(update);
        }
    }
    return result;
}

std::string ProjectManagerAgent::generateTeamProductivityReport(
    const std::vector<UUID>& teamMemberIds, std::chrono::hours timeWindow) const {
    std::vector<UUID> ids = teamMemberIds;
    if (ids.empty()) {
        for (const auto& member : getAllTeamMembers()) ids.push_back(member.id);
    }
    std::sort(ids.begin(), ids.end());
    ids.erase(std::unique(ids.begin(), ids.end()), ids.end());

    std::ostringstream out;
    out << "Team Productivity Report (" << timeWindow.count() << "h)";
    for (const auto& id : ids) {
        const auto updates = getMemberUpdates(id, timeWindow);
        std::chrono::minutes worked{0};
        {
            std::lock_guard<std::mutex> lock(metricsMutex_);
            auto it = workHours_.find(id);
            if (it != workHours_.end()) {
                for (const auto& entry : it->second) worked += entry.second;
            }
        }
        std::size_t accomplishments = 0;
        std::size_t blockers = 0;
        for (const auto& update : updates) {
            accomplishments += update.accomplishments.size();
            blockers += update.blockers.size();
        }
        out << "\n" << id << ": updates=" << updates.size()
            << ", accomplishments=" << accomplishments
            << ", blockers=" << blockers
            << ", minutes=" << worked.count();
    }
    return out.str();
}

void ProjectManagerAgent::distributeReport(const std::string& report, const std::vector<std::string>& channelIds) {
    if (report.empty() || channelIds.empty()) return;
    std::vector<PlatformType> platforms;
    {
        std::lock_guard<std::mutex> lock(platformMutex_);
        for (const auto& [type, config] : platforms_) {
            (void)config;
            platforms.push_back(type);
        }
    }
    for (const auto platform : platforms) {
        for (const auto& channelId : channelIds) {
            if (!channelId.empty()) sendMessage(platform, channelId, report);
        }
    }
}

void ProjectManagerAgent::linkTaskToProject(const UUID& projectId, const UUID& taskId) {
    std::lock_guard<std::mutex> lock(projectMutex_);
    auto it = projects_.find(projectId);
    if (it != projects_.end()) {
        it->second.taskIds.push_back(taskId);
    }
}

void ProjectManagerAgent::reportBlocker(const UUID& projectId, const UUID& teamMemberId,
                                         const std::string& description) {
    if(projectId.empty()||teamMemberId.empty()||description.empty()||!getProject(projectId)||!getTeamMember(teamMemberId))return;
    Blocker b; b.id=config_.agentId+"-blocker-"+std::to_string(sequence_.fetch_add(1));b.projectId=projectId;b.reportedBy=teamMemberId;b.description=description;b.reportedAt=std::chrono::system_clock::now();b.isActive=true;
    std::lock_guard<std::mutex> lock(blockerMutex_);blockers_.push_back(std::move(b));
}

void ProjectManagerAgent::resolveBlocker(const UUID& blockerId, const std::string& resolution) {
    if (blockerId.empty() || resolution.empty()) return;
    std::lock_guard<std::mutex> lock(blockerMutex_);
    for (auto& blocker : blockers_) {
        if (blocker.id == blockerId && blocker.isActive) {
            blocker.isActive = false;
            blocker.resolution = resolution;
            blocker.resolvedAt = std::chrono::system_clock::now();
            return;
        }
    }
}

std::vector<std::string> ProjectManagerAgent::getActiveBlockers(const UUID& projectId) const {
    std::lock_guard<std::mutex> lock(blockerMutex_);std::vector<std::string> out;for(const auto&b:blockers_)if(b.projectId==projectId&&b.isActive)out.push_back(b.id+":"+b.description);return out;
}

std::vector<std::string> ProjectManagerAgent::getCheckinReminders() const { std::lock_guard<std::mutex> lock(checkinMutex_); return checkinReminders_; }

// ============================================================================
// Additional TheOrgManager Methods
// ============================================================================

void TheOrgManager::pauseAllAgents() { auto snapshot=getAllAgents(); for(auto& agent:snapshot) agent->pause(); }

void TheOrgManager::resumeAllAgents() { auto snapshot=getAllAgents(); for(auto& agent:snapshot) agent->resume(); }

void TheOrgManager::sendDirectMessage(const UUID& fromAgentId, const UUID& toAgentId, const std::string& message) {
    if (message.empty()) return;
    auto agent = getAgent(toAgentId);
    if (agent) {
        agent->processMessage(message, fromAgentId.empty() ? "manager" : fromAgentId);
    }
}

void TheOrgManager::loadConfiguration(const std::string& configPath) {
    try {
        if (configPath.empty()) throw std::runtime_error("empty configuration path");
        std::ifstream input(configPath);
        if (!input) throw std::runtime_error("cannot open configuration");
        nlohmann::json doc;
        input >> doc;
        if (!doc.is_object() || doc.value("schema", "") != "elizaos.the_org.configuration" ||
            doc.value("version", 0) != 1 || !doc.contains("globalSettings") ||
            !doc["globalSettings"].is_object() || !doc.contains("platforms") ||
            !doc["platforms"].is_array()) {
            throw std::runtime_error("invalid configuration schema");
        }
        std::unordered_map<std::string, std::string> settings;
        for (auto it = doc["globalSettings"].begin(); it != doc["globalSettings"].end(); ++it) {
            if (it.key().empty() || !it.value().is_string()) throw std::runtime_error("invalid setting");
            settings.emplace(it.key(), it.value().get<std::string>());
        }
        std::unordered_map<PlatformType, PlatformConfig> platforms;
        std::set<int> platformTypes;
        for (const auto& item : doc["platforms"]) {
            if (!item.is_object() || !item.contains("type") || !item["type"].is_number_integer() ||
                !item.contains("applicationId") || !item["applicationId"].is_string() ||
                !item.contains("apiToken") || !item["apiToken"].is_string() ||
                !item.contains("webhookUrl") || !item["webhookUrl"].is_string() ||
                !item.contains("additionalSettings") || !item["additionalSettings"].is_object()) {
                throw std::runtime_error("invalid platform");
            }
            const int raw = item["type"].get<int>();
            if (raw < 0 || raw > static_cast<int>(PlatformType::GITHUB) || !platformTypes.insert(raw).second)
                throw std::runtime_error("invalid or duplicate platform type");
            PlatformConfig config{};
            config.type = static_cast<PlatformType>(raw);
            config.applicationId = item["applicationId"].get<std::string>();
            config.apiToken = item["apiToken"].get<std::string>();
            config.webhookUrl = item["webhookUrl"].get<std::string>();
            for (auto setting = item["additionalSettings"].begin(); setting != item["additionalSettings"].end(); ++setting) {
                if (setting.key().empty() || !setting.value().is_string())
                    throw std::runtime_error("invalid platform setting");
                config.additionalSettings.emplace(setting.key(), setting.value().get<std::string>());
            }
            platforms.emplace(config.type, std::move(config));
        }
        {
            std::scoped_lock lock(settingsMutex_, platformMutex_);
            globalSettings_ = std::move(settings);
            globalPlatforms_ = std::move(platforms);
            lastConfigurationError_.clear();
        }
    } catch (const std::exception& error) {
        std::lock_guard<std::mutex> lock(settingsMutex_);
        lastConfigurationError_ = error.what();
    }
}

void TheOrgManager::saveConfiguration(const std::string& configPath) const {
    try {
        if (configPath.empty()) throw std::runtime_error("empty configuration path");
        nlohmann::json doc = {{"schema", "elizaos.the_org.configuration"}, {"version", 1}};
        {
            std::scoped_lock lock(settingsMutex_, platformMutex_);
            doc["globalSettings"] = globalSettings_;
            doc["platforms"] = nlohmann::json::array();
            std::vector<int> types;
            for (const auto& [type, config] : globalPlatforms_) types.push_back(static_cast<int>(type));
            std::sort(types.begin(), types.end());
            for (int raw : types) {
                const auto& config = globalPlatforms_.at(static_cast<PlatformType>(raw));
                doc["platforms"].push_back({{"type", raw}, {"applicationId", config.applicationId},
                    {"apiToken", config.apiToken}, {"webhookUrl", config.webhookUrl},
                    {"additionalSettings", config.additionalSettings}});
            }
        }
        const std::filesystem::path target(configPath);
        const std::filesystem::path temporary = target.string() + ".tmp";
        if (target.has_parent_path()) std::filesystem::create_directories(target.parent_path());
        {
            std::ofstream output(temporary, std::ios::trunc);
            if (!output) throw std::runtime_error("cannot open temporary configuration");
            output << doc.dump(2) << '\n';
            output.flush();
            if (!output) throw std::runtime_error("configuration write failed");
        }
        std::error_code error;
        std::filesystem::rename(temporary, target, error);
        if (error) {
            std::filesystem::remove(temporary);
            throw std::runtime_error("configuration commit failed: " + error.message());
        }
        std::lock_guard<std::mutex> lock(settingsMutex_);
        lastConfigurationError_.clear();
    } catch (const std::exception& error) {
        std::lock_guard<std::mutex> lock(settingsMutex_);
        lastConfigurationError_ = error.what();
    }
}

void TheOrgManager::updateGlobalSetting(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(settingsMutex_);
    globalSettings_[key] = value;
}

std::string TheOrgManager::getGlobalSetting(const std::string& key) const {
    std::lock_guard<std::mutex> lock(settingsMutex_);auto it=globalSettings_.find(key);return it!=globalSettings_.end()?it->second:"";
}
std::string TheOrgManager::getLastConfigurationError() const { std::lock_guard<std::mutex> lock(settingsMutex_); return lastConfigurationError_; }

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
    auto snapshot=getAllAgents();
    for (const auto& agent : snapshot) {
        const auto& id=agent->getId();
        if (!agent->isRunning()) {
            AgentLogger logger;
            logger.log("Agent " + id + " is not running", "", "Health", LogLevel::WARNING);
        }
    }
}

void TheOrgManager::enableEventLogging(const std::string& logPath) { std::lock_guard<std::mutex> lock(logMutex_);eventLoggingEnabled_=!logPath.empty();logPath_=logPath; }

void TheOrgManager::disableEventLogging() { std::lock_guard<std::mutex> lock(logMutex_);eventLoggingEnabled_=false; }

std::vector<std::string> TheOrgManager::getRecentEvents(std::chrono::hours timeWindow) const { std::lock_guard<std::mutex> lock(logMutex_);std::vector<std::string> out;auto cutoff=std::chrono::system_clock::now()-timeWindow;for(const auto&[time,event]:timedEventLog_)if(time>=cutoff)out.push_back(event);return out; }

void TheOrgManager::setLogLevel(const std::string& level) {
    logLevel_ = level;
}

// ============================================================================
// Additional DeveloperRelationsAgent Methods
// ============================================================================

std::string DeveloperRelationsAgent::provideAPIReference(const std::string& apiName) const {
    if (apiName.empty()) return "API reference unavailable: empty API name";
    std::lock_guard<std::mutex> lock(knowledgeMutex_);
    auto it = knowledgeBase_.find(apiName);
    if (it == knowledgeBase_.end()) return "API reference unavailable for " + apiName;
    return "API Reference: " + apiName + "\n" + it->second.content;
}

std::string DeveloperRelationsAgent::diagnoseIssue(
    const std::string& errorMessage, const std::string& context) const {
    if (errorMessage.empty()) return "No error supplied.";
    std::string lower = errorMessage + " " + context;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    std::ostringstream out;
    out << "Diagnosis: ";
    if (lower.find("permission") != std::string::npos) {
        out << "permission or ownership failure";
    } else if (lower.find("timeout") != std::string::npos) {
        out << "operation exceeded its deadline";
    } else if (lower.find("not found") != std::string::npos ||
               lower.find("missing") != std::string::npos) {
        out << "required resource is absent";
    } else if (lower.find("compile") != std::string::npos) {
        out << "compiler reported a source or dependency error";
    } else {
        out << "unclassified issue; inspect the exact error and configured dependencies";
    }
    if (!context.empty()) out << "; context: " << context;
    return out.str();
}

std::vector<std::string> DeveloperRelationsAgent::suggestSolutions(
    const std::string& problemDescription) const {
    if (problemDescription.empty()) return {};
    std::vector<std::string> solutions;
    std::string lower = problemDescription;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    if (lower.find("permission") != std::string::npos)
        solutions.push_back("Verify file ownership and least-privilege access");
    if (lower.find("timeout") != std::string::npos)
        solutions.push_back("Measure the slow operation and verify its deadline");
    if (lower.find("compile") != std::string::npos)
        solutions.push_back("Rebuild the narrow target and fix the first compiler diagnostic");
    {
        std::lock_guard<std::mutex> lock(knowledgeMutex_);
        for (const auto& [topic, entry] : knowledgeBase_) {
            if (lower.find(topic) != std::string::npos) solutions.push_back(entry.content);
        }
    }
    if (solutions.empty())
        solutions.push_back("Collect reproducible inputs, exact diagnostics, and relevant configuration");
    return solutions;
}

std::string DeveloperRelationsAgent::generateTutorial(
    const std::string& topic, const std::string& difficulty) const {
    if (topic.empty()) return {};
    std::ostringstream out;
    out << "# " << topic << " Tutorial (" << difficulty << ")\n\n"
        << "1. Define the expected outcome.\n"
        << "2. Create the smallest reproducible example.\n"
        << "3. Validate the result with deterministic tests.";
    const auto knowledge = retrieveKnowledge(topic);
    if (knowledge.rfind("Knowledge not found", 0) != 0)
        out << "\n\nConfigured reference:\n" << knowledge;
    return out.str();
}

std::string DeveloperRelationsAgent::reviewCode(
    const std::string& code, const std::string& language) const {
    if (code.empty()) return "Code review unavailable: empty input";
    const auto practices = identifyBestPractices(code, language);
    std::ostringstream out;
    out << "Code review (" << language << "): " << code.size() << " bytes";
    for (const auto& practice : practices) out << "\n- " << practice;
    return out.str();
}

std::vector<std::string> DeveloperRelationsAgent::getRelatedTopics(
    const std::string& topic) const {
    std::lock_guard<std::mutex> lock(knowledgeMutex_);
    std::vector<std::string> related;
    auto it = knowledgeBase_.find(topic);
    if (it != knowledgeBase_.end()) related = it->second.relatedTopics;
    for (const auto& [name, entry] : knowledgeBase_) {
        if (name != topic &&
            (std::find(entry.tags.begin(), entry.tags.end(), topic) != entry.tags.end() ||
             std::find(entry.relatedTopics.begin(), entry.relatedTopics.end(), topic) != entry.relatedTopics.end())) {
            related.push_back(name);
        }
    }
    std::sort(related.begin(), related.end());
    related.erase(std::unique(related.begin(), related.end()), related.end());
    return related;
}

std::string DeveloperRelationsAgent::generateOnboardingGuide(
    const std::string& project, const std::string& role) const {
    if (project.empty() || role.empty()) return {};
    std::ostringstream out;
    out << "# Onboarding Guide for " << project << "\n\n"
        << "Role: " << role << "\n\n"
        << "1. Obtain repository and environment access.\n"
        << "2. Build and run focused tests.\n"
        << "3. Read configured project knowledge.";
    const auto knowledge = retrieveKnowledge(project);
    if (knowledge.rfind("Knowledge not found", 0) != 0) out << "\n\n" << knowledge;
    return out.str();
}

void DeveloperRelationsAgent::updateKnowledgeBase(
    const std::string& topic, const std::string& updatedContent) {
    if (topic.empty() || updatedContent.empty()) return;
    std::lock_guard<std::mutex> lock(knowledgeMutex_);
    auto it = knowledgeBase_.find(topic);
    if (it == knowledgeBase_.end()) {
        KnowledgeEntry entry{topic, updatedContent, {}, {},
                             std::chrono::system_clock::now(), 1.0};
        knowledgeBase_[topic] = std::move(entry);
    } else {
        it->second.content = updatedContent;
        it->second.lastUpdated = std::chrono::system_clock::now();
        it->second.relevanceScore = 1.0;
    }
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
            if (current.size() > 1 && inHashtag) {
                hashtags.push_back(current);
            }
            current = "#";
            inHashtag = true;
        } else if (inHashtag) {
            if (std::isalnum(c) || c == '_') {
                current += c;
            } else {
                if (current.size() > 1) {
                    hashtags.push_back(current);
                }
                current.clear();
                inHashtag = false;
            }
        }
    }

    if (current.size() > 1 && inHashtag) {
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
