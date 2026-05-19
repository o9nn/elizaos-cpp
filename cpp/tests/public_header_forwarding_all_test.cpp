#include <gtest/gtest.h>

// Aggregator translation unit for the generated per-header forwarding compile tests.
// Individual headers are compiled in isolated translation units under
// public_header_forwarding_headers/ to catch missing dependencies without
// manufacturing cross-header symbol collisions.

namespace {

TEST(PublicHeaderForwardingAllTest, GeneratedPerHeaderSourcesAreLinked) {
    SUCCEED();
}

} // namespace
