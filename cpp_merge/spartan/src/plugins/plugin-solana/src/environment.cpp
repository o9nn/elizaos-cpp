#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/plugin-solana/src/environment.h"

std::shared_ptr<Promise<SolanaConfig>> validateSolanaConfig(std::shared_ptr<IAgentRuntime> runtime)
{
    try
    {
        auto config = object{
            object::pair{std:("WALLET_SECRET_SALT"), OR((runtime->getSetting(std:("WALLET_SECRET_SALT"))), (process->env->WALLET_SECRET_SALT))}, 
            object::pair{std:("WALLET_SECRET_KEY"), OR((runtime->getSetting(std:("WALLET_SECRET_KEY"))), (process->env->WALLET_SECRET_KEY))}, 
            object::pair{std:("WALLET_PUBLIC_KEY"), OR((OR((runtime->getSetting(std:("SOLANA_PUBLIC_KEY"))), (runtime->getSetting(std:("WALLET_PUBLIC_KEY"))))), (process->env->WALLET_PUBLIC_KEY))}, 
            object::pair{std:("SOL_ADDRESS"), OR((runtime->getSetting(std:("SOL_ADDRESS"))), (process->env->SOL_ADDRESS))}, 
            object::pair{std:("SLIPPAGE"), OR((runtime->getSetting(std:("SLIPPAGE"))), (process->env->SLIPPAGE))}, 
            object::pair{std:("SOLANA_RPC_URL"), OR((runtime->getSetting(std:("SOLANA_RPC_URL"))), (process->env->SOLANA_RPC_URL))}, 
            object::pair{std:("HELIUS_API_KEY"), OR((runtime->getSetting(std:("HELIUS_API_KEY"))), (process->env->HELIUS_API_KEY))}, 
            object::pair{std:("BIRDEYE_API_KEY"), OR((runtime->getSetting(std:("BIRDEYE_API_KEY"))), (process->env->BIRDEYE_API_KEY))}
        };
        return solanaEnvSchema->parse(config);
    }
    catch (const any& error)
    {
        if (is<z->ZodError>(error)) {
            auto errorMessages = error["errors"]["map"]([=](auto err) mutable
            {
                return string_empty + err["path"]->join(std:(".")) + std:(": ") + err["message"] + string_empty;
            }
            )["join"](std:("\
"));
            throw any(std::make_shared<Error>(std:("Solana configuration validation failed:\
") + errorMessages + string_empty));
        }
        throw any(error);
    }
};


any solanaEnvSchema = z->object(object{
    object::pair{std:("WALLET_SECRET_SALT"), z->string()->optional()}
})->and(z->union(array<any>{ z->object(object{
    object::pair{std:("WALLET_SECRET_KEY"), z->string()->min(1, std:("Wallet secret key is required"))}, 
    object::pair{std:("WALLET_PUBLIC_KEY"), z->string()->min(1, std:("Wallet public key is required"))}
}), z->object(object{
    object::pair{std:("WALLET_SECRET_SALT"), z->string()->min(1, std:("Wallet secret salt is required"))}
}) }))->and(z->object(object{
    object::pair{std:("SOL_ADDRESS"), z->string()->min(1, std:("SOL address is required"))}, 
    object::pair{std:("SLIPPAGE"), z->string()->min(1, std:("Slippage is required"))}, 
    object::pair{std:("SOLANA_RPC_URL"), z->string()->min(1, std:("RPC URL is required"))}, 
    object::pair{std:("HELIUS_API_KEY"), z->string()->min(1, std:("Helius API key is required"))}, 
    object::pair{std:("BIRDEYE_API_KEY"), z->string()->min(1, std:("Birdeye API key is required"))}
}));

void Main(void)
{
}

MAIN
