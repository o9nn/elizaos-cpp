#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/embodiment.hpp>.
#include <elizaos/embodiment.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, embodiment_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
