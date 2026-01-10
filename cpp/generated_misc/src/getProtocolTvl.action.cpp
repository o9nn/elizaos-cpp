#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-defillama/src/actions/getProtocolTvl.action.h"

std::shared_ptr<Action> getProtocolTvlAction = object{
    object::pair{std::string("name"), std::string("GET_PROTOCOL_TVL")}, 
    object::pair{std::string("similes"), array<string>{ std::string("PROTOCOL_TVL"), std::string("COMPARE_TVL"), std::string("DEFILLAMA_PROTOCOL_TVL"), std::string("TVL") }}, 
    object::pair{std::string("description"), std::string("Use this action to fetch DeFi protocol TVL and change metrics by protocol name or symbol.")}, 
    object::pair{std::string("parameters"), object{
        object::pair{std::string("protocols"), object{
            object::pair{std::string("type"), std::string("string")}, 
            object::pair{std::string("description"), std::string("Comma-separated list of DeFi protocol names or symbols (e.g., 'Aave,Curve' or 'EIGEN,MORPHO')")}, 
            object::pair{std::string("required"), true}
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
            auto protocolsRaw = params["protocols"]["trim"]();
            if (!protocolsRaw) {
                auto errorMsg = std::string("Missing required parameter 'protocols'. Please specify which DeFi protocol(s) to fetch TVL for (e.g., 'Aave,Curve' or 'EIGEN,MORPHO').");
                logger->error(std::string("[GET_PROTOCOL_TVL] ") + errorMsg + string_empty);
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
            auto names = protocolsRaw->split(std::string(","))->map([=](auto s) mutable
            {
                return s->trim();
            }
            )->filter(Boolean);
            if (!names->get_length()) {
                auto errorMsg = std::string("No valid protocol names found. Please provide DeFi protocol names or symbols.");
                logger->error(std::string("[GET_PROTOCOL_TVL] ") + errorMsg + string_empty);
                auto errorResult = object{
                    object::pair{std::string("text"), errorMsg}, 
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("error"), std::string("invalid_parameter")}
                };
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), errorResult->text}, 
                        object::pair{std::string("content"), object{
                            object::pair{std::string("error"), std::string("invalid_parameter")}, 
                            object::pair{std::string("details"), errorMsg}
                        }}
                    }); });
                }
                return errorResult;
            }
            logger->info(std::string("[GET_PROTOCOL_TVL] Fetching TVL for: ") + names->join(std::string(", ")) + string_empty);
            auto inputParams = object{
                object::pair{std::string("protocols"), protocolsRaw}
            };
            auto results = std::async([=]() { svc->getProtocolsByNames(names); });
            if (OR((!Array->isArray(results)), (results->get_length() == 0))) {
                auto errorMsg = std::string("No protocols matched the provided names");
                logger->error(std::string("[GET_PROTOCOL_TVL] ") + errorMsg + string_empty);
                auto errorResult = as<any>(object{
                    object::pair{std::string("text"), errorMsg}, 
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("error"), std::string("no_results")}, 
                    object::pair{std::string("input"), inputParams}
                });
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), errorResult->text}, 
                        object::pair{std::string("content"), object{
                            object::pair{std::string("error"), std::string("no_results")}, 
                            object::pair{std::string("details"), errorMsg}
                        }}
                    }); });
                }
                return errorResult;
            }
            auto successes = results->filter([=](auto result) mutable
            {
                return Boolean(AND((result["success"]), (result["data"])));
            }
            );
            auto failed = results->filter([=](auto result) mutable
            {
                return !result["success"];
            }
            );
            if (successes->get_length() == 0) {
                auto errorMsg = std::string("No protocols matched the provided names");
                logger->error(std::string("[GET_PROTOCOL_TVL] ") + errorMsg + string_empty);
                auto errorResult = as<any>(object{
                    object::pair{std::string("text"), errorMsg}, 
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("error"), std::string("no_matches")}, 
                    object::pair{std::string("input"), inputParams}
                });
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), errorResult->text}, 
                        object::pair{std::string("content"), object{
                            object::pair{std::string("error"), std::string("no_matches")}, 
                            object::pair{std::string("details"), errorMsg}
                        }}
                    }); });
                }
                return errorResult;
            }
            auto messageText = (failed->get_length() > 0) ? std::string("Fetched TVL for ") + successes->get_length() + std::string(" protocol(s); ") + failed->get_length() + std::string(" not matched") : std::string("Fetched TVL for ") + successes->get_length() + std::string(" protocol(s)");
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), messageText}, 
                    object::pair{std::string("actions"), array<string>{ std::string("GET_PROTOCOL_TVL") }}, 
                    object::pair{std::string("content"), results}, 
                    object::pair{std::string("source"), message->content->source}
                }); });
            }
            return as<any>(object{
                object::pair{std::string("text"), messageText}, 
                object::pair{std::string("success"), true}, 
                object::pair{std::string("data"), results}, 
                object::pair{std::string("values"), successes->map([=](auto r) mutable
                {
                    return r->data;
                }
                )}, 
                object::pair{std::string("input"), inputParams}
            });
        }
        catch (const any& error)
        {
            auto msg = (is<Error>(error)) ? error->message : String(error);
            logger->error(std::string("[GET_PROTOCOL_TVL] Action failed: ") + msg + string_empty);
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std::string("ACTION_STATE") }, true); });
            auto params = OR((composedState->data->actionParams), (object{}));
            auto failureInputParams = object{
                object::pair{std::string("protocols"), params["protocols"]}
            };
            auto errorResult = as<any>(object{
                object::pair{std::string("text"), std::string("Failed to fetch protocol TVL: ") + msg + string_empty}, 
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
            object::pair{std::string("text"), std::string("Compare EIGEN and MORPHO TVL")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Fetched TVL for 2 protocol(s)")}, 
            object::pair{std::string("actions"), array<string>{ std::string("GET_PROTOCOL_TVL") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("What is the TVL of Aave and Curve?")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Fetched TVL for 2 protocol(s)")}, 
            object::pair{std::string("actions"), array<string>{ std::string("GET_PROTOCOL_TVL") }}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
