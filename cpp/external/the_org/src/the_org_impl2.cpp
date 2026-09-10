// the_org_impl2.cpp
// Implements all missing functions from the_org.hpp declarations
// Complements the_org_impl.cpp (which handles the first ~50% of implementations)

#include "elizaos/the_org.hpp"
#include "elizaos/agentlogger.hpp"
#include <algorithm>
#include <sstream>
#include <fstream>
#include <random>
#include <iomanip>
#include <numeric>
#include <set>
#include <ctime>
#include <regex>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace elizaos {

namespace {
    // Platform character limits
    constexpr size_t TWITTER_MAX_CONTENT_LENGTH   = 240;
    constexpr size_t TWITTER_MAX_TOTAL_LENGTH      = 280;
    constexpr size_t LINKEDIN_MAX_LENGTH           = 500;
    constexpr size_t DISCORD_MAX_LENGTH            = 1800;

    // Time/scheduling constants
    constexpr int HOURS_PER_DAY                    = 24;
    constexpr int DEFAULT_START_HOUR               = 9;
    constexpr int DEFAULT_END_HOUR                 = 17;
    constexpr int DEFAULT_POST_HOUR                = 9;

    // Relevance decay constants
    constexpr double RELEVANCE_DECAY_HALFLIFE_DAYS = 7.0;

    // Coordination loop constants
    constexpr int DEFAULT_BACKUP_INTERVAL_TICKS      = 60;
    constexpr int SCHEDULED_TASK_INTERVAL_TICKS      = 60;
} // anonymous namespace

// ============================================================================
// DeveloperRelationsAgent - Missing Methods
// ============================================================================

std::vector<std::string> DeveloperRelationsAgent::identifyBestPractices(
    const std::string& code, const std::string& language) const {
    std::vector<std::string> practices;

    // Scan for common best-practice signals in the code
    if (language == "cpp" || language == "c++") {
        if (code.find("std::shared_ptr") == std::string::npos &&
            code.find("std::unique_ptr") == std::string::npos &&
            code.find("new ") != std::string::npos) {
            practices.push_back("Use smart pointers (std::unique_ptr / std::shared_ptr) instead of raw new/delete");
        }
        if (code.find("std::mutex") == std::string::npos &&
            (code.find("std::thread") != std::string::npos || code.find("std::async") != std::string::npos)) {
            practices.push_back("Add mutex protection when using threads to avoid data races");
        }
        if (code.find("const ") == std::string::npos) {
            practices.push_back("Mark variables and methods const wherever possible to communicate intent");
        }
        if (code.find("noexcept") == std::string::npos && code.find("void ") != std::string::npos) {
            practices.push_back("Consider marking non-throwing functions noexcept for better performance");
        }
        if (code.find("auto ") == std::string::npos) {
            practices.push_back("Use 'auto' for type deduction where the type is obvious from the RHS");
        }
        practices.push_back("Prefer range-based for loops over index-based loops");
        practices.push_back("Use RAII to manage resource lifetimes");
        practices.push_back("Prefer algorithm functions (std::find, std::transform) over hand-rolled loops");
    } else if (language == "python") {
        if (code.find("def ") != std::string::npos && code.find("\"\"\"") == std::string::npos) {
            practices.push_back("Add docstrings to all functions and classes");
        }
        if (code.find("except:") != std::string::npos) {
            practices.push_back("Avoid bare except clauses; catch specific exceptions");
        }
        practices.push_back("Use type hints for function signatures");
        practices.push_back("Prefer list comprehensions over manual loops where readable");
        practices.push_back("Use context managers (with statement) for resource management");
    } else {
        practices.push_back("Ensure functions have a single responsibility");
        practices.push_back("Write self-documenting code with descriptive variable names");
        practices.push_back("Keep functions short (under ~30 lines) for maintainability");
        practices.push_back("Avoid magic numbers – use named constants");
        practices.push_back("Write unit tests for every public API");
    }

    return practices;
}

std::string DeveloperRelationsAgent::suggestRefactoring(
    const std::string& code, const std::string& language) const {
    std::ostringstream suggestions;
    suggestions << "# Refactoring Suggestions (" << language << ")\n\n";

    // Count rough complexity indicators
    size_t ifCount = 0, pos = 0;
    while ((pos = code.find("if ", pos)) != std::string::npos) { ++ifCount; ++pos; }

    size_t loopCount = 0; pos = 0;
    while ((pos = code.find("for ", pos)) != std::string::npos) { ++loopCount; ++pos; }
    pos = 0;
    while ((pos = code.find("while ", pos)) != std::string::npos) { ++loopCount; ++pos; }

    if (ifCount > 5) {
        suggestions << "- **High conditional complexity** (" << ifCount
                    << " if-statements detected): consider extracting logic into smaller functions "
                       "or using a strategy/lookup pattern.\n";
    }
    if (loopCount > 3) {
        suggestions << "- **Multiple loops** detected: evaluate whether any can be merged or "
                       "replaced with STL algorithms.\n";
    }
    if (code.length() > 500) {
        suggestions << "- **Long code block**: consider splitting into focused functions, each "
                       "doing one thing.\n";
    }

    // Duplicate substring detection (very heuristic)
    if (language == "cpp" || language == "c++") {
        if (code.find("cout") != std::string::npos) {
            suggestions << "- Replace std::cout debug output with a proper logging facility.\n";
        }
        if (code.find("using namespace std") != std::string::npos) {
            suggestions << "- Avoid 'using namespace std' in header files to prevent naming conflicts.\n";
        }
    }

    suggestions << "\n## Suggested Approach\n";
    suggestions << "1. Extract duplicated code into helper functions.\n";
    suggestions << "2. Apply the Single Responsibility Principle to classes.\n";
    suggestions << "3. Replace magic values with named constants.\n";
    suggestions << "4. Add comprehensive tests before refactoring to ensure behaviour is preserved.\n";

    return suggestions.str();
}

std::vector<std::string> DeveloperRelationsAgent::createLearningPath(
    const std::string& goal, const std::string& currentLevel) const {
    std::vector<std::string> path;

    if (currentLevel == "beginner") {
        path.push_back("1. Setup development environment and toolchain");
        path.push_back("2. Read the Getting Started guide");
        path.push_back("3. Complete the introductory tutorial");
        path.push_back("4. Run the example projects");
        path.push_back("5. Understand core data structures and patterns used in " + goal);
    } else if (currentLevel == "intermediate") {
        path.push_back("1. Deep-dive into the architecture documentation");
        path.push_back("2. Explore the source code for " + goal);
        path.push_back("3. Write unit tests for a component related to " + goal);
        path.push_back("4. Submit a small feature or bug-fix PR");
        path.push_back("5. Review advanced usage patterns");
    } else { // advanced
        path.push_back("1. Contribute a non-trivial feature for " + goal);
        path.push_back("2. Perform a code review and provide feedback");
        path.push_back("3. Write design documentation for a new subsystem");
        path.push_back("4. Mentor a junior contributor");
        path.push_back("5. Propose architectural improvements");
    }

    path.push_back("Resources: https://github.com/elizaos/eliza/docs");
    return path;
}

void DeveloperRelationsAgent::trackDeveloperProgress(
    const UUID& developerId, const std::string& milestone) {
    std::lock_guard<std::mutex> lock(progressMutex_);
    developerProgress_[developerId].push_back(milestone);

    AgentLogger logger;
    logger.log("Developer " + developerId + " reached milestone: " + milestone);
}

void DeveloperRelationsAgent::shareWeeklyTechUpdates(
    const std::vector<std::string>& channelIds) {
    std::vector<std::pair<std::string, std::string>> knowledge;
    {
        std::lock_guard<std::mutex> lock(knowledgeMutex_);
        for (const auto& [topic, entry] : knowledgeBase_) knowledge.emplace_back(topic, entry.content);
    }

    std::ostringstream update;
    update << "📚 **Weekly Tech Updates**\n\n";
    update << "Here are this week's highlights from the ElizaOS developer community:\n\n";

    size_t count = 0;
    for (const auto& [topic, content] : knowledge) {
        if (count >= 5) break;
        update << "• **" << topic << "**: " << content.substr(0, 120);
        if (content.length() > 120) update << "...";
        update << "\n";
        ++count;
    }

    if (count == 0) {
        update << "• Stay tuned – exciting updates are on the way!\n";
    }

    update << "\nHave questions? Drop them here and I'll help! 🚀";

    const std::string msg = update.str();
    for (const auto& channelId : channelIds) {
        sendMessage(PlatformType::DISCORD, channelId, msg);
    }

    AgentLogger logger;
    logger.log("Shared weekly tech updates to " + std::to_string(channelIds.size()) + " channels");
}

// Private helpers
void DeveloperRelationsAgent::processQuestion(
    const std::string& question, const std::string& userId, const std::string& channelId) {
    if (!isCodeRelated(question)) return;

    std::string answer;
    {
        std::lock_guard<std::mutex> lock(knowledgeMutex_);
        for (const auto& [topic, entry] : knowledgeBase_) {
            if (question.find(topic) != std::string::npos) {
                answer = entry.content;
                break;
            }
        }
    }

    if (answer.empty()) {
        answer = diagnoseIssue(question);
    }

    sendMessage(PlatformType::DISCORD, channelId, answer);
    trackDeveloperProgress(userId, "asked-question:" + question.substr(0, 40));
}

