#include <gtest/gtest.h>
#include "elizaos/classified.hpp"
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>

using namespace elizaos;

class ClassifiedTest : public ::testing::Test {
protected:
    void SetUp() override {
        game = std::make_unique<ClassifiedGame>();
    }
    void TearDown() override {
        game.reset();
    }
    std::unique_ptr<ClassifiedGame> game;
};

TEST_F(ClassifiedTest, CanCreateGame) {
    EXPECT_TRUE(game != nullptr);
}

TEST_F(ClassifiedTest, CanInitialize) {
    EXPECT_TRUE(game->initialize());
}

TEST_F(ClassifiedTest, CanShutdown) {
    game->initialize();
    game->shutdown();
    // Should not crash
    SUCCEED();
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return testing::RUN_ALL_TESTS();
}
