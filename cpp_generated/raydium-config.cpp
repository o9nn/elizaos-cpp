#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/raydium/src/raydium-config.h"

std::function<string()> getRpcUrl = [=]() mutable
{
    auto env = process->env;
    return (process->env->NETWORK == std::string("devnet")) ? process->env->DEVNET_SOLANA_RPC_URL : process->env->MAINNET_SOLANA_RPC_URL;
};
any txVersion = TxVersion->V0;
any raydium;
std::function<std::shared_ptr<Promise<any>>(object)> initSdk = [=](auto params) mutable
{
    auto cluster = as<Cluster>(process->env->NETWORK);
    auto connection = std::make_shared<Connection>(getRpcUrl());
    auto owner = Keypair->fromSecretKey(Uint8Array->from(JSON->parse(process->env->EXECUTOR_PRIVATE_KEY)));
    if (raydium) return raydium;
    console->log(std::string("Raydium SDK: Connected to RPC ") + connection->rpcEndpoint + std::string(" in ") + cluster + string_empty);
    raydium = std::async([=]() { Raydium->load(object{
        object::pair{std::string("owner"), OR((params["owner"]), (as<any>(owner)))}, 
        object::pair{std::string("connection"), std::string("connection")}, 
        object::pair{std::string("cluster"), std::string("cluster")}, 
        object::pair{std::string("disableFeatureCheck"), true}, 
        object::pair{std::string("disableLoadToken"), !params["loadToken"]}, 
        object::pair{std::string("blockhashCommitment"), std::string("finalized")}
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
        object::pair{std::string("programId"), TOKEN_PROGRAM_ID}
    }); });
    auto token2022Req = std::async([=]() { connection->getTokenAccountsByOwner(owner->publicKey, object{
        object::pair{std::string("programId"), TOKEN_2022_PROGRAM_ID}
    }); });
    auto tokenAccountData = parseTokenAccountResp(object{
        object::pair{std::string("owner"), owner->publicKey}, 
        object::pair{std::string("solAccountResp"), std::string("solAccountResp")}, 
        object::pair{std::string("tokenAccountResp"), object{
            object::pair{std::string("context"), tokenAccountResp->context}, 
            object::pair{std::string("value"), array<any>{ tokenAccountResp->value, token2022Req->value }}
        }}
    });
    return tokenAccountData;
};

void Main(void)
{
}

MAIN
