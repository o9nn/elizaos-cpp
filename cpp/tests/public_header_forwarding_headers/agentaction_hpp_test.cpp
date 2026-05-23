#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/agentaction.hpp>.
#include <elizaos/agentaction.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, agentaction_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
