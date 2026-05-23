#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/plugins_automation.hpp>.
#include <elizaos/plugins_automation.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, plugins_automation_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
