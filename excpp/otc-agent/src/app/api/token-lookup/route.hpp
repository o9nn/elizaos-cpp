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

;

// Codex GraphQL endpoint and Solana network ID
const CODEX_GRAPHQL_URL = "https://graph.codex.io/graphql";
const SOLANA_NETWORK_ID = 1399811149;

struct TokenInfo {
    std::string address;
    std::string symbol;
    std::string name;
    double decimals;
    string | null logoUrl;
    std::string chain;
    number | null priceUsd;
};


/**
 * Check if address looks like a Solana address (base58, 32-44 chars)
 */
$/;
  return base58Regex.test(address);
}

/**
 * Check if address looks like an EVM address (0x followed by 40 hex chars)
 */
$/.test(address);
}

/**
 * Look up Solana token via Codex API
 */
std::future<TokenInfo | null> lookupSolanaToken(const std::string& address, const std::string& codexKey);
      }
    }
  `;

  try {
    const response = await fetch(CODEX_GRAPHQL_URL, {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
        Authorization: codexKey,
      },
      body: JSON.stringify({
        query,
        variables: {
          input: {
            address,
            networkId: SOLANA_NETWORK_ID,
          },
        },
      }),
      signal: AbortSignal.timeout(8000),
    });

    if (!response.ok) return null;

    const data = await response.json();
    const token = data.data?.token;

    if (!token) return null;

    return {
      address: token.address,
      symbol: token.symbol || "SPL",
      name: token.name || "Unknown Token",
      decimals: token.decimals ?? 9,
      logoUrl: token.info?.imageSmallUrl || null,
      chain: "solana",
      priceUsd: null, // Could fetch price separately if needed
    };
  } catch (error) {
    console.error("[Token Lookup] Codex error:", error);
    return null;
  }
}

/**
 * Look up EVM token via Alchemy API
 */
std::future<TokenInfo | null> lookupEvmToken(const std::string& address, const std::string& chain, const std::string& alchemyKey);.g.alchemy.com/v2/${alchemyKey}`;

  try {
    const response = await fetch(url, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({
        jsonrpc: "2.0",
        id: 1,
        method: "alchemy_getTokenMetadata",
        params: [address],
      }),
      signal: AbortSignal.timeout(5000),
    });

    if (!response.ok) return null;

    const data = await response.json();
    const result = data.result;

    if (!result || !result.symbol) return null;

    return {
      address: address.toLowerCase(),
      symbol: result.symbol || "ERC20",
      name: result.name || "Unknown Token",
      decimals: result.decimals ?? 18,
      logoUrl: result.logo || null,
      chain,
      priceUsd: null,
    };
  } catch (error) {
    console.error("[Token Lookup] Alchemy error:", error);
    return null;
  }
}

/**
 * GET /api/token-lookup?address=0x...&chain=base
 * GET /api/token-lookup?address=So111...
 *
 * Looks up a single token by contract address.
 * Auto-detects chain if not provided for Solana addresses.
 */
async , { status: 400 });
  }

  // Auto-detect chain from address format
  const looksLikeSolana = isSolanaAddress(address);
  const looksLikeEvm = isEvmAddress(address);

  if (!looksLikeSolana && !looksLikeEvm) {
    return NextResponse.json(
      { error: "Invalid address format" },
      { status: 400 },
    );
  }

  // If chain not provided, infer from address
  if (!chain) {
    chain = looksLikeSolana ? "solana" : "base";
  }

  let token: TokenInfo | null = null;

  if (chain === "solana") {
    const codexKey = process.env.CODEX_API_KEY;
    if (!codexKey) {
      return NextResponse.json(
        { error: "Solana token lookup not configured" },
        { status: 503 },
      );
    }
    token = await lookupSolanaToken(address, codexKey);
  } else {
    const alchemyKey = process.env.ALCHEMY_API_KEY;
    if (!alchemyKey) {
      return NextResponse.json(
        { error: "EVM token lookup not configured" },
        { status: 503 },
      );
    }
    token = await lookupEvmToken(address, chain, alchemyKey);
  }

  if (!token) {
    return NextResponse.json(
      { error: "Token not found", address, chain },
      { status: 404 },
    );
  }

  return NextResponse.json({
    success: true,
    token,
  });
}

} // namespace elizaos
