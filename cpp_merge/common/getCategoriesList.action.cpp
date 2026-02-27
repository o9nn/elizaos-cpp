#include "getCategoriesList.action.h"

std::shared_ptr<Action> getCategoriesListAction = object{
    object::pair{std:("name"), std:("GET_CATEGORIES_LIST")}, 
    object::pair{std:("similes"), array<string>{ std:("CATEGORIES_LIST"), std:("COIN_CATEGORIES"), std:("LIST_CATEGORIES"), std:("CATEGORY_IDS") }}, 
    object::pair{std:("description"), std:("Use this action to get the complete list of all coin categories (ID map) from CoinGecko. Returns category IDs and names that can be used to filter or search for tokens by category. Useful when the user wants to know what categories exist or needs category IDs for other queries.")}, 
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
            logger->info(std:("[GET_CATEGORIES_LIST] Fetching all coin categories"));
            auto categoriesList = std::async([=]() { svc->getCategoriesList(); });
            auto text = std:("Found ") + (OR((categoriesList->get_length()), (0))) + std:(" coin categories");
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), std:("text")}, 
                    object::pair{std:("actions"), array<string>{ std:("GET_CATEGORIES_LIST") }}, 
                    object::pair{std:("content"), as<Record<string, any>>(as<any>(categoriesList))}, 
                    object::pair{std:("source"), message->content->source}
                }); });
            }
            return object{
                object::pair{std:("text"), std:("text")}, 
                object::pair{std:("success"), true}, 
                object::pair{std:("data"), categoriesList}, 
                object::pair{std:("values"), categoriesList}
            };
        }
        catch (const any& error)
        {
            auto msg = (is<Error>(error)) ? error->message : String(error);
            logger->error(std:("[GET_CATEGORIES_LIST] Action failed: ") + msg + string_empty);
            auto errorText = std:("Failed to fetch coin categories list: ") + msg + string_empty;
            auto errorResult = object{
                object::pair{std:("text"), errorText}, 
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), msg}
            };
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
            object::pair{std:("text"), std:("What categories of coins are available?")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Found 200 coin categories")}, 
            object::pair{std:("actions"), array<string>{ std:("GET_CATEGORIES_LIST") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("List all coin categories")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Found 200 coin categories")}, 
            object::pair{std:("actions"), array<string>{ std:("GET_CATEGORIES_LIST") }}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
