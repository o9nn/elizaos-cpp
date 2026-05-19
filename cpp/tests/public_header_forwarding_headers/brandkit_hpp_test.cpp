#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/brandkit.hpp>.
#include <elizaos/brandkit.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, brandkit_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
