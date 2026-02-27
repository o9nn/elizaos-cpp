#include "getCategoriesWithMarketData.action.h"

std::shared_ptr<Action> getCategoriesWithMarketDataAction = object{
    object::pair{std:("name"), std:("GET_CATEGORIES_WITH_MARKET_DATA")}, 
    object::pair{std:("similes"), array<string>{ std:("CATEGORIES_MARKET_DATA"), std:("CATEGORY_STATS"), std:("TOP_CATEGORIES"), std:("TRENDING_CATEGORIES") }}, 
    object::pair{std:("description"), std:("Use this action to get all coin categories with market data including market cap, volume, 24h change, and top 3 coins in each category. Returns comprehensive category statistics that can be sorted by market cap or name. Useful when the user wants to analyze category performance or find top performing categories.")}, 
    object::pair{std:("parameters"), object{
        object::pair{std:("order"), object{
            object::pair{std:("type"), std:("string")}, 
            object::pair{std:("description"), std:("Sort order for categories. Options: 'market_cap_desc' (default), 'market_cap_asc', 'name_desc', 'name_asc', 'market_cap_change_24h_desc', 'market_cap_change_24h_asc'")}, 
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
            auto validOrders = as<std::shared_ptr<const>>(array<std::shared_ptr<const>>{ std:("market_cap_desc"), std:("market_cap_asc"), std:("name_desc"), std:("name_asc"), std:("market_cap_change_24h_desc"), std:("market_cap_change_24h_asc") });
            auto orderRaw = OR((params["order"]["trim"]()["toLowerCase"]()), (std:("market_cap_desc")));
            auto order = (validOrders->includes(as<OrderType>(orderRaw))) ? any((as<OrderType>(orderRaw))) (std:("market_cap_desc"));
            logger->info(std:("[GET_CATEGORIES_WITH_MARKET_DATA] Fetching categories with order: ") + order + string_empty);
            auto inputParams = object{
                object::pair{std:("order"), std:("order")}
            };
            auto categoriesData = std::async([=]() { svc->getCategoriesWithMarketData(order); });
            auto text = std:("Found ") + (OR((categoriesData->get_length()), (0))) + std:(" categories with market data");
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), std:("text")}, 
                    object::pair{std:("actions"), array<string>{ std:("GET_CATEGORIES_WITH_MARKET_DATA") }}, 
                    object::pair{std:("content"), as<Record<string, any>>(categoriesData)}, 
                    object::pair{std:("source"), message->content->source}
                }); });
            }
            return as<any>(object{
                object::pair{std:("text"), std:("text")}, 
                object::pair{std:("success"), true}, 
                object::pair{std:("data"), categoriesData}, 
                object::pair{std:("values"), categoriesData}, 
                object::pair{std:("input"), inputParams}
            });
        }
        catch (const any& error)
        {
            auto msg = (is<Error>(error)) ? error->message : String(error);
            logger->error(std:("[GET_CATEGORIES_WITH_MARKET_DATA] Action failed: ") + msg + string_empty);
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std:("ACTION_STATE") }, true); });
            auto params = OR((composedState->data->actionParams), (object{}));
            auto failureInputParams = object{
                object::pair{std:("order"), OR((params["order"]), (std:("market_cap_desc")))}
            };
            auto errorText = std:("Failed to fetch categories with market data: ") + msg + string_empty;
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
            object::pair{std:("text"), std:("Show me the top crypto categories by market cap")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Found 150 categories with market data")}, 
            object::pair{std:("actions"), array<string>{ std:("GET_CATEGORIES_WITH_MARKET_DATA") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("What are the trending categories in the last 24h?")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Found 150 categories with market data")}, 
            object::pair{std:("actions"), array<string>{ std:("GET_CATEGORIES_WITH_MARKET_DATA") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Get category market data sorted by 24h change")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Found 150 categories with market data")}, 
            object::pair{std:("actions"), array<string>{ std:("GET_CATEGORIES_WITH_MARKET_DATA") }}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
