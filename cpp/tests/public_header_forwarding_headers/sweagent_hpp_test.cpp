#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/sweagent.hpp>.
#include <elizaos/sweagent.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, sweagent_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
