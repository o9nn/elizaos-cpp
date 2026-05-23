#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/comprehensive_e2e.hpp>.
#include <elizaos/comprehensive_e2e.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, comprehensive_e2e_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
