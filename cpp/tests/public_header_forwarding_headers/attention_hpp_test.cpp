#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/attention.hpp>.
#include <elizaos/attention.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, attention_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
