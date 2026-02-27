#include "index.hpp"
#include <string>

std::shared_ptr<Plugin> etherscanPlugin = object{
    object::pair{std::string("name"), std::string("etherscan")}, 
    object::pair{std::string("description"), std::string("Etherscan integration for checking transaction confirmations, contract verification status, and blockchain data on Ethereum and other EVM chains")}, 
    object::pair{std::string("actions"), array<any>{ checkTransactionConfirmationAction }}, 
    object::pair{std::string("services"), array<EtherscanService>{ EtherscanService }}, 
    object::pair{std::string("evaluators"), array<any>()}, 
    object::pair{std::string("providers"), array<any>()}
};

void Main(void)
{
}

MAIN
