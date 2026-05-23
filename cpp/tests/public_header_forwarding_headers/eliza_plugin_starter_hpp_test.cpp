#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/eliza_plugin_starter.hpp>.
#include <elizaos/eliza_plugin_starter.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, eliza_plugin_starter_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
