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

/**
 * Environment interface for Cloudflare Workers
 */
struct Env {
    std::string NETWORK;
    std::string DECIMALS;
    std::string TOKEN_SUPPLY;
    std::string VIRTUAL_RESERVES;
    std::string CURVE_LIMIT;
    std::string EXECUTOR_PRIVATE_KEY;
    std::string CODEX_API_KEY;
    string; // URL for self, for accessing cached assets API_URL;
    std::string FAL_API_KEY;
    std::string LOCAL_DEV;
    std::string NODE_ENV;
    std::string JWT_SECRET;
    std::string RPC_URL;
    std::string MAINNET_SOLANA_RPC_URL;
    std::string DEVNET_SOLANA_RPC_URL;
    std::string DEVNET_PROGRAM_ID;
    std::string PROGRAM_ID;
    string; // Used in tests to track the current test token tokenPubkey;
    std::string AUTH_TOKEN_SALT;
    std::string DEVNET_FRONTEND_URL;
    std::string MAINNET_FRONTEND_URL;
    std::string TWITTER_BEARER_TOKEN;
    std::string TWITTER_CLIENT_ID;
    std::string TWITTER_API_KEY;
    std::string TWITTER_API_SECRET;
    std::string TWITTER_ACCESS_TOKEN_SECRET;
    std::string TWITTER_ACCESS_TOKEN;
    std::optional<std::string> NEWS_API_KEY;
    std::optional<std::string> PREGENERATED_TOKENS_COUNT;
    std::string HELIUS_WEBHOOK_AUTH_TOKEN;
    std::optional<std::string> ADMIN_ADDRESSES;
    std::string MANAGER_MULTISIG_ADDRESS;
    std::string FIXED_FEE;
    std::string FEE_WALLET;
    std::string DATABASE_URL;
    std::string REDIS_HOST;
    std::string REDIS_PORT;
    std::string REDIS_PASSWORD;
    std::optional<std::string> ALLOWED_ORIGINS;
};


} // namespace elizaos
