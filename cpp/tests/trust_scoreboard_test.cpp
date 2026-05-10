// trust_scoreboard_test.cpp - Tests for the TrustScoreboard.
#include <gtest/gtest.h>
#include "elizaos/trust_scoreboard.hpp"

using namespace elizaos;

TEST(TrustEvent, ConstructionFromComponents) {
    TrustEvent e("agent-1", TrustEventType{}, TrustOutcome{}, 0.5);
    SUCCEED();
}

TEST(TrustScore, BasicConstruction) {
    TrustScore s;
    SUCCEED();
}

TEST(TrustConfig, BasicConstruction) {
    TrustConfig c;
    SUCCEED();
}

TEST(TrustDecayConfig, BasicConstruction) {
    TrustDecayConfig c;
    SUCCEED();
}

TEST(TrustScoreboard, ConstructWithDeps) {
    // TrustScoreboard requires a memory manager dependency. The point of this
    // test is that the constructor signature is reachable; we deliberately
    // pass nullptr so we can validate the binding without exercising memory.
    EXPECT_NO_THROW({
        TrustScoreboard t(nullptr, TrustConfig{});
        (void)t;
    });
}
