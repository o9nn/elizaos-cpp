#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/elizaos.hpp>.
#include <elizaos/elizaos.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, elizaos_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
