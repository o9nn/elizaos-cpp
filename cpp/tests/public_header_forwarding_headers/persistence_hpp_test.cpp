#include <gtest/gtest.h>
// Isolated public-header compile regression for <elizaos/persistence.hpp>.
#include <elizaos/persistence.hpp>
namespace {
TEST(PublicHeaderForwardingAllTest, persistence_hpp_CompilesInIsolation) {
    SUCCEED();
}
} // namespace
