#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_SERVER_SRC_ENV_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_SERVER_SRC_ENV_H
#include "core.h"

class Env;

class Env : public object, public std::enable_shared_from_this<Env> {
public:
    using std::enable_shared_from_this<Env>::shared_from_this;
    std::string NETWORK;

    std::string DECIMALS;

    std::string TOKEN_SUPPLY;

    std::string VIRTUAL_RESERVES;

    std::string CURVE_LIMIT;

    std::string EXECUTOR_PRIVATE_KEY;

    std::string CODEX_API_KEY;

    std::string API_URL;

    std::string FAL_API_KEY;

    std::string LOCAL_DEV;

    std::string NODE_ENV;

    std::string JWT_SECRET;

    std::string RPC_URL;

    std::string MAINNET_SOLANA_RPC_URL;

    std::string DEVNET_SOLANA_RPC_URL;

    std::string DEVNET_PROGRAM_ID;

    std::string PROGRAM_ID;

    std::string tokenPubkey;

    std::string AUTH_TOKEN_SALT;

    std::string DEVNET_FRONTEND_URL;

    std::string MAINNET_FRONTEND_URL;

    std::string TWITTER_BEARER_TOKEN;

    std::string TWITTER_CLIENT_ID;

    std::string TWITTER_API_KEY;

    std::string TWITTER_API_SECRET;

    std::string TWITTER_ACCESS_TOKEN_SECRET;

    std::string TWITTER_ACCESS_TOKEN;

    std::string NEWS_API_KEY;

    std::string PREGENERATED_TOKENS_COUNT;

    std::string HELIUS_WEBHOOK_AUTH_TOKEN;

    std::string ADMIN_ADDRESSES;

    std::string MANAGER_MULTISIG_ADDRESS;

    std::string FIXED_FEE;

    std::string FEE_WALLET;

    std::string DATABASE_URL;

    std::string REDIS_HOST;

    std::string REDIS_PORT;

    std::string REDIS_PASSWORD;

    std::string ALLOWED_ORIGINS;
};

#endif
