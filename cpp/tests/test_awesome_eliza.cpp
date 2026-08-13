#include <gtest/gtest.h>
#include "elizaos/awesome_eliza.hpp"

class AwesomeElizaTest : public ::testing::Test {
protected:
    void SetUp() override {
        manager = std::make_unique<elizaos::AwesomeElizaManager>();
    }

    std::unique_ptr<elizaos::AwesomeElizaManager> manager;
};

TEST_F(AwesomeElizaTest, InitializationTest) {
    EXPECT_GT(manager->getResourceCount(), 0);
}

TEST_F(AwesomeElizaTest, OfficialResourcesTest) {
    auto official = manager->getOfficialResources();
    EXPECT_GT(official.size(), 0);
    
    // Should contain the main elizaOS GitHub repository
    bool found_main_repo = false;
    for (const auto& resource : official) {
        if (resource.name == "elizaOS GitHub Repository") {
            found_main_repo = true;
            EXPECT_EQ(resource.url, "https://github.com/elizaos/eliza");
            EXPECT_EQ(resource.category, "official");
            break;
        }
    }
    EXPECT_TRUE(found_main_repo);
}

TEST_F(AwesomeElizaTest, TutorialsTest) {
    auto tutorials = manager->getTutorials();
    EXPECT_GT(tutorials.size(), 0);
    
    // Should contain the quickstart guide
    bool found_quickstart = false;
    for (const auto& resource : tutorials) {
        if (resource.name == "Quickstart Guide") {
            found_quickstart = true;
            EXPECT_EQ(resource.category, "tutorials");
            break;
        }
    }
    EXPECT_TRUE(found_quickstart);
}

TEST_F(AwesomeElizaTest, PluginSubcategoriesTest) {
    auto blockchain_plugins = manager->getBlockchainPlugins();
    auto trading_plugins = manager->getTradingPlugins();
    auto ai_plugins = manager->getAIPlugins();
    
    EXPECT_GT(blockchain_plugins.size(), 0);
    EXPECT_GT(trading_plugins.size(), 0);
    EXPECT_GT(ai_plugins.size(), 0);
    
    // Check that blockchain plugins have correct subcategory
    for (const auto& plugin : blockchain_plugins) {
        EXPECT_EQ(plugin.subcategory, "blockchain");
        EXPECT_EQ(plugin.category, "plugins");
    }
}

TEST_F(AwesomeElizaTest, SearchFunctionalityTest) {
    auto solana_results = manager->searchResources("solana");
    EXPECT_GT(solana_results.size(), 0);
    
    auto github_results = manager->searchResources("github");
    EXPECT_GT(github_results.size(), 0);
    
    // Search should be case insensitive
    auto GitHub_results = manager->searchResources("GitHub");
    EXPECT_EQ(github_results.size(), GitHub_results.size());
}

TEST_F(AwesomeElizaTest, CategoryCountsTest) {
    auto counts = manager->getCategoryCounts();
    
    EXPECT_GT(counts["official"], 0);
    EXPECT_GT(counts["tutorials"], 0);
    EXPECT_GT(counts["tools"], 0);
    EXPECT_GT(counts["clients"], 0);
    EXPECT_GT(counts["adapters"], 0);
    EXPECT_GT(counts["plugins"], 0);
    EXPECT_GT(counts["community"], 0);
    EXPECT_GT(counts["videos"], 0);
    EXPECT_GT(counts["research"], 0);
    
    // Total should match individual counts
    size_t total_from_counts = 0;
    for (const auto& pair : counts) {
        total_from_counts += pair.second;
    }
    EXPECT_EQ(total_from_counts, manager->getResourceCount());
}

TEST_F(AwesomeElizaTest, ClientsAndAdaptersTest) {
    auto clients = manager->getClients();
    auto adapters = manager->getAdapters();
    
    EXPECT_GT(clients.size(), 0);
    EXPECT_GT(adapters.size(), 0);
    
    // Check for specific client
    bool found_discord = false;
    for (const auto& client : clients) {
        if (client.name == "Discord") {
            found_discord = true;
            EXPECT_EQ(client.category, "clients");
            break;
        }
    }
    EXPECT_TRUE(found_discord);
    
    // Check for specific adapter
    bool found_postgres = false;
    for (const auto& adapter : adapters) {
        if (adapter.name == "PostgreSQL") {
            found_postgres = true;
            EXPECT_EQ(adapter.category, "adapters");
            break;
        }
    }
    EXPECT_TRUE(found_postgres);
}