#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-coingecko/src/actions/getTrendingTokens.action.h"

std::shared_ptr<Action> getTrendingTokensAction = object{
    object::pair{std::string("name"), std::string("GET_TRENDING_TOKENS")}, 
    object::pair{std::string("similes"), array<string>{ std::string("TRENDING_TOKENS"), std::string("HOT_TOKENS"), std::string("TRENDING_POOLS"), std::string("TOP_TOKENS"), std::string("POPULAR_TOKENS") }}, 
    object::pair{std::string("description"), std::string("Use this action when the user asks about trending or popular tokens on a specific blockchain network. Returns trending pools with token metadata including price, volume, market cap, and price changes. Supports networks like 'base', 'ethereum', 'arbitrum', 'optimism', 'polygon', 'bsc', 'solana', and more.")}, 
    object::pair{std::string("parameters"), object{
        object::pair{std::string("network"), object{
            object::pair{std::string("type"), std::string("string")}, 
            object::pair{std::string("description"), std::string("The blockchain network to fetch trending tokens for (e.g., 'base', 'ethereum', 'arbitrum', 'optimism', 'polygon', 'bsc', 'solana'). Defaults to 'base'.")}, 
            object::pair{std::string("required"), true}
        }}, 
        object::pair{std::string("limit"), object{
            object::pair{std::string("type"), std::string("number")}, 
            object::pair{std::string("description"), std::string("Number of trending tokens to return (1-30). Defaults to 10.")}, 
            object::pair{std::string("required"), false}
        }}
    }}, 
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
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std::string("ACTION_STATE") }, true); });
            auto params = OR((composedState->data->actionParams), (object{}));
            auto network = (OR((params["network"]["trim"]()), (std::string("base"))))["toLowerCase"]();
            auto limitRaw = params["limit"];
            auto limit = (type_of(limitRaw) == std::string("number")) ? Math->max(1, Math->min(30, Math->floor(limitRaw))) : (type_of(limitRaw) == std::string("string")) ? any(Math->max(1, Math->min(30, Math->floor(OR((Number(limitRaw)), (10)))))) : any(10);
            logger->info(std::string("[GET_TRENDING_TOKENS] Fetching trending tokens for network: ") + network + std::string(", limit: ") + limit + string_empty);
            auto inputParams = object{
                object::pair{std::string("network"), std::string("network")}, 
                object::pair{std::string("limit"), std::string("limit")}
            };
            auto trendingData = std::async([=]() { svc->getTrendingTokens(network, limit); });
            auto text = std::string("Found ") + (OR((trendingData["length"]), (0))) + std::string(" trending token(s) on ") + network + string_empty;
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("text")}, 
                    object::pair{std::string("actions"), array<string>{ std::string("GET_TRENDING_TOKENS") }}, 
                    object::pair{std::string("content"), as<any>(trendingData)}, 
                    object::pair{std::string("source"), message->content->source}
                }); });
            }
            return as<any>(object{
                object::pair{std::string("text"), std::string("text")}, 
                object::pair{std::string("success"), true}, 
                object::pair{std::string("data"), trendingData}, 
                object::pair{std::string("values"), trendingData}, 
                object::pair{std::string("input"), inputParams}
            });
        }
        catch (const any& error)
        {
            auto msg = (is<Error>(error)) ? error->message : String(error);
            logger->error(std::string("[GET_TRENDING_TOKENS] Action failed: ") + msg + string_empty);
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std::string("ACTION_STATE") }, true); });
            auto params = OR((composedState->data->actionParams), (object{}));
            auto failureInputParams = object{
                object::pair{std::string("network"), OR((params["network"]), (std::string("base")))}, 
                object::pair{std::string("limit"), OR((params["limit"]), (10))}
            };
            auto errorText = std::string("Failed to fetch trending tokens: ") + msg + std::string("\
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
                object::pair{std::string("text"), errorText}, 
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), msg}, 
                object::pair{std::string("input"), failureInputParams}
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
            object::pair{std::string("text"), std::string("What are the trending tokens on Base?")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Found 10 trending token(s) on base")}, 
            object::pair{std::string("actions"), array<string>{ std::string("GET_TRENDING_TOKENS") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Show me the top 5 trending tokens on ethereum")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Found 5 trending token(s) on ethereum")}, 
            object::pair{std::string("actions"), array<string>{ std::string("GET_TRENDING_TOKENS") }}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
