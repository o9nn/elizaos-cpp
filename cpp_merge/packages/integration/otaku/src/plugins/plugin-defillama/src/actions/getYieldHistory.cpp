#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-defillama/src/actions/getYieldHistory.action.h"

std::shared_ptr<Action> getYieldHistoryAction = object{
    object::pair{std:("name"), std:("GET_YIELD_HISTORY")}, 
    object::pair{std:("similes"), array<string>{ std:("YIELD_HISTORY"), std:("YIELD_CHART"), std:("APY_TREND"), std:("YIELD_TREND"), std:("HISTORICAL_YIELD"), std:("APY_HISTORY") }}, 
    object::pair{std:("description"), std:("Use this action to fetch historical yield (APY) data and trends for a specific DeFi pool. Requires protocol, token, and optionally chain to identify the pool.")}, 
    object::pair{std:("parameters"), object{
        object::pair{std:("protocol"), object{
            object::pair{std:("type"), std:("string")}, 
            object::pair{std:("description"), std:("DeFi protocol name (e.g., 'Aave', 'Morpho', 'Compound')")}, 
            object::pair{std:("required"), true}
        }}, 
        object::pair{std:("token"), object{
            object::pair{std:("type"), std:("string")}, 
            object::pair{std:("description"), std:("Token symbol (e.g., 'USDC', 'ETH', 'DAI')")}, 
            object::pair{std:("required"), true}
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
            auto protocol = params["protocol"]["trim"]();
            auto token = params["token"]["trim"]();
            auto chain = OR((params["chain"]["trim"]()), (undefined));
            if (OR((!protocol), (!token))) {
                auto errorMsg = std:("Missing required parameters. Please specify both 'protocol' (e.g., 'Aave') and 'token' (e.g., 'USDC').");
                logger->error(std:("[GET_YIELD_HISTORY] ") + errorMsg + string_empty);
                auto errorResult = object{
                    object::pair{std:("text"), errorMsg}, 
                    object::pair{std:("success"), false}, 
                    object::pair{std:("error"), std:("missing_required_parameters")}
                };
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), errorResult->text}, 
                        object::pair{std:("content"), object{
                            object::pair{std:("error"), std:("missing_required_parameters")}, 
                            object::pair{std:("details"), errorMsg}
                        }}
                    }); });
                }
                return errorResult;
            }
            auto inputParams = object{
                object::pair{std:("protocol"), std:("protocol")}, 
                object::pair{std:("token"), std:("token")}, 
                object::pair{std:("chain"), std:("chain")}
            };
            logger->info(std:("[GET_YIELD_HISTORY] Finding pool for: protocol=") + protocol + std:(", token=") + token + string_empty + (chain) ? any(std:(", chain=") + chain + string_empty) (string_empty) + string_empty);
            auto pools = std::async([=]() { svc->searchYields(object{
                object::pair{std:("protocol"), std:("protocol")}, 
                object::pair{std:("token"), std:("token")}, 
                object::pair{std:("chain"), std:("chain")}, 
                object::pair{std:("limit"), 1}
            }); });
            if (OR((!pools), (pools->get_length() == 0))) {
                auto errorMsg = std:("No pool found for ") + protocol + std:(" ") + token + string_empty + (chain) ? any(std:(" on ") + chain + string_empty) (string_empty) + string_empty;
                logger->error(std:("[GET_YIELD_HISTORY] ") + errorMsg + string_empty);
                auto errorResult = as<any>(object{
                    object::pair{std:("text"), errorMsg}, 
                    object::pair{std:("success"), false}, 
                    object::pair{std:("error"), std:("pool_not_found")}, 
                    object::pair{std:("input"), inputParams}
                });
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), errorResult->text}, 
                        object::pair{std:("content"), object{
                            object::pair{std:("error"), std:("pool_not_found")}, 
                            object::pair{std:("details"), errorMsg}
                        }}
                    }); });
                }
                return errorResult;
            }
            auto pool = const_(pools)[0];
            auto poolId = pool["pool"];
            logger->info(std:("[GET_YIELD_HISTORY] Fetching historical data for pool: ") + poolId + string_empty);
            auto chartData = std::async([=]() { svc->getPoolChart(poolId); });
            if (OR((!chartData), (chartData->get_length() == 0))) {
                auto errorMsg = std:("No historical data available for ") + protocol + std:(" ") + token + string_empty + (chain) ? any(std:(" on ") + chain + string_empty) (string_empty) + string_empty;
                logger->warn(std:("[GET_YIELD_HISTORY] ") + errorMsg + string_empty);
                auto errorResult = as<any>(object{
                    object::pair{std:("text"), errorMsg}, 
                    object::pair{std:("success"), false}, 
                    object::pair{std:("error"), std:("no_historical_data")}, 
                    object::pair{std:("input"), inputParams}, 
                    object::pair{std:("data"), object{
                        object::pair{std:("pool"), object{
                            object::pair{std:("protocol"), pool["project"]}, 
                            object::pair{std:("token"), pool["symbol"]}, 
                            object::pair{std:("chain"), pool["chain"]}, 
                            object::pair{std:("poolId"), std:("poolId")}
                        }}
                    }}
                });
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), errorResult->text}, 
                        object::pair{std:("content"), object{
                            object::pair{std:("error"), std:("no_historical_data")}, 
                            object::pair{std:("details"), errorMsg}
                        }}
                    }); });
                }
                return errorResult;
            }
            auto apyValues = as<array<double>>(chartData->map([=](auto d) mutable
            {
                return d["apy"];
            }
            )->filter([=](auto apy) mutable
            {
                return apy != nullptr;
            }
            ));
            auto currentApy = OR((const_(apyValues)[apyValues->get_length() - 1]), (0));
            auto avgApy = (apyValues->get_length() > 0) ? any(apyValues->reduce([=](auto sum, auto apy) mutable
            {
                return sum + apy;
            }
            , 0) / apyValues->get_length()) (0);
            auto minApy = (apyValues->get_length() > 0) ? any(Math->min(const_(apyValues)[0])) (0);
            auto maxApy = (apyValues->get_length() > 0) ? any(Math->max(const_(apyValues)[0])) (0);
            auto recentData = chartData->slice(-30);
            auto messageText = std:("Retrieved ") + chartData->get_length() + std:(" days of yield history for ") + pool["project"] + std:(" ") + pool["symbol"] + string_empty + (pool["chain"]) ? any(std:(" on ") + pool["chain"] + string_empty) (string_empty) + string_empty;
            auto result = object{
                object::pair{std:("pool"), object{
                    object::pair{std:("protocol"), pool["project"]}, 
                    object::pair{std:("token"), pool["symbol"]}, 
                    object::pair{std:("chain"), pool["chain"]}, 
                    object::pair{std:("poolId"), std:("poolId")}, 
                    object::pair{std:("currentApy"), pool["apy"]}, 
                    object::pair{std:("currentTvl"), pool["tvlUsd"]}
                }}, 
                object::pair{std:("statistics"), object{
                    object::pair{std:("dataPoints"), chartData->get_length()}, 
                    object::pair{std:("currentApy"), std:("currentApy")}, 
                    object::pair{std:("avgApy"), std:("avgApy")}, 
                    object::pair{std:("minApy"), std:("minApy")}, 
                    object::pair{std:("maxApy"), std:("maxApy")}
                }}, 
                object::pair{std:("recentHistory"), recentData}, 
                object::pair{std:("fullHistory"), chartData}
            };
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), messageText}, 
                    object::pair{std:("actions"), array<string>{ std:("GET_YIELD_HISTORY") }}, 
                    object::pair{std:("content"), result}, 
                    object::pair{std:("source"), message->content->source}
                }); });
            }
            return as<any>(object{
                object::pair{std:("text"), messageText}, 
                object::pair{std:("success"), true}, 
                object::pair{std:("data"), result}, 
                object::pair{std:("input"), inputParams}
            });
        }
        catch (const any& error)
        {
            auto msg = (is<Error>(error)) ? error->message : String(error);
            logger->error(std:("[GET_YIELD_HISTORY] Action failed: ") + msg + string_empty);
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std:("ACTION_STATE") }, true); });
            auto params = OR((composedState->data->actionParams), (object{}));
            auto failureInputParams = object{
                object::pair{std:("protocol"), params["protocol"]}, 
                object::pair{std:("token"), params["token"]}, 
                object::pair{std:("chain"), params["chain"]}
            };
            auto errorResult = as<any>(object{
                object::pair{std:("text"), std:("Failed to fetch yield history: ") + msg + string_empty}, 
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
            object::pair{std:("text"), std:("Show me the yield history for Aave USDC")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Retrieved 998 days of yield history for aave-v3 USDC on Ethereum")}, 
            object::pair{std:("actions"), array<string>{ std:("GET_YIELD_HISTORY") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("How has the APY changed for Morpho ETH?")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Retrieved 365 days of yield history for morpho-v1 ETH on Ethereum")}, 
            object::pair{std:("actions"), array<string>{ std:("GET_YIELD_HISTORY") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("What's the yield trend for Compound USDC on Ethereum?")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Retrieved 750 days of yield history for compound-v3 USDC on Ethereum")}, 
            object::pair{std:("actions"), array<string>{ std:("GET_YIELD_HISTORY") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Show me historical APY for Aave DAI")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Retrieved 990 days of yield history for aave-v3 DAI on Ethereum")}, 
            object::pair{std:("actions"), array<string>{ std:("GET_YIELD_HISTORY") }}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
