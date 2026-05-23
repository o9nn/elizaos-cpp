#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/otaku.hpp>.
#include <elizaos/otaku.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, otaku_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
