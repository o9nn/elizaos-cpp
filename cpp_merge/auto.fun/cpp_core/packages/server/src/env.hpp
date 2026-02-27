#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Environment interface for Cloudflare Workers
 */
struct Env {
    std: NETWORK;
    std: DECIMALS;
    std: TOKEN_SUPPLY;
    std: VIRTUAL_RESERVES;
    std: CURVE_LIMIT;
    std: EXECUTOR_PRIVATE_KEY;
    std: CODEX_API_KEY;
    std: API_URL;
    std: FAL_API_KEY;
    std: LOCAL_DEV;
    std: NODE_ENV;
    std: JWT_SECRET;
    std: RPC_URL;
    std: MAINNET_SOLANA_RPC_URL;
    std: DEVNET_SOLANA_RPC_URL;
    std: DEVNET_PROGRAM_ID;
    std: PROGRAM_ID;
    std: tokenPubkey;
    std: AUTH_TOKEN_SALT;
    std: DEVNET_FRONTEND_URL;
    std: MAINNET_FRONTEND_URL;
    std: TWITTER_BEARER_TOKEN;
    std: TWITTER_CLIENT_ID;
    std: TWITTER_API_KEY;
    std: TWITTER_API_SECRET;
    std: TWITTER_ACCESS_TOKEN_SECRET;
    std: TWITTER_ACCESS_TOKEN;
    std::optional<std:> NEWS_API_KEY;
    std::optional<std:> PREGENERATED_TOKENS_COUNT;
    std: HELIUS_WEBHOOK_AUTH_TOKEN;
    std::optional<std:> ADMIN_ADDRESSES;
    std: MANAGER_MULTISIG_ADDRESS;
    std: FIXED_FEE;
    std: FEE_WALLET;
    std: DATABASE_URL;
    std: REDIS_HOST;
    std: REDIS_PORT;
    std: REDIS_PASSWORD;
    std::optional<std:> ALLOWED_ORIGINS;
};


} // namespace elizaos
