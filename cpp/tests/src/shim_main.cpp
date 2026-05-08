/**
 * Custom main() for src/test_*.cpp files that use the lightweight gtest shim
 * (include/gtest/gtest.h). We must NOT link against the FetchContent gtest_main
 * to avoid conflicting test registries.
 */
#include <gtest/gtest.h>

int main(int /*argc*/, char** /*argv*/) {
    return testing::RUN_ALL_TESTS();
}
