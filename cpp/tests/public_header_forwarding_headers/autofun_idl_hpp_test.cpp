#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/autofun_idl.hpp>.
#include <elizaos/autofun_idl.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, autofun_idl_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
