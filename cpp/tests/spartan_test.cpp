// spartan_test.cpp - local-only safety and lifecycle tests for SpartanAgent.
#include <gtest/gtest.h>

#include "elizaos/spartan.hpp"

#include <algorithm>
#include <atomic>
#include <cmath>
#include <limits>
#include <set>
#include <string>
#include <thread>
#include <vector>

using namespace elizaos;

namespace {
constexpr const char* kOwnerA = "Owner11111111111111111111111111111111111";
constexpr const char* kOwnerB = "Owner22222222222222222222222222222222222";
constexpr const char* kWallet = "11111111111111111111111111111111";
} // namespace

TEST(SpartanConfigDefaults, SafeLocalOnlyAndNoSecretRetention) {
    auto cfg = getDefaultSpartanConfig();
    EXPECT_TRUE(cfg.solanaRpcUrl.empty());
    EXPECT_TRUE(cfg.solanaPrivateKey.empty());
    EXPECT_TRUE(cfg.requireConfirmation);

    cfg.solanaPrivateKey = "super-secret-never-retain";
    SpartanAgent agent(cfg);
    EXPECT_TRUE(agent.getConfig().solanaPrivateKey.empty());
    EXPECT_EQ(agent.generateResponse("hello").find("super-secret"), std::string::npos);
}

class SpartanLocalTest : public ::testing::Test {
protected:
    SpartanLocalTest() : cfg(getDefaultSpartanConfig()), agent(cfg) {}
    SpartanConfig cfg;
    SpartanAgent agent;
};

TEST_F(SpartanLocalTest, InitializationDistinguishesLocalFromExternalReadiness) {
    EXPECT_TRUE(agent.initialize());
    EXPECT_TRUE(agent.isInitialized());
    EXPECT_FALSE(agent.isExternallyConnected());
    const auto status = agent.getConnectionStatus();
    EXPECT_EQ(status.status, OperationStatus::Unsupported);
    EXPECT_NE(status.message.find("local bookkeeping is ready"), std::string::npos);

    agent.shutdown();
    EXPECT_FALSE(agent.isInitialized());
    EXPECT_FALSE(agent.isExternallyConnected());
    EXPECT_TRUE(agent.initialize());
    EXPECT_TRUE(agent.isInitialized());
}

TEST_F(SpartanLocalTest, DefaultModeNeverFabricatesExternalSuccessOrMarketData) {
    ASSERT_TRUE(agent.initialize());
    const auto token = agent.queryTokenInfo("SOL");
    EXPECT_EQ(token.status, OperationStatus::Unsupported);
    EXPECT_FALSE(token.verified());
    EXPECT_TRUE(agent.getTokenInfo("SOL").symbol.empty());
    EXPECT_TRUE(agent.getTopTokens().empty());

    const auto swap = agent.executeSwap("SOL", "USDC", 10.0, "Orca", true);
    EXPECT_EQ(swap.status, OperationStatus::Unsupported);
    EXPECT_FALSE(swap.success);
    EXPECT_TRUE(swap.transactionId.empty());

    const auto add = agent.addLiquidity("SOL", "USDC", 5.0, 5.0, "Orca", true);
    EXPECT_EQ(add.status, OperationStatus::Unsupported);
    EXPECT_FALSE(add.success);

    const auto copy = agent.setupCopyTradingResult(kWallet, 0.05, true);
    EXPECT_EQ(copy.status, OperationStatus::Unsupported);
    EXPECT_TRUE(agent.getCopyTradingWallets().empty());
}

TEST_F(SpartanLocalTest, PoolLifecycleIsDeterministicAndSnapshotsAreIsolated) {
    ASSERT_TRUE(agent.initialize());
    const std::string first = agent.createTradingPool("Alpha", {kOwnerB, kOwnerA});
    const std::string second = agent.createTradingPool("Beta", {kOwnerA});
    EXPECT_EQ(first, "pool_0000000000000001");
    EXPECT_EQ(second, "pool_0000000000000002");

    auto snapshot = agent.getTradingPoolSnapshot(first);
    ASSERT_TRUE(snapshot.has_value());
    EXPECT_EQ(snapshot->owners, (std::vector<std::string>{kOwnerA, kOwnerB}));
    snapshot->name = "mutated snapshot";
    EXPECT_EQ(agent.getTradingPoolSnapshot(first)->name, "Alpha");

    TradingPool* compatibility = agent.getTradingPool(first);
    ASSERT_NE(compatibility, nullptr);
    compatibility->name = "also isolated";
    EXPECT_EQ(agent.getTradingPoolSnapshot(first)->name, "Alpha");

    EXPECT_TRUE(agent.addPoolOwner(second, kOwnerB));
    EXPECT_FALSE(agent.addPoolOwner(second, kOwnerB));
    EXPECT_TRUE(agent.removePoolOwner(second, kOwnerB));
    EXPECT_FALSE(agent.removePoolOwner(second, kOwnerA));
    EXPECT_TRUE(agent.setPoolAllocation(first, "SOL", 0.6));
    EXPECT_TRUE(agent.setPoolAllocation(first, "USDC", 0.4));
    EXPECT_FALSE(agent.setPoolAllocation(first, "BONK", 0.01));
    EXPECT_TRUE(agent.deleteTradingPool(second));
    EXPECT_FALSE(agent.getTradingPoolSnapshot(second).has_value());
}

