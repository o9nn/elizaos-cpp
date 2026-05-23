#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/easycompletion.hpp>.
#include <elizaos/easycompletion.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, easycompletion_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
