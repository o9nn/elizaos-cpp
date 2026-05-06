#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-world/src/app/api/tokens/route.h"

any GET()
{
    try
    {
        auto cached = getCachedData();
        if (AND((cached), (!shouldRefreshCache()))) {
            return std::make_shared<NextResponse>(JSON->stringify(cached->holdings), object{
                object::pair{std::string("headers"), object{
                    object::pair{std::string("Content-Type"), std::string("application/json")}, 
                    object::pair{std::string("Cache-Control"), std::string("public, max-age=60")}
                }}
            });
        }
        auto tokenBalances = std::async([=]() { getTokenData(connection); });
        auto marketData = std::async([=]() { fetchDexScreenerData(tokenBalances->map([=](auto t) mutable
        {
            return t["mint"];
        }
        )); });
        auto holdings = std::async([=]() { calculateHoldings(connection, tokenBalances, marketData, WALLET_ADDRESS); });
        setCachedData(holdings);
        return std::make_shared<NextResponse>(JSON->stringify(holdings), object{
            object::pair{std::string("headers"), object{
                object::pair{std::string("Content-Type"), std::string("application/json")}, 
                object::pair{std::string("Cache-Control"), std::string("public, max-age=60")}
            }}
        });
    }
    catch (const any& error)
    {
        console->error(std::string("API Error:"), error);
        auto cached = getCachedData();
        if (cached) {
            return std::make_shared<NextResponse>(JSON->stringify(cached->holdings), object{
                object::pair{std::string("headers"), object{
                    object::pair{std::string("Content-Type"), std::string("application/json")}, 
                    object::pair{std::string("Cache-Control"), std::string("public, max-age=60")}
                }}
            });
        }
        return NextResponse->error();
    }
};


string WALLET_ADDRESS = std::string("AM84n1iLdxgVTAyENBcLdjXoyvjentTbu5Q6EpKV1PeG");
any connection = std::make_shared<Connection>(OR((process->env->NEXT_PUBLIC_RPC_ENDPOINT), (std::string("https://solana-mainnet.g.alchemy.com/v2/your-api-key"))), std::string("confirmed"));

void Main(void)
{
}

MAIN
