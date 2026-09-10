// discord_summarizer_test.cpp - Behavioral tests for the canonical local Discord summarizer.
#include <gtest/gtest.h>
#include "elizaos/discord_summarizer.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <functional>
#include <sstream>
#include <thread>
#include <unordered_map>

using namespace elizaos;

namespace {

using Clock = std::chrono::system_clock;

DiscordMessage makeMessage(const std::string& id,
                           const std::string& channel,
                           const std::string& author,
                           const std::string& content,
                           Clock::time_point timestamp) {
    DiscordMessage message(id, channel, author, content);
    message.authorId = "id-" + author;
    message.timestamp = timestamp;
    return message;
}

std::filesystem::path temporaryPath(const std::string& suffix) {
    static std::atomic<unsigned long long> sequence{0};
    return std::filesystem::temp_directory_path() /
           ("elizaos-discord-" + std::to_string(++sequence) + "-" + suffix);
}

std::string readFile(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    std::ostringstream result;
    result << input.rdbuf();
    return result.str();
}

bool waitUntil(const std::function<bool()>& condition,
               std::chrono::milliseconds timeout = std::chrono::milliseconds(1000)) {
    const auto deadline = std::chrono::steady_clock::now() + timeout;
    while (std::chrono::steady_clock::now() < deadline) {
        if (condition()) return true;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    return condition();
}

class TemporaryFile {
public:
    explicit TemporaryFile(const std::string& suffix) : path(temporaryPath(suffix)) {}
    ~TemporaryFile() {
        std::error_code error;
        std::filesystem::remove_all(path, error);
    }
    std::filesystem::path path;
};

} // namespace

TEST(MessageAnalyzer, ProducesCoherentAnalysisAndHandlesEmptyContent) {
    MessageAnalyzer analyzer;
    const auto empty = analyzer.analyzeMessage(makeMessage("empty", "c", "A", "", Clock::now()));
    EXPECT_DOUBLE_EQ(empty.sentiment, 0.0);
    EXPECT_EQ(empty.toxicityLevel, 0);
    EXPECT_FALSE(empty.containsSpam);

    const auto positive = analyzer.analyzeMessage(
        makeMessage("positive", "c", "A", "I love excellent AI software", Clock::now()));
    EXPECT_GT(positive.sentiment, 0.0);
    EXPECT_NE(std::find(positive.topics.begin(), positive.topics.end(), "technology"),
              positive.topics.end());
    EXPECT_EQ(positive.language, "en");

    analyzer.setToxicityThreshold(9);
    EXPECT_EQ(analyzer.assessToxicity("damn"), 10);
    analyzer.addTopicCategory("science", {"quantum"});
    const auto topics = analyzer.extractTopics("Quantum research is amazing");
    EXPECT_NE(std::find(topics.begin(), topics.end(), "science"), topics.end());
}

TEST(DiscordDataManager, StoresRetrievesOrdersLimitsUpdatesAndClearsAllData) {
    DiscordDataManager data;
    const auto base = Clock::time_point(std::chrono::seconds(100));
    EXPECT_FALSE(data.storeMessage({}));
    EXPECT_FALSE(data.storeMessages({}));

    EXPECT_TRUE(data.storeMessages({
        makeMessage("old", "alpha", "Alice", "old", base),
        makeMessage("new", "alpha", "Bob", "new", base + std::chrono::seconds(2)),
        makeMessage("other", "beta", "Cara", "other", base + std::chrono::seconds(3))}));

    auto messages = data.retrieveMessages("alpha", 1);
    ASSERT_EQ(messages.size(), 1U);
    EXPECT_EQ(messages.front().id, "new");
    EXPECT_TRUE(data.retrieveMessages("alpha", 0).empty());

    auto replacement = makeMessage("new", "alpha", "Bob", "replacement", base + std::chrono::seconds(4));
    EXPECT_TRUE(data.storeMessage(replacement));
    messages = data.retrieveMessages("alpha", 10);
    ASSERT_EQ(messages.size(), 2U);
    EXPECT_EQ(messages.front().content, "replacement");

    MessageAnalysis analysis;
    analysis.messageId = "new";
    analysis.sentiment = 0.75;
    EXPECT_TRUE(data.storeAnalysis(analysis));
    EXPECT_DOUBLE_EQ(data.retrieveAnalysis("new").sentiment, 0.75);
    EXPECT_TRUE(data.retrieveAnalysis("missing").messageId.empty());

    ChannelSummary summary;
    summary.channelId = "alpha";
    summary.periodStart = base;
    summary.periodEnd = base + std::chrono::hours(1);
    summary.totalMessages = 2;
    EXPECT_TRUE(data.storeSummary(summary));
    ASSERT_EQ(data.retrieveSummaries("alpha").size(), 1U);

    data.clearCache();
    EXPECT_TRUE(data.retrieveMessages("alpha").empty());
    EXPECT_TRUE(data.retrieveAnalysis("new").messageId.empty());
    EXPECT_TRUE(data.retrieveSummaries("alpha").empty());
}

TEST(DiscordDataManager, BoundedCachesEvictOldestStoredEntries) {
    DiscordDataManager data;
    data.setCacheSize(2);
    const auto base = Clock::time_point(std::chrono::seconds(100));
    EXPECT_TRUE(data.storeMessage(makeMessage("m1", "c", "A", "one", base)));
    EXPECT_TRUE(data.storeMessage(makeMessage("m2", "c", "A", "two", base + std::chrono::seconds(1))));
    EXPECT_TRUE(data.storeMessage(makeMessage("m3", "c", "A", "three", base + std::chrono::seconds(2))));
    auto messages = data.retrieveMessages("c", 10);
    ASSERT_EQ(messages.size(), 2U);
    EXPECT_EQ(messages[0].id, "m3");
    EXPECT_EQ(messages[1].id, "m2");

    MessageAnalysis a1; a1.messageId = "m1";
    MessageAnalysis a2; a2.messageId = "m2";
    MessageAnalysis a3; a3.messageId = "m3";
    EXPECT_TRUE(data.storeAnalyses({a1, a2, a3}));
    EXPECT_TRUE(data.retrieveAnalysis("m1").messageId.empty());
    EXPECT_EQ(data.retrieveAnalysis("m2").messageId, "m2");

    ChannelSummary s1; s1.channelId = "c"; s1.periodStart = base; s1.periodEnd = base;
    ChannelSummary s2 = s1; s2.periodEnd += std::chrono::seconds(1);
    ChannelSummary s3 = s1; s3.periodEnd += std::chrono::seconds(2);
    EXPECT_TRUE(data.storeSummary(s1));
    EXPECT_TRUE(data.storeSummary(s2));
    EXPECT_TRUE(data.storeSummary(s3));
    const auto summaries = data.retrieveSummaries("c");
    ASSERT_EQ(summaries.size(), 2U);
    EXPECT_EQ(summaries[0].periodEnd, s3.periodEnd);
    EXPECT_EQ(summaries[1].periodEnd, s2.periodEnd);

    data.setCacheSize(0);
    EXPECT_TRUE(data.retrieveMessages("c").empty());
    EXPECT_FALSE(data.storeMessage(makeMessage("m4", "c", "A", "four", base)));
    EXPECT_FALSE(data.storeAnalysis(a1));
    EXPECT_FALSE(data.storeSummary(s1));
}

TEST(DiscordDataManager, PersistsAndLoadsDeterministicSnapshot) {
    TemporaryFile snapshot("snapshot.json");
    const auto base = Clock::time_point(std::chrono::milliseconds(123456));
    {
        DiscordDataManager data;
        data.setCacheSize(7);
        data.enablePersistence(snapshot.path.string());
        auto message = makeMessage("persisted", "channel", "Alice", "great software", base);
        message.attachments = {"asset"};
        ASSERT_TRUE(data.storeMessage(message));
        MessageAnalysis analysis;
        analysis.messageId = message.id;
        analysis.sentiment = 1.0;
        analysis.topics = {"technology"};
        analysis.categories["technology"] = 0.5;
        ASSERT_TRUE(data.storeAnalysis(analysis));
        ChannelSummary summary;
        summary.channelId = "channel";
        summary.channelName = "Channel";
        summary.periodStart = base;
        summary.periodEnd = base + std::chrono::seconds(1);
        summary.totalMessages = 1;
        ASSERT_TRUE(data.storeSummary(summary));
    }
    const auto firstBytes = readFile(snapshot.path);
    ASSERT_FALSE(firstBytes.empty());

    DiscordDataManager restored;
    restored.enablePersistence(snapshot.path.string());
    const auto messages = restored.retrieveMessages("channel");
    ASSERT_EQ(messages.size(), 1U);
    EXPECT_EQ(messages.front().content, "great software");
    EXPECT_EQ(messages.front().attachments, std::vector<std::string>({"asset"}));
    EXPECT_DOUBLE_EQ(restored.retrieveAnalysis("persisted").sentiment, 1.0);
    ASSERT_EQ(restored.retrieveSummaries("channel").size(), 1U);

    restored.setCacheSize(7);
    EXPECT_EQ(readFile(snapshot.path), firstBytes);
    restored.clearCache();

    DiscordDataManager cleared;
    cleared.enablePersistence(snapshot.path.string());
    EXPECT_TRUE(cleared.retrieveMessages("channel").empty());
    EXPECT_TRUE(cleared.retrieveAnalysis("persisted").messageId.empty());
    EXPECT_TRUE(cleared.retrieveSummaries("channel").empty());
}

TEST(DiscordDataManager, FailedPersistenceDoesNotClaimOrRetainSuccessfulMutation) {
    TemporaryFile blockingFile("not-a-directory");
    {
        std::ofstream output(blockingFile.path);
        output << "block";
    }
    DiscordDataManager data;
    data.enablePersistence((blockingFile.path / "snapshot.json").string());
    EXPECT_FALSE(data.storeMessage(makeMessage("m", "c", "A", "content", Clock::now())));
    EXPECT_TRUE(data.retrieveMessages("c").empty());
}

TEST(DiscordDataManager, ConcurrentWritersRemainConsistent) {
    DiscordDataManager data;
    data.setCacheSize(512);
    std::vector<std::thread> threads;
    for (int worker = 0; worker < 8; ++worker) {
        threads.emplace_back([&, worker]() {
            for (int index = 0; index < 25; ++index) {
                const auto id = std::to_string(worker) + "-" + std::to_string(index);
                EXPECT_TRUE(data.storeMessage(makeMessage(id, "concurrent", id, "message", Clock::now())));
            }
        });
    }
    for (auto& thread : threads) thread.join();
    EXPECT_EQ(data.retrieveMessages("concurrent", 1000).size(), 200U);
}

TEST(ChannelSummarizer, DerivesStatisticsFromProvidedMessagesAndAnalyses) {
    const auto base = Clock::time_point(std::chrono::hours(12));
    std::vector<DiscordMessage> messages = {
        makeMessage("m1", "c", "Alice", "great AI software", base + std::chrono::minutes(5)),
        makeMessage("m2", "c", "Alice", "awesome programming", base + std::chrono::minutes(10)),
        makeMessage("m3", "c", "Bob", "bad game", base + std::chrono::hours(1))};
    std::unordered_map<std::string, MessageAnalysis> analyses;
    analyses["m1"].messageId = "m1"; analyses["m1"].sentiment = 1.0; analyses["m1"].topics = {"technology"};
    analyses["m2"].messageId = "m2"; analyses["m2"].sentiment = 1.0; analyses["m2"].topics = {"technology"};
    analyses["m3"].messageId = "m3"; analyses["m3"].sentiment = -1.0; analyses["m3"].topics = {"gaming"};

    ChannelSummarizer summarizer;
    summarizer.setMinimumMessages(0);
    summarizer.setTopUsersLimit(1);
    summarizer.setTopTopicsLimit(1);
    summarizer.setDataProviders(
        [&](const std::string&) { return messages; },
        [&](const std::string& id) { return analyses.at(id); },
        [](const std::string& id) { return DiscordChannel(id, "General", "guild"); },
        [](const std::string&) { return std::vector<DiscordChannel>{DiscordChannel("c", "General", "guild")}; });

    const auto summary = summarizer.summarizeChannel("c", base, base + std::chrono::hours(2));
    EXPECT_EQ(summary.channelName, "General");
    EXPECT_EQ(summary.totalMessages, 3);
    EXPECT_EQ(summary.uniqueUsers, 2);
    EXPECT_EQ(summary.topUsers, std::vector<std::string>({"Alice"}));
    EXPECT_EQ(summary.mainTopics, std::vector<std::string>({"technology"}));
    EXPECT_NEAR(summary.averageSentiment, 1.0 / 3.0, 1e-12);
    EXPECT_EQ(summary.mostActiveTime, "12:00-13:00 UTC");

    const auto all = summarizer.summarizeAllChannels("guild", base, base + std::chrono::hours(2));
    ASSERT_EQ(all.size(), 1U);
    EXPECT_EQ(all.front().totalMessages, 3);
}

TEST(ChannelSummarizer, HonorsPeriodMinimumAndGeneratesEscapedReports) {
    const auto base = Clock::time_point(std::chrono::seconds(10));
    ChannelSummarizer summarizer;
    summarizer.setMinimumMessages(2);
    summarizer.setDataProviders(
        [&](const std::string&) { return std::vector<DiscordMessage>{makeMessage("one", "c", "A", "x", base)}; },
        [](const std::string&) { return MessageAnalysis{}; },
        [](const std::string& id) { return DiscordChannel(id, "<General>", "g"); },
        [](const std::string&) { return std::vector<DiscordChannel>{}; });
    const auto summary = summarizer.summarizeChannel("c", base, base + std::chrono::seconds(1));
    EXPECT_EQ(summary.totalMessages, 1);
    EXPECT_EQ(summary.uniqueUsers, 1);
    EXPECT_TRUE(summary.topUsers.empty());
    EXPECT_TRUE(summary.mainTopics.empty());
    EXPECT_DOUBLE_EQ(summary.averageSentiment, 0.0);
    EXPECT_TRUE(summary.mostActiveTime.empty());

    const auto text = summarizer.generateTextReport(summary);
    EXPECT_NE(text.find("Messages: 1"), std::string::npos);
    const auto json = summarizer.generateJsonReport(summary);
    EXPECT_NE(json.find("\"totalMessages\":1"), std::string::npos);
    const auto html = summarizer.generateHtmlReport(summary);
    EXPECT_NE(html.find("&lt;General&gt;"), std::string::npos);
    EXPECT_EQ(html.find("<h1><General></h1>"), std::string::npos);

    const auto invalid = summarizer.summarizeChannel("c", base + std::chrono::seconds(1), base);
    EXPECT_EQ(invalid.totalMessages, 0);
}

TEST(ChannelSummarizer, DailyAndWeeklyWrappersUseTheirExpectedPeriods) {
    const auto now = Clock::now();
    ChannelSummarizer summarizer;
    summarizer.setMinimumMessages(0);
    summarizer.setDataProviders(
        [&](const std::string&) {
            return std::vector<DiscordMessage>{
                makeMessage("recent", "c", "A", "recent", now - std::chrono::hours(1)),
                makeMessage("older", "c", "B", "older", now - std::chrono::hours(48))};
        },
        [](const std::string& id) { MessageAnalysis result; result.messageId = id; return result; },
        [](const std::string& id) { return DiscordChannel(id, "Channel", "g"); },
        [](const std::string&) { return std::vector<DiscordChannel>{}; });
    EXPECT_EQ(summarizer.summarizeChannelDaily("c").totalMessages, 1);
    EXPECT_EQ(summarizer.summarizeChannelWeekly("c").totalMessages, 2);
}

TEST(InProcessDiscordClient, OperationsAreStatefulAndTruthful) {
    DiscordSummarizer system;
    auto& client = system.getClient();
    EXPECT_FALSE(client.sendMessage("channel", "before connect"));
    EXPECT_FALSE(client.deleteMessage("channel", "missing"));
    EXPECT_TRUE(client.getGuilds().empty());
    EXPECT_FALSE(system.initializeWithToken(""));
    ASSERT_TRUE(system.initializeWithToken("token"));

    EXPECT_TRUE(client.sendMessage("channel", "first"));
    EXPECT_TRUE(client.sendMessage("channel", "second"));
    const auto messages = client.getMessages("channel", 10);
    ASSERT_EQ(messages.size(), 2U);
    EXPECT_EQ(messages.front().content, "second");
    EXPECT_EQ(client.getChannel("channel").guildId, "local-guild");
    ASSERT_EQ(client.getChannels("local-guild").size(), 1U);
    ASSERT_EQ(client.getGuilds().size(), 1U);
    EXPECT_EQ(client.getGuild("local-guild").channels, std::vector<std::string>({"channel"}));

    EXPECT_TRUE(client.deleteMessage("channel", messages.front().id));
    EXPECT_FALSE(client.deleteMessage("channel", messages.front().id));
    EXPECT_EQ(client.getMessages("channel", 10).size(), 1U);
    EXPECT_TRUE(client.disconnect());
    EXPECT_FALSE(client.disconnect());
    EXPECT_TRUE(client.getMessages("channel", 10).empty());
}

TEST(DiscordSummarizer, ChannelAndGuildSummariesUseClientDataAndStoredAnalyses) {
    DiscordSummarizer system;
    ASSERT_TRUE(system.initializeWithToken("token"));
    system.getSummarizer().setMinimumMessages(0);
    ASSERT_TRUE(system.getClient().sendMessage("technology", "great AI software"));
    ASSERT_TRUE(system.getClient().sendMessage("technology", "awesome programming"));
    ASSERT_TRUE(system.getClient().sendMessage("gaming", "bad game score"));
    const auto start = Clock::now() - std::chrono::minutes(1);
    const auto end = Clock::now() + std::chrono::minutes(1);

    const auto channel = system.generateChannelSummary("technology", start, end).get();
    EXPECT_EQ(channel.totalMessages, 2);
    EXPECT_EQ(channel.uniqueUsers, 1);
    EXPECT_GT(channel.averageSentiment, 0.0);
    EXPECT_NE(std::find(channel.mainTopics.begin(), channel.mainTopics.end(), "technology"),
              channel.mainTopics.end());
    ASSERT_EQ(system.getDataManager().retrieveSummaries("technology").size(), 1U);

    const auto guild = system.generateGuildSummary("local-guild", start, end).get();
    ASSERT_EQ(guild.size(), 2U);
    EXPECT_EQ(guild[0].channelId, "gaming");
    EXPECT_EQ(guild[0].totalMessages, 1);
    EXPECT_EQ(guild[1].channelId, "technology");
    EXPECT_EQ(guild[1].totalMessages, 2);

    const auto invalid = system.generateChannelSummary("technology", end, start).get();
    EXPECT_EQ(invalid.totalMessages, 0);
    EXPECT_TRUE(system.generateGuildSummary("missing", start, end).get().empty());
}

TEST(DiscordSummarizer, MonitoringRunsCollectsMessagesStopsAndRestartsJoinably) {
    DiscordSummarizer system;
    system.startMonitoring({"channel"});
    EXPECT_FALSE(system.isMonitoring());
    ASSERT_TRUE(system.initializeWithToken("token"));
    ASSERT_TRUE(system.getClient().sendMessage("channel", "great software"));
    system.getDataManager().clearCache();

    system.startMonitoring({"channel", "channel", ""});
    ASSERT_TRUE(system.isMonitoring());
    ASSERT_TRUE(waitUntil([&]() {
        return system.getDataManager().retrieveMessages("channel").size() == 1U &&
               !system.getDataManager().retrieveAnalysis("local-message-1").messageId.empty();
    }));
    system.stopMonitoring();
    EXPECT_FALSE(system.isMonitoring());

    system.startMonitoring({"channel"});
    EXPECT_TRUE(system.isMonitoring());
    system.stopMonitoring();
    system.stopMonitoring();
    EXPECT_FALSE(system.isMonitoring());
}

TEST(DiscordSummarizer, ConfigurationRoundTripIsDeterministicAndAppliesSettings) {
    TemporaryFile input("input.conf");
    TemporaryFile first("first.conf");
    TemporaryFile second("second.conf");
    {
        std::ofstream config(input.path);
        config << "version=1\n"
               << "top_users_limit=1\n"
               << "top_topics_limit=1\n"
               << "minimum_messages=0\n"
               << "cache_size=2\n"
               << "monitoring_interval_ms=5\n"
               << "persistence_path=\n"
               << "monitored_channels=z,a,z\n";
    }

    DiscordSummarizer configured;
    configured.loadConfiguration(input.path.string());
    const auto base = Clock::time_point(std::chrono::seconds(1));
    EXPECT_TRUE(configured.getDataManager().storeMessage(makeMessage("1", "c", "A", "one", base)));
    EXPECT_TRUE(configured.getDataManager().storeMessage(makeMessage("2", "c", "B", "two", base)));
    EXPECT_TRUE(configured.getDataManager().storeMessage(makeMessage("3", "c", "C", "three", base)));
    EXPECT_EQ(configured.getDataManager().retrieveMessages("c", 10).size(), 2U);
    const auto configuredSummary = configured.getSummarizer().summarizeChannel(
        "c", base - std::chrono::seconds(1), base + std::chrono::seconds(1));
    EXPECT_EQ(configuredSummary.totalMessages, 2);
    EXPECT_EQ(configuredSummary.topUsers.size(), 1U);
    configured.saveConfiguration(first.path.string());

    DiscordSummarizer restored;
    restored.loadConfiguration(first.path.string());
    restored.saveConfiguration(second.path.string());
    EXPECT_EQ(readFile(first.path), readFile(second.path));
    EXPECT_EQ(readFile(first.path),
              "version=1\n"
              "cache_size=2\n"
              "minimum_messages=0\n"
              "monitoring_interval_ms=5\n"
              "persistence_path=\n"
              "top_topics_limit=1\n"
              "top_users_limit=1\n"
              "monitored_channels=a,z\n");
}

TEST(DiscordSummarizer, InvalidConfigurationLeavesExistingBehaviorUntouched) {
    TemporaryFile invalid("invalid.conf");
    TemporaryFile saved("saved.conf");
    {
        std::ofstream config(invalid.path);
        config << "version=1\ncache_size=not-a-number\n";
    }
    DiscordSummarizer system;
    system.loadConfiguration(invalid.path.string());
    system.saveConfiguration(saved.path.string());
    EXPECT_NE(readFile(saved.path).find("cache_size=1000"), std::string::npos);
}
