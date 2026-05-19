#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/autonomous_starter.hpp>.
#include <elizaos/autonomous_starter.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, autonomous_starter_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