void DeveloperRelationsAgent::updateTechnicalKnowledge() {
    // Refresh knowledge entries – bump relevance scores and update timestamps
    std::lock_guard<std::mutex> lock(knowledgeMutex_);
    auto now = std::chrono::system_clock::now();
    for (auto& [topic, entry] : knowledgeBase_) {
        auto age = std::chrono::duration_cast<std::chrono::hours>(now - entry.lastUpdated).count();
        // Decay relevance over time (half-life ~7 days)
        entry.relevanceScore *= std::exp(-static_cast<double>(age) /
                                         (RELEVANCE_DECAY_HALFLIFE_DAYS * static_cast<double>(HOURS_PER_DAY)));
        if (entry.relevanceScore < 0.01) entry.relevanceScore = 0.01;
    }
}

std::string DeveloperRelationsAgent::formatCodeForPlatform(
    const std::string& code, PlatformType platform) const {
    switch (platform) {
        case PlatformType::DISCORD:
            return "```cpp\n" + code + "\n```";
        case PlatformType::SLACK:
            return "```\n" + code + "\n```";
        case PlatformType::GITHUB:
            return "```cpp\n" + code + "\n```";
        default:
            return code;
    }
}

bool DeveloperRelationsAgent::isCodeRelated(const std::string& message) const {
    static const std::vector<std::string> codeKeywords = {
        "error", "exception", "compile", "build", "debug", "function", "class",
        "template", "nullptr", "segfault", "crash", "api", "sdk", "cmake",
        "header", "include", "library", "plugin", "runtime", "memory", "thread"
    };
    std::string lower = message;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    for (const auto& kw : codeKeywords) {
        if (lower.find(kw) != std::string::npos) return true;
    }
    return false;
}

// ============================================================================
// CommunityLiaisonAgent - Missing Methods
// ============================================================================

void CommunityLiaisonAgent::updateOrganizationTopics(
    const UUID& orgId, const std::vector<std::string>& topics) {
    std::lock_guard<std::mutex> lock(orgMutex_);
    auto it = organizations_.find(orgId);
    if (it != organizations_.end()) {
        it->second.subscribedTopics = topics;
    }
}

std::vector<std::string> CommunityLiaisonAgent::findCollaborationOpportunities(
    const std::vector<UUID>& orgIds) const {
    std::lock_guard<std::mutex> lock(orgMutex_);
    std::vector<std::string> opportunities;

    // Collect topics from each requested org
    std::unordered_map<UUID, std::vector<std::string>> orgTopics;
    for (const auto& id : orgIds) {
        auto it = organizations_.find(id);
        if (it != organizations_.end()) {
            orgTopics[id] = it->second.subscribedTopics;
        }
    }

    // Find shared topics between pairs of orgs
    for (size_t i = 0; i < orgIds.size(); ++i) {
        for (size_t j = i + 1; j < orgIds.size(); ++j) {
            const auto& t1 = orgTopics[orgIds[i]];
            const auto& t2 = orgTopics[orgIds[j]];
            for (const auto& topic : t1) {
                if (std::find(t2.begin(), t2.end(), topic) != t2.end()) {
                    opportunities.push_back(
                        "Shared interest in '" + topic + "' between orgs " +
                        orgIds[i] + " and " + orgIds[j]);
                }
            }
        }
    }

    if (opportunities.empty()) {
        opportunities.push_back(
            "Consider cross-posting updates about shared open-source tooling");
        opportunities.push_back("Joint webinar or AMA about shared technology stacks");
    }

    return opportunities;
}

double CommunityLiaisonAgent::calculateTopicRelevance(
    const std::string& topic, const std::vector<UUID>& orgIds) const {
    std::lock_guard<std::mutex> lock(orgMutex_);

    size_t matchingOrgs = 0;
    for (const auto& id : orgIds) {
        auto it = organizations_.find(id);
        if (it == organizations_.end()) continue;
        const auto& topics = it->second.subscribedTopics;
        if (std::find(topics.begin(), topics.end(), topic) != topics.end()) {
            ++matchingOrgs;
        }
    }

    if (orgIds.empty()) return 0.0;
    return static_cast<double>(matchingOrgs) / static_cast<double>(orgIds.size());
}

CrossOrgReport CommunityLiaisonAgent::generateTopicSpecificReport(
    const std::string& topic, const std::vector<UUID>& recipientOrgIds) const {
    std::vector<DiscussionEntry> discussions;
    {
        std::lock_guard<std::mutex> lock(discussionMutex_);
        discussions = discussionHistory_;
    }
    CrossOrgReport report;
    report.id = config_.agentId + "-report-topic-" + std::to_string(sequence_.fetch_add(1));
    report.type = ReportType::TOPIC_SPECIFIC;
    report.generatedAt = std::chrono::system_clock::now();
    report.recipientOrgIds = recipientOrgIds;
    std::ostringstream overview;
    overview << "Topic Report: " << topic << "\n\n";
    for (const auto& entry : discussions) {
        if (entry.topic.find(topic) != std::string::npos || topic.find(entry.topic) != std::string::npos) {
            overview << "- [Org " << entry.orgId << "] " << entry.summary << "\n";
            report.content.knowledgeGaps.push_back(entry.summary);
        }
    }
    if (report.content.knowledgeGaps.empty()) overview << "No recent discussions found on this topic.\n";
    report.content.overview = overview.str();
    std::vector<UUID> collaboratorIds = recipientOrgIds;
    if (collaboratorIds.empty()) {
        const auto organizations = getMonitoredOrganizations();
        for (const auto& organization : organizations) collaboratorIds.push_back(organization.id);
    }
    report.content.collaborationOpportunities = findCollaborationOpportunities(collaboratorIds);
    return report;
}

void CommunityLiaisonAgent::distributeReport(const CrossOrgReport& report) {
    std::ostringstream msg;
    msg << "📊 **Cross-Org Report** (";
    switch (report.type) {
        case ReportType::DAILY:   msg << "Daily";   break;
        case ReportType::WEEKLY:  msg << "Weekly";  break;
        case ReportType::TOPIC_SPECIFIC: msg << "Topic";  break;
        case ReportType::MONTHLY: msg << "Monthly"; break;
        case ReportType::QUARTERLY: msg << "Quarterly"; break;
    }
    msg << ")\n\n" << report.content.overview;

    if (!report.content.collaborationOpportunities.empty()) {
        msg << "\n**Collaboration Opportunities:**\n";
        for (const auto& opp : report.content.collaborationOpportunities) {
            msg << "• " << opp << "\n";
        }
    }

    std::vector<std::pair<PlatformType, UUID>> deliveries;
    {
        std::lock_guard<std::mutex> lock(orgMutex_);
        for (const auto& orgId : report.recipientOrgIds) {
            auto it = organizations_.find(orgId);
            if (it != organizations_.end() && !it->second.platforms.empty())
                deliveries.emplace_back(it->second.platforms.front().type, orgId);
        }
    }
    const std::string payload = msg.str();
    for (const auto& [platform, channel] : deliveries) sendMessage(platform, channel, payload);

    AgentLogger logger;
    logger.log("Distributed " + std::to_string(report.recipientOrgIds.size()) +
               "-recipient report ID " + report.id);
}

void CommunityLiaisonAgent::shareKnowledge(
    const UUID& sourceOrgId, const UUID& targetOrgId,
    const std::string& topic, const std::string& content) {
    std::string msg = "🔗 **Knowledge Share** from org " + sourceOrgId + "\n\n"
                      "**Topic:** " + topic + "\n\n" + content;

    std::optional<PlatformType> platform;
    {
        std::lock_guard<std::mutex> lock(orgMutex_);
        auto it = organizations_.find(targetOrgId);
        if (it != organizations_.end() && !it->second.platforms.empty())
            platform = it->second.platforms.front().type;
    }
    if (platform) sendMessage(*platform, targetOrgId, msg);

    AgentLogger logger;
    logger.log("Shared knowledge on '" + topic + "' from " + sourceOrgId + " to " + targetOrgId);
}

void CommunityLiaisonAgent::facilitateIntroduction(
    const UUID& org1Id, const UUID& org2Id, const std::string& sharedInterest) {
    std::string org1Name = org1Id;
    std::string org2Name = org2Id;
    std::vector<std::pair<PlatformType, UUID>> deliveries;
    {
        std::lock_guard<std::mutex> lock(orgMutex_);
        auto it1 = organizations_.find(org1Id);
        auto it2 = organizations_.find(org2Id);
        if (it1 != organizations_.end()) {
            org1Name = it1->second.name;
            if (!it1->second.platforms.empty()) deliveries.emplace_back(it1->second.platforms.front().type, org1Id);
        }
        if (it2 != organizations_.end()) {
            org2Name = it2->second.name;
            if (!it2->second.platforms.empty()) deliveries.emplace_back(it2->second.platforms.front().type, org2Id);
        }
    }
    const std::string msg = "👋 **Introduction**\n\n"
        "I'd like to connect **" + org1Name + "** and **" + org2Name + "**.\n"
        "Both communities share a strong interest in **" + sharedInterest + "**.\n\n"
        "I believe there are great collaboration opportunities here! Feel free to reach out.";
    for (const auto& [platform, channel] : deliveries) sendMessage(platform, channel, msg);

    AgentLogger logger;
    logger.log("Facilitated introduction between " + org1Name + " and " + org2Name +
               " on topic: " + sharedInterest);
}

