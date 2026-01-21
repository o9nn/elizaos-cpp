#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otc-agent/src/utils/oracle-finder-solana.h"

std::shared_ptr<Promise<any>> findSolanaOracle(string tokenMint)
{
    auto pythFeed = std::async([=]() { findPythFeed(tokenMint); });
    if (pythFeed) {
        return pythFeed;
    }
    auto jupiterPool = std::async([=]() { findJupiterPool(tokenMint); });
    if (jupiterPool) {
        return jupiterPool;
    }
    auto raydiumPool = std::async([=]() { findRaydiumPool(tokenMint); });
    if (raydiumPool) {
        return raydiumPool;
    }
    return nullptr;
};


std::shared_ptr<Promise<any>> findPythFeed(string tokenMint)
{
    auto response = std::async([=]() { fetch(std::string("https://hermes.pyth.network/api/latest_price_feeds?ids[]=") + tokenMint + string_empty); });
    if (!response->ok) {
        return nullptr;
    }
    auto data = std::async([=]() { response->json(); });
    if (AND((data), (data["length"] > 0))) {
        auto feed = const_(data)[0];
        return object{
            object::pair{std::string("type"), std::string("pyth")}, 
            object::pair{std::string("address"), PYTH_PROGRAM_ID}, 
            object::pair{std::string("feedId"), feed["id"]}, 
            object::pair{std::string("valid"), true}
        };
    }
    return nullptr;
};


std::shared_ptr<Promise<any>> findJupiterPool(string tokenMint)
{
    auto response = std::async([=]() { fetch(std::string("https://quote-api.jup.ag/v6/quote?inputMint=") + tokenMint + std::string("&outputMint=EPjFWdd5AufqSSqeM2qN1xzybapC8G4wEGGkZwyTDt1v&amount=1000000")); });
    if (!response->ok) {
        return nullptr;
    }
    auto data = std::async([=]() { response->json(); });
    if (AND((AND((data), (data["routePlan"]))), (data["routePlan"]["length"] > 0))) {
        auto firstRoute = const_(data["routePlan"])[0];
        return object{
            object::pair{std::string("type"), std::string("jupiter")}, 
            object::pair{std::string("address"), JUPITER_PROGRAM_ID}, 
            object::pair{std::string("poolAddress"), OR((firstRoute["swapInfo"]["ammKey"]), (std::string("unknown")))}, 
            object::pair{std::string("valid"), true}
        };
    }
    return nullptr;
};


std::shared_ptr<Promise<any>> findRaydiumPool(string tokenMint)
{
    auto response = std::async([=]() { fetch(std::string("https://api.raydium.io/v2/main/pairs")); });
    if (!response->ok) {
        return nullptr;
    }
    auto pools = std::async([=]() { response->json(); });
    auto pool = pools->find([=](auto p) mutable
    {
        return OR((p->baseMint == tokenMint), (p->quoteMint == tokenMint));
    }
    );
    if (pool) {
        auto liquidity = parseFloat(OR((pool->liquidity), (std::string("0"))));
        if (liquidity < 50000) {
            return object{
                object::pair{std::string("type"), std::string("raydium")}, 
                object::pair{std::string("address"), pool->ammId}, 
                object::pair{std::string("poolAddress"), pool->ammId}, 
                object::pair{std::string("liquidity"), std::string("liquidity")}, 
                object::pair{std::string("valid"), false}, 
                object::pair{std::string("warning"), std::string("Low liquidity: $") + liquidity->toLocaleString() + std::string(". Minimum recommended: $50,000")}
            };
        }
        return object{
            object::pair{std::string("type"), std::string("raydium")}, 
            object::pair{std::string("address"), pool->ammId}, 
            object::pair{std::string("poolAddress"), pool->ammId}, 
            object::pair{std::string("liquidity"), std::string("liquidity")}, 
            object::pair{std::string("valid"), true}
        };
    }
    return nullptr;
};


