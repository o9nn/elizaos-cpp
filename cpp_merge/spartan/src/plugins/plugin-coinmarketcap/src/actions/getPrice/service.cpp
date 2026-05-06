#include "service.hpp"
#include <string>

string BASE_URL = std::string("https://pro-api.coinmarketcap.com/v1");
std::function<object(string)> createPriceService = [=](auto apiKey) mutable
{
    shared client = axios->create(object{
        object::pair{std::string("baseURL"), BASE_URL}, 
        object::pair{std::string("headers"), object{
            object::pair{std::string("X-CMC_PRO_API_KEY"), apiKey}, 
            object::pair{std::string("Accept"), std::string("application/json")}
        }}
    });
    auto getPrice = [=](auto symbol, auto currency) mutable
    {
        auto normalizedSymbol = symbol->toUpperCase()->trim();
        auto normalizedCurrency = currency->toUpperCase()->trim();
        try
        {
            auto response = std::async([=]() { client->get<std::shared_ptr<ApiResponse>>(std::string("/cryptocurrency/quotes/latest"), object{
                object::pair{std::string("params"), object{
                    object::pair{std::string("symbol"), normalizedSymbol}, 
                    object::pair{std::string("convert"), normalizedCurrency}
                }}
            }); });
            console->log(std::string("API Response:"), JSON->stringify(response->data, nullptr, 2));
            auto symbolData = const_(response->data->data)[normalizedSymbol];
            if (!symbolData) {
                throw any(std::make_shared<Error>(std::string("No data found for symbol: ") + normalizedSymbol + string_empty));
            }
            auto quoteData = const_(symbolData->quote)[normalizedCurrency];
            if (!quoteData) {
                throw any(std::make_shared<Error>(std::string("No quote data found for currency: ") + normalizedCurrency + string_empty));
            }
            return object{
                object::pair{std::string("price"), quoteData->price}, 
                object::pair{std::string("marketCap"), quoteData->market_cap}, 
                object::pair{std::string("volume24h"), quoteData->volume_24h}, 
                object::pair{std::string("percentChange24h"), quoteData->percent_change_24h}
            };
        }
        catch (const any& error)
        {
            if (axios->isAxiosError(error)) {
                auto errorMessage = OR((error["response"]["data"]["status"]["error_message"]), (error["message"]));
                console->error(std::string("API Error:"), errorMessage);
                throw any(std::make_shared<Error>(std::string("API Error: ") + errorMessage + string_empty));
            }
            throw any(error);
        }
    };
    return object{
        object::pair{std::string("getPrice"), std::string("getPrice")}
    };
};

void Main(void)
{
}

MAIN