TEST_F(SpartanLocalTest, RejectsMalformedPoolsAmountsRiskAndDexes) {
    ASSERT_TRUE(agent.initialize());
    EXPECT_TRUE(agent.createTradingPool("", {kOwnerA}).empty());
    EXPECT_TRUE(agent.createTradingPool("bad owners", {"has whitespace"}).empty());
    EXPECT_TRUE(agent.createTradingPool("duplicates", {kOwnerA, kOwnerA}).empty());

    EXPECT_FALSE(agent.validateTrade("", "USDC", 1.0));
    EXPECT_FALSE(agent.validateTrade("SOL", "SOL", 1.0));
    EXPECT_FALSE(agent.validateTrade("SOL", "USDC", std::numeric_limits<double>::quiet_NaN()));
    EXPECT_FALSE(agent.validateTrade("SOL", "USDC", std::numeric_limits<double>::infinity()));
    EXPECT_FALSE(agent.validateTrade("SOL", "USDC", 1001.0));
    EXPECT_TRUE(std::isinf(agent.calculateRiskPercentage(10.0, 0.0)));

    const auto badDex = agent.executeSwap("SOL", "USDC", 1.0, "UnknownDex", true);
    EXPECT_EQ(badDex.status, OperationStatus::Rejected);
    const auto malformed = agent.addLiquidity("SOL", "USDC", -1.0, 2.0, "Orca", true);
    EXPECT_EQ(malformed.status, OperationStatus::Rejected);
}

TEST_F(SpartanLocalTest, ConfirmationPolicyRejectsBeforeAdapterBoundary) {
    ASSERT_TRUE(agent.initialize());
    EXPECT_TRUE(agent.requiresConfirmation("execute swap"));
    EXPECT_TRUE(agent.requiresConfirmation("copy trade"));
    EXPECT_FALSE(agent.requiresConfirmation("get token price"));

    const auto swap = agent.executeSwap("SOL", "USDC", 1.0, "Orca");
    EXPECT_EQ(swap.status, OperationStatus::Rejected);
    EXPECT_NE(swap.message.find("confirmation"), std::string::npos);
}

TEST_F(SpartanLocalTest, ConcurrentLocalAccessMaintainsDeterministicSafeState) {
    ASSERT_TRUE(agent.initialize());
    const std::string poolId = agent.createTradingPool("Concurrent", {kOwnerA});
    ASSERT_FALSE(poolId.empty());

    std::atomic<int> successfulAdds{0};
    std::vector<std::thread> threads;
    for (int i = 0; i < 24; ++i) {
        threads.emplace_back([&, i] {
            const std::string owner = "worker_" + std::to_string(i);
            if (agent.addPoolOwner(poolId, owner)) {
                ++successfulAdds;
            }
            for (int read = 0; read < 100; ++read) {
                EXPECT_TRUE(agent.getTradingPoolSnapshot(poolId).has_value());
                EXPECT_FALSE(agent.getTradingPools().empty());
            }
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }
    EXPECT_EQ(successfulAdds.load(), 24);
    const auto pool = agent.getTradingPoolSnapshot(poolId);
    ASSERT_TRUE(pool.has_value());
    EXPECT_EQ(pool->owners.size(), 25U);
    EXPECT_TRUE(std::is_sorted(pool->owners.begin(), pool->owners.end()));
}

TEST_F(SpartanLocalTest, ResponsesContainNoFabricatedCurrentMarketClaims) {
    ASSERT_TRUE(agent.initialize());
    for (const char* query : {"BONK price", "current market", "Orca APR", "add liquidity"}) {
        const std::string response = agent.generateResponse(query);
        EXPECT_EQ(response.find("12.4"), std::string::npos);
        EXPECT_EQ(response.find("180.50"), std::string::npos);
        EXPECT_EQ(response.find("Current SOL"), std::string::npos);
        EXPECT_EQ(response.find("Current BONK"), std::string::npos);
    }
    TokenInfo unverified("SOL", "So11111111111111111111111111111111111111112");
    unverified.price = 999.0;
    EXPECT_EQ(agent.formatTokenPrice(unverified),
              "unsupported: no verified market snapshot is available");
}

TEST_F(SpartanLocalTest, InvalidConfigurationFailsAndUpdateRequiresShutdown) {
    SpartanConfig invalid = getDefaultSpartanConfig();
    invalid.maxRiskPercentage = 2.0;
    SpartanAgent invalidAgent(invalid);
    EXPECT_FALSE(invalidAgent.initialize());

    invalid = getDefaultSpartanConfig();
    invalid.solanaRpcUrl = "https://";
    EXPECT_FALSE(SpartanAgent(invalid).initialize());
    invalid.solanaRpcUrl = "https://user:pass@example.invalid";
    EXPECT_FALSE(SpartanAgent(invalid).initialize());
    invalid = getDefaultSpartanConfig();
    invalid.solanaPublicKey = "not-a-solana-address";
    EXPECT_FALSE(SpartanAgent(invalid).initialize());
    invalid = getDefaultSpartanConfig();
    invalid.preferredDexes = {"Orca", "orca"};
    EXPECT_FALSE(SpartanAgent(invalid).initialize());
    invalid.preferredDexes = {"bad dex"};
    EXPECT_FALSE(SpartanAgent(invalid).initialize());

    ASSERT_TRUE(agent.initialize());
    SpartanConfig changed = getDefaultSpartanConfig();
    changed.maxTradeAmount = 42.0;
    EXPECT_FALSE(agent.updateConfig(changed));
    agent.shutdown();
    EXPECT_TRUE(agent.updateConfig(changed));
    EXPECT_DOUBLE_EQ(agent.getConfig().maxTradeAmount, 42.0);
}
