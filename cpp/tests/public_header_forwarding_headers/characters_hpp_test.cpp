#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/characters.hpp>.
#include <elizaos/characters.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, characters_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
