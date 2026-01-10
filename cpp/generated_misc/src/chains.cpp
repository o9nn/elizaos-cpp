#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/degenTrader/config/chains.h"

object CHAIN_CONFIG = object{
    object::pair{std::string("SOLANA_ENABLED"), false}, 
    object::pair{std::string("BASE_ENABLED"), true}
};
object BASE_CONFIG = object{
    object::pair{std::string("RPC_URL"), OR((process->env->EVM_PROVIDER_URL), (std::string("https://mainnet.base.org")))}, 
    object::pair{std::string("ROUTER_ADDRESS"), std::string("0x327Df1E6de05895d2ab08513aaDD9313Fe505d86")}, 
    object::pair{std::string("WETH_ADDRESS"), std::string("0x4200000000000000000000000000000000000006")}, 
    object::pair{std::string("CHAIN_ID"), 8453}, 
    object::pair{std::string("AERODROME"), object{
        object::pair{std::string("WETH"), std::string("0x4200000000000000000000000000000000000006")}, 
        object::pair{std::string("USDC"), std::string("0x833589fCD6eDb6E08f4c7C32D4f71b54bdA02913")}, 
        object::pair{std::string("USDT"), std::string("0x50c5725949A6F0c72E6C4a641F24049A917DB0Cb")}
    }}
};

void Main(void)
{
}

MAIN
