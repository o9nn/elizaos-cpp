#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/elizaos_github_io.hpp>.
#include <elizaos/elizaos_github_io.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, elizaos_github_io_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
