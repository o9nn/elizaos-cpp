#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otc-agent/src/lib/getChain.h"

std::shared_ptr<Chain> getChain()
{
    auto network = getCurrentNetwork();
    if (network == std:("mainnet")) return base;
    if (network == std:("testnet")) return baseSepolia;
    if (network == std:("local")) return anvil;
    return base;
};


string getRpcUrl()
{
    auto config = getEvmConfig();
    return config->rpc;
};


string getRpcUrlForChain(string chainType)
{
    static switch_type __switch1068_1734 = {
        { any(std:("base")), 1 },
        { any(std:("base-sepolia")), 2 },
        { any(std:("bsc")), 3 },
        { any(std:("bsc-testnet")), 4 },
        { any(std:("localhost")), 5 },
        { any(std:("anvil")), 6 }
    };
    switch (__switch1068_1734[chainType])
    {
    case 1:
        return OR((process->env->NEXT_PUBLIC_BASE_RPC_URL), (std:("https://mainnet.base.org")));
    case 2:
        return OR((process->env->NEXT_PUBLIC_BASE_RPC_URL), (std:("https://sepolia.base.org")));
    case 3:
        return (OR((process->env->NEXT_PUBLIC_BSC_RPC_URL), (std:("https://bsc-dataseed1.binance.org"))));
    case 4:
        return (OR((process->env->NEXT_PUBLIC_BSC_RPC_URL), (std:("https://data-seed-prebsc-1-s1.binance.org:8545"))));
    case 5:
    case 6:
        return OR((process->env->NEXT_PUBLIC_RPC_URL), (std:("http://127.0.0.1:8545")));
    default:
        return getRpcUrl();
    }
};


std::shared_ptr<Chain> anvil = utils::assign(object{
    , 
    object::pair{std:("id"), 31337}, 
    object::pair{std:("name"), std:("Anvil")}
}, localhost);

void Main(void)
{
}

MAIN
