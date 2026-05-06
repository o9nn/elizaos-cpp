#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-coingecko/src/actions/getTrendingSearch.action.h"

std::shared_ptr<Action> getTrendingSearchAction = object{
    object::pair{std::string("name"), std::string("GET_TRENDING_SEARCH")}, 
    object::pair{std::string("similes"), array<string>{ std::string("TRENDING_SEARCH"), std::string("TRENDING_COINS_NFTS"), std::string("HOT_SEARCHES"), std::string("POPULAR_SEARCHES"), std::string("TRENDING_NOW") }}, 
    object::pair{std::string("description"), std::string("Use this action when the user asks about overall trending coins, NFTs, and categories. Returns comprehensive trending data including coins with search scores, trending NFTs with floor prices, and trending categories. This is different from GET_TRENDING_TOKENS which shows trending pools on specific networks.")}, 
    object::pair{std::string("parameters"), object{}}, 
    object::pair{std::string("validate"), [=](auto runtime) mutable
    {
        auto svc = as<any>(runtime->getService(CoinGeckoService::serviceType));
        if (!svc) {
            logger->error(std::string("CoinGeckoService not available"));
            return false;
        }
        return true;
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto _state = undefined, auto _options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto svc = as<any>(runtime->getService(CoinGeckoService::serviceType));
            if (!svc) {
                throw any(std::make_shared<Error>(std::string("CoinGeckoService not available")));
            }
            logger->info(std::string("[GET_TRENDING_SEARCH] Fetching trending searches"));
            auto trendingData = std::async([=]() { svc->getTrendingSearch(); });
            auto text = std::string("Found ") + (OR((trendingData["trending_coins"]["length"]), (0))) + std::string(" trending coins, ") + (OR((trendingData["trending_nfts"]["length"]), (0))) + std::string(" trending NFTs, and ") + (OR((trendingData["trending_categories"]["length"]), (0))) + std::string(" trending categories");
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("text")}, 
                    object::pair{std::string("actions"), array<string>{ std::string("GET_TRENDING_SEARCH") }}, 
                    object::pair{std::string("content"), as<any>(trendingData)}, 
                    object::pair{std::string("source"), message->content->source}
                }); });
            }
            return as<any>(object{
                object::pair{std::string("text"), std::string("text")}, 
                object::pair{std::string("success"), true}, 
                object::pair{std::string("data"), trendingData}, 
                object::pair{std::string("values"), trendingData}, 
                object::pair{std::string("input"), object{}}
            });
        }
        catch (const any& error)
        {
            auto msg = (is<Error>(error)) ? error->message : String(error);
            logger->error(std::string("[GET_TRENDING_SEARCH] Action failed: ") + msg + string_empty);
            auto errorText = std::string("Failed to fetch trending searches: ") + msg + std::string("\
\
This action fetches overall trending data from CoinGecko including:\
- Trending coins with search scores and market data\
- Trending NFTs with floor prices and volumes\
- Trending categories by market cap\
\
No parameters are required for this action.");
            auto errorResult = as<any>(object{
                object::pair{std::string("text"), errorText}, 
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), msg}, 
                object::pair{std::string("input"), object{}}
            });
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), errorResult->text}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("error"), std::string("action_failed")}, 
                        object::pair{std::string("details"), msg}
                    }}
                }); });
            }
            return errorResult;
        }
    }
    }, 
    object::pair{std::string("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("What are the trending coins right now?")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Found 10 trending coins, 7 trending NFTs, and 5 trending categories")}, 
            object::pair{std::string("actions"), array<string>{ std::string("GET_TRENDING_SEARCH") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Show me trending searches on CoinGecko")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Found 10 trending coins, 7 trending NFTs, and 5 trending categories")}, 
            object::pair{std::string("actions"), array<string>{ std::string("GET_TRENDING_SEARCH") }}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
