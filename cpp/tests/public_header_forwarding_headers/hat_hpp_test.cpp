#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/hat.hpp>.
#include <elizaos/hat.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, hat_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
