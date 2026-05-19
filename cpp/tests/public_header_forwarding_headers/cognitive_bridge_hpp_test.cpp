#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/cognitive_bridge.hpp>.
#include <elizaos/cognitive_bridge.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, cognitive_bridge_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
