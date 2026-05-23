#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/elizas_world.hpp>.
#include <elizaos/elizas_world.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, elizas_world_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
