#include "index.hpp"

std::shared_ptr<Plugin> cdpPlugin = object{
    object::pair{std:("name"), std:("cdp")}, 
    object::pair{std:("description"), std:("Coinbase Developer Platform plugin providing authenticated EVM account creation, token transfers, NFT transfers, swaps, and x402 paid API requests via CDP SDK")}, 
    object::pair{std:("evaluators"), array<any>()}, 
    object::pair{std:("providers"), array<any>{ walletStateProvider }}, 
    object::pair{std:("actions"), array<any>{ cdpWalletInfo, cdpWalletCheckBalance, cdpWalletTokenTransfer, cdpWalletNftTransfer, cdpWalletSwap, cdpResolveEns, cdpTxExplorerLink, cdpCheckTxConfirmation }}, 
    object::pair{std:("services"), array<CdpService>{ CdpService }}
};

void Main(void)
{
}

MAIN
