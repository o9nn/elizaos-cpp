// discord_summarizer_test.cpp - End-to-end tests for Discord transport and analytics.
#include <gtest/gtest.h>

#include "elizaos/discord_summarizer.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <set>
#include <string>
#include <thread>

using namespace elizaos;
namespace fs = std::filesystem;

namespace {

fs::path uniqueTempPath(const std::string& name) {
    return fs::temp_directory_path() /
        (name + "_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
}

DiscordMessage makeMessage(const std::string& id, const std::string& channel,
                           const std::string& author, const std::string& content,
                           std::chrono::system_clock::time_point timestamp) {
    DiscordMessage message(id, channel, author, content);
    message.authorId = author + "-id";
    message.timestamp = timestamp;
    return message;
}

}  // namespace

TEST(DiscordValueTypes, ConstructorsInitializeSafeDefaults) {
    DiscordMessage message;
    DiscordChannel channel;
    DiscordGuild guild;
    MessageAnalysis analysis;
    ChannelSummary summary;
    EXPECT_TRUE(message.id.empty());
    EXPECT_FALSE(message.isBot);
    EXPECT_TRUE(channel.id.empty());
    EXPECT_FALSE(channel.isNsfw);
    EXPECT_TRUE(guild.id.empty());
    EXPECT_DOUBLE_EQ(analysis.sentiment, 0.0);
    EXPECT_EQ(analysis.toxicityLevel, 0);
    EXPECT_EQ(summary.totalMessages, 0);
    EXPECT_EQ(summary.uniqueUsers, 0);
}

TEST(DiscordClient, BaseTransportReportsUnsupportedOperationsTruthfully) {
    DiscordClient client;
    EXPECT_FALSE(client.connect(""));
    EXPECT_TRUE(client.connect("local-token"));
    EXPECT_TRUE(client.isConnected());
    EXPECT_TRUE(client.getMessages("channel").empty());
    EXPECT_FALSE(client.sendMessage("channel", "message"));
    EXPECT_FALSE(client.deleteMessage("channel", "message-id"));
    client.setMessageHandler([](const DiscordMessage&) {});
    client.setChannelHandler([](const DiscordChannel&) {});
    EXPECT_TRUE(client.disconnect());
    EXPECT_FALSE(client.disconnect());
    EXPECT_FALSE(client.isConnected());
}

TEST(MessageAnalyzer, ProducesDeterministicSentimentTopicsKeywordsToxicityAndSpam) {
    MessageAnalyzer analyzer;
    DiscordMessage positive = makeMessage("m1", "c", "alice",
        "Awesome AI software makes programming excellent", std::chrono::system_clock::now());
    const auto analysis = analyzer.analyzeMessage(positive);
    EXPECT_EQ(analysis.messageId, "m1");
    EXPECT_GT(analysis.sentiment, 0.0);
    EXPECT_EQ(analyzer.classifySentiment(analysis.sentiment), "positive");
    EXPECT_NE(std::find(analysis.topics.begin(), analysis.topics.end(), "technology"),
              analysis.topics.end());
    EXPECT_NE(std::find(analysis.keywords.begin(), analysis.keywords.end(), "awesome"),
              analysis.keywords.end());
    EXPECT_EQ(analysis.language, "en");
    EXPECT_FALSE(analysis.containsSpam);

    DiscordMessage spam = makeMessage("m2", "c", "bot", "xxxxxxxxxxxx http://spam",
                                      std::chrono::system_clock::now());
    spam.isBot = true;
    EXPECT_TRUE(analyzer.detectSpam(spam));
    EXPECT_GT(analyzer.assessToxicity("THIS IS AN AGGRESSIVE SHOUT"), 1);
    EXPECT_EQ(analyzer.assessToxicity(""), 1);
}

TEST(DiscordDataManager, StoresOrdersLimitsEvictsAndClearsRealData) {
    DiscordDataManager manager;
    const auto now = std::chrono::system_clock::now();
    EXPECT_FALSE(manager.storeMessage({}));
    ASSERT_TRUE(manager.storeMessage(makeMessage("old", "channel", "alice", "old", now - std::chrono::hours(2))));
    ASSERT_TRUE(manager.storeMessage(makeMessage("new", "channel", "bob", "new", now)));
    ASSERT_TRUE(manager.storeMessage(makeMessage("other", "other-channel", "carol", "other", now)));

    auto messages = manager.retrieveMessages("channel", 1);
    ASSERT_EQ(messages.size(), 1u);
    EXPECT_EQ(messages.front().id, "new");
    EXPECT_EQ(manager.getCachedChannelIds(),
              (std::vector<std::string>{"channel", "other-channel"}));

    MessageAnalysis analysis;
    analysis.messageId = "new";
    analysis.sentiment = 0.75;
    ASSERT_TRUE(manager.storeAnalysis(analysis));
    EXPECT_DOUBLE_EQ(manager.retrieveAnalysis("new").sentiment, 0.75);
    EXPECT_TRUE(manager.retrieveAnalysis("missing").messageId.empty());

    ChannelSummary summary;
    summary.channelId = "channel";
    summary.channelName = "General";
    summary.periodStart = now - std::chrono::hours(1);
    summary.periodEnd = now;
    summary.totalMessages = 1;
    ASSERT_TRUE(manager.storeSummary(summary));
    ASSERT_EQ(manager.retrieveSummaries("channel").size(), 1u);

    manager.setCacheSize(1);
    EXPECT_EQ(manager.retrieveMessages("channel", 10).size(), 1u);
    EXPECT_TRUE(manager.retrieveMessages("other-channel", 10).empty());
    manager.clearCache();
    EXPECT_TRUE(manager.retrieveMessages("channel", 10).empty());
    EXPECT_TRUE(manager.retrieveSummaries("channel").empty());
}

TEST(DiscordDataManager, PersistenceRoundTripRestoresMessagesAnalysesAndSummaries) {
    const fs::path directory = uniqueTempPath("discord_cache_roundtrip");
    const auto now = std::chrono::system_clock::now();
    {
        DiscordDataManager source;
        source.enablePersistence(directory.string());
        ASSERT_TRUE(source.storeMessage(makeMessage("m1", "channel", "alice", "AI is great", now)));
        MessageAnalysis analysis;
        analysis.messageId = "m1";
        analysis.sentiment = 1.0;
        analysis.topics = {"technology"};
        ASSERT_TRUE(source.storeAnalysis(analysis));
        ChannelSummary summary;
        summary.channelId = "channel";
        summary.channelName = "General";
        summary.periodStart = now - std::chrono::hours(1);
        summary.periodEnd = now;
        summary.totalMessages = 1;
        summary.uniqueUsers = 1;
        summary.averageSentiment = 1.0;
        ASSERT_TRUE(source.storeSummary(summary));
    }

    DiscordDataManager restored;
    restored.enablePersistence(directory.string());
    const auto messages = restored.retrieveMessages("channel", 10);
    ASSERT_EQ(messages.size(), 1u);
    EXPECT_EQ(messages.front().content, "AI is great");
    EXPECT_DOUBLE_EQ(restored.retrieveAnalysis("m1").sentiment, 1.0);
    const auto summaries = restored.retrieveSummaries("channel");
    ASSERT_EQ(summaries.size(), 1u);
    EXPECT_EQ(summaries.front().uniqueUsers, 1);

    std::error_code ec;
    fs::remove_all(directory, ec);
}

TEST(ChannelSummarizer, ComputesRealStatisticsAndProducesReports) {
    DiscordDataManager manager;
    MessageAnalyzer analyzer;
    ChannelSummarizer summarizer;
    summarizer.setDataSources(&manager, &analyzer);
    summarizer.setMinimumMessages(1);
    summarizer.setTopUsersLimit(2);
    summarizer.setTopTopicsLimit(2);

    const auto now = std::chrono::system_clock::now();
    ASSERT_TRUE(manager.storeMessages({
        makeMessage("m1", "general", "alice", "AI programming is awesome", now - std::chrono::minutes(20)),
        makeMessage("m2", "general", "alice", "Software is excellent", now - std::chrono::minutes(10)),
        makeMessage("m3", "general", "bob", "Gaming is good", now - std::chrono::minutes(5))
    }));

    const auto summary = summarizer.summarizeChannel(
        "general", now - std::chrono::hours(1), now + std::chrono::minutes(1));
    EXPECT_EQ(summary.totalMessages, 3);
    EXPECT_EQ(summary.uniqueUsers, 2);
    ASSERT_FALSE(summary.topUsers.empty());
    EXPECT_EQ(summary.topUsers.front(), "alice");
    EXPECT_GT(summary.averageSentiment, 0.0);
    EXPECT_FALSE(summary.mostActiveTime.empty());
    EXPECT_NE(std::find(summary.mainTopics.begin(), summary.mainTopics.end(), "technology"),
              summary.mainTopics.end());
    EXPECT_NE(summarizer.generateTextReport(summary).find("Messages: 3"), std::string::npos);
    EXPECT_NE(summarizer.generateJsonReport(summary).find("\"total_messages\": 3"), std::string::npos);
    EXPECT_NE(summarizer.generateHtmlReport(summary).find("<!doctype html>"), std::string::npos);
}

TEST(DiscordSummarizer, LocalTransportFeedsAnalysisSummaryGuildAndMonitoringLifecycle) {
    DiscordSummarizer summarizer;
    EXPECT_FALSE(summarizer.initializeWithToken(""));
    ASSERT_TRUE(summarizer.initializeWithToken("local-token"));
    auto& client = summarizer.getClient();
    ASSERT_TRUE(client.sendMessage("general", "AI programming is awesome"));
    ASSERT_TRUE(client.sendMessage("general", "Software engineering is excellent"));
    ASSERT_TRUE(client.sendMessage("random", "Gaming is good"));

    auto generalMessages = client.getMessages("general", 10);
    ASSERT_EQ(generalMessages.size(), 2u);
    EXPECT_TRUE(client.deleteMessage("general", generalMessages.back().id));
    EXPECT_FALSE(client.deleteMessage("general", "missing"));

    const auto now = std::chrono::system_clock::now();
    const auto summary = summarizer.generateChannelSummary(
        "general", now - std::chrono::hours(1), now + std::chrono::minutes(1)).get();
    EXPECT_GE(summary.totalMessages, 2);
    EXPECT_EQ(summary.uniqueUsers, 1);
    EXPECT_GT(summary.averageSentiment, 0.0);

    const auto guildSummaries = summarizer.generateGuildSummary(
        "", now - std::chrono::hours(1), now + std::chrono::minutes(1)).get();
    EXPECT_GE(guildSummaries.size(), 2u);

    summarizer.startMonitoring({"general", "random"});
    EXPECT_TRUE(summarizer.isMonitoring());
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    summarizer.stopMonitoring();
    EXPECT_FALSE(summarizer.isMonitoring());
    summarizer.startMonitoring({});
    EXPECT_FALSE(summarizer.isMonitoring());
}

TEST(DiscordSummarizer, ConfigurationRoundTripAndLoadedLimitsAffectBehavior) {
    const fs::path directory = uniqueTempPath("discord_config_roundtrip");
    const fs::path configPath = directory / "config.json";
    fs::create_directories(directory);

    DiscordSummarizer defaults;
    defaults.saveConfiguration(configPath.string());
    ASSERT_TRUE(fs::is_regular_file(configPath));

    {
        std::ofstream output(configPath, std::ios::trunc);
        output << R"({
  "schema": "elizaos.discord_summarizer_config",
  "version": 1,
  "settings": {
    "cache_size": "10",
    "top_users_limit": "1",
    "top_topics_limit": "1",
    "minimum_messages": "5",
    "poll_interval_ms": "10",
    "persistence_path": ""
  }
})";
    }

    DiscordSummarizer configured;
    configured.loadConfiguration(configPath.string());
    ASSERT_TRUE(configured.initializeWithToken("local-token"));
    ASSERT_TRUE(configured.getClient().sendMessage("general", "AI is awesome"));
    const auto now = std::chrono::system_clock::now();
    const auto summary = configured.generateChannelSummary(
        "general", now - std::chrono::hours(1), now + std::chrono::minutes(1)).get();
    EXPECT_EQ(summary.totalMessages, 1);
    EXPECT_TRUE(summary.topUsers.empty());
    EXPECT_TRUE(summary.mainTopics.empty());

    std::error_code ec;
    fs::remove_all(directory, ec);
}
