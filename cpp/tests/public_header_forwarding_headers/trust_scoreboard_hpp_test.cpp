#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/trust_scoreboard.hpp>.
#include <elizaos/trust_scoreboard.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, trust_scoreboard_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
