#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/hats.hpp>.
#include <elizaos/hats.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, hats_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
