#include "basic.setup.h"

string SEED_PHRASE = OR((process->env->SEED_PHRASE), (std:("test test test test test test test test test test test junk")));
string PASSWORD = OR((process->env->WALLET_PASSWORD), (std:("Tester@1234")));
any setupWallet = defineWalletSetup(PASSWORD, [=](auto context, auto walletPage) mutable
{
    std::async([=]() { walletPage["waitForLoadState"](std:("domcontentloaded")); });
    auto metamask = std::make_shared<MetaMask>(context, walletPage, PASSWORD);
    std::async([=]() { metamask->importWallet(SEED_PHRASE); });
    auto chainId = parseInt(OR((process->env->CHAIN_ID), (std:("31337"))));
    auto rpcUrl = OR((process->env->NEXT_PUBLIC_RPC_URL), (std:("http://localhost:8545")));
    try
    {
        std::async([=]() { metamask->addNetwork(object{
            object::pair{std:("name"), std:("Anvil Localnet")}, 
            object::pair{std:("rpcUrl"), rpcUrl}, 
            object::pair{std:("chainId"), chainId}, 
            object::pair{std:("symbol"), std:("ETH")}
        }); });
    }
    catch (const any& e)
    {
        console->log(std:("Network may already be added, continuing..."), e);
    }
    try
    {
        std::async([=]() { metamask->switchNetwork(std:("Anvil Localnet")); });
    }
    catch (const any& e)
    {
        console->log(std:("Could not switch network, continuing..."), e);
    }
}
);
string walletPassword = PASSWORD;
string seedPhrase = SEED_PHRASE;

void Main(void)
{
}

MAIN
