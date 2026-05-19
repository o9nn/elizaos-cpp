#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/mcp_gateway.hpp>.
#include <elizaos/mcp_gateway.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, mcp_gateway_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
