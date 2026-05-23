#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/eliza_nextjs_starter.hpp>.
#include <elizaos/eliza_nextjs_starter.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, eliza_nextjs_starter_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
