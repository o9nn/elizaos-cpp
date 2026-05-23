#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/website.hpp>.
#include <elizaos/website.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, website_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
