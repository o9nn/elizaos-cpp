#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/frontend/constants/chains.h"

any getChainConfig(string chain)
{
    return OR((const_(CHAIN_UI_CONFIGS)[as<SupportedChain>(chain)]), (nullptr));
};


any getChainIcon(string chain)
{
    auto config = getChainConfig(chain);
    return OR((config->icon), (nullptr));
};


any getChainWalletIcon(string chain)
{
    auto config = getChainConfig(chain);
    return OR((config->walletIcon), (nullptr));
};


any getNativeTokenIcon(string chain)
{
    auto config = getChainConfig(chain);
    return OR((config->nativeToken["icon"]), (nullptr));
};


any getNativeTokenSymbol(string chain)
{
    auto config = getChainConfig(chain);
    return OR((config->nativeToken["symbol"]), (nullptr));
};


string getChainColor(string chain)
{
    auto config = getChainConfig(chain);
    return OR((config->color), (std::string("#6B7280")));
};


any getTxExplorerUrl(string chain, string txHash)
{
    auto config = getChainConfig(chain);
    return (config) ? any(string_empty + config->explorerUrl + std::string("/tx/") + txHash + string_empty) : any(nullptr);
};


any getAddressExplorerUrl(string chain, string address)
{
    auto config = getChainConfig(chain);
    return (config) ? any(string_empty + config->explorerUrl + std::string("/address/") + address + string_empty) : any(nullptr);
};


any isSupportedChain(string chain)
{
    return in(chain, CHAIN_UI_CONFIGS);
};


string getChainDisplayName(string chain)
{
    auto config = getChainConfig(chain);
    return OR((config->displayName), (chain));
};


any getTokenIconBySymbol(string symbol)
{
    return OR((const_(TOKEN_ICONS)[symbol->toUpperCase()]), (nullptr));
};


any getTokenIcon(string symbol, string chain)
{
    auto tokenIcon = getTokenIconBySymbol(symbol);
    if (tokenIcon) return tokenIcon;
    if (chain) {
        return getNativeTokenIcon(chain);
    }
    return nullptr;
};


