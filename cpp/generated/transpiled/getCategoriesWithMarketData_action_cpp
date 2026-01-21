#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-coingecko/src/actions/getCategoriesWithMarketData.action.h"

std::shared_ptr<Action> getCategoriesWithMarketDataAction = object{
    object::pair{std::string("name"), std::string("GET_CATEGORIES_WITH_MARKET_DATA")}, 
    object::pair{std::string("similes"), array<string>{ std::string("CATEGORIES_MARKET_DATA"), std::string("CATEGORY_STATS"), std::string("TOP_CATEGORIES"), std::string("TRENDING_CATEGORIES") }}, 
    object::pair{std::string("description"), std::string("Use this action to get all coin categories with market data including market cap, volume, 24h change, and top 3 coins in each category. Returns comprehensive category statistics that can be sorted by market cap or name. Useful when the user wants to analyze category performance or find top performing categories.")}, 
    object::pair{std::string("parameters"), object{
        object::pair{std::string("order"), object{
            object::pair{std::string("type"), std::string("string")}, 
            object::pair{std::string("description"), std::string("Sort order for categories. Options: 'market_cap_desc' (default), 'market_cap_asc', 'name_desc', 'name_asc', 'market_cap_change_24h_desc', 'market_cap_change_24h_asc'")}, 
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
            auto validOrders = as<std::shared_ptr<const>>(array<std::shared_ptr<const>>{ std::string("market_cap_desc"), std::string("market_cap_asc"), std::string("name_desc"), std::string("name_asc"), std::string("market_cap_change_24h_desc"), std::string("market_cap_change_24h_asc") });
            auto orderRaw = OR((params["order"]["trim"]()["toLowerCase"]()), (std::string("market_cap_desc")));
            auto order = (validOrders->includes(as<OrderType>(orderRaw))) ? any((as<OrderType>(orderRaw))) : any(std::string("market_cap_desc"));
            logger->info(std::string("[GET_CATEGORIES_WITH_MARKET_DATA] Fetching categories with order: ") + order + string_empty);
            auto inputParams = object{
                object::pair{std::string("order"), std::string("order")}
            };
            auto categoriesData = std::async([=]() { svc->getCategoriesWithMarketData(order); });
            auto text = std::string("Found ") + (OR((categoriesData->get_length()), (0))) + std::string(" categories with market data");
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("text")}, 
                    object::pair{std::string("actions"), array<string>{ std::string("GET_CATEGORIES_WITH_MARKET_DATA") }}, 
                    object::pair{std::string("content"), as<Record<string, any>>(categoriesData)}, 
                    object::pair{std::string("source"), message->content->source}
                }); });
            }
            return as<any>(object{
                object::pair{std::string("text"), std::string("text")}, 
                object::pair{std::string("success"), true}, 
                object::pair{std::string("data"), categoriesData}, 
                object::pair{std::string("values"), categoriesData}, 
                object::pair{std::string("input"), inputParams}
            });
        }
        catch (const any& error)
        {
            auto msg = (is<Error>(error)) ? error->message : String(error);
            logger->error(std::string("[GET_CATEGORIES_WITH_MARKET_DATA] Action failed: ") + msg + string_empty);
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std::string("ACTION_STATE") }, true); });
            auto params = OR((composedState->data->actionParams), (object{}));
            auto failureInputParams = object{
                object::pair{std::string("order"), OR((params["order"]), (std::string("market_cap_desc")))}
            };
            auto errorText = std::string("Failed to fetch categories with market data: ") + msg + string_empty;
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
            object::pair{std::string("text"), std::string("Show me the top crypto categories by market cap")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Found 150 categories with market data")}, 
            object::pair{std::string("actions"), array<string>{ std::string("GET_CATEGORIES_WITH_MARKET_DATA") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("What are the trending categories in the last 24h?")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Found 150 categories with market data")}, 
            object::pair{std::string("actions"), array<string>{ std::string("GET_CATEGORIES_WITH_MARKET_DATA") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Get category market data sorted by 24h change")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Found 150 categories with market data")}, 
            object::pair{std::string("actions"), array<string>{ std::string("GET_CATEGORIES_WITH_MARKET_DATA") }}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
