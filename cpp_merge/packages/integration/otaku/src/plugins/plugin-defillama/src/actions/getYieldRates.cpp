#include "getYieldRates.action.h"

std::shared_ptr<Action> getYieldRatesAction = object{
    object::pair{std:("name"), std:("GET_YIELD_RATES")}, 
    object::pair{std:("similes"), array<string>{ std:("YIELD_RATES"), std:("CHECK_APY"), std:("FIND_YIELD"), std:("COMPARE_YIELDS"), std:("GET_APY"), std:("LENDING_RATES") }}, 
    object::pair{std:("description"), std:("Use this action to fetch DeFi yield rates (APY) for protocols, tokens, and chains. Can compare yields across different protocols.")}, 
    object::pair{std:("parameters"), object{
        object::pair{std:("protocol"), object{
            object::pair{std:("type"), std:("string")}, 
            object::pair{std:("description"), std:("DeFi protocol name (e.g., 'Aave', 'Morpho', 'Compound'). Optional.")}, 
            object::pair{std:("required"), false}
        }}, 
        object::pair{std:("token"), object{
            object::pair{std:("type"), std:("string")}, 
            object::pair{std:("description"), std:("Token symbol (e.g., 'USDC', 'ETH', 'DAI'). Optional.")}, 
            object::pair{std:("required"), false}
        }}, 
        object::pair{std:("chain"), object{
            object::pair{std:("type"), std:("string")}, 
            object::pair{std:("description"), std:("Blockchain name (e.g., 'Ethereum', 'Base', 'Arbitrum'). Optional.")}, 
            object::pair{std:("required"), false}
        }}
    }}, 
    object::pair{std:("validate"), [=](auto runtime) mutable
    {
        auto svc = as<any>(runtime->getService(DefiLlamaService::serviceType));
        if (!svc) {
            logger->error(std:("DefiLlamaService not available"));
            return false;
        }
        return true;
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto _state = undefined, auto _options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto svc = as<any>(runtime->getService(DefiLlamaService::serviceType));
            if (!svc) {
                throw any(std::make_shared<Error>(std:("DefiLlamaService not available")));
            }
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std:("ACTION_STATE") }, true); });
            auto params = OR((composedState->data->actionParams), (object{}));
            shared protocol = OR((params["protocol"]["trim"]()), (undefined));
            shared token = OR((params["token"]["trim"]()), (undefined));
            shared chain = OR((params["chain"]["trim"]()), (undefined));
            auto searchCriteria = array<any>();
            if (protocol) searchCriteria->push(std:("protocol: ") + protocol + string_empty);
            if (token) searchCriteria->push(std:("token: ") + token + string_empty);
            if (chain) searchCriteria->push(std:("chain: ") + chain + string_empty);
            auto searchDesc = (searchCriteria->get_length() > 0) ? any(searchCriteria->join(std:(", "))) (std:("all yields"));
            logger->info(std:("[GET_YIELD_RATES] Searching for yields: ") + searchDesc + string_empty);
            auto inputParams = object{
                object::pair{std:("protocol"), std:("protocol")}, 
                object::pair{std:("token"), std:("token")}, 
                object::pair{std:("chain"), std:("chain")}
            };
            auto results = std::async([=]() { svc->searchYields(object{
                object::pair{std:("protocol"), std:("protocol")}, 
                object::pair{std:("token"), std:("token")}, 
                object::pair{std:("chain"), std:("chain")}, 
                object::pair{std:("limit"), 10}
            }); });
            if (OR((!Array->isArray(results)), (results->get_length() == 0))) {
                auto errorMsg = std:("No yield opportunities found for ") + searchDesc + string_empty;
                logger->info(std:("[GET_YIELD_RATES] ") + errorMsg + string_empty);
                auto errorResult = as<any>(object{
                    object::pair{std:("text"), errorMsg}, 
                    object::pair{std:("success"), true}, 
                    object::pair{std:("data"), array<any>()}, 
                    object::pair{std:("input"), inputParams}
                });
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), errorResult->text}, 
                        object::pair{std:("content"), object{
                            object::pair{std:("results"), array<any>()}, 
                            object::pair{std:("searchCriteria"), inputParams}
                        }}
                    }); });
                }
                return errorResult;
            }
            auto formattedResults = results->map([=](auto pool) mutable
            {
                return (object{
                    object::pair{std:("protocol"), pool["project"]}, 
                    object::pair{std:("chain"), pool["chain"]}, 
                    object::pair{std:("token"), pool["symbol"]}, 
                    object::pair{std:("apy"), pool["apy"]}, 
                    object::pair{std:("apyBase"), pool["apyBase"]}, 
                    object::pair{std:("apyReward"), pool["apyReward"]}, 
                    object::pair{std:("tvlUsd"), pool["tvlUsd"]}, 
                    object::pair{std:("stablecoin"), pool["stablecoin"]}, 
                    object::pair{std:("poolId"), pool["pool"]}, 
                    object::pair{std:("apyChange1d"), pool["apyPct1D"]}, 
                    object::pair{std:("apyChange7d"), pool["apyPct7D"]}, 
                    object::pair{std:("apyChange30d"), pool["apyPct30D"]}, 
                    object::pair{std:("apyMean30d"), pool["apyMean30d"]}
                });
            }
            );
            auto messageText = std:("Found ") + results->get_length() + std:(" yield opportunit") + (results->get_length() == 1) ? std:("y") : std:("ies") + std:(" for ") + searchDesc + string_empty;
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), messageText}, 
                    object::pair{std:("actions"), array<string>{ std:("GET_YIELD_RATES") }}, 
                    object::pair{std:("content"), formattedResults}, 
                    object::pair{std:("source"), message->content->source}
                }); });
            }
            return as<any>(object{
                object::pair{std:("text"), messageText}, 
                object::pair{std:("success"), true}, 
                object::pair{std:("data"), formattedResults}, 
                object::pair{std:("values"), formattedResults}, 
                object::pair{std:("input"), inputParams}
            });
        }
        catch (const any& error)
        {
            auto msg = (is<Error>(error)) ? error->message : String(error);
            logger->error(std:("[GET_YIELD_RATES] Action failed: ") + msg + string_empty);
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std:("ACTION_STATE") }, true); });
            auto params = OR((composedState->data->actionParams), (object{}));
            auto failureInputParams = object{
                object::pair{std:("protocol"), params["protocol"]}, 
                object::pair{std:("token"), params["token"]}, 
                object::pair{std:("chain"), params["chain"]}
            };
            auto errorResult = as<any>(object{
                object::pair{std:("text"), std:("Failed to fetch yield rates: ") + msg + string_empty}, 
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
            object::pair{std:("text"), std:("What's the current APY on Aave for USDC?")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Found 3 yield opportunities for protocol: Aave, token: USDC")}, 
            object::pair{std:("actions"), array<string>{ std:("GET_YIELD_RATES") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Compare USDC yields on Aave and Morpho")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Found 8 yield opportunities for token: USDC")}, 
            object::pair{std:("actions"), array<string>{ std:("GET_YIELD_RATES") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Show me the best ETH yields on Ethereum")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Found 10 yield opportunities for token: ETH, chain: Ethereum")}, 
            object::pair{std:("actions"), array<string>{ std:("GET_YIELD_RATES") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("What can I earn on stablecoins in Aave?")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Found 5 yield opportunities for protocol: Aave")}, 
            object::pair{std:("actions"), array<string>{ std:("GET_YIELD_RATES") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Find the best yields on Base")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Found 10 yield opportunities for chain: Base")}, 
            object::pair{std:("actions"), array<string>{ std:("GET_YIELD_RATES") }}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
