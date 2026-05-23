#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/spartan.hpp>.
#include <elizaos/spartan.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, spartan_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
