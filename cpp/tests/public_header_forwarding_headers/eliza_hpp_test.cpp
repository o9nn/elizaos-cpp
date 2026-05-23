#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/eliza.hpp>.
#include <elizaos/eliza.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, eliza_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
