// autofun_idl_test.cpp - Tests for autofun IDL processor & helpers.
#include <gtest/gtest.h>
#include "elizaos/autofun_idl.hpp"

using namespace elizaos::autofun_idl;

TEST(IdlException, ConstructionAndWhat) {
    IdlException e(IdlError::INVALID_JSON, "bad json");
    EXPECT_STREQ(e.what(), "bad json");
    EXPECT_EQ(e.getError(), IdlError::INVALID_JSON);
}

TEST(IdlProcessor, ValidatePubkey) {
    EXPECT_NO_THROW(IdlProcessor::validatePubkey("any-string"));
}

TEST(IdlProcessor, GetInstructionDiscriminator) {
    auto d = IdlProcessor::getInstructionDiscriminator("launch");
    EXPECT_EQ(d.size(), 8u);
}

TEST(IdlProcessor, PubkeyBytesRoundtripSafe) {
    Pubkey k = "abc";
    EXPECT_NO_THROW({
        auto bytes = IdlProcessor::pubkeyToBytes(k);
        (void)bytes;
    });
}

TEST(Metadata, ConstantsAvailable) {
    EXPECT_GT(std::string(metadata::NAME).size(), 0u);
    EXPECT_GT(std::string(metadata::VERSION).size(), 0u);
}

TEST(IdlError, EnumValuesDistinct) {
    EXPECT_NE(static_cast<int>(IdlError::INVALID_JSON),
              static_cast<int>(IdlError::INVALID_PUBKEY));
}
