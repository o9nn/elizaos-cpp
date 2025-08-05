#include "elizaos/autofun_idl.hpp"
#include <iostream>
#include <iomanip>

using namespace elizaos::autofun_idl;

void demonstrateAutofunIdl() {
    std::cout << "=== AutoFun IDL C++ Implementation Demo ===" << std::endl;
    std::cout << std::endl;
    
    // 1. Display metadata
    std::cout << "Program Metadata:" << std::endl;
    std::cout << "  Name: " << metadata::NAME << std::endl;
    std::cout << "  Version: " << metadata::VERSION << std::endl;
    std::cout << "  Address: " << metadata::PROGRAM_ADDRESS << std::endl;
    std::cout << "  Description: " << metadata::DESCRIPTION << std::endl;
    std::cout << std::endl;
    
    // 2. Create and configure a Config struct
    std::cout << "Creating Config structure:" << std::endl;
    Config config;
    config.authority = "9WzDXwBbmkg8ZTbNMqUxvQRAyrZzDsGYdLVL9zYtAWWM";
    config.pending_authority = "11111111111111111111111111111111";
    config.team_wallet = "So11111111111111111111111111111111111111112";
    config.init_bonding_curve = 0.85;
    config.platform_buy_fee = 1000000; // 1%
    config.platform_sell_fee = 1000000; // 1%
    config.curve_limit = 1000000000; // 1B tokens
    
    std::cout << "  Authority: " << config.authority << std::endl;
    std::cout << "  Team Wallet: " << config.team_wallet << std::endl;
    std::cout << "  Bonding Curve Factor: " << config.init_bonding_curve << std::endl;
    std::cout << "  Buy Fee: " << static_cast<uint64_t>(config.platform_buy_fee) << std::endl;
    std::cout << "  Sell Fee: " << static_cast<uint64_t>(config.platform_sell_fee) << std::endl;
    std::cout << "  Curve Limit: " << config.curve_limit << std::endl;
    std::cout << std::endl;
    
    // 3. JSON serialization
    std::cout << "Serializing Config to JSON:" << std::endl;
    std::string config_json = IdlProcessor::configToJson(config);
    std::cout << config_json << std::endl;
    std::cout << std::endl;
    
    // 4. JSON deserialization
    std::cout << "Deserializing Config from JSON:" << std::endl;
    Config parsed_config = IdlProcessor::parseConfigFromJson(config_json);
    std::cout << "  Parsed Authority: " << parsed_config.authority << std::endl;
    std::cout << "  Parsed Curve Factor: " << parsed_config.init_bonding_curve << std::endl;
    std::cout << std::endl;
    
    // 5. Create a BondingCurve
    std::cout << "Creating BondingCurve structure:" << std::endl;
    BondingCurve curve;
    curve.token_mint = "EPjFWdd5AufqSSqeM2qN1xzybapC8G4wEGGkZwyTDt1v";
    curve.creator = "9WzDXwBbmkg8ZTbNMqUxvQRAyrZzDsGYdLVL9zYtAWWM";
    curve.init_lamport = 1000000000; // 1 SOL
    curve.reserve_lamport = 5000000000; // 5 SOL  
    curve.reserve_token = 800000000; // 800M tokens
    curve.curve_limit = 1000000000; // 1B tokens
    curve.is_completed = false;
    
    std::cout << "  Token Mint: " << curve.token_mint << std::endl;
    std::cout << "  Creator: " << curve.creator << std::endl;
    std::cout << "  Initial Lamports: " << curve.init_lamport << std::endl;
    std::cout << "  Reserve Lamports: " << curve.reserve_lamport << std::endl;
    std::cout << "  Reserve Tokens: " << curve.reserve_token << std::endl;
    std::cout << "  Completed: " << (curve.is_completed ? "Yes" : "No") << std::endl;
    std::cout << std::endl;
    
    // 6. Binary serialization
    std::cout << "Binary serialization test:" << std::endl;
    std::vector<u8> binary_data = IdlProcessor::serializeBondingCurve(curve);
    std::cout << "  Serialized size: " << binary_data.size() << " bytes" << std::endl;
    
    BondingCurve deserialized_curve = IdlProcessor::deserializeBondingCurve(binary_data);
    std::cout << "  Deserialized token mint: " << deserialized_curve.token_mint << std::endl;
    std::cout << "  Deserialized creator: " << deserialized_curve.creator << std::endl;
    std::cout << std::endl;
    
    // 7. AmountConfig examples
    std::cout << "AmountConfig examples:" << std::endl;
    
    // Range config
    AmountConfig<u64> range_config(1000000ULL, 100000000ULL); // 1M to 100M
    std::cout << "  Range config (1M-100M): " << (range_config.isRange() ? "Range" : "Enum") << std::endl;
    if (range_config.isRange()) {
        auto range = range_config.getRange();
        std::cout << "    Min: " << range.min << ", Max: " << range.max << std::endl;
    }
    
    // Enum config
    std::vector<u8> decimals = {6, 8, 9, 18};
    AmountConfig<u8> enum_config(decimals);
    std::cout << "  Enum config (decimals): " << (enum_config.isEnum() ? "Enum" : "Range") << std::endl;
    if (enum_config.isEnum()) {
        auto enum_vals = enum_config.getEnum();
        std::cout << "    Values: ";
        for (size_t i = 0; i < enum_vals.values.size(); ++i) {
            std::cout << static_cast<int>(enum_vals.values[i]);
            if (i < enum_vals.values.size() - 1) std::cout << ", ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    
    // 8. Instruction building
    std::cout << "Building instructions:" << std::endl;
    
    // Accept authority instruction
    std::string new_admin = "DjVE6JNiYqPL2QXyCUUh8rNjHrbz9hXHNYt99MQ59qw1";
    std::string global_config = "11111111111111111111111111111111";
    
    Instruction accept_instr = IdlProcessor::buildAcceptAuthorityInstruction(new_admin, global_config);
    std::cout << "  Accept Authority Instruction:" << std::endl;
    std::cout << "    Discriminator: [";
    for (size_t i = 0; i < accept_instr.discriminator.size(); ++i) {
        std::cout << static_cast<int>(accept_instr.discriminator[i]);
        if (i < accept_instr.discriminator.size() - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
    std::cout << "    Accounts: " << accept_instr.accounts.size() << std::endl;
    for (size_t i = 0; i < accept_instr.accounts.size(); ++i) {
        const auto& acc = accept_instr.accounts[i];
        std::cout << "      " << i << ": " << acc.pubkey 
                 << " (signer: " << (acc.is_signer ? "yes" : "no")
                 << ", writable: " << (acc.is_writable ? "yes" : "no") << ")" << std::endl;
    }
    std::cout << std::endl;
    
    // 9. Discriminator utilities
    std::cout << "Instruction discriminators:" << std::endl;
    std::vector<std::string> instruction_names = {
        "accept_authority", "configure", "launch", "launch_and_swap", 
        "nominate_authority", "swap", "withdraw"
    };
    
    for (const auto& name : instruction_names) {
        try {
            auto disc = IdlProcessor::getInstructionDiscriminator(name);
            std::cout << "  " << name << ": [";
            for (size_t i = 0; i < disc.size(); ++i) {
                std::cout << static_cast<int>(disc[i]);
                if (i < disc.size() - 1) std::cout << ", ";
            }
            std::cout << "]" << std::endl;
        } catch (const IdlException& e) {
            std::cout << "  " << name << ": Error - " << e.what() << std::endl;
        }
    }
    std::cout << std::endl;
    
    // 10. Pubkey validation
    std::cout << "Pubkey validation tests:" << std::endl;
    std::vector<std::string> test_pubkeys = {
        "9WzDXwBbmkg8ZTbNMqUxvQRAyrZzDsGYdLVL9zYtAWWM",  // Valid
        "So11111111111111111111111111111111111111112",       // Valid
        "invalid",                                             // Invalid - too short
        "waytoolongpubkeythatexceedsmaximumlength",           // Invalid - too long
        "0O1l"                                                // Invalid - bad chars
    };
    
    for (const auto& pubkey : test_pubkeys) {
        bool valid = IdlProcessor::validatePubkey(pubkey);
        std::cout << "  " << pubkey << ": " << (valid ? "Valid" : "Invalid") << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "=== Demo completed successfully! ===" << std::endl;
}

int main() {
    try {
        demonstrateAutofunIdl();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}