#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <variant>
#include <optional>
#include <array>
#include <cstdint>

namespace elizaos {
namespace autofun_idl {

// Type aliases matching Solana/Anchor conventions
using Pubkey = std::string;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using u128 = __uint128_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;
using f32 = float;
using f64 = double;

// Discriminator type for instruction identification
using Discriminator = std::array<u8, 8>;

/**
 * Generic AmountConfig type matching the IDL
 * Can be either Range or Enum variants
 */
template<typename T>
struct AmountConfig {
    enum class Variant {
        RANGE,
        ENUM
    };
    
    struct Range {
        T min;
        T max;
    };
    
    struct Enum {
        std::vector<T> values;
    };
    
    Variant variant;
    std::variant<Range, Enum> data;
    
    // Constructors for Range variant
    AmountConfig(T min, T max) 
        : variant(Variant::RANGE), data(Range{min, max}) {}
    
    // Constructor for Enum variant
    AmountConfig(const std::vector<T>& values) 
        : variant(Variant::ENUM), data(Enum{values}) {}
    
    // Helper methods
    bool isRange() const { return variant == Variant::RANGE; }
    bool isEnum() const { return variant == Variant::ENUM; }
    
    const Range& getRange() const { return std::get<Range>(data); }
    const Enum& getEnum() const { return std::get<Enum>(data); }
};

/**
 * BondingCurve account structure
 */
struct BondingCurve {
    Pubkey token_mint;
    Pubkey creator;
    u64 init_lamport;
    u64 reserve_lamport;
    u64 reserve_token;
    u64 curve_limit;
    bool is_completed;
    
    BondingCurve() 
        : init_lamport(0), reserve_lamport(0), reserve_token(0), 
          curve_limit(0), is_completed(false) {}
};

/**
 * CompleteEvent structure
 */
struct CompleteEvent {
    Pubkey user;
    Pubkey mint;
    Pubkey bonding_curve;
};

/**
 * Config account structure
 */
struct Config {
    Pubkey authority;
    Pubkey pending_authority;
    Pubkey team_wallet;
    f64 init_bonding_curve;
    u128 platform_buy_fee;
    u128 platform_sell_fee;
    u64 curve_limit;
    AmountConfig<u64> lamport_amount_config;
    AmountConfig<u64> token_supply_config;
    AmountConfig<u8> token_decimals_config;
    
    Config()
        : init_bonding_curve(0.0), platform_buy_fee(0), platform_sell_fee(0), 
          curve_limit(0), lamport_amount_config(0ULL, UINT64_MAX), 
          token_supply_config(0ULL, UINT64_MAX), token_decimals_config(static_cast<u8>(0), static_cast<u8>(18)) {}
};

/**
 * Instruction discriminators
 */
namespace discriminators {
    constexpr Discriminator ACCEPT_AUTHORITY = {107, 86, 198, 91, 33, 12, 107, 160};
    constexpr Discriminator CONFIGURE = {245, 7, 108, 117, 95, 196, 54, 217};
    constexpr Discriminator LAUNCH = {153, 241, 93, 225, 22, 69, 74, 61};
    constexpr Discriminator LAUNCH_AND_SWAP = {67, 201, 190, 15, 185, 41, 47, 122};
    constexpr Discriminator NOMINATE_AUTHORITY = {148, 182, 144, 91, 186, 12, 118, 18};
    constexpr Discriminator SWAP = {248, 198, 158, 145, 225, 117, 135, 200};
    constexpr Discriminator WITHDRAW = {183, 18, 70, 156, 148, 109, 161, 34};
}

/**
 * Instruction account types
 */
struct AccountMeta {
    Pubkey pubkey;
    bool is_signer;
    bool is_writable;
    
    AccountMeta(const Pubkey& pk, bool signer = false, bool writable = false)
        : pubkey(pk), is_signer(signer), is_writable(writable) {}
};

/**
 * Instruction arguments for each instruction type
 */
namespace instruction_args {
    
    struct Configure {
        Config new_config;
    };
    
    struct Launch {
        u8 decimals;
        u64 token_supply;
        u64 virtual_lamport_reserves;
        std::string name;
        std::string symbol;
        std::string uri;
    };
    
    struct LaunchAndSwap {
        u8 decimals;
        u64 token_supply;
        u64 virtual_lamport_reserves;
        std::string name;
        std::string symbol;
        std::string uri;
        u64 swap_amount;
        u64 minimum_receive_amount;
        i64 deadline;
    };
    
    struct NominateAuthority {
        Pubkey new_admin;
    };
    
    struct Swap {
        u64 amount;
        u8 direction;  // 0 = buy, 1 = sell
        u64 minimum_receive_amount;
        i64 deadline;
    };
    
} // namespace instruction_args

/**
 * Generic instruction wrapper
 */
struct Instruction {
    Discriminator discriminator;
    std::vector<AccountMeta> accounts;
    std::vector<u8> data;
    
