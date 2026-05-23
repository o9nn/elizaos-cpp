#include <gtest/gtest.h>

// Isolated public-header compile regression for <elizaos/knowledge.hpp>.
#include <elizaos/knowledge.hpp>

namespace {

TEST(PublicHeaderForwardingAllTest, knowledge_hpp_CompilesInIsolation) {
    SUCCEED();
}

} // namespace