std::vector<std::string> CommunityLiaisonAgent::suggestKnowledgeExchange(
    const UUID& orgId) const {
    std::lock_guard<std::mutex> lock(orgMutex_);
    std::vector<std::string> suggestions;

    auto it = organizations_.find(orgId);
    if (it == organizations_.end()) return suggestions;

    const auto& orgTopics = it->second.subscribedTopics;

    // Find other orgs with complementary topics
    for (const auto& [otherId, otherOrg] : organizations_) {
        if (otherId == orgId) continue;
        for (const auto& topic : orgTopics) {
            if (std::find(otherOrg.subscribedTopics.begin(),
                          otherOrg.subscribedTopics.end(), topic) != otherOrg.subscribedTopics.end()) {
                suggestions.push_back("Exchange insights on '" + topic + "' with " + otherOrg.name);
            }
        }
    }

    if (suggestions.empty()) {
        suggestions.push_back("Explore adjacent communities around your core topics");
        suggestions.push_back("Post a summary of recent learnings to the cross-org channel");
    }

    return suggestions;
}

std::vector<std::string> CommunityLiaisonAgent::getTrendingTopics(
    std::chrono::hours timeWindow) const {
    std::lock_guard<std::mutex> lock(discussionMutex_);

    auto cutoff = std::chrono::system_clock::now() - timeWindow;
    std::unordered_map<std::string, size_t> counts;

    for (const auto& entry : discussionHistory_) {
        if (entry.timestamp >= cutoff) {
            counts[entry.topic]++;
        }
    }

    // Sort by frequency
    std::vector<std::pair<std::string, size_t>> sorted(counts.begin(), counts.end());
    std::sort(sorted.begin(), sorted.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    std::vector<std::string> result;
    for (const auto& [topic, cnt] : sorted) {
        result.push_back(topic);
        if (result.size() >= 10) break;
    }

    return result;
}

std::unordered_map<std::string, double> CommunityLiaisonAgent::analyzeTopicSentiment(
    const std::string& topic) const {
    std::lock_guard<std::mutex> lock(discussionMutex_);

    // Simple keyword-based sentiment scoring per org
    static const std::vector<std::string> positiveWords = {
        "great", "awesome", "excellent", "good", "love", "amazing", "progress", "success"};
    static const std::vector<std::string> negativeWords = {
        "bad", "issue", "problem", "fail", "error", "bug", "broken", "concern"};

    std::unordered_map<std::string, std::pair<double, size_t>> orgScores; // orgId -> (sumScore, count)

    for (const auto& entry : discussionHistory_) {
        if (entry.topic.find(topic) == std::string::npos &&
            topic.find(entry.topic) == std::string::npos) continue;

        std::string lower = entry.summary;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

        double score = 0.0;
        for (const auto& w : positiveWords)
            if (lower.find(w) != std::string::npos) score += 1.0;
        for (const auto& w : negativeWords)
            if (lower.find(w) != std::string::npos) score -= 1.0;

        orgScores[entry.orgId].first += score;
        orgScores[entry.orgId].second++;
    }

    std::unordered_map<std::string, double> result;
    for (const auto& [orgId, scoreData] : orgScores) {
        result[orgId] = (scoreData.second > 0)
                            ? scoreData.first / static_cast<double>(scoreData.second)
                            : 0.0;
    }

    return result;
}

std::vector<std::string> CommunityLiaisonAgent::predictEmergingTopics() const {
    std::lock_guard<std::mutex> lock(trendMutex_);

    // Find topics with rapidly increasing trend scores
    std::vector<std::pair<std::string, double>> scored;
    for (const auto& trend : topicTrends_) {
        if (trend.trendScore > 0.5) {
            scored.emplace_back(trend.topic, trend.trendScore);
        }
    }

    std::sort(scored.begin(), scored.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    std::vector<std::string> result;
    for (const auto& [topic, score] : scored) {
        result.push_back(topic);
        if (result.size() >= 5) break;
    }

    if (result.empty()) {
        result.push_back("AI agent frameworks");
        result.push_back("Decentralized identity");
        result.push_back("Multi-agent orchestration");
    }

    return result;
}

void CommunityLiaisonAgent::mapOrganizationRelationship(
    const UUID& org1Id, const UUID& org2Id, const std::string& relationshipType) {
    std::lock_guard<std::mutex> lock(orgMutex_);

    // Store as a custom setting on both orgs (using relationship key)
    auto key = "rel_" + org2Id;
    auto key2 = "rel_" + org1Id;

    auto it1 = organizations_.find(org1Id);
    if (it1 != organizations_.end()) {
        it1->second.customSettings[key] = relationshipType;
    }
    auto it2 = organizations_.find(org2Id);
    if (it2 != organizations_.end()) {
        it2->second.customSettings[key2] = relationshipType;
    }

    AgentLogger logger;
    logger.log("Mapped relationship '" + relationshipType + "' between " + org1Id + " and " + org2Id);
}

std::vector<UUID> CommunityLiaisonAgent::getRelatedOrganizations(const UUID& orgId) const {
    std::lock_guard<std::mutex> lock(orgMutex_);

    auto it = organizations_.find(orgId);
    if (it == organizations_.end()) return {};

    std::vector<UUID> related;
    const auto& settings = it->second.customSettings;
    for (const auto& [key, val] : settings) {
        if (key.substr(0, 4) == "rel_") {
            related.push_back(key.substr(4));
        }
    }

    return related;
}

double CommunityLiaisonAgent::calculateOrganizationSimilarity(
    const UUID& org1Id, const UUID& org2Id) const {
    std::lock_guard<std::mutex> lock(orgMutex_);

    auto it1 = organizations_.find(org1Id);
    auto it2 = organizations_.find(org2Id);

    if (it1 == organizations_.end() || it2 == organizations_.end()) return 0.0;

    const auto& t1 = it1->second.subscribedTopics;
    const auto& t2 = it2->second.subscribedTopics;

    if (t1.empty() && t2.empty()) return 1.0;
    if (t1.empty() || t2.empty()) return 0.0;

    size_t matches = 0;
    for (const auto& topic : t1) {
        if (std::find(t2.begin(), t2.end(), topic) != t2.end()) ++matches;
    }

    // Jaccard similarity
    size_t unionSize = t1.size() + t2.size() - matches;
    return static_cast<double>(matches) / static_cast<double>(unionSize);
}

// Private helpers
void CommunityLiaisonAgent::monitorOrganizations() {
    const auto organizations = getMonitoredOrganizations();
    AgentLogger logger;
    for (const auto& organization : organizations) {
        logger.log("Monitoring org: " + organization.name + " (" +
                   std::to_string(organization.subscribedTopics.size()) + " topics)");
    }
}

void CommunityLiaisonAgent::analyzeCrossOrgPatterns() {
    auto allOrgs = getMonitoredOrganizations();
    std::vector<UUID> orgIds;
    orgIds.reserve(allOrgs.size());
    for (const auto& org : allOrgs) orgIds.push_back(org.id);

    auto parallel = identifyParallelTopics(std::chrono::hours(168));
    AgentLogger logger;
    logger.log("Cross-org analysis: " + std::to_string(parallel.size()) + " parallel topic clusters found");
}

void CommunityLiaisonAgent::generatePeriodicReports() {
    auto daily = generateDailyReport({});
    distributeReport(daily);
}

std::string CommunityLiaisonAgent::formatReportForPlatform(
    const CrossOrgReport& report, PlatformType platform) const {
    std::ostringstream formatted;

    if (platform == PlatformType::DISCORD) {
        formatted << "**Cross-Org Report**\n```\n" << report.content.overview << "\n```";
    } else if (platform == PlatformType::SLACK) {
        formatted << "*Cross-Org Report*\n```" << report.content.overview << "```";
    } else {
        formatted << "Cross-Org Report\n" << report.content.overview;
    }

    return formatted.str();
}

void CommunityLiaisonAgent::updateTopicTrends() {
    std::vector<DiscussionEntry> discussions;
    {
        std::lock_guard<std::mutex> lock(discussionMutex_);
        discussions = discussionHistory_;
    }
    const auto now = std::chrono::system_clock::now();
    std::unordered_map<std::string, TopicTrend> trendMap;
    for (const auto& entry : discussions) {
        auto& trend = trendMap[entry.topic];
        trend.topic = entry.topic;
        trend.trendScore += entry.engagementLevel;
        if (std::find(trend.activeOrganizations.begin(), trend.activeOrganizations.end(), entry.orgId) ==
            trend.activeOrganizations.end()) trend.activeOrganizations.push_back(entry.orgId);
        if (trend.firstSeen == Timestamp{} || entry.timestamp < trend.firstSeen) trend.firstSeen = entry.timestamp;
        if (entry.timestamp > trend.lastSeen) trend.lastSeen = entry.timestamp;
    }
    std::vector<TopicTrend> trends;
    for (auto& [topic, trend] : trendMap) {
        trend.duration = std::chrono::duration_cast<std::chrono::hours>(now - trend.firstSeen);
        trends.push_back(std::move(trend));
    }
    std::lock_guard<std::mutex> lock(trendMutex_);
    topicTrends_ = std::move(trends);
}

// ============================================================================
// ProjectManagerAgent - Missing Methods
// ============================================================================

void ProjectManagerAgent::assignTaskToMember(
    const UUID& taskId, const UUID& teamMemberId) {
    if (taskId.empty() || !getTeamMember(teamMemberId)) return;
    {
        std::lock_guard<std::mutex> lock(taskMutex_);
        if (!tasks_.count(taskId)) return;
    }
    std::lock_guard<std::mutex> lock(teamMutex_);
    auto& assignments = memberTasks_[teamMemberId];
    if (std::find(assignments.begin(), assignments.end(), taskId) == assignments.end()) assignments.push_back(taskId);
}

std::vector<UUID> ProjectManagerAgent::getProjectTasks(const UUID& projectId) const {
    std::lock_guard<std::mutex> lock(projectMutex_);
    auto it = projects_.find(projectId);
    if (it != projects_.end()) return it->second.taskIds;
    return {};
}

std::vector<UUID> ProjectManagerAgent::getMemberTasks(const UUID& teamMemberId) const {
    std::lock_guard<std::mutex> lock(teamMutex_);
    auto it = memberTasks_.find(teamMemberId);
    return it == memberTasks_.end() ? std::vector<UUID>{} : it->second;
}

bool ProjectManagerAgent::isTeamMemberAvailable(
    const UUID& teamMemberId, Timestamp time) const {
    return isInWorkingHours(teamMemberId, time);
}

std::vector<Timestamp> ProjectManagerAgent::findTeamMeetingTime(
    const std::vector<UUID>& teamMemberIds,
    std::chrono::minutes /* duration */) const {
    // Simple implementation: return next few hours if all members are in working hours
    std::vector<Timestamp> slots;
    auto candidate = std::chrono::system_clock::now();

    for (int i = 0; i < 5; ++i) {
        candidate += std::chrono::hours(1);
        bool allAvailable = true;
        for (const auto& id : teamMemberIds) {
            if (!isTeamMemberAvailable(id, candidate)) {
                allAvailable = false;
                break;
            }
        }
        if (allAvailable) slots.push_back(candidate);
    }

    return slots;
}

void ProjectManagerAgent::trackWorkHours(
    const UUID& teamMemberId, const UUID& projectId,
    std::chrono::minutes duration) {
    if (duration.count() <= 0 || !getTeamMember(teamMemberId) || !getProject(projectId)) return;
    std::lock_guard<std::mutex> lock(metricsMutex_);
    workHours_[teamMemberId].emplace_back(projectId, duration);

    // Update project metrics
    auto& pm = projectMetrics_[projectId];
    pm.projectId = projectId;
    pm.totalTimeSpent += std::chrono::duration_cast<std::chrono::hours>(duration);
    pm.lastUpdate = std::chrono::system_clock::now();
}

void ProjectManagerAgent::assessProjectRisk(const UUID& projectId) {
    const auto projectValue = getProject(projectId);
    if (!projectValue) return;
    const auto project = *projectValue;
    std::ostringstream risk;
    risk << "Risk Assessment for project: " << project.name << "\n";

    // Check deadline proximity
    if (project.deadline.has_value()) {
        auto now = std::chrono::system_clock::now();
        auto remaining = project.deadline.value() - now;
        auto days = std::chrono::duration_cast<std::chrono::hours>(remaining).count() / 24;
        if (days < 7) {
            risk << "  ⚠️  HIGH RISK: Deadline in " << days << " days\n";
        } else if (days < 30) {
            risk << "  ⚡ MEDIUM RISK: Deadline in " << days << " days\n";
        }
    }

    // Check active blockers
    auto activeBlockers = getActiveBlockers(projectId);
    if (!activeBlockers.empty()) {
        risk << "  🚧 " << activeBlockers.size() << " active blocker(s)\n";
    }

    // Check team size vs tasks
    if (project.taskIds.size() > project.teamMemberIds.size() * 5) {
        risk << "  ⚠️  HIGH RISK: Task-to-member ratio is high ("
             << project.taskIds.size() << " tasks / "
             << project.teamMemberIds.size() << " members)\n";
    }

    AgentLogger logger;
    logger.log(risk.str(), "", "Risk", LogLevel::WARNING);
}

// Private helpers
void ProjectManagerAgent::sendDailyCheckins() {
    std::vector<std::pair<UUID, UUID>> reminders;
    const auto projects = getActiveProjects();
    for (const auto& project : projects) {
        for (const auto& memberId : project.teamMemberIds) reminders.emplace_back(memberId, project.id);
    }
    for (const auto& [memberId, projectId] : reminders) sendCheckinReminder(memberId, projectId);
}

void ProjectManagerAgent::processCheckinResponses() {
    std::vector<DailyUpdate> pending;
    {
        std::lock_guard<std::mutex> lock(updateMutex_);
        for (const auto& update : dailyUpdates_) {
            if (processedCheckinUpdateIds_.insert(update.id).second) pending.push_back(update);
        }
    }
    for (const auto& update : pending) {
        for (const auto& blocker : update.blockers) reportBlocker(update.projectId, update.teamMemberId, blocker);
    }
}

void ProjectManagerAgent::generateAutomaticReports() {
    auto projects = getActiveProjects();
    for (const auto& project : projects) {
        auto report = generateProjectStatusReport(project.id);
        AgentLogger logger;
        logger.log("Auto-report for project " + project.name + ":\n" + report);
    }
}

void ProjectManagerAgent::monitorProjectHealth() {
    auto projects = getActiveProjects();
    for (const auto& project : projects) {
        assessProjectRisk(project.id);
    }
}

std::string ProjectManagerAgent::formatMemberAvailability(
    const TeamMemberAvailability& availability) const {
    std::ostringstream oss;
    oss << "Work Days: ";
    for (size_t i = 0; i < availability.workDays.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << availability.workDays[i];
    }
    oss << "\nHours: " << availability.workHours.start << " - " << availability.workHours.end;
    oss << " (" << availability.timeZone << ")";
    oss << "\n" << availability.hoursPerWeek << " hrs/week";
    switch (availability.employmentStatus) {
        case TeamMemberAvailability::EmploymentStatus::FULL_TIME:  oss << " [Full-time]";  break;
        case TeamMemberAvailability::EmploymentStatus::PART_TIME:  oss << " [Part-time]";  break;
        case TeamMemberAvailability::EmploymentStatus::FREELANCE:  oss << " [Freelance]";  break;
        default: oss << " [N/A]"; break;
    }
    return oss.str();
}

bool ProjectManagerAgent::isInWorkingHours(
    const UUID& teamMemberId, Timestamp time) const {
    TeamMemberAvailability avail;
    {
        std::lock_guard<std::mutex> lock(teamMutex_);
        auto it = teamMembers_.find(teamMemberId);
        if (it == teamMembers_.end()) return false;
        avail = it->second.availability;
    }
    if (avail.workDays.empty()) return true;

    // Check day of week
    auto time_t = std::chrono::system_clock::to_time_t(time);
    std::tm local{};
#if defined(_WIN32)
    localtime_s(&local, &time_t);
#else
    localtime_r(&time_t, &local);
#endif
    static const std::vector<std::string> days = {
        "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    const std::string& dayName = days[local.tm_wday];

    if (std::find(avail.workDays.begin(), avail.workDays.end(), dayName) == avail.workDays.end()) {
        return false;
    }

    // Check hour range (simplified – just check hour number)
    int startHour = DEFAULT_START_HOUR, endHour = DEFAULT_END_HOUR;
    if (!avail.workHours.start.empty()) {
        try { startHour = std::stoi(avail.workHours.start.substr(0, 2)); } catch (...) {}
    }
    if (!avail.workHours.end.empty()) {
        try { endHour = std::stoi(avail.workHours.end.substr(0, 2)); } catch (...) {}
    }

    return (local.tm_hour >= startHour && local.tm_hour < endHour);
}

// ============================================================================
// SocialMediaManagerAgent - Missing Methods
// ============================================================================

void SocialMediaManagerAgent::updateContent(
    const UUID& contentId, const SocialMediaContent& updatedContent) {
    std::lock_guard<std::mutex> lock(contentMutex_);
    auto it = content_.find(contentId);
    if (it != content_.end()) {
        it->second = updatedContent;
        it->second.updatedAt = std::chrono::system_clock::now();
    }
}

void SocialMediaManagerAgent::deleteContent(const UUID& contentId) {
    std::lock_guard<std::mutex> lock(contentMutex_);
    content_.erase(contentId);
}

std::optional<SocialMediaContent> SocialMediaManagerAgent::getContent(
    const UUID& contentId) const {
    std::lock_guard<std::mutex> lock(contentMutex_);
    auto it = content_.find(contentId);
    if (it != content_.end()) return it->second;
    return std::nullopt;
}

std::vector<SocialMediaContent> SocialMediaManagerAgent::getContentByStatus(
    ContentStatus status) const {
    std::lock_guard<std::mutex> lock(contentMutex_);
    std::vector<SocialMediaContent> result;
    for (const auto& [id, c] : content_) {
        if (c.status == status) result.push_back(c);
    }
    return result;
}

bool SocialMediaManagerAgent::tryPublishContentToPlatform(
    const UUID& contentId, PlatformType platform) {
    SocialMediaContent snapshot;
    {
        std::lock_guard<std::mutex> lock(contentMutex_);
        auto it = content_.find(contentId);
        if (it == content_.end() ||
            std::find(it->second.targetPlatforms.begin(), it->second.targetPlatforms.end(), platform) ==
                it->second.targetPlatforms.end()) return false;
        snapshot = it->second;
    }
    const bool accepted = sendMessage(platform, "general", formatContentForPlatform(snapshot, platform));
    {
        std::lock_guard<std::mutex> lock(contentMutex_);
        auto it = content_.find(contentId);
        if (it == content_.end()) return false;
        it->second.platformSpecificData["delivery." + std::to_string(static_cast<int>(platform))] =
            accepted ? "accepted" : "rejected";
        bool allAccepted = !it->second.targetPlatforms.empty();
        for (const auto target : it->second.targetPlatforms) {
            auto evidence = it->second.platformSpecificData.find(
                "delivery." + std::to_string(static_cast<int>(target)));
            allAccepted = allAccepted && evidence != it->second.platformSpecificData.end() &&
                          evidence->second == "accepted";
        }
        it->second.status = allAccepted ? ContentStatus::PUBLISHED : ContentStatus::FAILED;
        it->second.updatedAt = std::chrono::system_clock::now();
    }
    return accepted;
}

bool SocialMediaManagerAgent::publishContentToPlatform(
    const UUID& contentId, PlatformType platform) {
    return tryPublishContentToPlatform(contentId, platform);
}

std::vector<UUID> SocialMediaManagerAgent::getScheduledContent(
    std::chrono::hours timeWindow) const {
    std::lock_guard<std::mutex> lock(contentMutex_);
    auto now = std::chrono::system_clock::now();
    auto cutoff = now + timeWindow;

    std::vector<UUID> result;
    for (const auto& [id, c] : content_) {
        if (c.status == ContentStatus::SCHEDULED && c.scheduledTime.has_value()) {
            if (c.scheduledTime.value() <= cutoff) {
                result.push_back(id);
            }
        }
    }

    return result;
}

std::string SocialMediaManagerAgent::optimizeContentForPlatform(
    const std::string& content, PlatformType platform) const {
    std::string optimized = content;

    switch (platform) {
        case PlatformType::TWITTER:
            // Twitter: max content chars, reserve space for hashtag
            if (optimized.length() > TWITTER_MAX_CONTENT_LENGTH) {
                optimized = optimized.substr(0, TWITTER_MAX_CONTENT_LENGTH - 3) + "...";
            }
            optimized += " #ElizaOS";
            break;
        case PlatformType::LINKEDIN:
            // LinkedIn: more professional, can be longer
            if (optimized.length() < 100) {
                optimized += "\n\nShare your thoughts in the comments below! #AI #Technology";
            }
            break;
        case PlatformType::FACEBOOK:
            if (optimized.length() > LINKEDIN_MAX_LENGTH) {
                optimized = optimized.substr(0, LINKEDIN_MAX_LENGTH - 3) + "...";
            }
            break;
        case PlatformType::DISCORD:
            // Discord supports markdown
            if (optimized.length() > DISCORD_MAX_LENGTH) {
                optimized = optimized.substr(0, DISCORD_MAX_LENGTH - 3) + "...";
            }
            break;
        default:
            break;
    }

    return optimized;
}

std::string SocialMediaManagerAgent::generateCaption(
    const std::string& imageDescription, PlatformType platform) const {
    std::string caption;
    std::string platformName = the_org_utils::platformTypeToString(platform);

    if (platform == PlatformType::TWITTER) {
        caption = imageDescription + " ✨\n\n#ElizaOS #AI #Agents #OpenSource";
    } else if (platform == PlatformType::LINKEDIN) {
        caption = "Excited to share: " + imageDescription +
                  "\n\nThis represents our continued commitment to innovation. "
                  "What do you think? Let us know in the comments!\n\n"
                  "#Innovation #AI #ElizaOS";
    } else {
        caption = imageDescription + " | Built with ElizaOS 🚀";
    }

    return caption;
}

void SocialMediaManagerAgent::createContentCalendar(
    const std::vector<std::string>& topics, std::chrono::hours planningWindow) {
    if (topics.empty() || planningWindow.count() <= 0) return;
    auto now = std::chrono::system_clock::now();
    size_t postsPerTopic = planningWindow.count() / static_cast<size_t>(HOURS_PER_DAY); // 1 post per day per topic

    std::vector<PlatformType> allPlatforms = {
        PlatformType::TWITTER, PlatformType::LINKEDIN, PlatformType::DISCORD};

    size_t slotOffset = 0;
    for (const auto& topic : topics) {
        for (size_t i = 0; i < std::max(size_t(1), postsPerTopic / topics.size()); ++i) {
            auto publishTime = now + std::chrono::hours(slotOffset * HOURS_PER_DAY + DEFAULT_POST_HOUR); // morning each day
            std::string generatedText = generateContent(topic, ContentType::TEXT_POST,
                                                        allPlatforms[slotOffset % allPlatforms.size()]);
            UUID id = createContent(ContentType::TEXT_POST, topic, generatedText, allPlatforms);
            scheduleContent(id, publishTime);
            ++slotOffset;
        }
    }

    AgentLogger logger;
    logger.log("Created content calendar with " + std::to_string(slotOffset) + " scheduled posts");
}

std::vector<std::string> SocialMediaManagerAgent::suggestContentTopics(
    PlatformType platform) const {
    std::vector<std::string> topics;

    switch (platform) {
        case PlatformType::TWITTER:
            topics = {"ElizaOS agent updates", "AI tips & tricks", "Open source spotlight",
                      "Community highlights", "Tech trivia"};
            break;
        case PlatformType::LINKEDIN:
            topics = {"Technical deep dives", "Case studies", "Team spotlights",
                      "Industry trends", "Product announcements"};
            break;
        case PlatformType::DISCORD:
            topics = {"Weekly dev updates", "Community polls", "Tutorial previews",
                      "Bug fix announcements", "Feature requests discussion"};
            break;
        default:
            topics = {"Product updates", "Community news", "Technical guides",
                      "Partner highlights", "Events"};
    }

    return topics;
}

void SocialMediaManagerAgent::setPostingSchedule(
    PlatformType platform, const std::vector<std::string>& postingTimes) {
    std::lock_guard<std::mutex> lock(scheduleMutex_);
    postingSchedules_[platform] = postingTimes;
}

std::string SocialMediaManagerAgent::analyzeContentPerformance(
    std::chrono::hours timeWindow) const {
    const auto cutoff = std::chrono::system_clock::now() - timeWindow;
    size_t published = 0, scheduled = 0, draft = 0, failed = 0;
    {
        std::lock_guard<std::mutex> lock(contentMutex_);
        for (const auto& [id, content] : content_) {
            if (content.updatedAt < cutoff) continue;
            switch (content.status) {
                case ContentStatus::PUBLISHED: ++published; break;
                case ContentStatus::SCHEDULED: ++scheduled; break;
                case ContentStatus::DRAFT: ++draft; break;
                case ContentStatus::FAILED: ++failed; break;
            }
        }
    }

    std::ostringstream report;
    report << "# Content Performance Report\n\n";
    report << "Time window: " << timeWindow.count() << " hours\n\n";
    report << "| Status    | Count |\n";
    report << "|-----------|-------|\n";
    report << "| Published | " << published << " |\n";
    report << "| Scheduled | " << scheduled << " |\n";
    report << "| Draft     | " << draft     << " |\n";
    report << "| Failed    | " << failed    << " |\n";

    {
        std::lock_guard<std::mutex> mLock(metricsMutex_);
        if (!platformMetrics_.empty()) {
            report << "\n## Platform Metrics\n\n";
            for (const auto& [platform, metrics] : platformMetrics_) {
                report << "- " << the_org_utils::platformTypeToString(platform)
                       << ": " << metrics.followers << " followers, "
                       << std::fixed << std::setprecision(1)
                       << (metrics.engagementRate * 100.0) << "% engagement\n";
            }
        }
    }

    return report.str();
}

bool SocialMediaManagerAgent::monitorMentions(PlatformType /* platform */) {
    return false;
}

bool SocialMediaManagerAgent::respondToComment(
    const std::string& commentId, const std::string& response, PlatformType platform) {
    return !commentId.empty() && sendMessage(platform, commentId, response);
}

bool SocialMediaManagerAgent::likePost(
    const std::string& /* postId */, PlatformType /* platform */) {
    return false;
}

bool SocialMediaManagerAgent::sharePost(
    const std::string& postId, const std::string& comment, PlatformType platform) {
    return !postId.empty() && sendMessage(platform, "feed", comment + " [shared post: " + postId + "]");
}

std::vector<std::string> SocialMediaManagerAgent::getRecentMentions(
    PlatformType platform, std::chrono::hours /* timeWindow */) const {
    // In production, fetch from platform API
    AgentLogger logger;
    logger.log("Fetching recent mentions from " + the_org_utils::platformTypeToString(platform));
    return {};
}

std::vector<std::string> SocialMediaManagerAgent::getBestPerformingContent(
    PlatformType /* platform */, size_t count) const {
    std::lock_guard<std::mutex> lock(contentMutex_);
    std::vector<std::string> result;

    for (const auto& [id, c] : content_) {
        if (c.status == ContentStatus::PUBLISHED) {
            result.push_back(id);
        }
        if (result.size() >= count) break;
    }

    return result;
}

std::vector<std::string> SocialMediaManagerAgent::getOptimalPostingTimes(
    PlatformType platform) const {
    std::lock_guard<std::mutex> lock(scheduleMutex_);
    auto it = postingSchedules_.find(platform);
    if (it != postingSchedules_.end()) return it->second;

    // Platform-specific defaults based on general best practices
    switch (platform) {
        case PlatformType::TWITTER:
            return {"08:00", "12:00", "17:00", "20:00"};
        case PlatformType::LINKEDIN:
            return {"08:00", "10:00", "12:00", "17:00"};
        case PlatformType::FACEBOOK:
            return {"09:00", "13:00", "15:00"};
        case PlatformType::DISCORD:
            return {"10:00", "14:00", "18:00"};
        default:
            return {"09:00", "14:00", "18:00"};
    }
}

UUID SocialMediaManagerAgent::createCampaign(
    const std::string& name, const std::string& description,
    const std::vector<PlatformType>& platforms,
    Timestamp startDate, Timestamp endDate) {
    std::lock_guard<std::mutex> lock(campaignMutex_);

    Campaign campaign;
    campaign.id = config_.agentId + "-campaign-" + std::to_string(campaigns_.size());
    campaign.name = name;
    campaign.description = description;
    campaign.platforms = platforms;
    campaign.startDate = startDate;
    campaign.endDate = endDate;
    campaign.isActive = false;

    campaigns_[campaign.id] = campaign;

    AgentLogger logger;
    logger.log("Created campaign: " + name);

    return campaign.id;
}

void SocialMediaManagerAgent::addContentToCampaign(
    const UUID& campaignId, const UUID& contentId) {
    std::lock_guard<std::mutex> lock(campaignMutex_);
    auto it = campaigns_.find(campaignId);
    if (it != campaigns_.end()) {
        it->second.contentIds.push_back(contentId);
    }
}

bool SocialMediaManagerAgent::launchCampaign(const UUID& campaignId) {
    Campaign campaign;
    {
        std::lock_guard<std::mutex> lock(campaignMutex_);
        auto it = campaigns_.find(campaignId);
        if (it == campaigns_.end() || it->second.contentIds.empty() || it->second.platforms.empty()) return false;
        campaign = it->second;
    }
    bool allAccepted = true;
    for (const auto& contentId : campaign.contentIds) {
        for (const auto platform : campaign.platforms)
            allAccepted = tryPublishContentToPlatform(contentId, platform) && allAccepted;
    }
    {
        std::lock_guard<std::mutex> lock(campaignMutex_);
        auto it = campaigns_.find(campaignId);
        if (it != campaigns_.end()) {
            it->second.isActive = allAccepted;
            it->second.metrics["launchStatus"] = allAccepted ? "accepted" : "rejected";
        }
    }
    return allAccepted;
}

std::string SocialMediaManagerAgent::analyzeCampaignPerformance(
    const UUID& campaignId) const {
    Campaign campaign;
    {
        std::lock_guard<std::mutex> lock(campaignMutex_);
        auto it = campaigns_.find(campaignId);
        if (it == campaigns_.end()) return "Campaign not found.";
        campaign = it->second;
    }
    std::ostringstream report;
    report << "# Campaign Performance: " << campaign.name << "\n\n";
    report << "Status: " << (campaign.isActive ? "Active" : "Inactive") << "\n";
    report << "Content pieces: " << campaign.contentIds.size() << "\n";
    report << "Platforms: ";
    for (size_t i = 0; i < campaign.platforms.size(); ++i) {
        if (i > 0) report << ", ";
        report << the_org_utils::platformTypeToString(campaign.platforms[i]);
    }
    report << "\n\n";

    // Aggregate metrics from published content
    size_t publishedCount = 0;
    {
        std::lock_guard<std::mutex> cLock(contentMutex_);
        for (const auto& cId : campaign.contentIds) {
            auto cit = content_.find(cId);
            if (cit != content_.end() && cit->second.status == ContentStatus::PUBLISHED) {
                ++publishedCount;
            }
        }
    }

    report << "Published: " << publishedCount << "/" << campaign.contentIds.size() << " pieces\n";

    for (const auto& [key, val] : campaign.metrics) {
        report << key << ": " << val << "\n";
    }

    return report.str();
}

// Private helpers
void SocialMediaManagerAgent::publishScheduledContent() {
    auto scheduled = getScheduledContent(std::chrono::hours(1));
    const auto now = std::chrono::system_clock::now();
    for (const auto& id : scheduled) {
        bool due = false;
        {
            std::lock_guard<std::mutex> lock(contentMutex_);
            auto it = content_.find(id);
            due = it != content_.end() && it->second.scheduledTime && *it->second.scheduledTime <= now;
        }
        if (due) publishContent(id);
    }
}

void SocialMediaManagerAgent::monitorEngagement() {
    std::unordered_map<PlatformType, size_t> acceptedPosts;
    {
        std::lock_guard<std::mutex> lock(platformMutex_);
        for (const auto& [platform, channels] : channelMessages_) {
            for (const auto& [channel, messages] : channels) {
                (void)channel;
                acceptedPosts[platform] += messages.size();
            }
        }
    }
    const auto observedAt = std::chrono::system_clock::now();
    std::lock_guard<std::mutex> lock(metricsMutex_);
    for (const auto& [platform, count] : acceptedPosts) {
        auto& metrics = platformMetrics_[platform];
        metrics.platform = platform;
        metrics.totalPosts = count;
        metrics.lastUpdated = observedAt;
    }
}

void SocialMediaManagerAgent::updateMetrics() {
    std::unordered_map<PlatformType, size_t> postCounts;
    {
        std::lock_guard<std::mutex> lock(contentMutex_);
        for (const auto& [id, content] : content_) {
            if (content.status == ContentStatus::PUBLISHED) {
                for (const auto platform : content.targetPlatforms) ++postCounts[platform];
            }
        }
    }
    std::lock_guard<std::mutex> lock(metricsMutex_);
    for (const auto& [platform, count] : postCounts) {
        auto& m = platformMetrics_[platform];
        m.platform = platform;
        m.totalPosts = count;
        m.lastUpdated = std::chrono::system_clock::now();
    }
}

void SocialMediaManagerAgent::generateContentSuggestions() {
    static const std::vector<PlatformType> platforms = {
        PlatformType::TWITTER, PlatformType::LINKEDIN, PlatformType::DISCORD};

    for (const auto& platform : platforms) {
        auto topics = suggestContentTopics(platform);
        AgentLogger logger;
        logger.log("Content suggestions for " + the_org_utils::platformTypeToString(platform) +
                   ": " + (topics.empty() ? "none" : topics.front()));
    }
}

std::string SocialMediaManagerAgent::formatContentForPlatform(
    const SocialMediaContent& content, PlatformType platform) const {
    std::string formatted = content.content;

    // Append hashtags
    if (!content.hashtags.empty()) {
        formatted += "\n";
        for (const auto& tag : content.hashtags) {
            formatted += "#" + tag + " ";
        }
    }

    // Apply platform-specific optimizations
    formatted = optimizeContentForPlatform(formatted, platform);

    return formatted;
}

bool SocialMediaManagerAgent::isOptimalPostingTime(PlatformType platform) const {
    auto times = getOptimalPostingTimes(platform);
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::tm local{};
#if defined(_WIN32)
    localtime_s(&local, &time_t);
#else
    localtime_r(&time_t, &local);
#endif
    char buf[6];
    std::snprintf(buf, sizeof(buf), "%02d:%02d", local.tm_hour, local.tm_min);
    std::string currentTime(buf);

    for (const auto& t : times) {
        if (currentTime.substr(0, 2) == t.substr(0, 2)) return true;
    }

    return false;
}

// ============================================================================
// TheOrgManager - Missing Methods
// ============================================================================

void TheOrgManager::subscribeToEvents(
    const UUID& agentId, const std::vector<std::string>& eventTypes) {
    if (agentId.empty()) return;
    std::vector<std::string> normalized;
    for (const auto& eventType : eventTypes) {
        if (!eventType.empty() &&
            std::find(normalized.begin(), normalized.end(), eventType) == normalized.end()) {
            normalized.push_back(eventType);
        }
    }
    std::lock_guard<std::mutex> lock(eventMutex_);
    auto it = std::find_if(eventSubscriptions_.begin(), eventSubscriptions_.end(),
                           [&](const EventSubscription& subscription) {
                               return subscription.agentId == agentId;
                           });
    if (normalized.empty()) {
        if (it != eventSubscriptions_.end()) eventSubscriptions_.erase(it);
    } else if (it == eventSubscriptions_.end()) {
        eventSubscriptions_.push_back({agentId, std::move(normalized)});
    } else {
        it->eventTypes = std::move(normalized);
    }
}

void TheOrgManager::publishEvent(
    const std::string& eventType, const std::string& data, const UUID& sourceAgentId) {
    if (eventType.empty()) return;
    std::vector<UUID> recipients;
    {
        std::lock_guard<std::mutex> lock(eventMutex_);
        for (const auto& subscription : eventSubscriptions_) {
            if (std::find(subscription.eventTypes.begin(), subscription.eventTypes.end(), eventType) !=
                subscription.eventTypes.end()) {
                recipients.push_back(subscription.agentId);
            }
        }
    }
    const std::string payload = eventType + ":" + data;
    const std::string logEntry = "[" + eventType + "] from " + sourceAgentId + ": " + data;
    {
        std::lock_guard<std::mutex> lock(logMutex_);
        if (eventLoggingEnabled_) {
            eventLog_.push_back(logEntry);
            timedEventLog_.emplace_back(std::chrono::system_clock::now(), logEntry);
            constexpr std::size_t capacity = 1000;
            if (eventLog_.size() > capacity) eventLog_.erase(eventLog_.begin(), eventLog_.begin() + (eventLog_.size() - capacity));
            if (timedEventLog_.size() > capacity) timedEventLog_.erase(timedEventLog_.begin(), timedEventLog_.begin() + (timedEventLog_.size() - capacity));
        }
    }
    std::vector<std::shared_ptr<TheOrgAgent>> agents;
    agents.reserve(recipients.size());
    for (const auto& recipient : recipients) {
        auto agent = getAgent(recipient);
        if (agent) agents.push_back(std::move(agent));
    }
    for (const auto& agent : agents) agent->processMessage(payload, sourceAgentId);
}

void TheOrgManager::addGlobalPlatform(const PlatformConfig& platform) {
    std::lock_guard<std::mutex> lock(platformMutex_);
    globalPlatforms_[platform.type] = platform;
}

void TheOrgManager::removeGlobalPlatform(PlatformType type) {
    std::lock_guard<std::mutex> lock(platformMutex_);
    globalPlatforms_.erase(type);
}

void TheOrgManager::propagatePlatformToAgents(
    PlatformType type, const std::vector<AgentRole>& targetRoles) {
    PlatformConfig platform;
    {
        std::lock_guard<std::mutex> lock(platformMutex_);
        auto it = globalPlatforms_.find(type);
        if (it == globalPlatforms_.end()) return;
        platform = it->second;
    }
    auto agents = getAllAgents();
    for (const auto& agent : agents) {
        if (targetRoles.empty() ||
            std::find(targetRoles.begin(), targetRoles.end(), agent->getRole()) != targetRoles.end()) {
            agent->addPlatform(platform);
        }
    }
}

UUID TheOrgManager::createCrossAgentWorkflow(
    const std::string& name, const std::vector<AgentRole>& involvedRoles) {
    if (name.empty() || involvedRoles.empty()) return {};
    Workflow workflow;
    workflow.id = "workflow-" + std::to_string(workflowSequence_.fetch_add(1) + 1);
    workflow.name = name;
    workflow.involvedRoles = involvedRoles;
    workflow.isActive = false;
    workflow.createdAt = std::chrono::system_clock::now();

    std::vector<std::shared_ptr<TheOrgAgent>> agents;
    agents.reserve(involvedRoles.size());
    for (const auto role : involvedRoles) {
        auto agent = getAgentByRole(role);
        if (agent) agents.push_back(std::move(agent));
    }
    for (const auto& agent : agents) {
        const auto taskId = agent->createTask(name, "Workflow: " + name);
        if (!taskId.empty()) workflow.taskIds.push_back(taskId);
    }
    {
        std::lock_guard<std::mutex> lock(workflowMutex_);
        workflows_[workflow.id] = workflow;
    }
    return workflow.id;
}

void TheOrgManager::executeWorkflow(
    const UUID& workflowId,
    const std::unordered_map<std::string, std::string>& parameters) {
    std::string name;
    std::vector<AgentRole> roles;
    {
        std::lock_guard<std::mutex> lock(workflowMutex_);
        auto it = workflows_.find(workflowId);
        if (it == workflows_.end()) return;
        it->second.isActive = true;
        it->second.lastExecuted = std::chrono::system_clock::now();
        it->second.parameters = parameters;
        name = it->second.name;
        roles = it->second.involvedRoles;
    }
    std::string payload = "workflow_start:" + workflowId + ":" + name;
    std::vector<std::pair<std::string, std::string>> ordered(parameters.begin(), parameters.end());
    std::sort(ordered.begin(), ordered.end());
    for (const auto& [key, value] : ordered) payload += ";" + key + "=" + value;
    std::vector<std::shared_ptr<TheOrgAgent>> agents;
    for (const auto role : roles) {
        auto agent = getAgentByRole(role);
        if (agent) agents.push_back(std::move(agent));
    }
    for (const auto& agent : agents) agent->processMessage(payload, "manager");
}

void TheOrgManager::monitorWorkflows() {
    std::vector<std::pair<std::string, Timestamp>> active;
    {
        std::lock_guard<std::mutex> lock(workflowMutex_);
        for (const auto& [id, workflow] : workflows_) {
            if (workflow.isActive) active.emplace_back(workflow.name, workflow.lastExecuted);
        }
    }
    AgentLogger logger;
    for (const auto& [name, lastExecuted] : active) {
        logger.log("Active workflow: " + name + " (last executed: " +
                   the_org_utils::formatTimestamp(lastExecuted) + ")");
    }
}

void TheOrgManager::saveSystemState(const std::string& backupPath) const {
    try {
        if (backupPath.empty()) throw std::runtime_error("empty state path");
        nlohmann::json document = {
            {"schema", "elizaos.the_org.system_state"},
            {"version", 1},
            {"globalSettings", nlohmann::json::object()},
            {"platforms", nlohmann::json::array()},
            {"workflows", nlohmann::json::array()}
        };
        {
            std::scoped_lock lock(settingsMutex_, platformMutex_, workflowMutex_);
            document["globalSettings"] = globalSettings_;
            std::vector<int> types;
            for (const auto& [type, config] : globalPlatforms_) types.push_back(static_cast<int>(type));
            std::sort(types.begin(), types.end());
            for (const int raw : types) {
                const auto& config = globalPlatforms_.at(static_cast<PlatformType>(raw));
                document["platforms"].push_back({{"type", raw}, {"applicationId", config.applicationId},
                    {"apiToken", config.apiToken}, {"webhookUrl", config.webhookUrl},
                    {"additionalSettings", config.additionalSettings}});
            }
            std::vector<UUID> ids;
            for (const auto& [id, workflow] : workflows_) ids.push_back(id);
            std::sort(ids.begin(), ids.end());
            for (const auto& id : ids) {
                const auto& workflow = workflows_.at(id);
                std::vector<int> roles;
                for (const auto role : workflow.involvedRoles) roles.push_back(static_cast<int>(role));
                const auto toMillis = [](Timestamp value) {
                    return std::chrono::duration_cast<std::chrono::milliseconds>(value.time_since_epoch()).count();
                };
                document["workflows"].push_back({{"id", workflow.id}, {"name", workflow.name},
                    {"involvedRoles", roles}, {"taskIds", workflow.taskIds},
                    {"parameters", workflow.parameters}, {"isActive", workflow.isActive},
                    {"createdAtMs", toMillis(workflow.createdAt)},
                    {"lastExecutedMs", toMillis(workflow.lastExecuted)}});
            }
        }
        const std::filesystem::path target(backupPath);
        const std::filesystem::path temporary = target.string() + ".tmp";
        if (target.has_parent_path()) std::filesystem::create_directories(target.parent_path());
        {
            std::ofstream output(temporary, std::ios::trunc);
            if (!output) throw std::runtime_error("cannot open temporary state file");
            output << document.dump(2) << '\n';
            output.flush();
            if (!output) throw std::runtime_error("state write failed");
        }
        std::error_code error;
        std::filesystem::rename(temporary, target, error);
        if (error) {
            std::filesystem::remove(temporary);
            throw std::runtime_error("state commit failed: " + error.message());
        }
        std::lock_guard<std::mutex> lock(settingsMutex_);
        lastConfigurationError_.clear();
    } catch (const std::exception& error) {
        std::lock_guard<std::mutex> lock(settingsMutex_);
        lastConfigurationError_ = error.what();
    }
}

void TheOrgManager::loadSystemState(const std::string& backupPath) {
    try {
        if (backupPath.empty()) throw std::runtime_error("empty state path");
        std::ifstream input(backupPath);
        if (!input) throw std::runtime_error("cannot open state file");
        nlohmann::json document;
        input >> document;
        if (!document.is_object() || document.value("schema", "") != "elizaos.the_org.system_state" ||
            document.value("version", 0) != 1 || !document.contains("globalSettings") ||
            !document["globalSettings"].is_object() || !document.contains("platforms") ||
            !document["platforms"].is_array() || !document.contains("workflows") ||
            !document["workflows"].is_array()) {
            throw std::runtime_error("invalid state schema");
        }
        std::unordered_map<std::string, std::string> settings;
        for (auto it = document["globalSettings"].begin(); it != document["globalSettings"].end(); ++it) {
            if (it.key().empty() || !it.value().is_string()) throw std::runtime_error("invalid state setting");
            settings.emplace(it.key(), it.value().get<std::string>());
        }
        std::unordered_map<PlatformType, PlatformConfig> platforms;
        std::set<int> seenPlatforms;
        for (const auto& item : document["platforms"]) {
            if (!item.is_object() || !item.contains("type") || !item["type"].is_number_integer() ||
                !item.contains("applicationId") || !item["applicationId"].is_string() ||
                !item.contains("apiToken") || !item["apiToken"].is_string() ||
                !item.contains("webhookUrl") || !item["webhookUrl"].is_string() ||
                !item.contains("additionalSettings") || !item["additionalSettings"].is_object())
                throw std::runtime_error("invalid state platform");
            const int raw = item["type"].get<int>();
            if (raw < 0 || raw > static_cast<int>(PlatformType::GITHUB) || !seenPlatforms.insert(raw).second)
                throw std::runtime_error("invalid or duplicate state platform type");
            PlatformConfig config{};
            config.type = static_cast<PlatformType>(raw);
            config.applicationId = item["applicationId"].get<std::string>();
            config.apiToken = item["apiToken"].get<std::string>();
            config.webhookUrl = item["webhookUrl"].get<std::string>();
            for (auto setting = item["additionalSettings"].begin(); setting != item["additionalSettings"].end(); ++setting) {
                if (setting.key().empty() || !setting.value().is_string())
                    throw std::runtime_error("invalid state platform setting");
                config.additionalSettings.emplace(setting.key(), setting.value().get<std::string>());
            }
            platforms.emplace(config.type, std::move(config));
        }
        std::unordered_map<UUID, Workflow> workflows;
        std::uint64_t maxSequence = 0;
        for (const auto& item : document["workflows"]) {
            if (!item.is_object() || !item.contains("id") || !item["id"].is_string() ||
                !item.contains("name") || !item["name"].is_string() ||
                !item.contains("involvedRoles") || !item["involvedRoles"].is_array() ||
                !item.contains("taskIds") || !item["taskIds"].is_array() ||
                !item.contains("parameters") || !item["parameters"].is_object() ||
                !item.contains("isActive") || !item["isActive"].is_boolean() ||
                !item.contains("createdAtMs") || !item["createdAtMs"].is_number_integer() ||
                !item.contains("lastExecutedMs") || !item["lastExecutedMs"].is_number_integer())
                throw std::runtime_error("invalid workflow");
            Workflow workflow;
            workflow.id = item["id"].get<std::string>();
            workflow.name = item["name"].get<std::string>();
            if (workflow.id.empty() || workflow.name.empty() || workflows.count(workflow.id))
                throw std::runtime_error("invalid or duplicate workflow id");
            for (const auto& roleValue : item["involvedRoles"]) {
                if (!roleValue.is_number_integer()) throw std::runtime_error("invalid workflow role");
                const int raw = roleValue.get<int>();
                if (raw < 0 || raw > static_cast<int>(AgentRole::SOCIAL_MEDIA_MANAGER))
                    throw std::runtime_error("invalid workflow role");
                workflow.involvedRoles.push_back(static_cast<AgentRole>(raw));
            }
            for (const auto& taskId : item["taskIds"]) {
                if (!taskId.is_string()) throw std::runtime_error("invalid workflow task");
                workflow.taskIds.push_back(taskId.get<std::string>());
            }
            for (auto parameter = item["parameters"].begin(); parameter != item["parameters"].end(); ++parameter) {
                if (parameter.key().empty() || !parameter.value().is_string())
                    throw std::runtime_error("invalid workflow parameter");
                workflow.parameters.emplace(parameter.key(), parameter.value().get<std::string>());
            }
            workflow.isActive = item["isActive"].get<bool>();
            workflow.createdAt = Timestamp(std::chrono::milliseconds(item["createdAtMs"].get<std::int64_t>()));
            workflow.lastExecuted = Timestamp(std::chrono::milliseconds(item["lastExecutedMs"].get<std::int64_t>()));
            const std::string prefix = "workflow-";
            if (workflow.id.rfind(prefix, 0) == 0) {
                try { maxSequence = std::max(maxSequence, static_cast<std::uint64_t>(std::stoull(workflow.id.substr(prefix.size())))); }
                catch (...) {}
            }
            workflows.emplace(workflow.id, std::move(workflow));
        }
        {
            std::scoped_lock lock(settingsMutex_, platformMutex_, workflowMutex_);
            globalSettings_ = std::move(settings);
            globalPlatforms_ = std::move(platforms);
            workflows_ = std::move(workflows);
            lastConfigurationError_.clear();
        }
        workflowSequence_.store(maxSequence);
    } catch (const std::exception& error) {
        std::lock_guard<std::mutex> lock(settingsMutex_);
        lastConfigurationError_ = error.what();
    }
}

void TheOrgManager::scheduleAutoBackup(std::chrono::minutes interval) {
    if (interval.count() <= 0) return;
    updateGlobalSetting("autoBackupInterval", std::to_string(interval.count()));
    updateGlobalSetting("autoBackupPath", "theorg_backup.json");
}

void TheOrgManager::coordinationLoop() {
    int backupTickCount = 0;
    int backupIntervalTicks = DEFAULT_BACKUP_INTERVAL_TICKS;
    while (running_) {
        const std::string backupInterval = getGlobalSetting("autoBackupInterval");
        if (!backupInterval.empty()) {
            try { backupIntervalTicks = std::max(1, std::stoi(backupInterval)); }
            catch (...) { backupIntervalTicks = DEFAULT_BACKUP_INTERVAL_TICKS; }
        }
        processInterAgentMessages();
        monitorAgentHealth();
        executeScheduledTasks();
        updateSystemMetrics();
        if (++backupTickCount >= backupIntervalTicks) {
            const std::string path = getGlobalSetting("autoBackupPath");
            if (!path.empty()) saveSystemState(path);
            backupTickCount = 0;
        }
        std::unique_lock<std::mutex> lock(lifecycleMutex_);
        lifecycleCv_.wait_for(lock, std::chrono::seconds(1), [this] { return !running_.load(); });
    }
}

void TheOrgManager::processInterAgentMessages() {
    const auto agents = getAllAgents();
    for (const auto& agent : agents) (void)agent->getIncomingMessages();
}

void TheOrgManager::monitorAgentHealth() {
    const auto agents = getAllAgents();
    for (const auto& agent : agents) {
        if (!agent->isRunning() && running_) agent->start();
    }
}

void TheOrgManager::executeScheduledTasks() {
    static std::atomic<std::size_t> tick{0};
    if ((tick.fetch_add(1) + 1) % SCHEDULED_TASK_INTERVAL_TICKS != 0) return;
    const auto agents = getAllAgents();
    const std::string payload = "scheduled_tick";
    for (const auto& agent : agents) agent->processMessage(payload, "manager");
}

void TheOrgManager::updateSystemMetrics() {
    const auto agents = getAllAgents();
    SystemMetrics next;
    next.totalAgents = agents.size();
    for (const auto& agent : agents) {
        if (agent->isRunning()) ++next.activeAgents;
        const auto pending = agent->getPendingTasks();
        next.pendingTasks += pending.size();
        next.totalTasks += pending.size();
    }
    next.systemLoad = next.totalAgents == 0 ? 0.0 :
        static_cast<double>(next.activeAgents) / static_cast<double>(next.totalAgents);
    next.lastUpdated = std::chrono::system_clock::now();
    std::lock_guard<std::mutex> lock(metricsMutex_);
    currentMetrics_ = next;
}

// ============================================================================
// the_org_utils - Missing utility functions
// ============================================================================

namespace the_org_utils {

bool isWorkingDay(const std::string& day) {
    static const std::set<std::string> workDays = {
        "Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
    std::string capitalised = day;
    if (!capitalised.empty()) {
        capitalised[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(capitalised[0])));
        for (size_t i = 1; i < capitalised.size(); ++i)
            capitalised[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(capitalised[i])));
    }
    return workDays.count(capitalised) > 0;
}

Timestamp parseTimeString(const std::string& timeStr, const std::string& /* format */) {
    // Parse "HH:MM" into today's timestamp at that time
    int hour = 0, minute = 0;
    if (timeStr.length() >= 5) {
        try {
            hour   = std::stoi(timeStr.substr(0, 2));
            minute = std::stoi(timeStr.substr(3, 2));
        } catch (...) {}
    }

    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::tm* tm_info = std::localtime(&time_t);

    tm_info->tm_hour = hour;
    tm_info->tm_min  = minute;
    tm_info->tm_sec  = 0;

    auto parsed = std::mktime(tm_info);
    return std::chrono::system_clock::from_time_t(parsed);
}

} // namespace the_org_utils

} // namespace elizaos
