// eliza_starter_test.cpp - Tests for ElizaStarterAgent factory.
#include <gtest/gtest.h>
#include "elizaos/eliza_starter.hpp"

using namespace elizaos;

TEST(ElizaStarterAgent, FactoryProducesAgent) {
    auto a = createElizaStarterAgent("Eliza", "id-1");
    EXPECT_NE(a, nullptr);
}

TEST(ElizaStarterAgent, MultipleAgentsHaveDistinctIdentity) {
    auto a = createElizaStarterAgent("A", "1");
    auto b = createElizaStarterAgent("B", "2");
    EXPECT_NE(a.get(), b.get());
}
