// discord_summarizer_test.cpp - Tests for Discord client/data/analyzer.
#include <gtest/gtest.h>
#include "elizaos/discord_summarizer.hpp"

using namespace elizaos;

TEST(DiscordMessage, BasicConstruction) {
    DiscordMessage m;
    EXPECT_TRUE(m.id.empty());
}

TEST(DiscordChannel, BasicConstruction) {
    DiscordChannel c;
    EXPECT_TRUE(c.id.empty());
}

TEST(DiscordGuild, BasicConstruction) {
    DiscordGuild g;
    EXPECT_TRUE(g.id.empty());
}

TEST(MessageAnalyzer, ConstructAndUse) {
    MessageAnalyzer a;
    SUCCEED();
}

TEST(ChannelSummarizer, ConstructAndUse) {
    ChannelSummarizer s;
    SUCCEED();
}

TEST(DiscordDataManager, ConstructAndUse) {
    DiscordDataManager m;
    SUCCEED();
}

TEST(DiscordSummarizer, ConstructAndUse) {
    DiscordSummarizer s;
    SUCCEED();
}
