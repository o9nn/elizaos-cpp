#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-cdp/types.h"

Record<double, string> DEFAULT_RPC_URLS = object{
    object::pair{mainnet->id, std:("https://ethereum.publicnode.com")}, 
    object::pair{base->id, std:("https://mainnet.base.org")}, 
    object::pair{arbitrum->id, std:("https://arb1.arbitrum.io/rpc")}, 
    object::pair{polygon->id, std:("https://polygon-rpc.com")}, 
    object::pair{baseSepolia->id, std:("https://sepolia.base.org")}, 
    object::pair{optimism->id, std:("https://mainnet.optimism.io")}
};

void Main(void)
{
}

MAIN
