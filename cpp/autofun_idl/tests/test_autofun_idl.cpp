#include <gtest/gtest.h>
#include "elizaos/autofun_idl.hpp"
#include <iostream>

using namespace elizaos::autofun_idl;

class AutofunIdlTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup test data
    }
};

TEST_F(AutofunIdlTest, ConfigStructCreation) {
    Config config;
    
    // Test default values
    EXPECT_EQ(config.init_bonding_curve, 0.0);
    EXPECT_EQ(config.platform_buy_fee, 0u);
    EXPECT_EQ(config.platform_sell_fee, 0u);
    EXPECT_EQ(config.curve_limit, 0u);
    EXPECT_TRUE(config.authority.empty());
    EXPECT_TRUE(config.pending_authority.empty());
    EXPECT_TRUE(config.team_wallet.empty());
}

TEST_F(AutofunIdlTest, BondingCurveStructCreation) {
    BondingCurve curve;
    
    // Test default values
    EXPECT_EQ(curve.init_lamport, 0u);
    EXPECT_EQ(curve.reserve_lamport, 0u);
    EXPECT_EQ(curve.reserve_token, 0u);
    EXPECT_EQ(curve.curve_limit, 0u);
    EXPECT_FALSE(curve.is_completed);
    EXPECT_TRUE(curve.token_mint.empty());
    EXPECT_TRUE(curve.creator.empty());
}

TEST_F(AutofunIdlTest, AmountConfigRangeCreation) {
    AmountConfig<u64> config(100ULL, 1000ULL);
    
    EXPECT_TRUE(config.isRange());
    EXPECT_FALSE(config.isEnum());
    
    auto range = config.getRange();
    EXPECT_EQ(range.min, 100ULL);
    EXPECT_EQ(range.max, 1000ULL);
}

TEST_F(AutofunIdlTest, AmountConfigEnumCreation) {
    std::vector<u64> values = {100, 200, 300, 400};
    AmountConfig<u64> config(values);
    
    EXPECT_FALSE(config.isRange());
    EXPECT_TRUE(config.isEnum());
    
    auto enum_config = config.getEnum();
    EXPECT_EQ(enum_config.values.size(), 4);
    EXPECT_EQ(enum_config.values[0], 100);
    EXPECT_EQ(enum_config.values[3], 400);
}

TEST_F(AutofunIdlTest, JsonSerialization) {
    Config config;
    config.authority = "test_authority";
    config.pending_authority = "test_pending";
    config.team_wallet = "test_wallet";
    config.init_bonding_curve = 1.5;
    config.platform_buy_fee = 100;
    config.platform_sell_fee = 200;
    config.curve_limit = 1000000;
    
    std::string json = IdlProcessor::configToJson(config);
    EXPECT_FALSE(json.empty());
    EXPECT_NE(json.find("test_authority"), std::string::npos);
    EXPECT_NE(json.find("test_pending"), std::string::npos);
    EXPECT_NE(json.find("test_wallet"), std::string::npos);
}

TEST_F(AutofunIdlTest, JsonDeserialization) {
    std::string json = R"({
        "authority": "test_auth",
        "pending_authority": "test_pending", 
        "team_wallet": "test_wallet",
        "init_bonding_curve": 2.5,
        "platform_buy_fee": 150,
        "platform_sell_fee": 250,
        "curve_limit": 2000000
    })";
    
    Config config = IdlProcessor::parseConfigFromJson(json);
    
    EXPECT_EQ(config.authority, "test_auth");
    EXPECT_EQ(config.pending_authority, "test_pending");
    EXPECT_EQ(config.team_wallet, "test_wallet");
    EXPECT_EQ(config.init_bonding_curve, 2.5);
    EXPECT_EQ(config.platform_buy_fee, 150u);
    EXPECT_EQ(config.platform_sell_fee, 250u);
    EXPECT_EQ(config.curve_limit, 2000000u);
}

TEST_F(AutofunIdlTest, BondingCurveJsonRoundTrip) {
    BondingCurve original;
    original.token_mint = "mint123";
    original.creator = "creator456";
    original.init_lamport = 1000;
    original.reserve_lamport = 2000;
    original.reserve_token = 3000;
    original.curve_limit = 4000;
    original.is_completed = true;
    
    std::string json = IdlProcessor::bondingCurveToJson(original);
    BondingCurve parsed = IdlProcessor::parseBondingCurveFromJson(json);
    
    EXPECT_EQ(original.token_mint, parsed.token_mint);
    EXPECT_EQ(original.creator, parsed.creator);
    EXPECT_EQ(original.init_lamport, parsed.init_lamport);
    EXPECT_EQ(original.reserve_lamport, parsed.reserve_lamport);
    EXPECT_EQ(original.reserve_token, parsed.reserve_token);
    EXPECT_EQ(original.curve_limit, parsed.curve_limit);
    EXPECT_EQ(original.is_completed, parsed.is_completed);
}

