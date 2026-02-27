#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-defillama/src/actions/getYieldHistory.action.h"

std::shared_ptr<Action> getYieldHistoryAction = object{
    object::pair{std::string("name"), std::string("GET_YIELD_HISTORY")}, 
    object::pair{std::string("similes"), array<string>{ std::string("YIELD_HISTORY"), std::string("YIELD_CHART"), std::string("APY_TREND"), std::string("YIELD_TREND"), std::string("HISTORICAL_YIELD"), std::string("APY_HISTORY") }}, 
    object::pair{std::string("description"), std::string("Use this action to fetch historical yield (APY) data and trends for a specific DeFi pool. Requires protocol, token, and optionally chain to identify the pool.")}, 
    object::pair{std::string("parameters"), object{
        object::pair{std::string("protocol"), object{
            object::pair{std::string("type"), std::string("string")}, 
            object::pair{std::string("description"), std::string("DeFi protocol name (e.g., 'Aave', 'Morpho', 'Compound')")}, 
            object::pair{std::string("required"), true}
        }}, 
        object::pair{std::string("token"), object{
            object::pair{std::string("type"), std::string("string")}, 
            object::pair{std::string("description"), std::string("Token symbol (e.g., 'USDC', 'ETH', 'DAI')")}, 
            object::pair{std::string("required"), true}
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
            auto protocol = params["protocol"]["trim"]();
            auto token = params["token"]["trim"]();
            auto chain = OR((params["chain"]["trim"]()), (undefined));
            if (OR((!protocol), (!token))) {
                auto errorMsg = std::string("Missing required parameters. Please specify both 'protocol' (e.g., 'Aave') and 'token' (e.g., 'USDC').");
                logger->error(std::string("[GET_YIELD_HISTORY] ") + errorMsg + string_empty);
                auto errorResult = object{
                    object::pair{std::string("text"), errorMsg}, 
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("error"), std::string("missing_required_parameters")}
                };
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), errorResult->text}, 
                        object::pair{std::string("content"), object{
                            object::pair{std::string("error"), std::string("missing_required_parameters")}, 
                            object::pair{std::string("details"), errorMsg}
                        }}
                    }); });
                }
                return errorResult;
            }
            auto inputParams = object{
                object::pair{std::string("protocol"), std::string("protocol")}, 
                object::pair{std::string("token"), std::string("token")}, 
                object::pair{std::string("chain"), std::string("chain")}
            };
            logger->info(std::string("[GET_YIELD_HISTORY] Finding pool for: protocol=") + protocol + std::string(", token=") + token + string_empty + (chain) ? any(std::string(", chain=") + chain + string_empty) : any(string_empty) + string_empty);
            auto pools = std::async([=]() { svc->searchYields(object{
                object::pair{std::string("protocol"), std::string("protocol")}, 
                object::pair{std::string("token"), std::string("token")}, 
                object::pair{std::string("chain"), std::string("chain")}, 
                object::pair{std::string("limit"), 1}
            }); });
            if (OR((!pools), (pools->get_length() == 0))) {
                auto errorMsg = std::string("No pool found for ") + protocol + std::string(" ") + token + string_empty + (chain) ? any(std::string(" on ") + chain + string_empty) : any(string_empty) + string_empty;
                logger->error(std::string("[GET_YIELD_HISTORY] ") + errorMsg + string_empty);
                auto errorResult = as<any>(object{
                    object::pair{std::string("text"), errorMsg}, 
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("error"), std::string("pool_not_found")}, 
                    object::pair{std::string("input"), inputParams}
                });
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), errorResult->text}, 
                        object::pair{std::string("content"), object{
                            object::pair{std::string("error"), std::string("pool_not_found")}, 
                            object::pair{std::string("details"), errorMsg}
                        }}
                    }); });
                }
                return errorResult;
            }
            auto pool = const_(pools)[0];
            auto poolId = pool["pool"];
            logger->info(std::string("[GET_YIELD_HISTORY] Fetching historical data for pool: ") + poolId + string_empty);
            auto chartData = std::async([=]() { svc->getPoolChart(poolId); });
            if (OR((!chartData), (chartData->get_length() == 0))) {
                auto errorMsg = std::string("No historical data available for ") + protocol + std::string(" ") + token + string_empty + (chain) ? any(std::string(" on ") + chain + string_empty) : any(string_empty) + string_empty;
                logger->warn(std::string("[GET_YIELD_HISTORY] ") + errorMsg + string_empty);
                auto errorResult = as<any>(object{
                    object::pair{std::string("text"), errorMsg}, 
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("error"), std::string("no_historical_data")}, 
                    object::pair{std::string("input"), inputParams}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("pool"), object{
                            object::pair{std::string("protocol"), pool["project"]}, 
                            object::pair{std::string("token"), pool["symbol"]}, 
                            object::pair{std::string("chain"), pool["chain"]}, 
                            object::pair{std::string("poolId"), std::string("poolId")}
                        }}
                    }}
                });
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), errorResult->text}, 
                        object::pair{std::string("content"), object{
                            object::pair{std::string("error"), std::string("no_historical_data")}, 
                            object::pair{std::string("details"), errorMsg}
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
            , 0) / apyValues->get_length()) : any(0);
            auto minApy = (apyValues->get_length() > 0) ? any(Math->min(const_(apyValues)[0])) : any(0);
            auto maxApy = (apyValues->get_length() > 0) ? any(Math->max(const_(apyValues)[0])) : any(0);
            auto recentData = chartData->slice(-30);
            auto messageText = std::string("Retrieved ") + chartData->get_length() + std::string(" days of yield history for ") + pool["project"] + std::string(" ") + pool["symbol"] + string_empty + (pool["chain"]) ? any(std::string(" on ") + pool["chain"] + string_empty) : any(string_empty) + string_empty;
            auto result = object{
                object::pair{std::string("pool"), object{
                    object::pair{std::string("protocol"), pool["project"]}, 
                    object::pair{std::string("token"), pool["symbol"]}, 
                    object::pair{std::string("chain"), pool["chain"]}, 
                    object::pair{std::string("poolId"), std::string("poolId")}, 
                    object::pair{std::string("currentApy"), pool["apy"]}, 
                    object::pair{std::string("currentTvl"), pool["tvlUsd"]}
                }}, 
                object::pair{std::string("statistics"), object{
                    object::pair{std::string("dataPoints"), chartData->get_length()}, 
                    object::pair{std::string("currentApy"), std::string("currentApy")}, 
                    object::pair{std::string("avgApy"), std::string("avgApy")}, 
                    object::pair{std::string("minApy"), std::string("minApy")}, 
                    object::pair{std::string("maxApy"), std::string("maxApy")}
                }}, 
                object::pair{std::string("recentHistory"), recentData}, 
                object::pair{std::string("fullHistory"), chartData}
            };
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), messageText}, 
                    object::pair{std::string("actions"), array<string>{ std::string("GET_YIELD_HISTORY") }}, 
                    object::pair{std::string("content"), result}, 
                    object::pair{std::string("source"), message->content->source}
                }); });
            }
            return as<any>(object{
                object::pair{std::string("text"), messageText}, 
                object::pair{std::string("success"), true}, 
                object::pair{std::string("data"), result}, 
                object::pair{std::string("input"), inputParams}
            });
        }
        catch (const any& error)
        {
            auto msg = (is<Error>(error)) ? error->message : String(error);
            logger->error(std::string("[GET_YIELD_HISTORY] Action failed: ") + msg + string_empty);
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std::string("ACTION_STATE") }, true); });
            auto params = OR((composedState->data->actionParams), (object{}));
            auto failureInputParams = object{
                object::pair{std::string("protocol"), params["protocol"]}, 
                object::pair{std::string("token"), params["token"]}, 
                object::pair{std::string("chain"), params["chain"]}
            };
            auto errorResult = as<any>(object{
                object::pair{std::string("text"), std::string("Failed to fetch yield history: ") + msg + string_empty}, 
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
            object::pair{std::string("text"), std::string("Show me the yield history for Aave USDC")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Retrieved 998 days of yield history for aave-v3 USDC on Ethereum")}, 
            object::pair{std::string("actions"), array<string>{ std::string("GET_YIELD_HISTORY") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("How has the APY changed for Morpho ETH?")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Retrieved 365 days of yield history for morpho-v1 ETH on Ethereum")}, 
            object::pair{std::string("actions"), array<string>{ std::string("GET_YIELD_HISTORY") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("What's the yield trend for Compound USDC on Ethereum?")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Retrieved 750 days of yield history for compound-v3 USDC on Ethereum")}, 
            object::pair{std::string("actions"), array<string>{ std::string("GET_YIELD_HISTORY") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Show me historical APY for Aave DAI")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Retrieved 990 days of yield history for aave-v3 DAI on Ethereum")}, 
            object::pair{std::string("actions"), array<string>{ std::string("GET_YIELD_HISTORY") }}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
