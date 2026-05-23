#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/agentbrowser.hpp>.
#include <elizaos/agentbrowser.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, agentbrowser_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
