#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/goal_manager.hpp>.
#include <elizaos/goal_manager.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, goal_manager_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
