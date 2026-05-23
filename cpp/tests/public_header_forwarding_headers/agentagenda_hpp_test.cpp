#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/agentagenda.hpp>.
#include <elizaos/agentagenda.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, agentagenda_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
