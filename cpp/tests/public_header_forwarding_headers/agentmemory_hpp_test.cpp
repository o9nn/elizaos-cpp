#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/agentmemory.hpp>.
#include <elizaos/agentmemory.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, agentmemory_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
