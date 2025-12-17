#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

struct TokenMetadata {
    std::string name;
    std::string symbol;
    std::string description;
    double initialSol;
    std::string twitter;
    std::string telegram;
    std::string farcaster;
    std::string website;
    std::string discord;
    std::optional<std::string> imageBase64;
    std::string tokenMint;
    double decimals;
    double supply;
    std::string freezeAuthority;
    std::string mintAuthority;
};


} // namespace elizaos
