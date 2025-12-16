#include "database.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class TokenRegistryService {
  async registerToken(params: {
    symbol: string;
    name: string;
    contractAddress: string;
    chain: Chain;
    decimals: number;
    logoUrl?: string;
    description?: string;
    website?: string;
    twitter?: string;
  }): Promise<Token> {
    // EVM addresses can be lowercased (case-insensitive)
    // Solana addresses are Base58 encoded and MUST preserve case
    const normalizedAddress =
      params.chain === "solana"
        ? params.contractAddress
        : params.contractAddress.toLowerCase();

    const token = await TokenDB.createToken({
      symbol: params.symbol.toUpperCase(),
      name: params.name,
      contractAddress: normalizedAddress,
      chain: params.chain,
      decimals: params.decimals,
      logoUrl: params.logoUrl || "",
      description: params.description || "",
      website: params.website,
      twitter: params.twitter,
      isActive: true,
    });
    return token;
  }


} // namespace elizaos
