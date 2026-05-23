#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/agentloop.hpp>.
#include <elizaos/agentloop.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, agentloop_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
