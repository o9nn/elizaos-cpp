import { NextRequest, NextResponse } from "next/server";

// Codex GraphQL endpoint and Solana network ID
const CODEX_GRAPHQL_URL = "https://graph.codex.io/graphql";
const SOLANA_NETWORK_ID = 1399811149;

interface TokenInfo {
  address: string;
  symbol: string;
  name: string;
  decimals: number;
  logoUrl: string | null;
  chain: string;
  priceUsd: number | null;
}

/**
 * Check if address looks like a Solana address (base58, 32-44 chars)
 */
function isSolanaAddress(address: string): boolean {
  // Solana addresses are base58 encoded, typically 32-44 characters
  // They don't contain 0, I, O, l characters
  const base58Regex = /^[1-9A-HJ-NP-Za-km-z]{32,44}$/;
  return base58Regex.test(address);
}

/**
 * Check if address looks like an EVM address (0x followed by 40 hex chars)
 */
function isEvmAddress(address: string): boolean {
  return /^0x[a-fA-F0-9]{40}$/.test(address);
}

/**
 * Look up Solana token via Codex API
 */
async function lookupSolanaToken(
  address: string,
  codexKey: string,
): Promise<TokenInfo | null> {
  const query = `
    query GetToken($input: TokenInput!) {
      token(input: $input) {
        name
        symbol
        address
        decimals
        info {
          imageSmallUrl
        }
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
async function lookupEvmToken(
  address: string,
  chain: string,
  alchemyKey: string,
): Promise<TokenInfo | null> {
  const alchemyNetwork = chain === "bsc" ? "bnb-mainnet" : "base-mainnet";

  const url = `https://${alchemyNetwork}.g.alchemy.com/v2/${alchemyKey}`;

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
export async function GET(request: NextRequest) {
  const address = request.nextUrl.searchParams.get("address");
  let chain = request.nextUrl.searchParams.get("chain");

  if (!address) {
    return NextResponse.json({ error: "Address required" }, { status: 400 });
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
