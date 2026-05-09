/**
 * Custom main() for src/test_*.cpp files.
 *
 * These targets link against gtest for the test registry and headers, but keep a
 * local entry point instead of gtest_main so the helper can be shared uniformly
 * by all real-test translation units.
 */
#include <gtest/gtest.h>

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
