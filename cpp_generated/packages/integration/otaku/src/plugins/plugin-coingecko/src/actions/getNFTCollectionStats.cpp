#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-coingecko/src/actions/getNFTCollectionStats.action.h"

std::shared_ptr<Action> getNFTCollectionStatsAction = object{
    object::pair{std::string("name"), std::string("GET_NFT_COLLECTION_STATS")}, 
    object::pair{std::string("similes"), array<string>{ std::string("NFT_STATS"), std::string("NFT_COLLECTION_INFO"), std::string("NFT_FLOOR_PRICE"), std::string("NFT_COLLECTION_DATA"), std::string("NFT_VOLUME") }}, 
    object::pair{std::string("description"), std::string("Use this action when the user asks about NFT collection statistics including floor price, market cap, volume, sales, number of owners, and other collection metrics. Accepts NFT collection ID, name, or contract address.")}, 
    object::pair{std::string("parameters"), object{
        object::pair{std::string("collection"), object{
            object::pair{std::string("type"), std::string("string")}, 
            object::pair{std::string("description"), std::string("NFT collection identifier (collection ID, name, or contract address). Examples: 'cryptopunks', 'bored-ape-yacht-club', '0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d'")}, 
            object::pair{std::string("required"), true}
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
            auto collectionRaw = params["collection"]["trim"]();
            if (!collectionRaw) {
                auto errorMsg = std::string("Missing required parameter 'collection'. Please specify which NFT collection to fetch stats for (e.g., 'cryptopunks', 'bored-ape-yacht-club', or a contract address).");
                logger->error(std::string("[GET_NFT_COLLECTION_STATS] ") + errorMsg + string_empty);
                auto errorResult = object{
                    object::pair{std::string("text"), errorMsg}, 
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("error"), std::string("missing_required_parameter")}
                };
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), errorResult->text}, 
                        object::pair{std::string("content"), object{
                            object::pair{std::string("error"), std::string("missing_required_parameter")}, 
                            object::pair{std::string("details"), errorMsg}
                        }}
                    }); });
                }
                return errorResult;
            }
            logger->info(std::string("[GET_NFT_COLLECTION_STATS] Fetching stats for collection: ") + collectionRaw + string_empty);
            auto inputParams = object{
                object::pair{std::string("collection"), collectionRaw}
            };
            auto stats = std::async([=]() { svc->getNFTCollectionStats(collectionRaw); });
            auto text = std::string("Retrieved stats for NFT collection: ") + (OR((stats["name"]), (collectionRaw))) + string_empty;
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("text")}, 
                    object::pair{std::string("actions"), array<string>{ std::string("GET_NFT_COLLECTION_STATS") }}, 
                    object::pair{std::string("content"), as<any>(stats)}, 
                    object::pair{std::string("source"), message->content->source}
                }); });
            }
            return as<any>(object{
                object::pair{std::string("text"), std::string("text")}, 
                object::pair{std::string("success"), true}, 
                object::pair{std::string("data"), stats}, 
                object::pair{std::string("values"), stats}, 
                object::pair{std::string("input"), inputParams}
            });
        }
        catch (const any& error)
        {
            auto msg = (is<Error>(error)) ? error->message : String(error);
            logger->error(std::string("[GET_NFT_COLLECTION_STATS] Action failed: ") + msg + string_empty);
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std::string("ACTION_STATE") }, true); });
            auto params = OR((composedState->data->actionParams), (object{}));
            auto failureInputParams = object{
                object::pair{std::string("collection"), params["collection"]}
            };
            auto errorText = std::string("Failed to fetch NFT collection stats: ") + msg + std::string("\
\
Please provide a valid NFT collection identifier:\
- Collection ID (e.g., 'cryptopunks', 'bored-ape-yacht-club', 'azuki')\
- Contract address (e.g., '0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d')\
\
Example: "Get stats for cryptopunks" or "Show me floor price for bored-ape-yacht-club"");
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
            object::pair{std::string("text"), std::string("What's the floor price of CryptoPunks?")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Retrieved stats for NFT collection: CryptoPunks")}, 
            object::pair{std::string("actions"), array<string>{ std::string("GET_NFT_COLLECTION_STATS") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Show me stats for bored-ape-yacht-club")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Retrieved stats for NFT collection: Bored Ape Yacht Club")}, 
            object::pair{std::string("actions"), array<string>{ std::string("GET_NFT_COLLECTION_STATS") }}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
