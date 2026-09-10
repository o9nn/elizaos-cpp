#include <gtest/gtest.h>

#include "elizaos/discrub_ext.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <set>
#include <stdexcept>
#include <thread>

namespace elizaos {
namespace {

using namespace std::chrono_literals;

constexpr const char* kGuild = "111111111111111111";
constexpr const char* kChannel = "222222222222222222";
constexpr const char* kChannelTwo = "333333333333333333";
constexpr const char* kUser = "444444444444444444";

DiscordMessage message(const std::string& id, const std::string& content,
                       std::chrono::system_clock::time_point timestamp,
                       bool bot = false) {
    DiscordMessage value;
    value.id = id;
    value.channelId = kChannel;
    value.guildId = kGuild;
    value.authorId = kUser;
    value.authorName = "fixture-user";
    value.content = content;
    value.timestamp = timestamp;
    value.isBot = bot;
    return value;
}

DiscordAcknowledgement acknowledged(const std::string& receipt) {
    DiscordAcknowledgement result;
    result.status = DiscordOperationStatus::ACKNOWLEDGED;
    result.receiptId = receipt;
    return result;
}

class FakeDataAdapter final : public DiscordDataAdapter {
public:
    std::vector<DiscordMessage> messages;
    std::vector<DiscordChannel> channels;
    bool throwOnFetch{false};
    bool malformedPagination{false};
    std::atomic<int> fetchCalls{0};

    MessagePage fetchMessages(const MessageQuery& query) override {
        ++fetchCalls;
        if (throwOnFetch) {
            throw std::runtime_error("fake fetch failure");
        }
        return page(query, false);
    }

    MessagePage searchMessages(const MessageQuery& query) override {
        return page(query, true);
    }

    ChannelPage fetchGuildChannels(const std::string& guildId,
                                   const std::string& cursor,
                                   std::size_t limit) override {
        ChannelPage result;
        result.acknowledgement = acknowledged("channels-" + cursor);
        std::size_t offset = cursor.empty() ? 0U : static_cast<std::size_t>(std::stoul(cursor));
        for (std::size_t index = offset;
             index < channels.size() && result.channels.size() < limit; ++index) {
            if (channels[index].guildId == guildId) {
                result.channels.push_back(channels[index]);
            }
        }
        const std::size_t next = offset + result.channels.size();
        result.hasMore = next < channels.size();
        result.nextCursor = result.hasMore ? std::to_string(next) : "";
        return result;
    }

private:
    MessagePage page(const MessageQuery& query, bool searching) {
        MessagePage result;
        result.acknowledgement = acknowledged(searching ? "search-page" : "fetch-page");
        const std::size_t offset = query.cursor.empty()
            ? 0U : static_cast<std::size_t>(std::stoul(query.cursor));
        std::vector<DiscordMessage> filtered;
        for (const auto& value : messages) {
            if (value.channelId != query.channelId ||
                (!query.guildId.empty() && value.guildId != query.guildId) ||
                (!query.includeBots && value.isBot) ||
                (query.after && value.timestamp < *query.after) ||
                (query.before && value.timestamp > *query.before) ||
                (searching && value.content.find(query.searchText) == std::string::npos)) {
                continue;
            }
            filtered.push_back(value);
        }
        std::sort(filtered.begin(), filtered.end(), [](const DiscordMessage& left, const DiscordMessage& right) {
            return left.timestamp == right.timestamp ? left.id < right.id : left.timestamp < right.timestamp;
        });
        for (std::size_t index = offset;
             index < filtered.size() && result.messages.size() < query.limit; ++index) {
            result.messages.push_back(filtered[index]);
        }
        const std::size_t next = offset + result.messages.size();
        result.hasMore = next < filtered.size();
        result.nextCursor = result.hasMore ? std::to_string(next) : "";
        if (malformedPagination && result.hasMore) {
            result.nextCursor = query.cursor;
        }
        return result;
    }
};

class FakeMutationAdapter final : public DiscordMutationAdapter {
public:
    std::mutex mutex;
    std::vector<std::string> operations;
    std::atomic<int> calls{0};
    std::atomic<int> rateLimitsRemaining{0};
    DiscordOperationStatus finalStatus{DiscordOperationStatus::ACKNOWLEDGED};
    bool omitReceipt{false};
    bool throwOnCall{false};
    std::function<void()> reentrant;
    std::vector<DiscordMessage> restored;

