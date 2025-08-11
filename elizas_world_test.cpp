#include <gtest/gtest.h>
#include "elizaos/elizas_world.hpp"

// Simple standalone test runner for elizas_world
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}