    Instruction(const Discriminator& disc) : discriminator(disc) {}
};

/**
 * IDL parser and serializer class
 */
class IdlProcessor {
public:
    // JSON parsing methods
    static Config parseConfigFromJson(const std::string& json);
    static BondingCurve parseBondingCurveFromJson(const std::string& json);
    static CompleteEvent parseCompleteEventFromJson(const std::string& json);
    
    // JSON serialization methods
    static std::string configToJson(const Config& config);
    static std::string bondingCurveToJson(const BondingCurve& curve);
    static std::string completeEventToJson(const CompleteEvent& event);
    
    // Binary serialization methods (Borsh format)
    static std::vector<u8> serializeConfig(const Config& config);
    static std::vector<u8> serializeBondingCurve(const BondingCurve& curve);
    static std::vector<u8> serializeCompleteEvent(const CompleteEvent& event);
    
    // Binary deserialization methods
    static Config deserializeConfig(const std::vector<u8>& data);
    static BondingCurve deserializeBondingCurve(const std::vector<u8>& data);
    static CompleteEvent deserializeCompleteEvent(const std::vector<u8>& data);
    
    // Instruction building methods
    static Instruction buildAcceptAuthorityInstruction(
        const Pubkey& new_admin, 
        const Pubkey& global_config
    );
    
    static Instruction buildConfigureInstruction(
        const instruction_args::Configure& args,
        const Pubkey& payer,
        const Pubkey& config,
        const std::vector<Pubkey>& additional_accounts
    );
    
    static Instruction buildLaunchInstruction(
        const instruction_args::Launch& args,
        const std::vector<AccountMeta>& accounts
    );
    
    static Instruction buildLaunchAndSwapInstruction(
        const instruction_args::LaunchAndSwap& args,
        const std::vector<AccountMeta>& accounts
    );
    
    static Instruction buildNominateAuthorityInstruction(
        const instruction_args::NominateAuthority& args,
        const Pubkey& admin,
        const Pubkey& global_config
    );
    
    static Instruction buildSwapInstruction(
        const instruction_args::Swap& args,
        const std::vector<AccountMeta>& accounts
    );
    
    static Instruction buildWithdrawInstruction(
        const std::vector<AccountMeta>& accounts
    );
    
    // Utility methods
    static Discriminator getInstructionDiscriminator(const std::string& instruction_name);
    static std::string getInstructionName(const Discriminator& discriminator);
    static bool validatePubkey(const std::string& pubkey);
    static std::vector<u8> pubkeyToBytes(const Pubkey& pubkey);
    static Pubkey bytesToPubkey(const std::vector<u8>& bytes);
    
private:
    // Helper methods for binary serialization
    template<typename T>
    static void serializePrimitive(std::vector<u8>& buffer, const T& value);
    
    static void serializeString(std::vector<u8>& buffer, const std::string& str);
    static void serializePubkey(std::vector<u8>& buffer, const Pubkey& pubkey);
    
    template<typename T>
    static void serializeAmountConfig(std::vector<u8>& buffer, const AmountConfig<T>& config);
    
    // Helper methods for binary deserialization
    template<typename T>
    static T deserializePrimitive(const std::vector<u8>& data, size_t& offset);
    
    static std::string deserializeString(const std::vector<u8>& data, size_t& offset);
    static Pubkey deserializePubkey(const std::vector<u8>& data, size_t& offset);
    
    template<typename T>
    static AmountConfig<T> deserializeAmountConfig(const std::vector<u8>& data, size_t& offset);
};

/**
 * IDL metadata constants
 */
namespace metadata {
    constexpr char PROGRAM_ADDRESS[] = "autoUmixaMaYKFjexMpQuBpNYntgbkzCo2b1ZqUaAZ5";
    constexpr char NAME[] = "autofun";
    constexpr char VERSION[] = "0.1.0";
    constexpr char SPEC[] = "0.1.0";
    constexpr char DESCRIPTION[] = "Created with Anchor";
}

/**
 * Error handling for IDL operations
 */
enum class IdlError {
    INVALID_JSON,
    INVALID_BINARY_DATA,
    UNKNOWN_INSTRUCTION,
    INVALID_PUBKEY,
    SERIALIZATION_ERROR,
    DESERIALIZATION_ERROR
};

class IdlException : public std::exception {
private:
    IdlError error_;
    std::string message_;
    
public:
    IdlException(IdlError error, const std::string& message)
        : error_(error), message_(message) {}
    
    const char* what() const noexcept override {
        return message_.c_str();
    }
    
    IdlError getError() const { return error_; }
};

} // namespace autofun_idl
} // namespace elizaos