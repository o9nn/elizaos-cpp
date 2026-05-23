#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/evolutionary.hpp>.
#include <elizaos/evolutionary.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, evolutionary_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
