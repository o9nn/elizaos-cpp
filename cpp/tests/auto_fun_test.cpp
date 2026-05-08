// auto_fun_test.cpp - Tests for auto_fun client and config types.
#include <gtest/gtest.h>
#include "elizaos/auto_fun.hpp"

using namespace elizaos::auto_fun;

TEST(AutoFunConfig, Constructible) {
    Config c;
    SUCCEED();
}

TEST(AutoFunClient, DefaultConstructionNoCrash) {
    AutoFunClient client;
    SUCCEED();
}

TEST(AutoFunClient, ExplicitProgramAddressNoCrash) {
    AutoFunClient client("test-program-address");
    SUCCEED();
}

TEST(AmountConfig, RangeAndEnumTypes) {
    AmountConfig<uint64_t> r(AmountConfig<uint64_t>::Type::RANGE);
    EXPECT_EQ(r.type, AmountConfig<uint64_t>::Type::RANGE);
    AmountConfig<uint64_t> e(AmountConfig<uint64_t>::Type::ENUM);
    EXPECT_EQ(e.type, AmountConfig<uint64_t>::Type::ENUM);
}
