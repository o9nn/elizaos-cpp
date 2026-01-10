#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-coingecko/src/actions/getCategoriesList.action.h"

std::shared_ptr<Action> getCategoriesListAction = object{
    object::pair{std::string("name"), std::string("GET_CATEGORIES_LIST")}, 
    object::pair{std::string("similes"), array<string>{ std::string("CATEGORIES_LIST"), std::string("COIN_CATEGORIES"), std::string("LIST_CATEGORIES"), std::string("CATEGORY_IDS") }}, 
    object::pair{std::string("description"), std::string("Use this action to get the complete list of all coin categories (ID map) from CoinGecko. Returns category IDs and names that can be used to filter or search for tokens by category. Useful when the user wants to know what categories exist or needs category IDs for other queries.")}, 
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
            logger->info(std::string("[GET_CATEGORIES_LIST] Fetching all coin categories"));
            auto categoriesList = std::async([=]() { svc->getCategoriesList(); });
            auto text = std::string("Found ") + (OR((categoriesList->get_length()), (0))) + std::string(" coin categories");
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("text")}, 
                    object::pair{std::string("actions"), array<string>{ std::string("GET_CATEGORIES_LIST") }}, 
                    object::pair{std::string("content"), as<Record<string, any>>(as<any>(categoriesList))}, 
                    object::pair{std::string("source"), message->content->source}
                }); });
            }
            return object{
                object::pair{std::string("text"), std::string("text")}, 
                object::pair{std::string("success"), true}, 
                object::pair{std::string("data"), categoriesList}, 
                object::pair{std::string("values"), categoriesList}
            };
        }
        catch (const any& error)
        {
            auto msg = (is<Error>(error)) ? error->message : String(error);
            logger->error(std::string("[GET_CATEGORIES_LIST] Action failed: ") + msg + string_empty);
            auto errorText = std::string("Failed to fetch coin categories list: ") + msg + string_empty;
            auto errorResult = object{
                object::pair{std::string("text"), errorText}, 
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), msg}
            };
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
            object::pair{std::string("text"), std::string("What categories of coins are available?")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Found 200 coin categories")}, 
            object::pair{std::string("actions"), array<string>{ std::string("GET_CATEGORIES_LIST") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("List all coin categories")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Found 200 coin categories")}, 
            object::pair{std::string("actions"), array<string>{ std::string("GET_CATEGORIES_LIST") }}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
