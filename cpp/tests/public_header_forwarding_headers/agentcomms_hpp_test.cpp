#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/agentcomms.hpp>.
#include <elizaos/agentcomms.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, agentcomms_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
