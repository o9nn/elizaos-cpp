#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/plugin_specification.hpp>.
#include <elizaos/plugin_specification.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, plugin_specification_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
