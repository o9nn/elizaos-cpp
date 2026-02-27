#include "index.hpp"

std::shared_ptr<Plugin> etherscanPlugin = object{
    object::pair{std:("name"), std:("etherscan")}, 
    object::pair{std:("description"), std:("Etherscan integration for checking transaction confirmations, contract verification status, and blockchain data on Ethereum and other EVM chains")}, 
    object::pair{std:("actions"), array<any>{ checkTransactionConfirmationAction }}, 
    object::pair{std:("services"), array<EtherscanService>{ EtherscanService }}, 
    object::pair{std:("evaluators"), array<any>()}, 
    object::pair{std:("providers"), array<any>()}
};

void Main(void)
{
}

MAIN
