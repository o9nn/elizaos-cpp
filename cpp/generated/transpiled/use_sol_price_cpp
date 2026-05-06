#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/client/src/hooks/use-sol-price.h"

any useSolPrice()
{
    return useQuery(object{
        object::pair{std::string("queryKey"), array<string>{ std::string("solPrice") }}, 
        object::pair{std::string("queryFn"), fetchSolPrice}, 
        object::pair{std::string("refetchInterval"), 60000}, 
        object::pair{std::string("staleTime"), 30000}
    });
};


std::function<std::shared_ptr<Promise<double>>()> fetchSolPrice = [=]() mutable
{
    try
    {
        try
        {
            auto response = std::async([=]() { fetch(string_empty + env->apiUrl + std::string("/api/sol-price")); });
            if (response->ok) {
                auto data = as<std::shared_ptr<SolPriceResponse>>((std::async([=]() { response->json(); })));
                if (AND((data), (data->price))) {
                    return Number(data->price);
                }
            }
        }
        catch (const any& error)
        {
            console->error(std::string("Error fetching SOL price from API:"), error);
        }
        try
        {
            auto response = std::async([=]() { fetch(std::string("https://api.coingecko.com/api/v3/simple/price?ids=solana&vs_currencies=usd")); });
            auto data = as<std::shared_ptr<CoinGeckoResponse>>((std::async([=]() { response->json(); })));
            if (AND((AND((data), (data->solana))), (data->solana["usd"]))) {
                return Number(data->solana["usd"]);
            }
        }
        catch (const any& error)
        {
            console->error(std::string("Error fetching SOL price from CoinGecko:"), error);
        }
        try
        {
            auto response = std::async([=]() { fetch(std::string("https://api.binance.com/api/v3/ticker/price?symbol=SOLUSDT")); });
            auto data = as<std::shared_ptr<BinanceResponse>>((std::async([=]() { response->json(); })));
            if (AND((data), (data->price))) {
                return Number(data->price);
            }
        }
        catch (const any& error)
        {
            console->error(std::string("Error fetching SOL price from Binance:"), error);
        }
        return 135;
    }
    catch (const any& error)
    {
        console->error(std::string("Error fetching SOL price:"), error);
        return 135;
    }
};

void Main(void)
{
}

MAIN
