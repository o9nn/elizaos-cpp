#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/ljspeechtools.hpp>.
#include <elizaos/ljspeechtools.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, ljspeechtools_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
