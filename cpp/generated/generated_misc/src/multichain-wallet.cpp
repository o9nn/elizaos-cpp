#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-relay/src/utils/multichain-wallet.h"

MultiChainWallet::MultiChainWallet(std::shared_ptr<Account> account, string defaultRpcUrl, double initialChainId) {
    this->account = account;
    this->currentChainId = initialChainId;
    this->defaultRpcUrl = defaultRpcUrl;
}

std::shared_ptr<WalletClient> MultiChainWallet::getWalletClient(double chainId)
{
    if (!this->walletClients->has(chainId)) {
        auto chain = const_(CHAIN_MAP)[chainId];
        if (!chain) {
            throw any(std::make_shared<Error>(std::string("Unsupported chain ID: ") + chainId + string_empty));
        }
        auto rpcUrl = const_(DEFAULT_RPC_URLS)[chainId];
        if (!rpcUrl) {
            throw any(std::make_shared<Error>(std::string("No RPC URL configured for chain ") + chainId + string_empty));
        }
        auto client = createWalletClient(object{
            object::pair{std::string("account"), this->account}, 
            object::pair{std::string("chain"), std::string("chain")}, 
            object::pair{std::string("transport"), http(rpcUrl)}
        });
        this->walletClients->set(chainId, client);
    }
    return this->walletClients->get(chainId);
}

std::shared_ptr<Promise<double>> MultiChainWallet::getChainId()
{
    return this->currentChainId;
}

std::shared_ptr<Promise<void>> MultiChainWallet::switchChain(double chainId)
{
    this->currentChainId = chainId;
    this->getWalletClient(chainId);
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<string>> MultiChainWallet::address()
{
    return this->account->address;
}

std::shared_ptr<WalletClient> MultiChainWallet::getCurrentWalletClient()
{
    return this->getWalletClient(this->currentChainId);
}

std::shared_ptr<Account> MultiChainWallet::getAccount()
{
    return this->account;
}

std::shared_ptr<MultiChainWallet> createMultiChainWallet(std::shared_ptr<Account> account, string defaultRpcUrl)
{
    return std::make_shared<MultiChainWallet>(account, defaultRpcUrl);
};


Record<double, std::shared_ptr<Chain>> CHAIN_MAP = object{
    object::pair{mainnet->id, mainnet}, 
    object::pair{base->id, base}, 
    object::pair{arbitrum->id, arbitrum}, 
    object::pair{polygon->id, polygon}, 
    object::pair{optimism->id, optimism}, 
    object::pair{zora->id, zora}, 
    object::pair{blast->id, blast}, 
    object::pair{scroll->id, scroll}, 
    object::pair{linea->id, linea}
};
Record<double, string> DEFAULT_RPC_URLS = object{
    object::pair{mainnet->id, std::string("https://eth.llamarpc.com")}, 
    object::pair{base->id, std::string("https://mainnet.base.org")}, 
    object::pair{arbitrum->id, std::string("https://arb1.arbitrum.io/rpc")}, 
    object::pair{polygon->id, std::string("https://polygon-rpc.com")}, 
    object::pair{optimism->id, std::string("https://mainnet.optimism.io")}, 
    object::pair{zora->id, std::string("https://rpc.zora.energy")}, 
    object::pair{blast->id, std::string("https://rpc.blast.io")}, 
    object::pair{scroll->id, std::string("https://rpc.scroll.io")}, 
    object::pair{linea->id, std::string("https://rpc.linea.build")}
};

void Main(void)
{
}

MAIN
