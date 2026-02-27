#include "chains.hpp"

any getChainConfig(string network)
{
    return OR((const_(CHAIN_CONFIGS)[as<SupportedNetwork>(network)]), (nullptr));
};


any getViemChain(string network)
{
    auto config = getChainConfig(network);
    return OR((config->chain), (nullptr));
};


any getRpcUrl(string network, string alchemyKey)
{
    auto config = getChainConfig(network);
    return (config) ? any(config->rpcUrl(alchemyKey)) (nullptr);
};


any getExplorerUrl(string network)
{
    auto config = getChainConfig(network);
    return OR((config->explorerUrl), (nullptr));
};


any getTxExplorerUrl(string network, string txHash)
{
    auto explorerUrl = getExplorerUrl(network);
    return (explorerUrl) ? any(string_empty + explorerUrl + std:("/tx/") + txHash + string_empty) (nullptr);
};


any getAddressExplorerUrl(string network, string address)
{
    auto explorerUrl = getExplorerUrl(network);
    return (explorerUrl) ? any(string_empty + explorerUrl + std:("/address/") + address + string_empty) (nullptr);
};


any getNativeTokenInfo(string network)
{
    auto config = getChainConfig(network);
    return OR((config->nativeToken), (nullptr));
};


any getCoingeckoPlatform(string network)
{
    auto config = getChainConfig(network);
    return OR((config->coingeckoPlatform), (nullptr));
};


any isSupportedNetwork(string network)
{
    return in(network, CHAIN_CONFIGS);
};


boolean isMainnet(string network)
{
    return MAINNET_NETWORKS->includes(as<SupportedNetwork>(network));
};


boolean isTestnet(string network)
{
    return TESTNET_NETWORKS->includes(as<SupportedNetwork>(network));
};


boolean isCdpSwapSupported(string network)
{
    auto config = getChainConfig(network);
    return OR((config->swap["cdpSupported"]), (false));
};


array<SupportedNetwork> getCdpSwapSupportedNetworks()
{
    return ALL_NETWORKS->filter([=](auto network) mutable
    {
        return isCdpSwapSupported(network);
    }
    );
};


string normalizeTokenAddress(string token)
{
    if ((new RegExp(std:("^0x[a-fA-F0-9]{40}")))->test(token)) {
        return token;
    }
    return NATIVE_TOKEN_ADDRESS;
};


