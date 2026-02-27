#include "chains.hpp"

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
    return OR((config->color), (std:("#6B7280")));
};


any getTxExplorerUrl(string chain, string txHash)
{
    auto config = getChainConfig(chain);
    return (config) ? any(string_empty + config->explorerUrl + std:("/tx/") + txHash + string_empty) (nullptr);
};


any getAddressExplorerUrl(string chain, string address)
{
    auto config = getChainConfig(chain);
    return (config) ? any(string_empty + config->explorerUrl + std:("/address/") + address + string_empty) (nullptr);
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
    object::pair{std:("base"), object{
        object::pair{std:("id"), std:("base")}, 
        object::pair{std:("name"), std:("Base")}, 
        object::pair{std:("displayName"), std:("Base")}, 
        object::pair{std:("icon"), std:("/assets/base.svg")}, 
        object::pair{std:("walletIcon"), std:("/assets/walletIcon/base.svg")}, 
        object::pair{std:("nativeToken"), object{
            object::pair{std:("symbol"), std:("ETH")}, 
            object::pair{std:("name"), std:("Ethereum")}, 
            object::pair{std:("icon"), std:("/assets/eth.svg")}
        }}, 
        object::pair{std:("explorerUrl"), std:("https://basescan.org")}, 
        object::pair{std:("color"), std:("#0052FF")}
    }}, 
    object::pair{std:("ethereum"), object{
        object::pair{std:("id"), std:("ethereum")}, 
        object::pair{std:("name"), std:("Ethereum")}, 
        object::pair{std:("displayName"), std:("Ethereum")}, 
        object::pair{std:("icon"), std:("/assets/eth.svg")}, 
        object::pair{std:("walletIcon"), std:("/assets/walletIcon/ethereum.svg")}, 
        object::pair{std:("nativeToken"), object{
            object::pair{std:("symbol"), std:("ETH")}, 
            object::pair{std:("name"), std:("Ethereum")}, 
            object::pair{std:("icon"), std:("/assets/eth.svg")}
        }}, 
        object::pair{std:("explorerUrl"), std:("https://etherscan.io")}, 
        object::pair{std:("color"), std:("#627EEA")}
    }}, 
    object::pair{std:("polygon"), object{
        object::pair{std:("id"), std:("polygon")}, 
        object::pair{std:("name"), std:("Polygon")}, 
        object::pair{std:("displayName"), std:("Polygon")}, 
        object::pair{std:("icon"), std:("/assets/polygon.svg")}, 
        object::pair{std:("walletIcon"), std:("/assets/walletIcon/polygon.svg")}, 
        object::pair{std:("nativeToken"), object{
            object::pair{std:("symbol"), std:("POL")}, 
            object::pair{std:("name"), std:("Polygon")}, 
            object::pair{std:("icon"), std:("/assets/polygon.svg")}
        }}, 
        object::pair{std:("explorerUrl"), std:("https://polygonscan.com")}, 
        object::pair{std:("color"), std:("#8247E5")}
    }}, 
    object::pair{std:("arbitrum"), object{
        object::pair{std:("id"), std:("arbitrum")}, 
        object::pair{std:("name"), std:("Arbitrum")}, 
        object::pair{std:("displayName"), std:("Arbitrum")}, 
        object::pair{std:("icon"), std:("/assets/arbitrum.svg")}, 
        object::pair{std:("walletIcon"), std:("/assets/walletIcon/arbitrum.svg")}, 
        object::pair{std:("nativeToken"), object{
            object::pair{std:("symbol"), std:("ETH")}, 
            object::pair{std:("name"), std:("Ethereum")}, 
            object::pair{std:("icon"), std:("/assets/eth.svg")}
        }}, 
        object::pair{std:("explorerUrl"), std:("https://arbiscan.io")}, 
        object::pair{std:("color"), std:("#28A0F0")}
    }}, 
    object::pair{std:("optimism"), object{
        object::pair{std:("id"), std:("optimism")}, 
        object::pair{std:("name"), std:("Optimism")}, 
        object::pair{std:("displayName"), std:("Optimism")}, 
        object::pair{std:("icon"), std:("/assets/optimism.svg")}, 
        object::pair{std:("walletIcon"), std:("/assets/walletIcon/optimism.svg")}, 
        object::pair{std:("nativeToken"), object{
            object::pair{std:("symbol"), std:("ETH")}, 
            object::pair{std:("name"), std:("Ethereum")}, 
            object::pair{std:("icon"), std:("/assets/eth.svg")}
        }}, 
        object::pair{std:("explorerUrl"), std:("https://optimistic.etherscan.io")}, 
        object::pair{std:("color"), std:("#FF0420")}
    }}, 
    object::pair{std:("scroll"), object{
        object::pair{std:("id"), std:("scroll")}, 
        object::pair{std:("name"), std:("Scroll")}, 
        object::pair{std:("displayName"), std:("Scroll")}, 
        object::pair{std:("icon"), std:("/assets/scroll.svg")}, 
        object::pair{std:("walletIcon"), std:("/assets/walletIcon/scroll.svg")}, 
        object::pair{std:("nativeToken"), object{
            object::pair{std:("symbol"), std:("ETH")}, 
            object::pair{std:("name"), std:("Ethereum")}, 
            object::pair{std:("icon"), std:("/assets/eth.svg")}
        }}, 
        object::pair{std:("explorerUrl"), std:("https://scrollscan.com")}, 
        object::pair{std:("color"), std:("#FFEEDA")}
    }}
};
array<SupportedChain> SUPPORTED_CHAINS = as<array<SupportedChain>>(Object->keys(CHAIN_UI_CONFIGS));
Record<string, string> TOKEN_ICONS = object{
    object::pair{std:("ETH"), std:("/assets/eth.svg")}, 
    object::pair{std:("WETH"), std:("/assets/eth.svg")}, 
    object::pair{std:("MATIC"), std:("/assets/polygon.svg")}, 
    object::pair{std:("POL"), std:("/assets/polygon.svg")}, 
    object::pair{std:("USDC"), std:("/assets/usdc.svg")}, 
    object::pair{std:("USDT"), std:("/assets/usdt.svg")}, 
    object::pair{std:("DAI"), std:("/assets/dai.svg")}
};

void Main(void)
{
}

MAIN
