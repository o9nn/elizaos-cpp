#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/vercel_api.hpp>.
#include <elizaos/vercel_api.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, vercel_api_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
