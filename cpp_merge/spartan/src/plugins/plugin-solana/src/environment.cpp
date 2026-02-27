#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/plugin-solana/src/environment.h"

std::shared_ptr<Promise<SolanaConfig>> validateSolanaConfig(std::shared_ptr<IAgentRuntime> runtime)
{
    try
    {
        auto config = object{
            object::pair{std::string("WALLET_SECRET_SALT"), OR((runtime->getSetting(std::string("WALLET_SECRET_SALT"))), (process->env->WALLET_SECRET_SALT))}, 
            object::pair{std::string("WALLET_SECRET_KEY"), OR((runtime->getSetting(std::string("WALLET_SECRET_KEY"))), (process->env->WALLET_SECRET_KEY))}, 
            object::pair{std::string("WALLET_PUBLIC_KEY"), OR((OR((runtime->getSetting(std::string("SOLANA_PUBLIC_KEY"))), (runtime->getSetting(std::string("WALLET_PUBLIC_KEY"))))), (process->env->WALLET_PUBLIC_KEY))}, 
            object::pair{std::string("SOL_ADDRESS"), OR((runtime->getSetting(std::string("SOL_ADDRESS"))), (process->env->SOL_ADDRESS))}, 
            object::pair{std::string("SLIPPAGE"), OR((runtime->getSetting(std::string("SLIPPAGE"))), (process->env->SLIPPAGE))}, 
            object::pair{std::string("SOLANA_RPC_URL"), OR((runtime->getSetting(std::string("SOLANA_RPC_URL"))), (process->env->SOLANA_RPC_URL))}, 
            object::pair{std::string("HELIUS_API_KEY"), OR((runtime->getSetting(std::string("HELIUS_API_KEY"))), (process->env->HELIUS_API_KEY))}, 
            object::pair{std::string("BIRDEYE_API_KEY"), OR((runtime->getSetting(std::string("BIRDEYE_API_KEY"))), (process->env->BIRDEYE_API_KEY))}
        };
        return solanaEnvSchema->parse(config);
    }
    catch (const any& error)
    {
        if (is<z->ZodError>(error)) {
            auto errorMessages = error["errors"]["map"]([=](auto err) mutable
            {
                return string_empty + err["path"]->join(std::string(".")) + std::string(": ") + err["message"] + string_empty;
            }
            )["join"](std::string("\
"));
            throw any(std::make_shared<Error>(std::string("Solana configuration validation failed:\
") + errorMessages + string_empty));
        }
        throw any(error);
    }
};


any solanaEnvSchema = z->object(object{
    object::pair{std::string("WALLET_SECRET_SALT"), z->string()->optional()}
})->and(z->union(array<any>{ z->object(object{
    object::pair{std::string("WALLET_SECRET_KEY"), z->string()->min(1, std::string("Wallet secret key is required"))}, 
    object::pair{std::string("WALLET_PUBLIC_KEY"), z->string()->min(1, std::string("Wallet public key is required"))}
}), z->object(object{
    object::pair{std::string("WALLET_SECRET_SALT"), z->string()->min(1, std::string("Wallet secret salt is required"))}
}) }))->and(z->object(object{
    object::pair{std::string("SOL_ADDRESS"), z->string()->min(1, std::string("SOL address is required"))}, 
    object::pair{std::string("SLIPPAGE"), z->string()->min(1, std::string("Slippage is required"))}, 
    object::pair{std::string("SOLANA_RPC_URL"), z->string()->min(1, std::string("RPC URL is required"))}, 
    object::pair{std::string("HELIUS_API_KEY"), z->string()->min(1, std::string("Helius API key is required"))}, 
    object::pair{std::string("BIRDEYE_API_KEY"), z->string()->min(1, std::string("Birdeye API key is required"))}
}));

void Main(void)
{
}

MAIN
