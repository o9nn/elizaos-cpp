#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/awesome_eliza.hpp>.
#include <elizaos/awesome_eliza.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, awesome_eliza_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
