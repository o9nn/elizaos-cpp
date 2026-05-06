#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/constants/chains.h"

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
    return (config) ? any(config->rpcUrl(alchemyKey)) : any(nullptr);
};


any getExplorerUrl(string network)
{
    auto config = getChainConfig(network);
    return OR((config->explorerUrl), (nullptr));
};


any getTxExplorerUrl(string network, string txHash)
{
    auto explorerUrl = getExplorerUrl(network);
    return (explorerUrl) ? any(string_empty + explorerUrl + std::string("/tx/") + txHash + string_empty) : any(nullptr);
};


any getAddressExplorerUrl(string network, string address)
{
    auto explorerUrl = getExplorerUrl(network);
    return (explorerUrl) ? any(string_empty + explorerUrl + std::string("/address/") + address + string_empty) : any(nullptr);
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
    if ((new RegExp(std::string("^0x[a-fA-F0-9]{40}")))->test(token)) {
        return token;
    }
    return NATIVE_TOKEN_ADDRESS;
};


Record<SupportedNetwork, std::shared_ptr<ChainConfig>> CHAIN_CONFIGS = object{
    object::pair{std::string("base"), object{
        object::pair{std::string("name"), std::string("Base")}, 
        object::pair{std::string("chain"), base}, 
        object::pair{std::string("rpcUrl"), [=](auto alchemyKey) mutable
        {
            return std::string("https://base-mainnet.g.alchemy.com/v2/") + alchemyKey + string_empty;
        }
        }, 
        object::pair{std::string("explorerUrl"), std::string("https://basescan.org")}, 
        object::pair{std::string("nativeToken"), object{
            object::pair{std::string("symbol"), std::string("ETH")}, 
            object::pair{std::string("name"), std::string("Ethereum")}, 
            object::pair{std::string("coingeckoId"), std::string("ethereum")}, 
            object::pair{std::string("decimals"), 18}
        }}, 
        object::pair{std::string("coingeckoPlatform"), std::string("base")}, 
        object::pair{std::string("swap"), object{
            object::pair{std::string("cdpSupported"), true}
        }}
    }}, 
    object::pair{std::string("ethereum"), object{
        object::pair{std::string("name"), std::string("Ethereum")}, 
        object::pair{std::string("chain"), mainnet}, 
        object::pair{std::string("rpcUrl"), [=](auto alchemyKey) mutable
        {
            return std::string("https://eth-mainnet.g.alchemy.com/v2/") + alchemyKey + string_empty;
        }
        }, 
        object::pair{std::string("explorerUrl"), std::string("https://etherscan.io")}, 
        object::pair{std::string("nativeToken"), object{
            object::pair{std::string("symbol"), std::string("ETH")}, 
            object::pair{std::string("name"), std::string("Ethereum")}, 
            object::pair{std::string("coingeckoId"), std::string("ethereum")}, 
            object::pair{std::string("decimals"), 18}
        }}, 
        object::pair{std::string("coingeckoPlatform"), std::string("ethereum")}, 
        object::pair{std::string("swap"), object{
            object::pair{std::string("cdpSupported"), true}
        }}
    }}, 
    object::pair{std::string("polygon"), object{
        object::pair{std::string("name"), std::string("Polygon")}, 
        object::pair{std::string("chain"), polygon}, 
        object::pair{std::string("rpcUrl"), [=](auto alchemyKey) mutable
        {
            return std::string("https://polygon-mainnet.g.alchemy.com/v2/") + alchemyKey + string_empty;
        }
        }, 
        object::pair{std::string("explorerUrl"), std::string("https://polygonscan.com")}, 
        object::pair{std::string("nativeToken"), object{
            object::pair{std::string("symbol"), std::string("POL")}, 
            object::pair{std::string("name"), std::string("Polygon")}, 
            object::pair{std::string("coingeckoId"), std::string("polygon-ecosystem-token")}, 
            object::pair{std::string("decimals"), 18}
        }}, 
        object::pair{std::string("coingeckoPlatform"), std::string("polygon-pos")}, 
        object::pair{std::string("swap"), object{
            object::pair{std::string("cdpSupported"), false}
        }}
    }}, 
    object::pair{std::string("arbitrum"), object{
        object::pair{std::string("name"), std::string("Arbitrum")}, 
        object::pair{std::string("chain"), arbitrum}, 
        object::pair{std::string("rpcUrl"), [=](auto alchemyKey) mutable
        {
            return std::string("https://arb-mainnet.g.alchemy.com/v2/") + alchemyKey + string_empty;
        }
        }, 
        object::pair{std::string("explorerUrl"), std::string("https://arbiscan.io")}, 
        object::pair{std::string("nativeToken"), object{
            object::pair{std::string("symbol"), std::string("ETH")}, 
            object::pair{std::string("name"), std::string("Ethereum")}, 
            object::pair{std::string("coingeckoId"), std::string("ethereum")}, 
            object::pair{std::string("decimals"), 18}
        }}, 
        object::pair{std::string("coingeckoPlatform"), std::string("arbitrum-one")}, 
        object::pair{std::string("swap"), object{
            object::pair{std::string("cdpSupported"), false}
        }}
    }}, 
    object::pair{std::string("optimism"), object{
        object::pair{std::string("name"), std::string("Optimism")}, 
        object::pair{std::string("chain"), optimism}, 
        object::pair{std::string("rpcUrl"), [=](auto alchemyKey) mutable
        {
            return std::string("https://opt-mainnet.g.alchemy.com/v2/") + alchemyKey + string_empty;
        }
        }, 
        object::pair{std::string("explorerUrl"), std::string("https://optimistic.etherscan.io")}, 
        object::pair{std::string("nativeToken"), object{
            object::pair{std::string("symbol"), std::string("ETH")}, 
            object::pair{std::string("name"), std::string("Ethereum")}, 
            object::pair{std::string("coingeckoId"), std::string("ethereum")}, 
            object::pair{std::string("decimals"), 18}
        }}, 
        object::pair{std::string("coingeckoPlatform"), std::string("optimistic-ethereum")}, 
        object::pair{std::string("swap"), object{
            object::pair{std::string("cdpSupported"), false}
        }}
    }}, 
    object::pair{std::string("scroll"), object{
        object::pair{std::string("name"), std::string("Scroll")}, 
        object::pair{std::string("chain"), scroll}, 
        object::pair{std::string("rpcUrl"), [=](auto alchemyKey) mutable
        {
            return std::string("https://scroll-mainnet.g.alchemy.com/v2/") + alchemyKey + string_empty;
        }
        }, 
        object::pair{std::string("explorerUrl"), std::string("https://scrollscan.com")}, 
        object::pair{std::string("nativeToken"), object{
            object::pair{std::string("symbol"), std::string("ETH")}, 
            object::pair{std::string("name"), std::string("Ethereum")}, 
            object::pair{std::string("coingeckoId"), std::string("ethereum")}, 
            object::pair{std::string("decimals"), 18}
        }}, 
        object::pair{std::string("coingeckoPlatform"), std::string("scroll")}, 
        object::pair{std::string("swap"), object{
            object::pair{std::string("cdpSupported"), false}
        }}
    }}, 
    object::pair{std::string("base-sepolia"), object{
        object::pair{std::string("name"), std::string("Base Sepolia")}, 
        object::pair{std::string("chain"), baseSepolia}, 
        object::pair{std::string("rpcUrl"), [=](auto alchemyKey) mutable
        {
            return std::string("https://base-sepolia.g.alchemy.com/v2/") + alchemyKey + string_empty;
        }
        }, 
        object::pair{std::string("explorerUrl"), std::string("https://sepolia.basescan.org")}, 
        object::pair{std::string("nativeToken"), object{
            object::pair{std::string("symbol"), std::string("ETH")}, 
            object::pair{std::string("name"), std::string("Ethereum")}, 
            object::pair{std::string("coingeckoId"), std::string("ethereum")}, 
            object::pair{std::string("decimals"), 18}
        }}, 
        object::pair{std::string("coingeckoPlatform"), std::string("base")}, 
        object::pair{std::string("swap"), object{
            object::pair{std::string("cdpSupported"), false}
        }}
    }}, 
    object::pair{std::string("ethereum-sepolia"), object{
        object::pair{std::string("name"), std::string("Ethereum Sepolia")}, 
        object::pair{std::string("chain"), sepolia}, 
        object::pair{std::string("rpcUrl"), [=](auto alchemyKey) mutable
        {
            return std::string("https://eth-sepolia.g.alchemy.com/v2/") + alchemyKey + string_empty;
        }
        }, 
        object::pair{std::string("explorerUrl"), std::string("https://sepolia.etherscan.io")}, 
        object::pair{std::string("nativeToken"), object{
            object::pair{std::string("symbol"), std::string("ETH")}, 
            object::pair{std::string("name"), std::string("Ethereum")}, 
            object::pair{std::string("coingeckoId"), std::string("ethereum")}, 
            object::pair{std::string("decimals"), 18}
        }}, 
        object::pair{std::string("coingeckoPlatform"), std::string("ethereum")}, 
        object::pair{std::string("swap"), object{
            object::pair{std::string("cdpSupported"), false}
        }}
    }}
};
array<SupportedNetwork> MAINNET_NETWORKS = array<SupportedNetwork>{ std::string("base"), std::string("ethereum"), std::string("polygon"), std::string("arbitrum"), std::string("optimism"), std::string("scroll") };
array<SupportedNetwork> TESTNET_NETWORKS = array<SupportedNetwork>{ std::string("base-sepolia"), std::string("ethereum-sepolia") };
array<SupportedNetwork> ALL_NETWORKS = as<array<SupportedNetwork>>(Object->keys(CHAIN_CONFIGS));
string NATIVE_TOKEN_ADDRESS = std::string("0xEeeeeEeeeEeEeeEeEeEeeEEEeeeeEeeeeeeeEEeE");
Record<string, string> UNISWAP_V3_ROUTER = object{
    object::pair{std::string("ethereum"), std::string("0xE592427A0AEce92De3Edee1F18E0157C05861564")}, 
    object::pair{std::string("polygon"), std::string("0xE592427A0AEce92De3Edee1F18E0157C05861564")}, 
    object::pair{std::string("arbitrum"), std::string("0xE592427A0AEce92De3Edee1F18E0157C05861564")}, 
    object::pair{std::string("optimism"), std::string("0xE592427A0AEce92De3Edee1F18E0157C05861564")}, 
    object::pair{std::string("base"), std::string("0x2626664c2603336E57B271c5C0b26F421741e481")}
};
Record<string, string> UNISWAP_V3_QUOTER = object{
    object::pair{std::string("ethereum"), std::string("0x61fFE014bA17989E743c5F6cB21bF9697530B21e")}, 
    object::pair{std::string("polygon"), std::string("0x61fFE014bA17989E743c5F6cB21bF9697530B21e")}, 
    object::pair{std::string("arbitrum"), std::string("0x61fFE014bA17989E743c5F6cB21bF9697530B21e")}, 
    object::pair{std::string("optimism"), std::string("0x61fFE014bA17989E743c5F6cB21bF9697530B21e")}, 
    object::pair{std::string("base"), std::string("0x3d4e44Eb1374240CE5F1B871ab261CD16335B76a")}
};
Record<string, string> WRAPPED_NATIVE_TOKEN = object{
    object::pair{std::string("ethereum"), std::string("0xC02aaA39b223FE8D0A0e5C4F27eAD9083C756Cc2")}, 
    object::pair{std::string("polygon"), std::string("0x0d500B1d8E8eF31E21C99d1Db9A6444d3ADf1270")}, 
    object::pair{std::string("arbitrum"), std::string("0x82aF49447D8a07e3bd95BD0d56f35241523fBab1")}, 
    object::pair{std::string("optimism"), std::string("0x4200000000000000000000000000000000000006")}, 
    object::pair{std::string("base"), std::string("0x4200000000000000000000000000000000000006")}
};
object UNISWAP_POOL_FEES = object{
    object::pair{std::string("LOW"), 500}, 
    object::pair{std::string("MEDIUM"), 3000}, 
    object::pair{std::string("HIGH"), 10000}
};

void Main(void)
{
}

MAIN
