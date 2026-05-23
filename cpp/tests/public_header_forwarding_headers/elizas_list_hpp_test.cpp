#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/elizas_list.hpp>.
#include <elizaos/elizas_list.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, elizas_list_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
