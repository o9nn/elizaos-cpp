#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/classified.hpp>.
#include <elizaos/classified.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, classified_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
