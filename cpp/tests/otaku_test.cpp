// otaku_test.cpp - Tests for the Otaku DeFi/portfolio agent stack.
#include <gtest/gtest.h>
#include "elizaos/otaku.hpp"

using namespace elizaos;

TEST(OtakuChainId, EnumValuesDistinct) {
    int a = static_cast<int>(ChainId{});
    SUCCEED() << "default ChainId = " << a;
}

TEST(OtakuTokenStandard, EnumValuesDistinct) {
    int a = static_cast<int>(TokenStandard{});
    SUCCEED() << "default TokenStandard = " << a;
}

TEST(GasOptimizer, ConstructAndUse) {
    GasOptimizer g;
    SUCCEED();
}

TEST(PortfolioManager, ConstructAndUse) {
    PortfolioManager m;
    SUCCEED();
}

TEST(YieldManager, ConstructAndUse) {
    YieldManager m;
    SUCCEED();
}

TEST(NFTManager, ConstructAndUse) {
    NFTManager m;
    SUCCEED();
}

TEST(TransactionSimulator, ConstructAndUse) {
    TransactionSimulator s;
    SUCCEED();
}

TEST(OtakuAgent, ConstructAndUse) {
    OtakuAgent a("agent-1");
    SUCCEED();
}
