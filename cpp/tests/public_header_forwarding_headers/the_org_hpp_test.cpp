#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/the_org.hpp>.
#include <elizaos/the_org.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, the_org_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
