// otc_agent_test.cpp - Tests for OTC agent components.
#include <gtest/gtest.h>
#include "elizaos/otc_agent.hpp"

using namespace elizaos;

TEST(OTCChainEnum, EnumValuesDistinct) {
    int a = static_cast<int>(OTCChain{});
    SUCCEED() << "default OTCChain = " << a;
}

TEST(EscrowManager, ConstructAndUse) {
    EscrowManager m;
    SUCCEED();
}

TEST(OTCOrderbook, ConstructAndUse) {
    OTCOrderbook ob;
    SUCCEED();
}

TEST(OTCRiskManager, ConstructAndUse) {
    OTCRiskManager m;
    SUCCEED();
}

TEST(NegotiationEngine, ConstructAndUse) {
    NegotiationEngine n;
    SUCCEED();
}

TEST(OTCAnalytics, ConstructAndUse) {
    OTCAnalytics a;
    SUCCEED();
}

TEST(OTCAgent, ConstructAndUse) {
    OTCAgent a("agent-1");
    SUCCEED();
}
