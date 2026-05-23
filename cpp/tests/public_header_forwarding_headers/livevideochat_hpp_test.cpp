#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/livevideochat.hpp>.
#include <elizaos/livevideochat.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, livevideochat_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
