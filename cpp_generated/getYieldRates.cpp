#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-defillama/src/actions/getYieldRates.action.h"

std::shared_ptr<Action> getYieldRatesAction = object{
    object::pair{std::string("name"), std::string("GET_YIELD_RATES")}, 
    object::pair{std::string("similes"), array<string>{ std::string("YIELD_RATES"), std::string("CHECK_APY"), std::string("FIND_YIELD"), std::string("COMPARE_YIELDS"), std::string("GET_APY"), std::string("LENDING_RATES") }}, 
    object::pair{std::string("description"), std::string("Use this action to fetch DeFi yield rates (APY) for protocols, tokens, and chains. Can compare yields across different protocols.")}, 
    object::pair{std::string("parameters"), object{
        object::pair{std::string("protocol"), object{
            object::pair{std::string("type"), std::string("string")}, 
            object::pair{std::string("description"), std::string("DeFi protocol name (e.g., 'Aave', 'Morpho', 'Compound'). Optional.")}, 
            object::pair{std::string("required"), false}
        }}, 
        object::pair{std::string("token"), object{
            object::pair{std::string("type"), std::string("string")}, 
            object::pair{std::string("description"), std::string("Token symbol (e.g., 'USDC', 'ETH', 'DAI'). Optional.")}, 
            object::pair{std::string("required"), false}
        }}, 
        object::pair{std::string("chain"), object{
            object::pair{std::string("type"), std::string("string")}, 
            object::pair{std::string("description"), std::string("Blockchain name (e.g., 'Ethereum', 'Base', 'Arbitrum'). Optional.")}, 
            object::pair{std::string("required"), false}
        }}
    }}, 
    object::pair{std::string("validate"), [=](auto runtime) mutable
    {
        auto svc = as<any>(runtime->getService(DefiLlamaService::serviceType));
        if (!svc) {
            logger->error(std::string("DefiLlamaService not available"));
            return false;
        }
        return true;
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto _state = undefined, auto _options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto svc = as<any>(runtime->getService(DefiLlamaService::serviceType));
            if (!svc) {
                throw any(std::make_shared<Error>(std::string("DefiLlamaService not available")));
            }
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std::string("ACTION_STATE") }, true); });
            auto params = OR((composedState->data->actionParams), (object{}));
            shared protocol = OR((params["protocol"]["trim"]()), (undefined));
            shared token = OR((params["token"]["trim"]()), (undefined));
            shared chain = OR((params["chain"]["trim"]()), (undefined));
            auto searchCriteria = array<any>();
            if (protocol) searchCriteria->push(std::string("protocol: ") + protocol + string_empty);
            if (token) searchCriteria->push(std::string("token: ") + token + string_empty);
            if (chain) searchCriteria->push(std::string("chain: ") + chain + string_empty);
            auto searchDesc = (searchCriteria->get_length() > 0) ? any(searchCriteria->join(std::string(", "))) : any(std::string("all yields"));
            logger->info(std::string("[GET_YIELD_RATES] Searching for yields: ") + searchDesc + string_empty);
            auto inputParams = object{
                object::pair{std::string("protocol"), std::string("protocol")}, 
                object::pair{std::string("token"), std::string("token")}, 
                object::pair{std::string("chain"), std::string("chain")}
            };
            auto results = std::async([=]() { svc->searchYields(object{
                object::pair{std::string("protocol"), std::string("protocol")}, 
                object::pair{std::string("token"), std::string("token")}, 
                object::pair{std::string("chain"), std::string("chain")}, 
                object::pair{std::string("limit"), 10}
            }); });
            if (OR((!Array->isArray(results)), (results->get_length() == 0))) {
                auto errorMsg = std::string("No yield opportunities found for ") + searchDesc + string_empty;
                logger->info(std::string("[GET_YIELD_RATES] ") + errorMsg + string_empty);
                auto errorResult = as<any>(object{
                    object::pair{std::string("text"), errorMsg}, 
                    object::pair{std::string("success"), true}, 
                    object::pair{std::string("data"), array<any>()}, 
                    object::pair{std::string("input"), inputParams}
                });
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), errorResult->text}, 
                        object::pair{std::string("content"), object{
                            object::pair{std::string("results"), array<any>()}, 
                            object::pair{std::string("searchCriteria"), inputParams}
                        }}
                    }); });
                }
                return errorResult;
            }
            auto formattedResults = results->map([=](auto pool) mutable
            {
                return (object{
                    object::pair{std::string("protocol"), pool["project"]}, 
                    object::pair{std::string("chain"), pool["chain"]}, 
                    object::pair{std::string("token"), pool["symbol"]}, 
                    object::pair{std::string("apy"), pool["apy"]}, 
                    object::pair{std::string("apyBase"), pool["apyBase"]}, 
                    object::pair{std::string("apyReward"), pool["apyReward"]}, 
                    object::pair{std::string("tvlUsd"), pool["tvlUsd"]}, 
                    object::pair{std::string("stablecoin"), pool["stablecoin"]}, 
                    object::pair{std::string("poolId"), pool["pool"]}, 
                    object::pair{std::string("apyChange1d"), pool["apyPct1D"]}, 
                    object::pair{std::string("apyChange7d"), pool["apyPct7D"]}, 
                    object::pair{std::string("apyChange30d"), pool["apyPct30D"]}, 
                    object::pair{std::string("apyMean30d"), pool["apyMean30d"]}
                });
            }
            );
            auto messageText = std::string("Found ") + results->get_length() + std::string(" yield opportunit") + (results->get_length() == 1) ? std::string("y") : std::string("ies") + std::string(" for ") + searchDesc + string_empty;
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), messageText}, 
                    object::pair{std::string("actions"), array<string>{ std::string("GET_YIELD_RATES") }}, 
                    object::pair{std::string("content"), formattedResults}, 
                    object::pair{std::string("source"), message->content->source}
                }); });
            }
            return as<any>(object{
                object::pair{std::string("text"), messageText}, 
                object::pair{std::string("success"), true}, 
                object::pair{std::string("data"), formattedResults}, 
                object::pair{std::string("values"), formattedResults}, 
                object::pair{std::string("input"), inputParams}
            });
        }
        catch (const any& error)
        {
            auto msg = (is<Error>(error)) ? error->message : String(error);
            logger->error(std::string("[GET_YIELD_RATES] Action failed: ") + msg + string_empty);
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std::string("ACTION_STATE") }, true); });
            auto params = OR((composedState->data->actionParams), (object{}));
            auto failureInputParams = object{
                object::pair{std::string("protocol"), params["protocol"]}, 
                object::pair{std::string("token"), params["token"]}, 
                object::pair{std::string("chain"), params["chain"]}
            };
            auto errorResult = as<any>(object{
                object::pair{std::string("text"), std::string("Failed to fetch yield rates: ") + msg + string_empty}, 
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
            object::pair{std::string("text"), std::string("What's the current APY on Aave for USDC?")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Found 3 yield opportunities for protocol: Aave, token: USDC")}, 
            object::pair{std::string("actions"), array<string>{ std::string("GET_YIELD_RATES") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Compare USDC yields on Aave and Morpho")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Found 8 yield opportunities for token: USDC")}, 
            object::pair{std::string("actions"), array<string>{ std::string("GET_YIELD_RATES") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Show me the best ETH yields on Ethereum")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Found 10 yield opportunities for token: ETH, chain: Ethereum")}, 
            object::pair{std::string("actions"), array<string>{ std::string("GET_YIELD_RATES") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("What can I earn on stablecoins in Aave?")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Found 5 yield opportunities for protocol: Aave")}, 
            object::pair{std::string("actions"), array<string>{ std::string("GET_YIELD_RATES") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Find the best yields on Base")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Found 10 yield opportunities for chain: Base")}, 
            object::pair{std::string("actions"), array<string>{ std::string("GET_YIELD_RATES") }}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