Record<SupportedChain, std::shared_ptr<ChainUIConfig>> CHAIN_UI_CONFIGS = object{
    object::pair{std::string("base"), object{
        object::pair{std::string("id"), std::string("base")}, 
        object::pair{std::string("name"), std::string("Base")}, 
        object::pair{std::string("displayName"), std::string("Base")}, 
        object::pair{std::string("icon"), std::string("/assets/base.svg")}, 
        object::pair{std::string("walletIcon"), std::string("/assets/walletIcon/base.svg")}, 
        object::pair{std::string("nativeToken"), object{
            object::pair{std::string("symbol"), std::string("ETH")}, 
            object::pair{std::string("name"), std::string("Ethereum")}, 
            object::pair{std::string("icon"), std::string("/assets/eth.svg")}
        }}, 
        object::pair{std::string("explorerUrl"), std::string("https://basescan.org")}, 
        object::pair{std::string("color"), std::string("#0052FF")}
    }}, 
    object::pair{std::string("ethereum"), object{
        object::pair{std::string("id"), std::string("ethereum")}, 
        object::pair{std::string("name"), std::string("Ethereum")}, 
        object::pair{std::string("displayName"), std::string("Ethereum")}, 
        object::pair{std::string("icon"), std::string("/assets/eth.svg")}, 
        object::pair{std::string("walletIcon"), std::string("/assets/walletIcon/ethereum.svg")}, 
        object::pair{std::string("nativeToken"), object{
            object::pair{std::string("symbol"), std::string("ETH")}, 
            object::pair{std::string("name"), std::string("Ethereum")}, 
            object::pair{std::string("icon"), std::string("/assets/eth.svg")}
        }}, 
        object::pair{std::string("explorerUrl"), std::string("https://etherscan.io")}, 
        object::pair{std::string("color"), std::string("#627EEA")}
    }}, 
    object::pair{std::string("polygon"), object{
        object::pair{std::string("id"), std::string("polygon")}, 
        object::pair{std::string("name"), std::string("Polygon")}, 
        object::pair{std::string("displayName"), std::string("Polygon")}, 
        object::pair{std::string("icon"), std::string("/assets/polygon.svg")}, 
        object::pair{std::string("walletIcon"), std::string("/assets/walletIcon/polygon.svg")}, 
        object::pair{std::string("nativeToken"), object{
            object::pair{std::string("symbol"), std::string("POL")}, 
            object::pair{std::string("name"), std::string("Polygon")}, 
            object::pair{std::string("icon"), std::string("/assets/polygon.svg")}
        }}, 
        object::pair{std::string("explorerUrl"), std::string("https://polygonscan.com")}, 
        object::pair{std::string("color"), std::string("#8247E5")}
    }}, 
    object::pair{std::string("arbitrum"), object{
        object::pair{std::string("id"), std::string("arbitrum")}, 
        object::pair{std::string("name"), std::string("Arbitrum")}, 
        object::pair{std::string("displayName"), std::string("Arbitrum")}, 
        object::pair{std::string("icon"), std::string("/assets/arbitrum.svg")}, 
        object::pair{std::string("walletIcon"), std::string("/assets/walletIcon/arbitrum.svg")}, 
        object::pair{std::string("nativeToken"), object{
            object::pair{std::string("symbol"), std::string("ETH")}, 
            object::pair{std::string("name"), std::string("Ethereum")}, 
            object::pair{std::string("icon"), std::string("/assets/eth.svg")}
        }}, 
        object::pair{std::string("explorerUrl"), std::string("https://arbiscan.io")}, 
        object::pair{std::string("color"), std::string("#28A0F0")}
    }}, 
    object::pair{std::string("optimism"), object{
        object::pair{std::string("id"), std::string("optimism")}, 
        object::pair{std::string("name"), std::string("Optimism")}, 
        object::pair{std::string("displayName"), std::string("Optimism")}, 
        object::pair{std::string("icon"), std::string("/assets/optimism.svg")}, 
        object::pair{std::string("walletIcon"), std::string("/assets/walletIcon/optimism.svg")}, 
        object::pair{std::string("nativeToken"), object{
            object::pair{std::string("symbol"), std::string("ETH")}, 
            object::pair{std::string("name"), std::string("Ethereum")}, 
            object::pair{std::string("icon"), std::string("/assets/eth.svg")}
        }}, 
        object::pair{std::string("explorerUrl"), std::string("https://optimistic.etherscan.io")}, 
        object::pair{std::string("color"), std::string("#FF0420")}
    }}, 
    object::pair{std::string("scroll"), object{
        object::pair{std::string("id"), std::string("scroll")}, 
        object::pair{std::string("name"), std::string("Scroll")}, 
        object::pair{std::string("displayName"), std::string("Scroll")}, 
        object::pair{std::string("icon"), std::string("/assets/scroll.svg")}, 
        object::pair{std::string("walletIcon"), std::string("/assets/walletIcon/scroll.svg")}, 
        object::pair{std::string("nativeToken"), object{
            object::pair{std::string("symbol"), std::string("ETH")}, 
            object::pair{std::string("name"), std::string("Ethereum")}, 
            object::pair{std::string("icon"), std::string("/assets/eth.svg")}
        }}, 
        object::pair{std::string("explorerUrl"), std::string("https://scrollscan.com")}, 
        object::pair{std::string("color"), std::string("#FFEEDA")}
    }}
};
array<SupportedChain> SUPPORTED_CHAINS = as<array<SupportedChain>>(Object->keys(CHAIN_UI_CONFIGS));
Record<string, string> TOKEN_ICONS = object{
    object::pair{std::string("ETH"), std::string("/assets/eth.svg")}, 
    object::pair{std::string("WETH"), std::string("/assets/eth.svg")}, 
    object::pair{std::string("MATIC"), std::string("/assets/polygon.svg")}, 
    object::pair{std::string("POL"), std::string("/assets/polygon.svg")}, 
    object::pair{std::string("USDC"), std::string("/assets/usdc.svg")}, 
    object::pair{std::string("USDT"), std::string("/assets/usdt.svg")}, 
    object::pair{std::string("DAI"), std::string("/assets/dai.svg")}
};

void Main(void)
{
}

MAIN
