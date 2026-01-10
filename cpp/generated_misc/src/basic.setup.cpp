#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otc-agent/tests/wallet-setup/basic.setup.h"

string SEED_PHRASE = OR((process->env->SEED_PHRASE), (std::string("test test test test test test test test test test test junk")));
string PASSWORD = OR((process->env->WALLET_PASSWORD), (std::string("Tester@1234")));
any setupWallet = defineWalletSetup(PASSWORD, [=](auto context, auto walletPage) mutable
{
    std::async([=]() { walletPage["waitForLoadState"](std::string("domcontentloaded")); });
    auto metamask = std::make_shared<MetaMask>(context, walletPage, PASSWORD);
    std::async([=]() { metamask->importWallet(SEED_PHRASE); });
    auto chainId = parseInt(OR((process->env->CHAIN_ID), (std::string("31337"))));
    auto rpcUrl = OR((process->env->NEXT_PUBLIC_RPC_URL), (std::string("http://localhost:8545")));
    try
    {
        std::async([=]() { metamask->addNetwork(object{
            object::pair{std::string("name"), std::string("Anvil Localnet")}, 
            object::pair{std::string("rpcUrl"), rpcUrl}, 
            object::pair{std::string("chainId"), chainId}, 
            object::pair{std::string("symbol"), std::string("ETH")}
        }); });
    }
    catch (const any& e)
    {
        console->log(std::string("Network may already be added, continuing..."), e);
    }
    try
    {
        std::async([=]() { metamask->switchNetwork(std::string("Anvil Localnet")); });
    }
    catch (const any& e)
    {
        console->log(std::string("Could not switch network, continuing..."), e);
    }
}
);
string walletPassword = PASSWORD;
string seedPhrase = SEED_PHRASE;

void Main(void)
{
}

MAIN
