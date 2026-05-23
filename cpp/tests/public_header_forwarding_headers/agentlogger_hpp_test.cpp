#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/agentlogger.hpp>.
#include <elizaos/agentlogger.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, agentlogger_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
