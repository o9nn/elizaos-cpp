#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-coingecko/src/actions/getTrendingSearch.action.h"

std::shared_ptr<Action> getTrendingSearchAction = object{
    object::pair{std:("name"), std:("GET_TRENDING_SEARCH")}, 
    object::pair{std:("similes"), array<string>{ std:("TRENDING_SEARCH"), std:("TRENDING_COINS_NFTS"), std:("HOT_SEARCHES"), std:("POPULAR_SEARCHES"), std:("TRENDING_NOW") }}, 
    object::pair{std:("description"), std:("Use this action when the user asks about overall trending coins, NFTs, and categories. Returns comprehensive trending data including coins with search scores, trending NFTs with floor prices, and trending categories. This is different from GET_TRENDING_TOKENS which shows trending pools on specific networks.")}, 
    object::pair{std:("parameters"), object{}}, 
    object::pair{std:("validate"), [=](auto runtime) mutable
    {
        auto svc = as<any>(runtime->getService(CoinGeckoService::serviceType));
        if (!svc) {
            logger->error(std:("CoinGeckoService not available"));
            return false;
        }
        return true;
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto _state = undefined, auto _options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto svc = as<any>(runtime->getService(CoinGeckoService::serviceType));
            if (!svc) {
                throw any(std::make_shared<Error>(std:("CoinGeckoService not available")));
            }
            logger->info(std:("[GET_TRENDING_SEARCH] Fetching trending searches"));
            auto trendingData = std::async([=]() { svc->getTrendingSearch(); });
            auto text = std:("Found ") + (OR((trendingData["trending_coins"]["length"]), (0))) + std:(" trending coins, ") + (OR((trendingData["trending_nfts"]["length"]), (0))) + std:(" trending NFTs, and ") + (OR((trendingData["trending_categories"]["length"]), (0))) + std:(" trending categories");
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), std:("text")}, 
                    object::pair{std:("actions"), array<string>{ std:("GET_TRENDING_SEARCH") }}, 
                    object::pair{std:("content"), as<any>(trendingData)}, 
                    object::pair{std:("source"), message->content->source}
                }); });
            }
            return as<any>(object{
                object::pair{std:("text"), std:("text")}, 
                object::pair{std:("success"), true}, 
                object::pair{std:("data"), trendingData}, 
                object::pair{std:("values"), trendingData}, 
                object::pair{std:("input"), object{}}
            });
        }
        catch (const any& error)
        {
            auto msg = (is<Error>(error)) ? error->message : String(error);
            logger->error(std:("[GET_TRENDING_SEARCH] Action failed: ") + msg + string_empty);
            auto errorText = std:("Failed to fetch trending searches: ") + msg + std:("\
\
This action fetches overall trending data from CoinGecko including:\
- Trending coins with search scores and market data\
- Trending NFTs with floor prices and volumes\
- Trending categories by market cap\
\
No parameters are required for this action.");
            auto errorResult = as<any>(object{
                object::pair{std:("text"), errorText}, 
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), msg}, 
                object::pair{std:("input"), object{}}
            });
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), errorResult->text}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("error"), std:("action_failed")}, 
                        object::pair{std:("details"), msg}
                    }}
                }); });
            }
            return errorResult;
        }
    }
    }, 
    object::pair{std:("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("What are the trending coins right now?")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Found 10 trending coins, 7 trending NFTs, and 5 trending categories")}, 
            object::pair{std:("actions"), array<string>{ std:("GET_TRENDING_SEARCH") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Show me trending searches on CoinGecko")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Found 10 trending coins, 7 trending NFTs, and 5 trending categories")}, 
            object::pair{std:("actions"), array<string>{ std:("GET_TRENDING_SEARCH") }}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
