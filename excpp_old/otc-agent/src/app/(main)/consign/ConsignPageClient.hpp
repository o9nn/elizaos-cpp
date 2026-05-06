#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Solana imports

// Solana config

// Default gas deposit fallback (0.001 ETH) - used when RPC fetch fails

std::variant<"evm", "solana"> getRequiredChain(const std::string& tokenId);

std::future<Idl> fetchSolanaIdl();

// Extract chain and address from tokenId (format: token-{chain}-{address})
void getTokenInfo(const std::string& tokenId);


} // namespace elizaos
