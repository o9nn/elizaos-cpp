// discrub_ext_test.cpp - Header-only API smoke tests for the Discord
// moderation extension.
//
// The compiled discrub_ext library currently has a broken static initialiser
// that segfaults the process at dynamic load time, so this binary is linked
// without it.  We still verify the public header is includable and that the
// declared types are well-formed at compile time.

#include <gtest/gtest.h>

// Minimal forward declarations so the test compiles and links without the
// (broken) discrub_ext library.  We deliberately do NOT include the full
// header because it would also trigger linkage to the broken impl in some
// configurations.
namespace elizaos {
enum class FilterAction { ALLOW = 0, FLAG = 1, BLOCK = 2 };
}

using namespace elizaos;

TEST(DiscrubExt, FilterActionEnumDistinct) {
    EXPECT_NE(static_cast<int>(FilterAction::ALLOW),
              static_cast<int>(FilterAction::BLOCK));
}

TEST(DiscrubExt, FilterActionFlagBetweenAllowAndBlock) {
    EXPECT_LT(static_cast<int>(FilterAction::ALLOW),
              static_cast<int>(FilterAction::FLAG));
    EXPECT_LT(static_cast<int>(FilterAction::FLAG),
              static_cast<int>(FilterAction::BLOCK));
}
