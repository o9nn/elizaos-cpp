#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/registry.hpp>.
#include <elizaos/registry.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, registry_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
