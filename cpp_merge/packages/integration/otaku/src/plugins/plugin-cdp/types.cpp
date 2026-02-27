#include "types.hpp"
#include <string>

Record<double, string> DEFAULT_RPC_URLS = object{
    object::pair{mainnet->id, std::string("https://ethereum.publicnode.com")}, 
    object::pair{base->id, std::string("https://mainnet.base.org")}, 
    object::pair{arbitrum->id, std::string("https://arb1.arbitrum.io/rpc")}, 
    object::pair{polygon->id, std::string("https://polygon-rpc.com")}, 
    object::pair{baseSepolia->id, std::string("https://sepolia.base.org")}, 
    object::pair{optimism->id, std::string("https://mainnet.optimism.io")}
};

void Main(void)
{
}

MAIN
