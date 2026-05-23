#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/discrub_ext.hpp>.
#include <elizaos/discrub_ext.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, discrub_ext_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