object validateSolanaOracle(std::shared_ptr<SolanaOracleInfo> oracle)
{
    if (!oracle->valid) {
        return object{
            object::pair{std::string("valid"), false}, 
            object::pair{std::string("message"), OR((oracle->warning), (std::string("Oracle validation failed")))}
        };
    }
    if (oracle->type == std::string("pyth")) {
        return object{
            object::pair{std::string("valid"), true}, 
            object::pair{std::string("message"), std::string("Pyth price feed available (recommended)")}
        };
    }
    if (oracle->type == std::string("jupiter")) {
        return object{
            object::pair{std::string("valid"), true}, 
            object::pair{std::string("message"), std::string("Jupiter aggregator available")}
        };
    }
    if (oracle->type == std::string("raydium")) {
        if (AND((oracle->liquidity), (oracle->liquidity >= 50000))) {
            return object{
                object::pair{std::string("valid"), true}, 
                object::pair{std::string("message"), std::string("Raydium pool available (TVL: $") + oracle->liquidity->toLocaleString() + std::string(")")}
            };
        } else {
            return object{
                object::pair{std::string("valid"), false}, 
                object::pair{std::string("message"), std::string("Raydium pool liquidity too low")}
            };
        }
    }
    if (oracle->type == std::string("orca")) {
        if (AND((oracle->liquidity), (oracle->liquidity >= 50000))) {
            return object{
                object::pair{std::string("valid"), true}, 
                object::pair{std::string("message"), std::string("Orca Whirlpool available (TVL: $") + oracle->liquidity->toLocaleString() + std::string(")")}
            };
        } else {
            return object{
                object::pair{std::string("valid"), false}, 
                object::pair{std::string("message"), std::string("Orca pool liquidity too low")}
            };
        }
    }
    if (oracle->type == std::string("pumpswap")) {
        if (AND((oracle->liquidity), (oracle->liquidity >= 10000))) {
            return object{
                object::pair{std::string("valid"), true}, 
                object::pair{std::string("message"), std::string("PumpSwap bonding curve available (TVL: $") + oracle->liquidity->toLocaleString() + std::string(")")}
            };
        } else {
            return object{
                object::pair{std::string("valid"), false}, 
                object::pair{std::string("message"), std::string("PumpSwap bonding curve liquidity too low (min $10k required)")}
            };
        }
    }
    return object{
        object::pair{std::string("valid"), false}, 
        object::pair{std::string("message"), std::string("Unknown oracle type")}
    };
};


string formatOracleInfo(std::shared_ptr<SolanaOracleInfo> oracle)
{
    auto tvl = (oracle->liquidity) ? any(std::string(" - TVL: $") + oracle->liquidity->toLocaleString() + string_empty) : any(string_empty);
    static switch_type __switch5759_6128 = {
        { any(std::string("pyth")), 1 },
        { any(std::string("jupiter")), 2 },
        { any(std::string("raydium")), 3 },
        { any(std::string("orca")), 4 },
        { any(std::string("pumpswap")), 5 }
    };
    switch (__switch5759_6128[oracle->type])
    {
    case 1:
        return std::string("Pyth Price Feed (Most Reliable)");
    case 2:
        return std::string("Jupiter Aggregator");
    case 3:
        return std::string("Raydium Pool") + tvl + string_empty;
    case 4:
        return std::string("Orca Whirlpool") + tvl + string_empty;
    case 5:
        return std::string("PumpSwap Bonding Curve") + tvl + string_empty;
    default:
        return std::string("Unknown Oracle");
    }
};


object getSolanaRegistrationCost()
{
    return object{
        object::pair{std::string("sol"), 0.01}, 
        object::pair{std::string("usd"), 2}, 
        object::pair{std::string("breakdown"), object{
            object::pair{std::string("accountCreation"), 0.005}, 
            object::pair{std::string("programExecution"), 0.003}, 
            object::pair{std::string("infrastructure"), 0.002}
        }}
    };
};


string PYTH_PROGRAM_ID = std::string("FsJ3A3u2vn5cTVofAjvy6y5kwABJAqYWpe4975bi2epH");
string JUPITER_PROGRAM_ID = std::string("JUP6LkbZbjS1jKKwapdHNy74zcZ3tLUZoi5QNyVTaV4");

void Main(void)
{
}

MAIN
