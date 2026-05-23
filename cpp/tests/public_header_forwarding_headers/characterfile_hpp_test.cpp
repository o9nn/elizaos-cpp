#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/characterfile.hpp>.
#include <elizaos/characterfile.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, characterfile_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
