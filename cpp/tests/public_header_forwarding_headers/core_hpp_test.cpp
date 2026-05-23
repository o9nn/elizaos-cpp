#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/core.hpp>.
#include <elizaos/core.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, core_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
