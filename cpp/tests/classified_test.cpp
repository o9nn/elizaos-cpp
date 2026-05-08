// classified_test.cpp - E2E tests for elizaos::ClassifiedGame lifecycle.
#include <gtest/gtest.h>
#include "elizaos/classified.hpp"

using namespace elizaos;

TEST(ClassifiedGame, ConstructDoesNotThrow) {
    ClassifiedGame g;
    SUCCEED();
}

TEST(ClassifiedGame, InitializeReturnsTrue) {
    ClassifiedGame g;
    EXPECT_TRUE(g.initialize());
}

TEST(ClassifiedGame, RunGameAfterInit) {
    ClassifiedGame g;
    ASSERT_TRUE(g.initialize());
    EXPECT_NO_THROW(g.runGame());
}

TEST(ClassifiedGame, ShutdownIsIdempotent) {
    ClassifiedGame g;
    g.initialize();
    EXPECT_NO_THROW(g.shutdown());
    EXPECT_NO_THROW(g.shutdown());
}

TEST(ClassifiedGame, FullLifecycle) {
    {
        ClassifiedGame g;
        EXPECT_TRUE(g.initialize());
        EXPECT_NO_THROW(g.runGame());
        EXPECT_NO_THROW(g.shutdown());
    }
    SUCCEED();
}
