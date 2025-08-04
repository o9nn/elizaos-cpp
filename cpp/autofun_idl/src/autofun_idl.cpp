#include "elizaos/autofun_idl.hpp"
#include <nlohmann/json.hpp>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <stdexcept>
#include <cstring>

namespace elizaos {
namespace autofun_idl {

using json = nlohmann::json;

// JSON parsing methods
Config IdlProcessor::parseConfigFromJson(const std::string& json_str) {
    try {
        json j = json::parse(json_str);
        Config config;
        
        config.authority = j.value("authority", "");
        config.pending_authority = j.value("pending_authority", "");
        config.team_wallet = j.value("team_wallet", "");
        config.init_bonding_curve = j.value("init_bonding_curve", 0.0);
        config.platform_buy_fee = j.value("platform_buy_fee", 0u);
        config.platform_sell_fee = j.value("platform_sell_fee", 0u);
        config.curve_limit = j.value("curve_limit", 0u);
        
        // Parse amount configs if present
        if (j.contains("lamport_amount_config")) {
            auto& lac = j["lamport_amount_config"];
            if (lac.contains("range")) {
                config.lamport_amount_config = AmountConfig<u64>(
                    lac["range"]["min"], lac["range"]["max"]
                );
            } else if (lac.contains("values")) {
                config.lamport_amount_config = AmountConfig<u64>(
                    lac["values"].get<std::vector<u64>>()
                );
            }
        }
        
        return config;
    } catch (const std::exception& e) {
        throw IdlException(IdlError::INVALID_JSON, 
            "Failed to parse Config from JSON: " + std::string(e.what()));
    }
}

BondingCurve IdlProcessor::parseBondingCurveFromJson(const std::string& json_str) {
    try {
        json j = json::parse(json_str);
        BondingCurve curve;
        
        curve.token_mint = j.value("token_mint", "");
        curve.creator = j.value("creator", "");
        curve.init_lamport = j.value("init_lamport", 0u);
        curve.reserve_lamport = j.value("reserve_lamport", 0u);
        curve.reserve_token = j.value("reserve_token", 0u);
        curve.curve_limit = j.value("curve_limit", 0u);
        curve.is_completed = j.value("is_completed", false);
        
        return curve;
    } catch (const std::exception& e) {
        throw IdlException(IdlError::INVALID_JSON, 
            "Failed to parse BondingCurve from JSON: " + std::string(e.what()));
    }
}

CompleteEvent IdlProcessor::parseCompleteEventFromJson(const std::string& json_str) {
    try {
        json j = json::parse(json_str);
        CompleteEvent event;
        
        event.user = j.value("user", "");
        event.mint = j.value("mint", "");
        event.bonding_curve = j.value("bonding_curve", "");
        
        return event;
    } catch (const std::exception& e) {
        throw IdlException(IdlError::INVALID_JSON, 
            "Failed to parse CompleteEvent from JSON: " + std::string(e.what()));
    }
}

// JSON serialization methods
std::string IdlProcessor::configToJson(const Config& config) {
    json j;
    j["authority"] = config.authority;
    j["pending_authority"] = config.pending_authority;
    j["team_wallet"] = config.team_wallet;
    j["init_bonding_curve"] = config.init_bonding_curve;
    j["platform_buy_fee"] = static_cast<uint64_t>(config.platform_buy_fee);
    j["platform_sell_fee"] = static_cast<uint64_t>(config.platform_sell_fee);
    j["curve_limit"] = config.curve_limit;
    
    // Serialize amount configs
    if (config.lamport_amount_config.isRange()) {
        auto range = config.lamport_amount_config.getRange();
        j["lamport_amount_config"]["range"]["min"] = range.min;
        j["lamport_amount_config"]["range"]["max"] = range.max;
    } else {
        auto enum_vals = config.lamport_amount_config.getEnum();
        j["lamport_amount_config"]["values"] = enum_vals.values;
    }
    
    return j.dump(2);
}

std::string IdlProcessor::bondingCurveToJson(const BondingCurve& curve) {
    json j;
    j["token_mint"] = curve.token_mint;
    j["creator"] = curve.creator;
    j["init_lamport"] = curve.init_lamport;
    j["reserve_lamport"] = curve.reserve_lamport;
    j["reserve_token"] = curve.reserve_token;
    j["curve_limit"] = curve.curve_limit;
    j["is_completed"] = curve.is_completed;
    
    return j.dump(2);
}

std::string IdlProcessor::completeEventToJson(const CompleteEvent& event) {
    json j;
    j["user"] = event.user;
    j["mint"] = event.mint;
    j["bonding_curve"] = event.bonding_curve;
    
    return j.dump(2);
}

// Binary serialization helpers (simplified Borsh-like format)
template<typename T>
void IdlProcessor::serializePrimitive(std::vector<u8>& buffer, const T& value) {
    const u8* bytes = reinterpret_cast<const u8*>(&value);
    buffer.insert(buffer.end(), bytes, bytes + sizeof(T));
}

void IdlProcessor::serializeString(std::vector<u8>& buffer, const std::string& str) {
    u32 len = static_cast<u32>(str.length());
    serializePrimitive(buffer, len);
    buffer.insert(buffer.end(), str.begin(), str.end());
}

void IdlProcessor::serializePubkey(std::vector<u8>& buffer, const Pubkey& pubkey) {
    // For simplicity, just serialize as string
    serializeString(buffer, pubkey);
}

// Binary serialization methods
std::vector<u8> IdlProcessor::serializeConfig(const Config& config) {
    std::vector<u8> buffer;
    
    serializePubkey(buffer, config.authority);
    serializePubkey(buffer, config.pending_authority);
    serializePubkey(buffer, config.team_wallet);
    serializePrimitive(buffer, config.init_bonding_curve);
    serializePrimitive(buffer, config.platform_buy_fee);
    serializePrimitive(buffer, config.platform_sell_fee);
    serializePrimitive(buffer, config.curve_limit);
    
    return buffer;
}

std::vector<u8> IdlProcessor::serializeBondingCurve(const BondingCurve& curve) {
    std::vector<u8> buffer;
    
    serializePubkey(buffer, curve.token_mint);
    serializePubkey(buffer, curve.creator);
    serializePrimitive(buffer, curve.init_lamport);
    serializePrimitive(buffer, curve.reserve_lamport);
    serializePrimitive(buffer, curve.reserve_token);
    serializePrimitive(buffer, curve.curve_limit);
    serializePrimitive(buffer, curve.is_completed);
    
    return buffer;
}

std::vector<u8> IdlProcessor::serializeCompleteEvent(const CompleteEvent& event) {
    std::vector<u8> buffer;
    
    serializePubkey(buffer, event.user);
    serializePubkey(buffer, event.mint);
    serializePubkey(buffer, event.bonding_curve);
    
    return buffer;
}

// Binary deserialization helpers
template<typename T>
T IdlProcessor::deserializePrimitive(const std::vector<u8>& data, size_t& offset) {
    if (offset + sizeof(T) > data.size()) {
        throw IdlException(IdlError::DESERIALIZATION_ERROR, 
            "Not enough data to deserialize primitive");
    }
    
    T value;
    std::memcpy(&value, data.data() + offset, sizeof(T));
    offset += sizeof(T);
    return value;
}

std::string IdlProcessor::deserializeString(const std::vector<u8>& data, size_t& offset) {
    u32 len = deserializePrimitive<u32>(data, offset);
    
    if (offset + len > data.size()) {
        throw IdlException(IdlError::DESERIALIZATION_ERROR, 
            "Not enough data to deserialize string");
    }
    
    std::string str(data.begin() + offset, data.begin() + offset + len);
    offset += len;
    return str;
}

Pubkey IdlProcessor::deserializePubkey(const std::vector<u8>& data, size_t& offset) {
    return deserializeString(data, offset);
}

// Binary deserialization methods
Config IdlProcessor::deserializeConfig(const std::vector<u8>& data) {
    Config config;
    size_t offset = 0;
    
    config.authority = deserializePubkey(data, offset);
    config.pending_authority = deserializePubkey(data, offset);
    config.team_wallet = deserializePubkey(data, offset);
    config.init_bonding_curve = deserializePrimitive<f64>(data, offset);
    config.platform_buy_fee = deserializePrimitive<u128>(data, offset);
    config.platform_sell_fee = deserializePrimitive<u128>(data, offset);
    config.curve_limit = deserializePrimitive<u64>(data, offset);
    
    return config;
}

BondingCurve IdlProcessor::deserializeBondingCurve(const std::vector<u8>& data) {
    BondingCurve curve;
    size_t offset = 0;
    
    curve.token_mint = deserializePubkey(data, offset);
    curve.creator = deserializePubkey(data, offset);
    curve.init_lamport = deserializePrimitive<u64>(data, offset);
    curve.reserve_lamport = deserializePrimitive<u64>(data, offset);
    curve.reserve_token = deserializePrimitive<u64>(data, offset);
    curve.curve_limit = deserializePrimitive<u64>(data, offset);
    curve.is_completed = deserializePrimitive<bool>(data, offset);
    
    return curve;
}

CompleteEvent IdlProcessor::deserializeCompleteEvent(const std::vector<u8>& data) {
    CompleteEvent event;
    size_t offset = 0;
    
    event.user = deserializePubkey(data, offset);
    event.mint = deserializePubkey(data, offset);
    event.bonding_curve = deserializePubkey(data, offset);
    
    return event;
}

// Instruction building methods
Instruction IdlProcessor::buildAcceptAuthorityInstruction(
    const Pubkey& new_admin, 
    const Pubkey& global_config) {
    
    Instruction instr(discriminators::ACCEPT_AUTHORITY);
    instr.accounts.emplace_back(new_admin, true, true);  // new_admin: signer, writable
    instr.accounts.emplace_back(global_config, false, true);  // global_config: writable
    
    return instr;
}

Instruction IdlProcessor::buildConfigureInstruction(
    const instruction_args::Configure& args,
    const Pubkey& payer,
    const Pubkey& config,
    const std::vector<Pubkey>& additional_accounts) {
    
    Instruction instr(discriminators::CONFIGURE);
    instr.accounts.emplace_back(payer, true, true);  // payer: signer, writable
    instr.accounts.emplace_back(config, false, true);  // config: writable
    
    // Add additional accounts
    for (const auto& account : additional_accounts) {
        instr.accounts.emplace_back(account, false, false);
    }
    
    // Serialize the config argument
    instr.data = serializeConfig(args.new_config);
    
    return instr;
}

Instruction IdlProcessor::buildNominateAuthorityInstruction(
    const instruction_args::NominateAuthority& args,
    const Pubkey& admin,
    const Pubkey& global_config) {
    
    Instruction instr(discriminators::NOMINATE_AUTHORITY);
    instr.accounts.emplace_back(admin, true, false);  // admin: signer
    instr.accounts.emplace_back(global_config, false, true);  // global_config: writable
    
    // Serialize the new_admin argument
    serializePubkey(instr.data, args.new_admin);
    
    return instr;
}

// Other instruction builders can be added similarly...

// Utility methods
Discriminator IdlProcessor::getInstructionDiscriminator(const std::string& instruction_name) {
    if (instruction_name == "accept_authority") return discriminators::ACCEPT_AUTHORITY;
    if (instruction_name == "configure") return discriminators::CONFIGURE;
    if (instruction_name == "launch") return discriminators::LAUNCH;
    if (instruction_name == "launch_and_swap") return discriminators::LAUNCH_AND_SWAP;
    if (instruction_name == "nominate_authority") return discriminators::NOMINATE_AUTHORITY;
    if (instruction_name == "swap") return discriminators::SWAP;
    if (instruction_name == "withdraw") return discriminators::WITHDRAW;
    
    throw IdlException(IdlError::UNKNOWN_INSTRUCTION, 
        "Unknown instruction: " + instruction_name);
}

std::string IdlProcessor::getInstructionName(const Discriminator& discriminator) {
    if (discriminator == discriminators::ACCEPT_AUTHORITY) return "accept_authority";
    if (discriminator == discriminators::CONFIGURE) return "configure";
    if (discriminator == discriminators::LAUNCH) return "launch";
    if (discriminator == discriminators::LAUNCH_AND_SWAP) return "launch_and_swap";
    if (discriminator == discriminators::NOMINATE_AUTHORITY) return "nominate_authority";
    if (discriminator == discriminators::SWAP) return "swap";
    if (discriminator == discriminators::WITHDRAW) return "withdraw";
    
    throw IdlException(IdlError::UNKNOWN_INSTRUCTION, "Unknown discriminator");
}

bool IdlProcessor::validatePubkey(const std::string& pubkey) {
    // Basic validation - should be base58 encoded and around 32-44 characters
    if (pubkey.empty() || pubkey.length() < 32 || pubkey.length() > 44) {
        return false;
    }
    
    // Check for valid base58 characters
    const std::string base58_chars = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
    return std::all_of(pubkey.begin(), pubkey.end(), 
        [&](char c) { return base58_chars.find(c) != std::string::npos; });
}

std::vector<u8> IdlProcessor::pubkeyToBytes(const Pubkey& pubkey) {
    // Simplified implementation - in practice would use base58 decoding
    std::vector<u8> bytes(pubkey.begin(), pubkey.end());
    if (bytes.size() < 32) {
        bytes.resize(32, 0);
    }
    return bytes;
}

Pubkey IdlProcessor::bytesToPubkey(const std::vector<u8>& bytes) {
    // Simplified implementation - in practice would use base58 encoding
    return std::string(bytes.begin(), bytes.end());
}

// Explicit template instantiations
template void IdlProcessor::serializePrimitive<u8>(std::vector<u8>&, const u8&);
template void IdlProcessor::serializePrimitive<u16>(std::vector<u8>&, const u16&);
template void IdlProcessor::serializePrimitive<u32>(std::vector<u8>&, const u32&);
template void IdlProcessor::serializePrimitive<u64>(std::vector<u8>&, const u64&);
template void IdlProcessor::serializePrimitive<u128>(std::vector<u8>&, const u128&);
template void IdlProcessor::serializePrimitive<f64>(std::vector<u8>&, const f64&);
template void IdlProcessor::serializePrimitive<bool>(std::vector<u8>&, const bool&);

template u8 IdlProcessor::deserializePrimitive<u8>(const std::vector<u8>&, size_t&);
template u16 IdlProcessor::deserializePrimitive<u16>(const std::vector<u8>&, size_t&);
template u32 IdlProcessor::deserializePrimitive<u32>(const std::vector<u8>&, size_t&);
template u64 IdlProcessor::deserializePrimitive<u64>(const std::vector<u8>&, size_t&);
template u128 IdlProcessor::deserializePrimitive<u128>(const std::vector<u8>&, size_t&);
template f64 IdlProcessor::deserializePrimitive<f64>(const std::vector<u8>&, size_t&);
template bool IdlProcessor::deserializePrimitive<bool>(const std::vector<u8>&, size_t&);

} // namespace autofun_idl
} // namespace elizaos
