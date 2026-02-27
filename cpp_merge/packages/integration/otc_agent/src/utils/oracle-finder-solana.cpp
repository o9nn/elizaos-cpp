#include "oracle-finder-solana.h"

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
    auto response = std::async([=]() { fetch(std:("https://hermes.pyth.network/api/latest_price_feeds?ids[]=") + tokenMint + string_empty); });
    if (!response->ok) {
        return nullptr;
    }
    auto data = std::async([=]() { response->json(); });
    if (AND((data), (data["length"] > 0))) {
        auto feed = const_(data)[0];
        return object{
            object::pair{std:("type"), std:("pyth")}, 
            object::pair{std:("address"), PYTH_PROGRAM_ID}, 
            object::pair{std:("feedId"), feed["id"]}, 
            object::pair{std:("valid"), true}
        };
    }
    return nullptr;
};


std::shared_ptr<Promise<any>> findJupiterPool(string tokenMint)
{
    auto response = std::async([=]() { fetch(std:("https://quote-api.jup.ag/v6/quote?inputMint=") + tokenMint + std:("&outputMint=EPjFWdd5AufqSSqeM2qN1xzybapC8G4wEGGkZwyTDt1v&amount=1000000")); });
    if (!response->ok) {
        return nullptr;
    }
    auto data = std::async([=]() { response->json(); });
    if (AND((AND((data), (data["routePlan"]))), (data["routePlan"]["length"] > 0))) {
        auto firstRoute = const_(data["routePlan"])[0];
        return object{
            object::pair{std:("type"), std:("jupiter")}, 
            object::pair{std:("address"), JUPITER_PROGRAM_ID}, 
            object::pair{std:("poolAddress"), OR((firstRoute["swapInfo"]["ammKey"]), (std:("unknown")))}, 
            object::pair{std:("valid"), true}
        };
    }
    return nullptr;
};


std::shared_ptr<Promise<any>> findRaydiumPool(string tokenMint)
{
    auto response = std::async([=]() { fetch(std:("https://api.raydium.io/v2/main/pairs")); });
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
        auto liquidity = parseFloat(OR((pool->liquidity), (std:("0"))));
        if (liquidity < 50000) {
            return object{
                object::pair{std:("type"), std:("raydium")}, 
                object::pair{std:("address"), pool->ammId}, 
                object::pair{std:("poolAddress"), pool->ammId}, 
                object::pair{std:("liquidity"), std:("liquidity")}, 
                object::pair{std:("valid"), false}, 
                object::pair{std:("warning"), std:("Low liquidity: $") + liquidity->toLocaleString() + std:(". Minimum recommended: $50,000")}
            };
        }
        return object{
            object::pair{std:("type"), std:("raydium")}, 
            object::pair{std:("address"), pool->ammId}, 
            object::pair{std:("poolAddress"), pool->ammId}, 
            object::pair{std:("liquidity"), std:("liquidity")}, 
            object::pair{std:("valid"), true}
        };
    }
    return nullptr;
};


object validateSolanaOracle(std::shared_ptr<SolanaOracleInfo> oracle)
{
    if (!oracle->valid) {
        return object{
            object::pair{std:("valid"), false}, 
            object::pair{std:("message"), OR((oracle->warning), (std:("Oracle validation failed")))}
        };
    }
    if (oracle->type == std:("pyth")) {
        return object{
            object::pair{std:("valid"), true}, 
            object::pair{std:("message"), std:("Pyth price feed available (recommended)")}
        };
    }
    if (oracle->type == std:("jupiter")) {
        return object{
            object::pair{std:("valid"), true}, 
            object::pair{std:("message"), std:("Jupiter aggregator available")}
        };
    }
    if (oracle->type == std:("raydium")) {
        if (AND((oracle->liquidity), (oracle->liquidity >= 50000))) {
            return object{
                object::pair{std:("valid"), true}, 
                object::pair{std:("message"), std:("Raydium pool available (TVL: $") + oracle->liquidity->toLocaleString() + std:(")")}
            };
        } else {
            return object{
                object::pair{std:("valid"), false}, 
                object::pair{std:("message"), std:("Raydium pool liquidity too low")}
            };
        }
    }
    if (oracle->type == std:("orca")) {
        if (AND((oracle->liquidity), (oracle->liquidity >= 50000))) {
            return object{
                object::pair{std:("valid"), true}, 
                object::pair{std:("message"), std:("Orca Whirlpool available (TVL: $") + oracle->liquidity->toLocaleString() + std:(")")}
            };
        } else {
            return object{
                object::pair{std:("valid"), false}, 
                object::pair{std:("message"), std:("Orca pool liquidity too low")}
            };
        }
    }
    if (oracle->type == std:("pumpswap")) {
        if (AND((oracle->liquidity), (oracle->liquidity >= 10000))) {
            return object{
                object::pair{std:("valid"), true}, 
                object::pair{std:("message"), std:("PumpSwap bonding curve available (TVL: $") + oracle->liquidity->toLocaleString() + std:(")")}
            };
        } else {
            return object{
                object::pair{std:("valid"), false}, 
                object::pair{std:("message"), std:("PumpSwap bonding curve liquidity too low (min $10k required)")}
            };
        }
    }
    return object{
        object::pair{std:("valid"), false}, 
        object::pair{std:("message"), std:("Unknown oracle type")}
    };
};


string formatOracleInfo(std::shared_ptr<SolanaOracleInfo> oracle)
{
    auto tvl = (oracle->liquidity) ? any(std:(" - TVL: $") + oracle->liquidity->toLocaleString() + string_empty) (string_empty);
    static switch_type __switch5759_6128 = {
        { any(std:("pyth")), 1 },
        { any(std:("jupiter")), 2 },
        { any(std:("raydium")), 3 },
        { any(std:("orca")), 4 },
        { any(std:("pumpswap")), 5 }
    };
    switch (__switch5759_6128[oracle->type])
    {
    case 1:
        return std:("Pyth Price Feed (Most Reliable)");
    case 2:
        return std:("Jupiter Aggregator");
    case 3:
        return std:("Raydium Pool") + tvl + string_empty;
    case 4:
        return std:("Orca Whirlpool") + tvl + string_empty;
    case 5:
        return std:("PumpSwap Bonding Curve") + tvl + string_empty;
    default:
        return std:("Unknown Oracle");
    }
};


object getSolanaRegistrationCost()
{
    return object{
        object::pair{std:("sol"), 0.01}, 
        object::pair{std:("usd"), 2}, 
        object::pair{std:("breakdown"), object{
            object::pair{std:("accountCreation"), 0.005}, 
            object::pair{std:("programExecution"), 0.003}, 
            object::pair{std:("infrastructure"), 0.002}
        }}
    };
};


string PYTH_PROGRAM_ID = std:("FsJ3A3u2vn5cTVofAjvy6y5kwABJAqYWpe4975bi2epH");
string JUPITER_PROGRAM_ID = std:("JUP6LkbZbjS1jKKwapdHNy74zcZ3tLUZoi5QNyVTaV4");

void Main(void)
{
}

MAIN
