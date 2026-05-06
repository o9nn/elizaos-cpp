#include "index.hpp"
#include <string>

std::shared_ptr<Plugin> cdpPlugin = object{
    object::pair{std::string("name"), std::string("cdp")}, 
    object::pair{std::string("description"), std::string("Coinbase Developer Platform plugin providing authenticated EVM account creation, token transfers, NFT transfers, swaps, and x402 paid API requests via CDP SDK")}, 
    object::pair{std::string("evaluators"), array<any>()}, 
    object::pair{std::string("providers"), array<any>{ walletStateProvider }}, 
    object::pair{std::string("actions"), array<any>{ cdpWalletInfo, cdpWalletCheckBalance, cdpWalletTokenTransfer, cdpWalletNftTransfer, cdpWalletSwap, cdpResolveEns, cdpTxExplorerLink, cdpCheckTxConfirmation }}, 
    object::pair{std::string("services"), array<CdpService>{ CdpService }}
};

void Main(void)
{
}

MAIN
