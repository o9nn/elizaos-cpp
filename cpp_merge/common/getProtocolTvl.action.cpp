#include "getProtocolTvl.action.h"

std::shared_ptr<Action> getProtocolTvlAction = object{
    object::pair{std:("name"), std:("GET_PROTOCOL_TVL")}, 
    object::pair{std:("similes"), array<string>{ std:("PROTOCOL_TVL"), std:("COMPARE_TVL"), std:("DEFILLAMA_PROTOCOL_TVL"), std:("TVL") }}, 
    object::pair{std:("description"), std:("Use this action to fetch DeFi protocol TVL and change metrics by protocol name or symbol.")}, 
    object::pair{std:("parameters"), object{
        object::pair{std:("protocols"), object{
            object::pair{std:("type"), std:("string")}, 
            object::pair{std:("description"), std:("Comma-separated list of DeFi protocol names or symbols (e.g., 'Aave,Curve' or 'EIGEN,MORPHO')")}, 
            object::pair{std:("required"), true}
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
            auto protocolsRaw = params["protocols"]["trim"]();
            if (!protocolsRaw) {
                auto errorMsg = std:("Missing required parameter 'protocols'. Please specify which DeFi protocol(s) to fetch TVL for (e.g., 'Aave,Curve' or 'EIGEN,MORPHO').");
                logger->error(std:("[GET_PROTOCOL_TVL] ") + errorMsg + string_empty);
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
            auto names = protocolsRaw->split(std:(","))->map([=](auto s) mutable
            {
                return s->trim();
            }
            )->filter(Boolean);
            if (!names->get_length()) {
                auto errorMsg = std:("No valid protocol names found. Please provide DeFi protocol names or symbols.");
                logger->error(std:("[GET_PROTOCOL_TVL] ") + errorMsg + string_empty);
                auto errorResult = object{
                    object::pair{std:("text"), errorMsg}, 
                    object::pair{std:("success"), false}, 
                    object::pair{std:("error"), std:("invalid_parameter")}
                };
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), errorResult->text}, 
                        object::pair{std:("content"), object{
                            object::pair{std:("error"), std:("invalid_parameter")}, 
                            object::pair{std:("details"), errorMsg}
                        }}
                    }); });
                }
                return errorResult;
            }
            logger->info(std:("[GET_PROTOCOL_TVL] Fetching TVL for: ") + names->join(std:(", ")) + string_empty);
            auto inputParams = object{
                object::pair{std:("protocols"), protocolsRaw}
            };
            auto results = std::async([=]() { svc->getProtocolsByNames(names); });
            if (OR((!Array->isArray(results)), (results->get_length() == 0))) {
                auto errorMsg = std:("No protocols matched the provided names");
                logger->error(std:("[GET_PROTOCOL_TVL] ") + errorMsg + string_empty);
                auto errorResult = as<any>(object{
                    object::pair{std:("text"), errorMsg}, 
                    object::pair{std:("success"), false}, 
                    object::pair{std:("error"), std:("no_results")}, 
                    object::pair{std:("input"), inputParams}
                });
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), errorResult->text}, 
                        object::pair{std:("content"), object{
                            object::pair{std:("error"), std:("no_results")}, 
                            object::pair{std:("details"), errorMsg}
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
                auto errorMsg = std:("No protocols matched the provided names");
                logger->error(std:("[GET_PROTOCOL_TVL] ") + errorMsg + string_empty);
                auto errorResult = as<any>(object{
                    object::pair{std:("text"), errorMsg}, 
                    object::pair{std:("success"), false}, 
                    object::pair{std:("error"), std:("no_matches")}, 
                    object::pair{std:("input"), inputParams}
                });
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), errorResult->text}, 
                        object::pair{std:("content"), object{
                            object::pair{std:("error"), std:("no_matches")}, 
                            object::pair{std:("details"), errorMsg}
                        }}
                    }); });
                }
                return errorResult;
            }
            auto messageText = (failed->get_length() > 0) ? std:("Fetched TVL for ") + successes->get_length() + std:(" protocol(s); ") + failed->get_length() + std:(" not matched") : std:("Fetched TVL for ") + successes->get_length() + std:(" protocol(s)");
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), messageText}, 
                    object::pair{std:("actions"), array<string>{ std:("GET_PROTOCOL_TVL") }}, 
                    object::pair{std:("content"), results}, 
                    object::pair{std:("source"), message->content->source}
                }); });
            }
            return as<any>(object{
                object::pair{std:("text"), messageText}, 
                object::pair{std:("success"), true}, 
                object::pair{std:("data"), results}, 
                object::pair{std:("values"), successes->map([=](auto r) mutable
                {
                    return r->data;
                }
                )}, 
                object::pair{std:("input"), inputParams}
            });
        }
        catch (const any& error)
        {
            auto msg = (is<Error>(error)) ? error->message : String(error);
            logger->error(std:("[GET_PROTOCOL_TVL] Action failed: ") + msg + string_empty);
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std:("ACTION_STATE") }, true); });
            auto params = OR((composedState->data->actionParams), (object{}));
            auto failureInputParams = object{
                object::pair{std:("protocols"), params["protocols"]}
            };
            auto errorResult = as<any>(object{
                object::pair{std:("text"), std:("Failed to fetch protocol TVL: ") + msg + string_empty}, 
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
            object::pair{std:("text"), std:("Compare EIGEN and MORPHO TVL")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Fetched TVL for 2 protocol(s)")}, 
            object::pair{std:("actions"), array<string>{ std:("GET_PROTOCOL_TVL") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("What is the TVL of Aave and Curve?")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Fetched TVL for 2 protocol(s)")}, 
            object::pair{std:("actions"), array<string>{ std:("GET_PROTOCOL_TVL") }}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