    DiscordAcknowledgement warnUser(const std::string&, const std::string& userId,
                                     const std::string&, const std::string&) override {
        return respond("warn:" + userId);
    }
    DiscordAcknowledgement timeoutUser(const std::string&, const std::string& userId,
                                        int, const std::string&) override {
        return respond("timeout:" + userId);
    }
    DiscordAcknowledgement kickUser(const std::string&, const std::string& userId,
                                     const std::string&) override {
        return respond("kick:" + userId);
    }
    DiscordAcknowledgement banUser(const std::string&, const std::string& userId,
                                    const std::string&, int) override {
        return respond("ban:" + userId);
    }
    DiscordAcknowledgement deleteMessage(const std::string&,
                                           const std::string& messageId,
                                           const std::string&) override {
        return respond("delete:" + messageId);
    }
    DiscordAcknowledgement bulkDeleteMessages(
        const std::string& channelId, const std::vector<std::string>& ids,
        const std::string&) override {
        return respond("bulk:" + channelId + ":" + std::to_string(ids.size()));
    }
    DiscordAcknowledgement restoreMessages(
        const std::string& channelId,
        const std::vector<DiscordMessage>& messages) override {
        restored = messages;
        return respond("restore:" + channelId);
    }

private:
    DiscordAcknowledgement respond(const std::string& operation) {
        ++calls;
        if (reentrant) {
            reentrant();
        }
        if (throwOnCall) {
            throw std::runtime_error("fake mutation failure");
        }
        {
            std::lock_guard<std::mutex> lock(mutex);
            operations.push_back(operation);
        }
        if (rateLimitsRemaining.load() > 0) {
            --rateLimitsRemaining;
            DiscordAcknowledgement result;
            result.status = DiscordOperationStatus::RATE_LIMITED;
            result.retryAfter = 1ms;
            return result;
        }
        DiscordAcknowledgement result;
        result.status = finalStatus;
        if (finalStatus == DiscordOperationStatus::ACKNOWLEDGED && !omitReceipt) {
            result.receiptId = "receipt-" + std::to_string(calls.load());
        }
        return result;
    }
};

class DiscrubExtTest : public ::testing::Test {
protected:
    void SetUp() override {
        root = std::filesystem::temp_directory_path() / "elizaos-discrub-test";
        std::error_code error;
        std::filesystem::remove_all(root, error);
        ASSERT_TRUE(std::filesystem::create_directories(root));
    }
    void TearDown() override {
        std::error_code error;
        std::filesystem::remove_all(root, error);
    }
    std::filesystem::path root;
};

TEST_F(DiscrubExtTest, FetchSearchPaginationAndFiltersRequireEvidence) {
    const auto now = std::chrono::system_clock::now();
    auto data = std::make_shared<FakeDataAdapter>();
    data->messages = {
        message("900000000000000003", "needle later", now - 1min),
        message("900000000000000001", "needle first", now - 3min),
        message("900000000000000002", "other", now - 2min, true)};
    ContentCleaner cleaner(data, nullptr);

    MessageQuery query;
    query.guildId = kGuild;
    query.channelId = kChannel;
    query.limit = 2;
    query.includeBots = false;
    query.after = now - 4min;
    auto first = cleaner.fetchMessages(query);
    ASSERT_TRUE(first.acknowledgement.acknowledged());
    ASSERT_EQ(first.messages.size(), 2U);
    EXPECT_EQ(first.messages[0].id, "900000000000000001");
    EXPECT_TRUE(first.hasMore == false);

    query.searchText = "needle";
    query.limit = 1;
    auto search = cleaner.searchMessages(query);
    ASSERT_TRUE(search.acknowledgement.acknowledged());
    ASSERT_EQ(search.messages.size(), 1U);
    EXPECT_TRUE(search.hasMore);
    query.cursor = search.nextCursor;
    search = cleaner.searchMessages(query);
    ASSERT_TRUE(search.acknowledgement.acknowledged());
    EXPECT_EQ(search.messages.front().id, "900000000000000003");

    data->malformedPagination = true;
    query.cursor.clear();
    search = cleaner.searchMessages(query);
    EXPECT_EQ(search.acknowledgement.status, DiscordOperationStatus::ADAPTER_ERROR);
    EXPECT_TRUE(search.messages.empty());
}

TEST_F(DiscrubExtTest, LocalClassificationHasExplicitReasonsAndNoRemoteClaim) {
    ContentScanner scanner;
    const auto clean = scanner.scanContent("A calm ordinary message");
    EXPECT_TRUE(clean.validInput);
    EXPECT_FALSE(clean.violation);

    auto phishing = scanner.scanContent(
        "Free nitro, click here to claim https://suspicious-site.com/gift");
    EXPECT_TRUE(phishing.validInput);
    EXPECT_TRUE(phishing.violation);
    EXPECT_EQ(phishing.recommendedAction, FilterAction::DELETE);
    EXPECT_FALSE(phishing.evidence.empty());
    EXPECT_NE(phishing.reason.find("local classification"), std::string::npos);

    auto tooLong = scanner.scanContent(std::string(DISCRUB_MAX_CONTENT_LENGTH + 1, 'x'));
    EXPECT_FALSE(tooLong.validInput);
    EXPECT_FALSE(tooLong.violation);
}

TEST_F(DiscrubExtTest, DeleteBanAcknowledgeOnlyWithReceiptsAndCommitState) {
    auto mutation = std::make_shared<FakeMutationAdapter>();
    AutoModerator moderator(mutation);
    moderator.setGuildContext(kGuild);
    moderator.setActionCooldown(0);

    auto deletion = moderator.deleteMessageAcknowledged(
        kChannel, "900000000000000001", "confirmed violation");
    EXPECT_TRUE(deletion.acknowledged());
    EXPECT_FALSE(deletion.receiptId.empty());

    auto ban = moderator.banUserAcknowledged(kGuild, kUser, "repeated abuse", 3);
    EXPECT_TRUE(ban.acknowledged());
    const auto reputation = moderator.getUserReputation(kUser);
    EXPECT_EQ(reputation.banCount, 1);
    const auto actions = moderator.getUserActions(kUser);
    ASSERT_EQ(actions.size(), 1U);
    EXPECT_EQ(actions[0].id, "action-000000000002");

    mutation->finalStatus = DiscordOperationStatus::PERMISSION_DENIED;
    EXPECT_FALSE(moderator.kickUser(kUser, "denied"));
    EXPECT_EQ(moderator.getUserReputation(kUser).kickCount, 0);
}

TEST_F(DiscrubExtTest, RetriesRateLimitsAndMalformedAcknowledgements) {
    auto mutation = std::make_shared<FakeMutationAdapter>();
    mutation->rateLimitsRemaining = 2;
    AutoModerator moderator(mutation);
    moderator.setRetryPolicy(3, 2ms);
    auto result = moderator.banUserAcknowledged(kGuild, kUser, "retry case", 0);
    EXPECT_TRUE(result.acknowledged());
    EXPECT_EQ(result.attempts, 3);

    mutation->omitReceipt = true;
    result = moderator.banUserAcknowledged(kGuild, kUser, "missing receipt", 0);
    EXPECT_EQ(result.status, DiscordOperationStatus::ADAPTER_ERROR);

    mutation->omitReceipt = false;
    mutation->throwOnCall = true;
    moderator.setRetryPolicy(1, 0ms);
    result = moderator.banUserAcknowledged(kGuild, kUser, "throw case", 0);
    EXPECT_EQ(result.status, DiscordOperationStatus::ADAPTER_ERROR);
}

TEST_F(DiscrubExtTest, InvalidInputAndNoAdapterRejectRemoteActions) {
    AutoModerator moderator;
    EXPECT_EQ(moderator.banUserAcknowledged(kGuild, kUser, "reason", 0).status,
              DiscordOperationStatus::NO_ADAPTER);
    EXPECT_EQ(moderator.banUserAcknowledged("bad", kUser, "reason", 0).status,
              DiscordOperationStatus::INVALID_INPUT);
    EXPECT_EQ(moderator.timeoutUserAcknowledged(kGuild, kUser, 0, "reason").status,
              DiscordOperationStatus::INVALID_INPUT);

    ContentCleaner cleaner;
    MessageQuery query;
    query.channelId = kChannel;
    EXPECT_EQ(cleaner.fetchMessages(query).acknowledgement.status,
              DiscordOperationStatus::NO_ADAPTER);
    EXPECT_EQ(cleaner.bulkDeleteMessagesAcknowledged(
                  kChannel, {"900000000000000001"}).status,
              DiscordOperationStatus::NO_ADAPTER);
    query.limit = DISCRUB_MAX_PAGE_SIZE + 1;
    EXPECT_EQ(cleaner.fetchMessages(query).acknowledgement.status,
              DiscordOperationStatus::INVALID_INPUT);
}

TEST_F(DiscrubExtTest, CleanupCountsOnlyAcknowledgedDeletes) {
    const auto now = std::chrono::system_clock::now();
    auto data = std::make_shared<FakeDataAdapter>();
    data->messages = {
        message("900000000000000001", "free nitro click here to claim", now - 1min),
        message("900000000000000002", "      ", now - 2min),
        message("900000000000000003", "ordinary", now - 3min)};
    auto mutation = std::make_shared<FakeMutationAdapter>();
    ContentCleaner cleaner(data, mutation);
    CleanupConfig config;
    config.deleteDuplicates = false;
    auto result = cleaner.cleanChannel(kChannel, config);
    EXPECT_TRUE(result.acknowledged());
    EXPECT_EQ(result.messagesScanned, 3);
    EXPECT_EQ(result.messagesDeleted, 2);
    EXPECT_EQ(result.spamRemoved, 1);
    EXPECT_EQ(result.emptyRemoved, 1);

    mutation->finalStatus = DiscordOperationStatus::PERMISSION_DENIED;
    result = cleaner.cleanChannel(kChannel, config);
    EXPECT_FALSE(result.acknowledged());
    EXPECT_EQ(result.messagesDeleted, 0);
    EXPECT_EQ(result.spamRemoved, 0);
}

TEST_F(DiscrubExtTest, ArchiveRestoreAndReportsArePathConfined) {
    const auto now = std::chrono::system_clock::now();
    auto data = std::make_shared<FakeDataAdapter>();
    data->messages = {message("900000000000000001", "evidence", now - 1min)};
    auto mutation = std::make_shared<FakeMutationAdapter>();
    ContentCleaner cleaner(data, mutation);
    ASSERT_TRUE(cleaner.setStorageRoot(root.string()));

    auto archive = cleaner.archiveChannelAcknowledged(kChannel, "archive.json");
    ASSERT_TRUE(archive.acknowledged());
    EXPECT_TRUE(std::filesystem::is_regular_file(root / "archive.json"));
    auto restore = cleaner.restoreFromArchiveAcknowledged(kChannel, "archive.json");
    EXPECT_TRUE(restore.acknowledged());
    ASSERT_EQ(mutation->restored.size(), 1U);
    EXPECT_EQ(mutation->restored[0].content, "evidence");
    EXPECT_EQ(cleaner.archiveChannelAcknowledged(kChannel, "../escape.json").status,
              DiscordOperationStatus::INVALID_INPUT);

    ModerationAnalytics analytics;
    ASSERT_TRUE(analytics.setStorageRoot(root.string()));
    ModerationAction action;
    action.id = "action-000000000001";
    action.guildId = kGuild;
    action.userId = kUser;
    action.channelId = kChannel;
    action.action = FilterAction::BAN;
    action.reason = "<unsafe>";
    action.timestamp = now;
    action.acknowledgement = acknowledged("receipt");
    analytics.recordAction(action);
    const auto report = analytics.generateReport(now - 1h, now + 1s);
    EXPECT_EQ(report.totalActions, 1);
    EXPECT_EQ(report.bansIssued, 1);
    const auto json = analytics.exportReportToFileAcknowledged(report, "report.json");
    EXPECT_TRUE(json.acknowledged());
    EXPECT_EQ(analytics.exportReportToFileAcknowledged(
                  report, "../escape.json").status,
              DiscordOperationStatus::INVALID_INPUT);
    EXPECT_NE(analytics.exportReportAsHtml(report).find("&lt;unsafe&gt;"),
              std::string::npos);
}

TEST_F(DiscrubExtTest, ConcurrentAndReentrantCallsDoNotRunAdaptersUnderLocks) {
    auto mutation = std::make_shared<FakeMutationAdapter>();
    AutoModerator moderator(mutation);
    moderator.setGuildContext(kGuild);
    moderator.setActionCooldown(0);
    mutation->reentrant = [&moderator] {
        (void)moderator.getUserReputation(kUser);
    };

    std::vector<std::thread> workers;
    for (int index = 0; index < 20; ++index) {
        workers.emplace_back([&moderator, index] {
            const std::string id = std::to_string(900000000000000000ULL +
                                                  static_cast<unsigned long long>(index));
            EXPECT_TRUE(moderator.deleteMessage(kChannel, id, "concurrent"));
        });
    }
    for (auto& worker : workers) {
        worker.join();
    }
    EXPECT_EQ(mutation->calls.load(), 20);
    const auto actions = moderator.getChannelActions(kChannel);
    ASSERT_EQ(actions.size(), 20U);
    EXPECT_EQ(actions.front().id, "action-000000000001");
    EXPECT_EQ(actions.back().id, "action-000000000020");
}

TEST_F(DiscrubExtTest, ExtensionCallbacksAreOutsideLocksAndIsolated) {
    auto mutation = std::make_shared<FakeMutationAdapter>();
    DiscrubExtension extension;
    ASSERT_TRUE(extension.initializeWithAdapters(nullptr, mutation));
    extension.getModerator().setActionCooldown(0);
    std::atomic<int> actionCallbacks{0};
    extension.setActionHandler([&](const ModerationAction&) {
        ++actionCallbacks;
        extension.setCleanupHandler({});
        throw std::runtime_error("isolated callback failure");
    });
    auto result = extension.getModerator().banUserAcknowledged(
        kGuild, kUser, "callback test", 0);
    EXPECT_TRUE(result.acknowledged());
    EXPECT_EQ(actionCallbacks.load(), 1);
    const auto report = extension.getAnalytics().generateDailyReport();
    EXPECT_EQ(report.bansIssued, 1);
}

TEST_F(DiscrubExtTest, SchedulingHasDeterministicRegistryAndCleanTeardown) {
    for (int iteration = 0; iteration < 50; ++iteration) {
        ContentCleaner cleaner;
        CleanupConfig config;
        cleaner.scheduleCleanup(kChannelTwo, config, 1h);
        cleaner.scheduleCleanup(kChannel, config, 1h);
        EXPECT_EQ(cleaner.getScheduledCleanups(),
                  (std::vector<std::string>{kChannel, kChannelTwo}));
        cleaner.cancelScheduledCleanup(kChannel);
        EXPECT_EQ(cleaner.getScheduledCleanups(),
                  (std::vector<std::string>{kChannelTwo}));
    }
}

TEST_F(DiscrubExtTest, ConfigurationIsBoundedJsonAndConfined) {
    DiscrubExtension extension;
    ASSERT_TRUE(extension.setStorageRoot(root.string()));
    extension.setDefaultModerationSettings();
    EXPECT_TRUE(extension.saveConfigurationAcknowledged("discrub.json").acknowledged());
    EXPECT_TRUE(extension.loadConfigurationAcknowledged("discrub.json").acknowledged());
    EXPECT_EQ(extension.saveConfigurationAcknowledged("../escape.json").status,
              DiscordOperationStatus::INVALID_INPUT);
    {
        std::ofstream malformed(root / "malformed.json");
        malformed << "not json";
    }
    EXPECT_EQ(extension.loadConfigurationAcknowledged("malformed.json").status,
              DiscordOperationStatus::PARSE_ERROR);
}

} // namespace
} // namespace elizaos