TEST_F(AutofunIdlTest, InstructionDiscriminators) {
    // Test known instruction discriminators
    EXPECT_EQ(IdlProcessor::getInstructionName(discriminators::ACCEPT_AUTHORITY), "accept_authority");
    EXPECT_EQ(IdlProcessor::getInstructionName(discriminators::CONFIGURE), "configure");
    EXPECT_EQ(IdlProcessor::getInstructionName(discriminators::LAUNCH), "launch");
    EXPECT_EQ(IdlProcessor::getInstructionName(discriminators::SWAP), "swap");
    
    // Test reverse mapping
    EXPECT_EQ(IdlProcessor::getInstructionDiscriminator("accept_authority"), discriminators::ACCEPT_AUTHORITY);
    EXPECT_EQ(IdlProcessor::getInstructionDiscriminator("configure"), discriminators::CONFIGURE);
    EXPECT_EQ(IdlProcessor::getInstructionDiscriminator("launch"), discriminators::LAUNCH);
    EXPECT_EQ(IdlProcessor::getInstructionDiscriminator("swap"), discriminators::SWAP);
}

TEST_F(AutofunIdlTest, InstructionBuilding) {
    // Test accept authority instruction
    std::string new_admin = "new_admin_pubkey";
    std::string global_config = "global_config_pubkey";
    
    Instruction instr = IdlProcessor::buildAcceptAuthorityInstruction(new_admin, global_config);
    
    EXPECT_EQ(instr.discriminator, discriminators::ACCEPT_AUTHORITY);
    EXPECT_EQ(instr.accounts.size(), 2);
    EXPECT_EQ(instr.accounts[0].pubkey, new_admin);
    EXPECT_TRUE(instr.accounts[0].is_signer);
    EXPECT_TRUE(instr.accounts[0].is_writable);
    EXPECT_EQ(instr.accounts[1].pubkey, global_config);
    EXPECT_FALSE(instr.accounts[1].is_signer);
    EXPECT_TRUE(instr.accounts[1].is_writable);
}

TEST_F(AutofunIdlTest, PubkeyValidation) {
    // Test valid pubkey format
    EXPECT_TRUE(IdlProcessor::validatePubkey("11111111111111111111111111111111"));
    EXPECT_TRUE(IdlProcessor::validatePubkey("So11111111111111111111111111111111111111112"));
    
    // Test invalid pubkey formats
    EXPECT_FALSE(IdlProcessor::validatePubkey(""));
    EXPECT_FALSE(IdlProcessor::validatePubkey("short"));
    EXPECT_FALSE(IdlProcessor::validatePubkey("waytoolongpubkeythatexceedsmaximumlength"));
    EXPECT_FALSE(IdlProcessor::validatePubkey("0O1l")); // contains invalid base58 characters
}

TEST_F(AutofunIdlTest, BinarySerializationBasic) {
    BondingCurve curve;
    curve.token_mint = "test_mint";
    curve.creator = "test_creator";
    curve.init_lamport = 1000;
    curve.reserve_lamport = 2000;
    curve.reserve_token = 3000;
    curve.curve_limit = 4000;
    curve.is_completed = false;
    
    std::vector<u8> serialized = IdlProcessor::serializeBondingCurve(curve);
    EXPECT_FALSE(serialized.empty());
    
    BondingCurve deserialized = IdlProcessor::deserializeBondingCurve(serialized);
    EXPECT_EQ(curve.token_mint, deserialized.token_mint);
    EXPECT_EQ(curve.creator, deserialized.creator);
    EXPECT_EQ(curve.init_lamport, deserialized.init_lamport);
    EXPECT_EQ(curve.reserve_lamport, deserialized.reserve_lamport);
    EXPECT_EQ(curve.reserve_token, deserialized.reserve_token);
    EXPECT_EQ(curve.curve_limit, deserialized.curve_limit);
    EXPECT_EQ(curve.is_completed, deserialized.is_completed);
}

TEST_F(AutofunIdlTest, MetadataConstants) {
    EXPECT_STREQ(metadata::PROGRAM_ADDRESS, "autoUmixaMaYKFjexMpQuBpNYntgbkzCo2b1ZqUaAZ5");
    EXPECT_STREQ(metadata::NAME, "autofun");
    EXPECT_STREQ(metadata::VERSION, "0.1.0");
    EXPECT_STREQ(metadata::SPEC, "0.1.0");
    EXPECT_STREQ(metadata::DESCRIPTION, "Created with Anchor");
}