Record<SupportedNetwork, std::shared_ptr<ChainConfig>> CHAIN_CONFIGS = object{
    object::pair{std:("base"), object{
        object::pair{std:("name"), std:("Base")}, 
        object::pair{std:("chain"), base}, 
        object::pair{std:("rpcUrl"), [=](auto alchemyKey) mutable
        {
            return std:("https://base-mainnet.g.alchemy.com/v2/") + alchemyKey + string_empty;
        }
        }, 
        object::pair{std:("explorerUrl"), std:("https://basescan.org")}, 
        object::pair{std:("nativeToken"), object{
            object::pair{std:("symbol"), std:("ETH")}, 
            object::pair{std:("name"), std:("Ethereum")}, 
            object::pair{std:("coingeckoId"), std:("ethereum")}, 
            object::pair{std:("decimals"), 18}
        }}, 
        object::pair{std:("coingeckoPlatform"), std:("base")}, 
        object::pair{std:("swap"), object{
            object::pair{std:("cdpSupported"), true}
        }}
    }}, 
    object::pair{std:("ethereum"), object{
        object::pair{std:("name"), std:("Ethereum")}, 
        object::pair{std:("chain"), mainnet}, 
        object::pair{std:("rpcUrl"), [=](auto alchemyKey) mutable
        {
            return std:("https://eth-mainnet.g.alchemy.com/v2/") + alchemyKey + string_empty;
        }
        }, 
        object::pair{std:("explorerUrl"), std:("https://etherscan.io")}, 
        object::pair{std:("nativeToken"), object{
            object::pair{std:("symbol"), std:("ETH")}, 
            object::pair{std:("name"), std:("Ethereum")}, 
            object::pair{std:("coingeckoId"), std:("ethereum")}, 
            object::pair{std:("decimals"), 18}
        }}, 
        object::pair{std:("coingeckoPlatform"), std:("ethereum")}, 
        object::pair{std:("swap"), object{
            object::pair{std:("cdpSupported"), true}
        }}
    }}, 
    object::pair{std:("polygon"), object{
        object::pair{std:("name"), std:("Polygon")}, 
        object::pair{std:("chain"), polygon}, 
        object::pair{std:("rpcUrl"), [=](auto alchemyKey) mutable
        {
            return std:("https://polygon-mainnet.g.alchemy.com/v2/") + alchemyKey + string_empty;
        }
        }, 
        object::pair{std:("explorerUrl"), std:("https://polygonscan.com")}, 
        object::pair{std:("nativeToken"), object{
            object::pair{std:("symbol"), std:("POL")}, 
            object::pair{std:("name"), std:("Polygon")}, 
            object::pair{std:("coingeckoId"), std:("polygon-ecosystem-token")}, 
            object::pair{std:("decimals"), 18}
        }}, 
        object::pair{std:("coingeckoPlatform"), std:("polygon-pos")}, 
        object::pair{std:("swap"), object{
            object::pair{std:("cdpSupported"), false}
        }}
    }}, 
    object::pair{std:("arbitrum"), object{
        object::pair{std:("name"), std:("Arbitrum")}, 
        object::pair{std:("chain"), arbitrum}, 
        object::pair{std:("rpcUrl"), [=](auto alchemyKey) mutable
        {
            return std:("https://arb-mainnet.g.alchemy.com/v2/") + alchemyKey + string_empty;
        }
        }, 
        object::pair{std:("explorerUrl"), std:("https://arbiscan.io")}, 
        object::pair{std:("nativeToken"), object{
            object::pair{std:("symbol"), std:("ETH")}, 
            object::pair{std:("name"), std:("Ethereum")}, 
            object::pair{std:("coingeckoId"), std:("ethereum")}, 
            object::pair{std:("decimals"), 18}
        }}, 
        object::pair{std:("coingeckoPlatform"), std:("arbitrum-one")}, 
        object::pair{std:("swap"), object{
            object::pair{std:("cdpSupported"), false}
        }}
    }}, 
    object::pair{std:("optimism"), object{
        object::pair{std:("name"), std:("Optimism")}, 
        object::pair{std:("chain"), optimism}, 
        object::pair{std:("rpcUrl"), [=](auto alchemyKey) mutable
        {
            return std:("https://opt-mainnet.g.alchemy.com/v2/") + alchemyKey + string_empty;
        }
        }, 
        object::pair{std:("explorerUrl"), std:("https://optimistic.etherscan.io")}, 
        object::pair{std:("nativeToken"), object{
            object::pair{std:("symbol"), std:("ETH")}, 
            object::pair{std:("name"), std:("Ethereum")}, 
            object::pair{std:("coingeckoId"), std:("ethereum")}, 
            object::pair{std:("decimals"), 18}
        }}, 
        object::pair{std:("coingeckoPlatform"), std:("optimistic-ethereum")}, 
        object::pair{std:("swap"), object{
            object::pair{std:("cdpSupported"), false}
        }}
    }}, 
    object::pair{std:("scroll"), object{
        object::pair{std:("name"), std:("Scroll")}, 
        object::pair{std:("chain"), scroll}, 
        object::pair{std:("rpcUrl"), [=](auto alchemyKey) mutable
        {
            return std:("https://scroll-mainnet.g.alchemy.com/v2/") + alchemyKey + string_empty;
        }
        }, 
        object::pair{std:("explorerUrl"), std:("https://scrollscan.com")}, 
        object::pair{std:("nativeToken"), object{
            object::pair{std:("symbol"), std:("ETH")}, 
            object::pair{std:("name"), std:("Ethereum")}, 
            object::pair{std:("coingeckoId"), std:("ethereum")}, 
            object::pair{std:("decimals"), 18}
        }}, 
        object::pair{std:("coingeckoPlatform"), std:("scroll")}, 
        object::pair{std:("swap"), object{
            object::pair{std:("cdpSupported"), false}
        }}
    }}, 
    object::pair{std:("base-sepolia"), object{
        object::pair{std:("name"), std:("Base Sepolia")}, 
        object::pair{std:("chain"), baseSepolia}, 
        object::pair{std:("rpcUrl"), [=](auto alchemyKey) mutable
        {
            return std:("https://base-sepolia.g.alchemy.com/v2/") + alchemyKey + string_empty;
        }
        }, 
        object::pair{std:("explorerUrl"), std:("https://sepolia.basescan.org")}, 
        object::pair{std:("nativeToken"), object{
            object::pair{std:("symbol"), std:("ETH")}, 
            object::pair{std:("name"), std:("Ethereum")}, 
            object::pair{std:("coingeckoId"), std:("ethereum")}, 
            object::pair{std:("decimals"), 18}
        }}, 
        object::pair{std:("coingeckoPlatform"), std:("base")}, 
        object::pair{std:("swap"), object{
            object::pair{std:("cdpSupported"), false}
        }}
    }}, 
    object::pair{std:("ethereum-sepolia"), object{
        object::pair{std:("name"), std:("Ethereum Sepolia")}, 
        object::pair{std:("chain"), sepolia}, 
        object::pair{std:("rpcUrl"), [=](auto alchemyKey) mutable
        {
            return std:("https://eth-sepolia.g.alchemy.com/v2/") + alchemyKey + string_empty;
        }
        }, 
        object::pair{std:("explorerUrl"), std:("https://sepolia.etherscan.io")}, 
        object::pair{std:("nativeToken"), object{
            object::pair{std:("symbol"), std:("ETH")}, 
            object::pair{std:("name"), std:("Ethereum")}, 
            object::pair{std:("coingeckoId"), std:("ethereum")}, 
            object::pair{std:("decimals"), 18}
        }}, 
        object::pair{std:("coingeckoPlatform"), std:("ethereum")}, 
        object::pair{std:("swap"), object{
            object::pair{std:("cdpSupported"), false}
        }}
    }}
};
array<SupportedNetwork> MAINNET_NETWORKS = array<SupportedNetwork>{ std:("base"), std:("ethereum"), std:("polygon"), std:("arbitrum"), std:("optimism"), std:("scroll") };
array<SupportedNetwork> TESTNET_NETWORKS = array<SupportedNetwork>{ std:("base-sepolia"), std:("ethereum-sepolia") };
array<SupportedNetwork> ALL_NETWORKS = as<array<SupportedNetwork>>(Object->keys(CHAIN_CONFIGS));
string NATIVE_TOKEN_ADDRESS = std:("0xEeeeeEeeeEeEeeEeEeEeeEEEeeeeEeeeeeeeEEeE");
Record<string, string> UNISWAP_V3_ROUTER = object{
    object::pair{std:("ethereum"), std:("0xE592427A0AEce92De3Edee1F18E0157C05861564")}, 
    object::pair{std:("polygon"), std:("0xE592427A0AEce92De3Edee1F18E0157C05861564")}, 
    object::pair{std:("arbitrum"), std:("0xE592427A0AEce92De3Edee1F18E0157C05861564")}, 
    object::pair{std:("optimism"), std:("0xE592427A0AEce92De3Edee1F18E0157C05861564")}, 
    object::pair{std:("base"), std:("0x2626664c2603336E57B271c5C0b26F421741e481")}
};
Record<string, string> UNISWAP_V3_QUOTER = object{
    object::pair{std:("ethereum"), std:("0x61fFE014bA17989E743c5F6cB21bF9697530B21e")}, 
    object::pair{std:("polygon"), std:("0x61fFE014bA17989E743c5F6cB21bF9697530B21e")}, 
    object::pair{std:("arbitrum"), std:("0x61fFE014bA17989E743c5F6cB21bF9697530B21e")}, 
    object::pair{std:("optimism"), std:("0x61fFE014bA17989E743c5F6cB21bF9697530B21e")}, 
    object::pair{std:("base"), std:("0x3d4e44Eb1374240CE5F1B871ab261CD16335B76a")}
};
Record<string, string> WRAPPED_NATIVE_TOKEN = object{
    object::pair{std:("ethereum"), std:("0xC02aaA39b223FE8D0A0e5C4F27eAD9083C756Cc2")}, 
    object::pair{std:("polygon"), std:("0x0d500B1d8E8eF31E21C99d1Db9A6444d3ADf1270")}, 
    object::pair{std:("arbitrum"), std:("0x82aF49447D8a07e3bd95BD0d56f35241523fBab1")}, 
    object::pair{std:("optimism"), std:("0x4200000000000000000000000000000000000006")}, 
    object::pair{std:("base"), std:("0x4200000000000000000000000000000000000006")}
};
object UNISWAP_POOL_FEES = object{
    object::pair{std:("LOW"), 500}, 
    object::pair{std:("MEDIUM"), 3000}, 
    object::pair{std:("HIGH"), 10000}
};

void Main(void)
{
}

MAIN
