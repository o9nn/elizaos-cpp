#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/plugin-coinmarketcap/src/actions/getPrice/service.h"

string BASE_URL = std:("https://pro-api.coinmarketcap.com/v1");
std::function<object(string)> createPriceService = [=](auto apiKey) mutable
{
    shared client = axios->create(object{
        object::pair{std:("baseURL"), BASE_URL}, 
        object::pair{std:("headers"), object{
            object::pair{std:("X-CMC_PRO_API_KEY"), apiKey}, 
            object::pair{std:("Accept"), std:("application/json")}
        }}
    });
    auto getPrice = [=](auto symbol, auto currency) mutable
    {
        auto normalizedSymbol = symbol->toUpperCase()->trim();
        auto normalizedCurrency = currency->toUpperCase()->trim();
        try
        {
            auto response = std::async([=]() { client->get<std::shared_ptr<ApiResponse>>(std:("/cryptocurrency/quotes/latest"), object{
                object::pair{std:("params"), object{
                    object::pair{std:("symbol"), normalizedSymbol}, 
                    object::pair{std:("convert"), normalizedCurrency}
                }}
            }); });
            console->log(std:("API Response:"), JSON->stringify(response->data, nullptr, 2));
            auto symbolData = const_(response->data->data)[normalizedSymbol];
            if (!symbolData) {
                throw any(std::make_shared<Error>(std:("No data found for symbol: ") + normalizedSymbol + string_empty));
            }
            auto quoteData = const_(symbolData->quote)[normalizedCurrency];
            if (!quoteData) {
                throw any(std::make_shared<Error>(std:("No quote data found for currency: ") + normalizedCurrency + string_empty));
            }
            return object{
                object::pair{std:("price"), quoteData->price}, 
                object::pair{std:("marketCap"), quoteData->market_cap}, 
                object::pair{std:("volume24h"), quoteData->volume_24h}, 
                object::pair{std:("percentChange24h"), quoteData->percent_change_24h}
            };
        }
        catch (const any& error)
        {
            if (axios->isAxiosError(error)) {
                auto errorMessage = OR((error["response"]["data"]["status"]["error_message"]), (error["message"]));
                console->error(std:("API Error:"), errorMessage);
                throw any(std::make_shared<Error>(std:("API Error: ") + errorMessage + string_empty));
            }
            throw any(error);
        }
    };
    return object{
        object::pair{std:("getPrice"), std:("getPrice")}
    };
};

void Main(void)
{
}

MAIN
