// otaku_liquidity_test.cpp
//
// End-to-end tests for the completed OtakuAgent liquidity-position lifecycle.
// Previously getLiquidityPositions() returned a hard-coded empty vector; these
// tests lock in the real behavior: add -> track -> query, partial removal
// scaling, full removal, and unknown-position rejection.

#include <gtest/gtest.h>

#include "elizaos/otaku.hpp"

#include <algorithm>
#include <string>

using namespace elizaos;

namespace {

OtakuAgent makeAgent() {
    return OtakuAgent("otaku-liquidity-test");
}

TEST(OtakuLiquidity, AddLiquidityIsTrackedAndQueryable) {
    OtakuAgent agent = makeAgent();
    EXPECT_TRUE(agent.getLiquidityPositions().empty());

    LiquidityPosition pos =
        agent.addLiquidity("WETH", "USDC", 2.0, 4000.0, DexProtocol::UNISWAP_V3);

    EXPECT_FALSE(pos.positionId.empty());
    EXPECT_EQ(pos.token0, "WETH");
    EXPECT_EQ(pos.token1, "USDC");

    auto positions = agent.getLiquidityPositions();
    ASSERT_EQ(positions.size(), 1u);
    EXPECT_EQ(positions.front().positionId, pos.positionId);
    EXPECT_DOUBLE_EQ(positions.front().amount0, 2.0);
    EXPECT_DOUBLE_EQ(positions.front().amount1, 4000.0);
}

TEST(OtakuLiquidity, MultiplePositionsAreAllTracked) {
    OtakuAgent agent = makeAgent();
    agent.addLiquidity("WETH", "USDC", 1.0, 2000.0, DexProtocol::UNISWAP_V2);
    agent.addLiquidity("WBTC", "USDC", 0.1, 6000.0, DexProtocol::SUSHISWAP);
    agent.addLiquidity("DAI", "USDC", 500.0, 500.0, DexProtocol::CURVE);

    EXPECT_EQ(agent.getLiquidityPositions().size(), 3u);
}

TEST(OtakuLiquidity, PartialRemovalScalesPositionDown) {
    OtakuAgent agent = makeAgent();
    LiquidityPosition pos =
        agent.addLiquidity("WETH", "USDC", 4.0, 8000.0, DexProtocol::UNISWAP_V3);
    const double originalLp = pos.liquidityTokens;

    ASSERT_TRUE(agent.removeLiquidity(pos.positionId, 25.0));

    auto positions = agent.getLiquidityPositions();
    ASSERT_EQ(positions.size(), 1u);  // still present, just reduced
    const auto& p = positions.front();
    EXPECT_DOUBLE_EQ(p.amount0, 3.0);     // 4.0 * 0.75
    EXPECT_DOUBLE_EQ(p.amount1, 6000.0);  // 8000 * 0.75
    EXPECT_DOUBLE_EQ(p.liquidityTokens, originalLp * 0.75);
}

TEST(OtakuLiquidity, FullRemovalDeletesPosition) {
    OtakuAgent agent = makeAgent();
    LiquidityPosition pos =
        agent.addLiquidity("WETH", "USDC", 1.0, 2000.0, DexProtocol::BALANCER);

    ASSERT_TRUE(agent.removeLiquidity(pos.positionId, 100.0));
    EXPECT_TRUE(agent.getLiquidityPositions().empty());
}

TEST(OtakuLiquidity, RemovalDefaultsToFull) {
    OtakuAgent agent = makeAgent();
    LiquidityPosition pos =
        agent.addLiquidity("WETH", "USDC", 1.0, 2000.0, DexProtocol::UNISWAP_V3);

    ASSERT_TRUE(agent.removeLiquidity(pos.positionId));  // default percentage = 100
    EXPECT_TRUE(agent.getLiquidityPositions().empty());
}

TEST(OtakuLiquidity, RemovingUnknownPositionFails) {
    OtakuAgent agent = makeAgent();
    agent.addLiquidity("WETH", "USDC", 1.0, 2000.0, DexProtocol::UNISWAP_V3);

    EXPECT_FALSE(agent.removeLiquidity("does-not-exist", 50.0));
    EXPECT_EQ(agent.getLiquidityPositions().size(), 1u);  // unchanged
}

TEST(OtakuLiquidity, OverPercentageIsClampedToFullRemoval) {
    OtakuAgent agent = makeAgent();
    LiquidityPosition pos =
        agent.addLiquidity("WETH", "USDC", 1.0, 2000.0, DexProtocol::UNISWAP_V3);

    ASSERT_TRUE(agent.removeLiquidity(pos.positionId, 250.0));  // clamped to 100
    EXPECT_TRUE(agent.getLiquidityPositions().empty());
}

}  // namespace
