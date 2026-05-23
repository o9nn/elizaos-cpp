#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/agentshell.hpp>.
#include <elizaos/agentshell.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, agentshell_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
