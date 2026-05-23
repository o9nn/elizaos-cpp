#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/ontogenesis.hpp>.
#include <elizaos/ontogenesis.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, ontogenesis_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
