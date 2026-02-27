#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/raydium/src/raydium-config.h"

std::function<string()> getRpcUrl = [=]() mutable
{
    auto env = process->env;
    return (process->env->NETWORK == std:("devnet")) ? process->env->DEVNET_SOLANA_RPC_URL : process->env->MAINNET_SOLANA_RPC_URL;
};
any txVersion = TxVersion->V0;
any raydium;
std::function<std::shared_ptr<Promise<any>>(object)> initSdk = [=](auto params) mutable
{
    auto cluster = as<Cluster>(process->env->NETWORK);
    auto connection = std::make_shared<Connection>(getRpcUrl());
    auto owner = Keypair->fromSecretKey(Uint8Array->from(JSON->parse(process->env->EXECUTOR_PRIVATE_KEY)));
    if (raydium) return raydium;
    console->log(std:("Raydium SDK: Connected to RPC ") + connection->rpcEndpoint + std:(" in ") + cluster + string_empty);
    raydium = std::async([=]() { Raydium->load(object{
        object::pair{std:("owner"), OR((params["owner"]), (as<any>(owner)))}, 
        object::pair{std:("connection"), std:("connection")}, 
        object::pair{std:("cluster"), std:("cluster")}, 
        object::pair{std:("disableFeatureCheck"), true}, 
        object::pair{std:("disableLoadToken"), !params["loadToken"]}, 
        object::pair{std:("blockhashCommitment"), std:("finalized")}
    }); });
    return raydium;
};
std::function<std::shared_ptr<Promise<any>>()> fetchTokenAccountData = [=]() mutable
{
    auto env = process->env;
    auto connection = std::make_shared<Connection>(getRpcUrl());
    auto owner = Keypair->fromSecretKey(Uint8Array->from(JSON->parse(process->env->EXECUTOR_PRIVATE_KEY)));
    auto solAccountResp = std::async([=]() { connection->getAccountInfo(owner->publicKey); });
    auto tokenAccountResp = std::async([=]() { connection->getTokenAccountsByOwner(owner->publicKey, object{
        object::pair{std:("programId"), TOKEN_PROGRAM_ID}
    }); });
    auto token2022Req = std::async([=]() { connection->getTokenAccountsByOwner(owner->publicKey, object{
        object::pair{std:("programId"), TOKEN_2022_PROGRAM_ID}
    }); });
    auto tokenAccountData = parseTokenAccountResp(object{
        object::pair{std:("owner"), owner->publicKey}, 
        object::pair{std:("solAccountResp"), std:("solAccountResp")}, 
        object::pair{std:("tokenAccountResp"), object{
            object::pair{std:("context"), tokenAccountResp->context}, 
            object::pair{std:("value"), array<any>{ tokenAccountResp->value, token2022Req->value }}
        }}
    });
    return tokenAccountData;
};

void Main(void)
{
}

MAIN
