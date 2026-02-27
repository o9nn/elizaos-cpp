#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-coingecko/src/actions/getNFTCollectionStats.action.h"

std::shared_ptr<Action> getNFTCollectionStatsAction = object{
    object::pair{std:("name"), std:("GET_NFT_COLLECTION_STATS")}, 
    object::pair{std:("similes"), array<string>{ std:("NFT_STATS"), std:("NFT_COLLECTION_INFO"), std:("NFT_FLOOR_PRICE"), std:("NFT_COLLECTION_DATA"), std:("NFT_VOLUME") }}, 
    object::pair{std:("description"), std:("Use this action when the user asks about NFT collection statistics including floor price, market cap, volume, sales, number of owners, and other collection metrics. Accepts NFT collection ID, name, or contract address.")}, 
    object::pair{std:("parameters"), object{
        object::pair{std:("collection"), object{
            object::pair{std:("type"), std:("string")}, 
            object::pair{std:("description"), std:("NFT collection identifier (collection ID, name, or contract address). Examples: 'cryptopunks', 'bored-ape-yacht-club', '0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d'")}, 
            object::pair{std:("required"), true}
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
            auto collectionRaw = params["collection"]["trim"]();
            if (!collectionRaw) {
                auto errorMsg = std:("Missing required parameter 'collection'. Please specify which NFT collection to fetch stats for (e.g., 'cryptopunks', 'bored-ape-yacht-club', or a contract address).");
                logger->error(std:("[GET_NFT_COLLECTION_STATS] ") + errorMsg + string_empty);
                auto errorResult = object{
                    object::pair{std:("text"), errorMsg}, 
                    object::pair{std:("success"), false}, 
                    object::pair{std:("error"), std:("missing_required_parameter")}
                };
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), errorResult->text}, 
                        object::pair{std:("content"), object{
                            object::pair{std:("error"), std:("missing_required_parameter")}, 
                            object::pair{std:("details"), errorMsg}
                        }}
                    }); });
                }
                return errorResult;
            }
            logger->info(std:("[GET_NFT_COLLECTION_STATS] Fetching stats for collection: ") + collectionRaw + string_empty);
            auto inputParams = object{
                object::pair{std:("collection"), collectionRaw}
            };
            auto stats = std::async([=]() { svc->getNFTCollectionStats(collectionRaw); });
            auto text = std:("Retrieved stats for NFT collection: ") + (OR((stats["name"]), (collectionRaw))) + string_empty;
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), std:("text")}, 
                    object::pair{std:("actions"), array<string>{ std:("GET_NFT_COLLECTION_STATS") }}, 
                    object::pair{std:("content"), as<any>(stats)}, 
                    object::pair{std:("source"), message->content->source}
                }); });
            }
            return as<any>(object{
                object::pair{std:("text"), std:("text")}, 
                object::pair{std:("success"), true}, 
                object::pair{std:("data"), stats}, 
                object::pair{std:("values"), stats}, 
                object::pair{std:("input"), inputParams}
            });
        }
        catch (const any& error)
        {
            auto msg = (is<Error>(error)) ? error->message : String(error);
            logger->error(std:("[GET_NFT_COLLECTION_STATS] Action failed: ") + msg + string_empty);
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std:("ACTION_STATE") }, true); });
            auto params = OR((composedState->data->actionParams), (object{}));
            auto failureInputParams = object{
                object::pair{std:("collection"), params["collection"]}
            };
            auto errorText = std:("Failed to fetch NFT collection stats: ") + msg + std:("\
\
Please provide a valid NFT collection identifier:\
- Collection ID (e.g., 'cryptopunks', 'bored-ape-yacht-club', 'azuki')\
- Contract address (e.g., '0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d')\
\
Example: "Get stats for cryptopunks" or "Show me floor price for bored-ape-yacht-club"");
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
            object::pair{std:("text"), std:("What's the floor price of CryptoPunks?")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Retrieved stats for NFT collection: CryptoPunks")}, 
            object::pair{std:("actions"), array<string>{ std:("GET_NFT_COLLECTION_STATS") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Show me stats for bored-ape-yacht-club")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Retrieved stats for NFT collection: Bored Ape Yacht Club")}, 
            object::pair{std:("actions"), array<string>{ std:("GET_NFT_COLLECTION_STATS") }}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
