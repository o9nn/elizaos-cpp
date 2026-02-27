#include "getTrendingTokens.action.h"

std::shared_ptr<Action> getTrendingTokensAction = object{
    object::pair{std:("name"), std:("GET_TRENDING_TOKENS")}, 
    object::pair{std:("similes"), array<string>{ std:("TRENDING_TOKENS"), std:("HOT_TOKENS"), std:("TRENDING_POOLS"), std:("TOP_TOKENS"), std:("POPULAR_TOKENS") }}, 
    object::pair{std:("description"), std:("Use this action when the user asks about trending or popular tokens on a specific blockchain network. Returns trending pools with token metadata including price, volume, market cap, and price changes. Supports networks like 'base', 'ethereum', 'arbitrum', 'optimism', 'polygon', 'bsc', 'solana', and more.")}, 
    object::pair{std:("parameters"), object{
        object::pair{std:("network"), object{
            object::pair{std:("type"), std:("string")}, 
            object::pair{std:("description"), std:("The blockchain network to fetch trending tokens for (e.g., 'base', 'ethereum', 'arbitrum', 'optimism', 'polygon', 'bsc', 'solana'). Defaults to 'base'.")}, 
            object::pair{std:("required"), true}
        }}, 
        object::pair{std:("limit"), object{
            object::pair{std:("type"), std:("number")}, 
            object::pair{std:("description"), std:("Number of trending tokens to return (1-30). Defaults to 10.")}, 
            object::pair{std:("required"), false}
        }}
    }}, 
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
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std:("ACTION_STATE") }, true); });
            auto params = OR((composedState->data->actionParams), (object{}));
            auto network = (OR((params["network"]["trim"]()), (std:("base"))))["toLowerCase"]();
            auto limitRaw = params["limit"];
            auto limit = (type_of(limitRaw) == std:("number")) ? Math->max(1, Math->min(30, Math->floor(limitRaw))) : (type_of(limitRaw) == std:("string")) ? any(Math->max(1, Math->min(30, Math->floor(OR((Number(limitRaw)), (10)))))) (10);
            logger->info(std:("[GET_TRENDING_TOKENS] Fetching trending tokens for network: ") + network + std:(", limit: ") + limit + string_empty);
            auto inputParams = object{
                object::pair{std:("network"), std:("network")}, 
                object::pair{std:("limit"), std:("limit")}
            };
            auto trendingData = std::async([=]() { svc->getTrendingTokens(network, limit); });
            auto text = std:("Found ") + (OR((trendingData["length"]), (0))) + std:(" trending token(s) on ") + network + string_empty;
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), std:("text")}, 
                    object::pair{std:("actions"), array<string>{ std:("GET_TRENDING_TOKENS") }}, 
                    object::pair{std:("content"), as<any>(trendingData)}, 
                    object::pair{std:("source"), message->content->source}
                }); });
            }
            return as<any>(object{
                object::pair{std:("text"), std:("text")}, 
                object::pair{std:("success"), true}, 
                object::pair{std:("data"), trendingData}, 
                object::pair{std:("values"), trendingData}, 
                object::pair{std:("input"), inputParams}
            });
        }
        catch (const any& error)
        {
            auto msg = (is<Error>(error)) ? error->message : String(error);
            logger->error(std:("[GET_TRENDING_TOKENS] Action failed: ") + msg + string_empty);
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std:("ACTION_STATE") }, true); });
            auto params = OR((composedState->data->actionParams), (object{}));
            auto failureInputParams = object{
                object::pair{std:("network"), OR((params["network"]), (std:("base")))}, 
                object::pair{std:("limit"), OR((params["limit"]), (10))}
            };
            auto errorText = std:("Failed to fetch trending tokens: ") + msg + std:("\
\
Please provide the correct chain parameter:\
| Chain        | GeckoTerminal Parameter |\
| ------------ | ----------------------- |\
| **base**     | base                    |\
| **ethereum** | eth                     |\
| **polygon**  | polygon_pos             |\
| **arbitrum** | arbitrum                |\
| **optimism** | optimism                |\
| **scroll**   | scroll                  |\
\
Example: "Get trending tokens on eth" or "Show me trending tokens on polygon_pos"");
            auto errorResult = as<any>(object{
                object::pair{std:("text"), errorText}, 
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), msg}, 
                object::pair{std:("input"), failureInputParams}
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
            object::pair{std:("text"), std:("What are the trending tokens on Base?")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Found 10 trending token(s) on base")}, 
            object::pair{std:("actions"), array<string>{ std:("GET_TRENDING_TOKENS") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Show me the top 5 trending tokens on ethereum")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Found 5 trending token(s) on ethereum")}, 
            object::pair{std:("actions"), array<string>{ std:("GET_TRENDING_TOKENS") }}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
