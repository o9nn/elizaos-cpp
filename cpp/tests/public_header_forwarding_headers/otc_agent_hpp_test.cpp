#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/otc_agent.hpp>.
#include <elizaos/otc_agent.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, otc_agent_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
