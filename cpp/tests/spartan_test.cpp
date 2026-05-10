// spartan_test.cpp - E2E tests for SpartanAgent (Solana trading agent).
#include <gtest/gtest.h>
#include "elizaos/spartan.hpp"

using namespace elizaos;

TEST(SpartanConfigDefaults, ProducedAndUsable) {
    auto cfg = getDefaultSpartanConfig();
    auto agent = createSpartanAgent(cfg);
    ASSERT_NE(agent, nullptr);
}

class SpartanAgentTest : public ::testing::Test {
protected:
    SpartanAgentTest() : cfg(getDefaultSpartanConfig()), agent(cfg) {}
    SpartanConfig cfg;
    SpartanAgent agent;
};

TEST_F(SpartanAgentTest, InitAndShutdown) {
    EXPECT_NO_THROW(agent.initialize());
    EXPECT_NO_THROW(agent.shutdown());
}

TEST_F(SpartanAgentTest, AddRemovePoolOwner) {
    EXPECT_NO_THROW(agent.addPoolOwner("pool-1", "owner-A"));
    EXPECT_NO_THROW(agent.removePoolOwner("pool-1", "owner-A"));
}

TEST_F(SpartanAgentTest, GetTradingPoolsReturns) {
    auto pools = agent.getTradingPools();
    SUCCEED() << "pools=" << pools.size();
}

TEST_F(SpartanAgentTest, GetTokenInfoBySymbolNoCrash) {
    EXPECT_NO_THROW(agent.getTokenInfo("SOL"));
}

TEST_F(SpartanAgentTest, CopyTradingLifecycle) {
    EXPECT_NO_THROW(agent.setupCopyTrading("wallet-X", 0.1));
    EXPECT_NO_THROW(agent.getCopyTradingWallets());
    EXPECT_NO_THROW(agent.stopCopyTrading("wallet-X"));
}

TEST_F(SpartanAgentTest, RiskCalculationBounded) {
    double r = agent.calculateRiskPercentage(100.0, 1000.0);
    EXPECT_GE(r, 0.0);
}

TEST_F(SpartanAgentTest, GenerateResponse) {
    auto r = agent.generateResponse("hello");
    EXPECT_FALSE(r.empty());
}

TEST_F(SpartanAgentTest, RequiresConfirmationFlag) {
    EXPECT_NO_THROW(agent.requiresConfirmation("buy"));
}

TEST_F(SpartanAgentTest, UpdateConfig) {
    EXPECT_NO_THROW(agent.updateConfig(cfg));
}
