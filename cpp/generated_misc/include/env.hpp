#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_SERVER_SRC_ENV_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_SERVER_SRC_ENV_H
#include "core.h"

class Env;

class Env : public object, public std::enable_shared_from_this<Env> {
public:
    using std::enable_shared_from_this<Env>::shared_from_this;
    string NETWORK;

    string DECIMALS;

    string TOKEN_SUPPLY;

    string VIRTUAL_RESERVES;

    string CURVE_LIMIT;

    string EXECUTOR_PRIVATE_KEY;

    string CODEX_API_KEY;

    string API_URL;

    string FAL_API_KEY;

    string LOCAL_DEV;

    string NODE_ENV;

    string JWT_SECRET;

    string RPC_URL;

    string MAINNET_SOLANA_RPC_URL;

    string DEVNET_SOLANA_RPC_URL;

    string DEVNET_PROGRAM_ID;

    string PROGRAM_ID;

    string tokenPubkey;

    string AUTH_TOKEN_SALT;

    string DEVNET_FRONTEND_URL;

    string MAINNET_FRONTEND_URL;

    string TWITTER_BEARER_TOKEN;

    string TWITTER_CLIENT_ID;

    string TWITTER_API_KEY;

    string TWITTER_API_SECRET;

    string TWITTER_ACCESS_TOKEN_SECRET;

    string TWITTER_ACCESS_TOKEN;

    string NEWS_API_KEY;

    string PREGENERATED_TOKENS_COUNT;

    string HELIUS_WEBHOOK_AUTH_TOKEN;

    string ADMIN_ADDRESSES;

    string MANAGER_MULTISIG_ADDRESS;

    string FIXED_FEE;

    string FEE_WALLET;

    string DATABASE_URL;

    string REDIS_HOST;

    string REDIS_PORT;

    string REDIS_PASSWORD;

    string ALLOWED_ORIGINS;
};

#endif
