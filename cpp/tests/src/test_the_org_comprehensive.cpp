#include <gtest/gtest.h>

#include "elizaos/the_org.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <future>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <vector>

using namespace elizaos;
using namespace std::chrono_literals;

namespace {

AgentConfig makeConfig(const std::string& id, const std::string& name) {
    AgentConfig config;
    config.agentId = id;
    config.agentName = name;
    config.bio = "TheOrg comprehensive test agent";
    config.lore = "Deterministic local test";
    return config;
}

PlatformConfig makePlatform(PlatformType type) {
    PlatformConfig config{};
    config.type = type;
    config.applicationId = "app";
    config.apiToken = "token";
    config.webhookUrl = "https://example.invalid/hook";
    config.additionalSettings = {{"workspace", "test"}};
    return config;
}

std::filesystem::path tempPath(const std::string& stem) {
    return std::filesystem::temp_directory_path() /
        (stem + "-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + ".json");
}

void waitUntil(const std::function<bool()>& predicate, std::chrono::milliseconds timeout = 2s) {
    const auto deadline = std::chrono::steady_clock::now() + timeout;
    while (!predicate() && std::chrono::steady_clock::now() < deadline) std::this_thread::sleep_for(2ms);
    ASSERT_TRUE(predicate());
}

}  // namespace

TEST(TheOrgAdapterContract, AcceptRejectThrowAndReentrantCallbackAreTruthful) {
    CommunityManagerAgent agent(makeConfig("community-adapter", "Eli5"));
    agent.addPlatform(makePlatform(PlatformType::DISCORD));

    EXPECT_FALSE(agent.sendMessage(PlatformType::DISCORD, "general", "missing adapter"));
    EXPECT_TRUE(agent.getRecentMessages(PlatformType::DISCORD, "general").empty());

    agent.setPlatformAdapter(PlatformType::DISCORD,
        [](const PlatformConfig&, const std::string&, const std::string&) { return false; });
    EXPECT_FALSE(agent.sendMessage(PlatformType::DISCORD, "general", "rejected"));
    EXPECT_TRUE(agent.getRecentMessages(PlatformType::DISCORD, "general").empty());

    agent.setPlatformAdapter(PlatformType::DISCORD,
        [](const PlatformConfig&, const std::string&, const std::string&) -> bool {
            throw std::runtime_error("adapter failure");
        });
    EXPECT_FALSE(agent.sendMessage(PlatformType::DISCORD, "general", "throws"));

    std::atomic<int> callbacks{0};
    agent.setPlatformAdapter(PlatformType::DISCORD,
        [&](const PlatformConfig&, const std::string& channel, const std::string& payload) {
            ++callbacks;
            EXPECT_EQ(channel, "general");
            EXPECT_EQ(payload, "accepted");
            agent.updateConfig({{"inside-adapter", "yes"}});
            agent.clearPlatformAdapter(PlatformType::DISCORD);
            return true;
        });
    auto send = std::async(std::launch::async, [&] {
        return agent.sendMessage(PlatformType::DISCORD, "general", "accepted");
    });
    ASSERT_EQ(send.wait_for(1s), std::future_status::ready);
    EXPECT_TRUE(send.get());
    EXPECT_EQ(callbacks.load(), 1);
    EXPECT_EQ(agent.getConfigValue("inside-adapter"), "yes");
    EXPECT_EQ(agent.getRecentMessages(PlatformType::DISCORD, "general"),
              std::vector<std::string>({"accepted"}));
    EXPECT_FALSE(agent.sendMessage(PlatformType::DISCORD, "general", "adapter cleared"));
}

TEST(TheOrgTaskLifecycle, ValidationPriorityAndTerminalCompletion) {
    CommunityManagerAgent agent(makeConfig("community-tasks", "Eli5"));
    EXPECT_TRUE(agent.createTask("", "invalid").empty());
    EXPECT_TRUE(agent.createTask("invalid", "priority", 1001).empty());
    const auto low = agent.createTask("low", "later", 1);
    const auto high = agent.createTask("high", "first", 9);
    ASSERT_FALSE(low.empty());
    ASSERT_FALSE(high.empty());
    const auto pending = agent.getPendingTasks();
    ASSERT_EQ(pending.size(), 2U);
    EXPECT_EQ(pending.front()->getId(), high);
    EXPECT_TRUE(agent.completeTask(high));
    EXPECT_FALSE(agent.completeTask(high));
    EXPECT_FALSE(agent.completeTask("unknown"));
    ASSERT_EQ(agent.getPendingTasks().size(), 1U);
    EXPECT_EQ(agent.getPendingTasks().front()->getId(), low);
}

TEST(TheOrgPublication, ContentStatusRequiresAllTargetAdaptersToAccept) {
    SocialMediaManagerAgent social(makeConfig("social-publication", "Laura"));
    social.addPlatform(makePlatform(PlatformType::DISCORD));
    social.addPlatform(makePlatform(PlatformType::TWITTER));
    social.setPlatformAdapter(PlatformType::DISCORD,
        [](const PlatformConfig&, const std::string&, const std::string&) { return true; });
    social.setPlatformAdapter(PlatformType::TWITTER,
        [](const PlatformConfig&, const std::string&, const std::string&) { return false; });

    const auto partial = social.createContent(ContentType::TEXT_POST, "Partial", "payload",
                                               {PlatformType::DISCORD, PlatformType::TWITTER});
    EXPECT_TRUE(social.publishContentToPlatform(partial, PlatformType::DISCORD));
    ASSERT_TRUE(social.getContent(partial));
    EXPECT_EQ(social.getContent(partial)->status, ContentStatus::FAILED);
    EXPECT_FALSE(social.publishContentToPlatform(partial, PlatformType::TELEGRAM));
    social.publishContent(partial);
    EXPECT_EQ(social.getContent(partial)->status, ContentStatus::FAILED);

    social.setPlatformAdapter(PlatformType::TWITTER,
        [](const PlatformConfig&, const std::string&, const std::string&) { return true; });
    const auto accepted = social.createContent(ContentType::TEXT_POST, "Accepted", "payload",
                                                {PlatformType::DISCORD, PlatformType::TWITTER});
    social.publishContent(accepted);
    ASSERT_TRUE(social.getContent(accepted));
    EXPECT_EQ(social.getContent(accepted)->status, ContentStatus::PUBLISHED);
    EXPECT_EQ(social.getPlatformMetrics(PlatformType::DISCORD).platform, PlatformType::DISCORD);
}

TEST(TheOrgPublication, ScheduledAndCampaignPublicationUseAdapterOutcomes) {
    SocialMediaManagerAgent social(makeConfig("social-campaign", "Laura"));
    social.addPlatform(makePlatform(PlatformType::DISCORD));
    std::atomic<bool> accept{false};
    social.setPlatformAdapter(PlatformType::DISCORD,
        [&](const PlatformConfig&, const std::string&, const std::string&) { return accept.load(); });

    const auto content = social.createContent(ContentType::TEXT_POST, "Campaign", "payload",
                                               {PlatformType::DISCORD});
    social.scheduleContent(content, std::chrono::system_clock::now() - 1s);
    social.start();
    waitUntil([&] {
        auto value = social.getContent(content);
        return value && value->status == ContentStatus::FAILED;
    });
    social.stop();

    const auto campaign = social.createCampaign("Launch", "adapter-gated",
        {PlatformType::DISCORD}, std::chrono::system_clock::now(),
        std::chrono::system_clock::now() + 1h);
    social.addContentToCampaign(campaign, content);
    EXPECT_FALSE(social.launchCampaign(campaign));
    EXPECT_NE(social.analyzeCampaignPerformance(campaign).find("Status: Inactive"), std::string::npos);

    accept = true;
    EXPECT_TRUE(social.launchCampaign(campaign));
    EXPECT_NE(social.analyzeCampaignPerformance(campaign).find("Status: Active"), std::string::npos);
    EXPECT_EQ(social.getContent(content)->status, ContentStatus::PUBLISHED);
}

TEST(TheOrgExternalApis, UnsupportedOperationsRejectAndSupportedMessagesReportAcceptance) {
    SocialMediaManagerAgent social(makeConfig("social-external", "Laura"));
    social.addPlatform(makePlatform(PlatformType::DISCORD));
    social.setPlatformAdapter(PlatformType::DISCORD,
        [](const PlatformConfig&, const std::string&, const std::string&) { return true; });

    EXPECT_FALSE(social.monitorMentions(PlatformType::DISCORD));
    EXPECT_TRUE(social.getRecentMentions(PlatformType::DISCORD).empty());
    EXPECT_FALSE(social.likePost("post", PlatformType::DISCORD));
    EXPECT_FALSE(social.respondToComment("", "response", PlatformType::DISCORD));
    EXPECT_TRUE(social.respondToComment("comment", "response", PlatformType::DISCORD));
    EXPECT_TRUE(social.sharePost("post", "sharing", PlatformType::DISCORD));

    social.clearPlatformAdapter(PlatformType::DISCORD);
    EXPECT_FALSE(social.respondToComment("comment", "response", PlatformType::DISCORD));
    EXPECT_FALSE(social.sharePost("post", "sharing", PlatformType::DISCORD));
}

TEST(TheOrgProjectManager, UnknownAvailabilityAndTaskAssignmentAreTruthful) {
    ProjectManagerAgent project(makeConfig("project-truth", "Jimmy"));
    EXPECT_FALSE(project.isTeamMemberAvailable("unknown", std::chrono::system_clock::now()));

    TeamMember member;
    member.id = "member";
    member.name = "Developer";
    member.availability.workDays = {};
    member.availability.employmentStatus = TeamMemberAvailability::EmploymentStatus::FULL_TIME;
    ASSERT_EQ(project.addTeamMember(member), "member");
    EXPECT_TRUE(project.isTeamMemberAvailable("member", std::chrono::system_clock::now()));

    const auto task = project.createTask("Assigned", "tracked", 2);
    project.assignTaskToMember(task, "unknown");
    EXPECT_TRUE(project.getMemberTasks("unknown").empty());
    project.assignTaskToMember("unknown", "member");
    EXPECT_TRUE(project.getMemberTasks("member").empty());
    project.assignTaskToMember(task, "member");
    project.assignTaskToMember(task, "member");
    EXPECT_EQ(project.getMemberTasks("member"), std::vector<UUID>({task}));
}

TEST(TheOrgCommunity, EventsModerationMetricsAndReportsPersistObservableResults) {
    CommunityManagerAgent community(makeConfig("community-events", "Eli5"));
    community.initialize();
    community.addPlatform(makePlatform(PlatformType::DISCORD));
    community.setPlatformAdapter(PlatformType::DISCORD,
        [](const PlatformConfig&, const std::string& channel, const std::string&) {
            return channel != "reject";
        });
    community.enableNewUserGreeting("welcome", "Welcome {user} to {server}");
    community.recordNewUser("alice", "server");
    EXPECT_FALSE(community.shouldGreetNewUser("alice"));
    community.addModerationRule("forbidden", ModerationAction::WARNING, "rule matched");
    community.recordCommunityMessage("forbidden deployment topic", "alice", "engineering");
    ASSERT_EQ(community.getModerationHistory().size(), 1U);

    community.scheduleEvent("Town hall", "Roadmap", std::chrono::system_clock::now() + 1h);
    auto events = community.getScheduledEvents();
    ASSERT_EQ(events.size(), 1U);
    community.announceEvent(events.front().id, {"accepted", "reject"});
    community.trackEventParticipation(events.front().id, "alice");
    events = community.getScheduledEvents();
    ASSERT_EQ(events.size(), 1U);
    EXPECT_EQ(events.front().announcedChannels, std::vector<std::string>({"accepted"}));
    EXPECT_EQ(events.front().participants, std::set<std::string>({"alice"}));

    community.initiateConflictResolution({"bob", "alice"}, "mediation");
    community.escalateIssue("needs review", {"alice", "bob"});
    EXPECT_EQ(community.getConflictCases().size(), 1U);
    EXPECT_EQ(community.getEscalations().size(), 1U);
    const std::string report = community.generateDailyCommunityReport();
    EXPECT_NE(report.find("members=1"), std::string::npos);
    EXPECT_NE(report.find("messages=1"), std::string::npos);
    EXPECT_EQ(community.getDailyReports().back(), report);
}

TEST(TheOrgLiaison, ReportsAndAdapterBackedDistributionRemainReentrant) {
    CommunityLiaisonAgent liaison(makeConfig("liaison-reports", "Ruby"));
    OrganizationConfig one;
    one.id = "org-one";
    one.name = "One";
    one.platforms = {makePlatform(PlatformType::DISCORD)};
    one.subscribedTopics = {"agents"};
    OrganizationConfig two = one;
    two.id = "org-two";
    two.name = "Two";
    liaison.addOrganization(one);
    liaison.addOrganization(two);
    liaison.addPlatform(makePlatform(PlatformType::DISCORD));
    std::atomic<int> accepted{0};
    liaison.setPlatformAdapter(PlatformType::DISCORD,
        [&](const PlatformConfig&, const std::string&, const std::string&) {
            ++accepted;
            (void)liaison.getMonitoredOrganizations();
            return true;
        });
    liaison.trackDiscussion(one.id, "agents", "great progress", "general");
    liaison.trackDiscussion(two.id, "agents", "excellent success", "general");
    const auto report = liaison.generateDailyReport({one.id, two.id});
    ASSERT_EQ(report.content.parallelTopics.size(), 1U);
    auto distribute = std::async(std::launch::async, [&] { liaison.distributeReport(report); });
    ASSERT_EQ(distribute.wait_for(1s), std::future_status::ready);
    distribute.get();
    EXPECT_EQ(accepted.load(), 2);
    EXPECT_EQ(liaison.generateTopicSpecificReport("agents").content.knowledgeGaps.size(), 2U);
}

TEST(TheOrgManagerCallbacks, EventAndWorkflowCallbacksAreDispatchedOutsideLocks) {
    TheOrgManager manager;
    auto community = std::make_shared<CommunityManagerAgent>(makeConfig("callback-community", "Eli5"));
    auto developer = std::make_shared<DeveloperRelationsAgent>(makeConfig("callback-developer", "Eddy"));
    manager.addAgent(community);
    manager.addAgent(developer);
    manager.subscribeToEvents(community->getId(), {"release"});
    manager.subscribeToEvents(community->getId(), {"release"});
    manager.enableEventLogging("memory-only");

    auto event = std::async(std::launch::async, [&] {
        manager.publishEvent("release", "ready", developer->getId());
    });
    ASSERT_EQ(event.wait_for(1s), std::future_status::ready);
    event.get();
    auto messages = community->getIncomingMessages();
    ASSERT_EQ(messages.size(), 1U);
    EXPECT_NE(messages.front().find("release:ready"), std::string::npos);
    EXPECT_EQ(manager.getRecentEvents().size(), 1U);

    const auto workflow = manager.createCrossAgentWorkflow(
        "Ship", {AgentRole::COMMUNITY_MANAGER, AgentRole::DEVELOPER_RELATIONS});
    ASSERT_FALSE(workflow.empty());
    EXPECT_EQ(community->getPendingTasks().size(), 1U);
    EXPECT_EQ(developer->getPendingTasks().size(), 1U);
    auto execute = std::async(std::launch::async, [&] {
        manager.executeWorkflow(workflow, {{"version", "1"}});
    });
    ASSERT_EQ(execute.wait_for(1s), std::future_status::ready);
    execute.get();
    EXPECT_FALSE(community->getIncomingMessages().empty());
    EXPECT_FALSE(developer->getIncomingMessages().empty());
}

TEST(TheOrgPersistence, ConfigurationIsVersionedIdempotentAndMalformedLoadRollsBack) {
    const auto path = tempPath("theorg-config");
    TheOrgManager source;
    source.updateGlobalSetting("mode", "strict");
    source.updateGlobalSetting("stale", "remove-on-load");
    source.addGlobalPlatform(makePlatform(PlatformType::DISCORD));
    source.saveConfiguration(path.string());
    ASSERT_TRUE(source.getLastConfigurationError().empty()) << source.getLastConfigurationError();

    TheOrgManager loaded;
    loaded.updateGlobalSetting("stale", "old");
    loaded.loadConfiguration(path.string());
    EXPECT_TRUE(loaded.getLastConfigurationError().empty()) << loaded.getLastConfigurationError();
    EXPECT_EQ(loaded.getGlobalSetting("mode"), "strict");
    loaded.loadConfiguration(path.string());
    EXPECT_TRUE(loaded.getLastConfigurationError().empty());
    EXPECT_EQ(loaded.getGlobalSetting("mode"), "strict");

    {
        std::ofstream malformed(path, std::ios::trunc);
        malformed << R"({"schema":"elizaos.the_org.configuration","version":1,"globalSettings":{"mode":7},"platforms":[]})";
    }
    loaded.loadConfiguration(path.string());
    EXPECT_FALSE(loaded.getLastConfigurationError().empty());
    EXPECT_EQ(loaded.getGlobalSetting("mode"), "strict");
    std::filesystem::remove(path);
}

TEST(TheOrgPersistence, SystemStateUsesValidatedAtomicJsonAndMalformedLoadRollsBack) {
    const auto path = tempPath("theorg-state");
    TheOrgManager source;
    source.updateGlobalSetting("mode", "stateful");
    source.addGlobalPlatform(makePlatform(PlatformType::SLACK));
    const auto workflow = source.createCrossAgentWorkflow("Persisted", {AgentRole::COMMUNITY_MANAGER});
    ASSERT_FALSE(workflow.empty());
    source.executeWorkflow(workflow, {{"key", "value"}});
    source.saveSystemState(path.string());
    ASSERT_TRUE(source.getLastConfigurationError().empty()) << source.getLastConfigurationError();

    std::ifstream input(path);
    std::string serialized((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    EXPECT_NE(serialized.find("elizaos.the_org.system_state"), std::string::npos);
    EXPECT_EQ(serialized.find("# TheOrg System State Backup"), std::string::npos);

    TheOrgManager loaded;
    loaded.updateGlobalSetting("mode", "before");
    loaded.loadSystemState(path.string());
    EXPECT_TRUE(loaded.getLastConfigurationError().empty()) << loaded.getLastConfigurationError();
    EXPECT_EQ(loaded.getGlobalSetting("mode"), "stateful");
    loaded.loadSystemState(path.string());
    EXPECT_TRUE(loaded.getLastConfigurationError().empty());
    const auto next = loaded.createCrossAgentWorkflow("After load", {AgentRole::COMMUNITY_MANAGER});
    EXPECT_NE(next, workflow);

    {
        std::ofstream malformed(path, std::ios::trunc);
        malformed << R"({"schema":"elizaos.the_org.system_state","version":99,"globalSettings":{},"platforms":[],"workflows":[]})";
    }
    loaded.loadSystemState(path.string());
    EXPECT_FALSE(loaded.getLastConfigurationError().empty());
    EXPECT_EQ(loaded.getGlobalSetting("mode"), "stateful");
    std::filesystem::remove(path);
}

TEST(TheOrgTeardown, RepeatedProcessThreadAndManagerShutdownIsPrompt) {
    const auto started = std::chrono::steady_clock::now();
    for (int iteration = 0; iteration < 40; ++iteration) {
        CommunityManagerAgent community(makeConfig("teardown-community-" + std::to_string(iteration), "Eli5"));
        community.start();
        community.processMessage("work", "test");
        community.stop();
        community.stop();
    }
    for (int iteration = 0; iteration < 12; ++iteration) {
        TheOrgManager manager;
        manager.addAgent(std::make_shared<DeveloperRelationsAgent>(
            makeConfig("teardown-developer-" + std::to_string(iteration), "Eddy")));
        manager.startAllAgents();
        manager.stopAllAgents();
    }
    EXPECT_LT(std::chrono::steady_clock::now() - started, 5s);
}

TEST(TheOrgConcurrency, ConcurrentAdaptersTasksEventsAndQueriesRemainConsistent) {
    SocialMediaManagerAgent social(makeConfig("concurrent-social", "Laura"));
    social.addPlatform(makePlatform(PlatformType::DISCORD));
    std::atomic<int> accepted{0};
    social.setPlatformAdapter(PlatformType::DISCORD,
        [&](const PlatformConfig&, const std::string&, const std::string&) {
            ++accepted;
            (void)social.getRecentMessages(PlatformType::DISCORD, "general", 4);
            return true;
        });
    constexpr int threadCount = 8;
    constexpr int perThread = 40;
    std::vector<std::thread> workers;
    for (int thread = 0; thread < threadCount; ++thread) {
        workers.emplace_back([&, thread] {
            for (int i = 0; i < perThread; ++i) {
                const auto id = social.createContent(ContentType::TEXT_POST,
                    "title-" + std::to_string(thread) + "-" + std::to_string(i), "payload",
                    {PlatformType::DISCORD});
                EXPECT_TRUE(social.publishContentToPlatform(id, PlatformType::DISCORD));
                const auto task = social.createTask("task-" + std::to_string(thread) + "-" + std::to_string(i), "work", i);
                EXPECT_TRUE(social.completeTask(task));
            }
        });
    }
    for (auto& worker : workers) worker.join();
    EXPECT_EQ(accepted.load(), threadCount * perThread);
    EXPECT_EQ(social.getContentByStatus(ContentStatus::PUBLISHED).size(),
              static_cast<std::size_t>(threadCount * perThread));
    EXPECT_TRUE(social.getPendingTasks().empty());
}
