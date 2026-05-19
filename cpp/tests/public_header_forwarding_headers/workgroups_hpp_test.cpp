#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/workgroups.hpp>.
#include <elizaos/workgroups.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, workgroups_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
