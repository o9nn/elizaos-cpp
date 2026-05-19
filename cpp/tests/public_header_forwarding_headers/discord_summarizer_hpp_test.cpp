#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/discord_summarizer.hpp>.
#include <elizaos/discord_summarizer.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, discord_summarizer_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
