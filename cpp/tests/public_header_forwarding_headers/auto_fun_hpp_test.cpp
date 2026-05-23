#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/auto_fun.hpp>.
#include <elizaos/auto_fun.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, auto_fun_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
