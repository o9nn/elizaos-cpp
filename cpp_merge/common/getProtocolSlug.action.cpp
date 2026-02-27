#include "getProtocolSlug.action.h"

std::shared_ptr<Action> getProtocolSlugAction = object{
    object::pair{std:("name"), std:("GET_PROTOCOL_SLUG")}, 
    object::pair{std:("similes"), array<string>{ std:("PROTOCOL_SLUG"), std:("FIND_PROTOCOL"), std:("SEARCH_PROTOCOL"), std:("PROTOCOL_INFO"), std:("DEFILLAMA_PROTOCOL_SLUG") }}, 
    object::pair{std:("description"), std:("Use this action to search for DeFi protocol slugs and basic information by protocol name or symbol. Returns protocol slugs needed for TVL history lookups.")}, 
    object::pair{std:("parameters"), object{
        object::pair{std:("protocols"), object{
            object::pair{std:("type"), std:("string")}, 
            object::pair{std:("description"), std:("Comma-separated list of DeFi protocol names or symbols to search for (e.g., 'Aave,Curve' or 'EIGEN,MORPHO')")}, 
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
                auto errorMsg = std:("Missing required parameter 'protocols'. Please specify which DeFi protocol(s) to search for (e.g., 'Aave,Curve' or 'EIGEN,MORPHO').");
                logger->error(std:("[GET_PROTOCOL_SLUG] ") + errorMsg + string_empty);
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
                logger->error(std:("[GET_PROTOCOL_SLUG] ") + errorMsg + string_empty);
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
            logger->info(std:("[GET_PROTOCOL_SLUG] Searching for protocols: ") + names->join(std:(", ")) + string_empty);
            auto inputParams = object{
                object::pair{std:("protocols"), protocolsRaw}
            };
            auto searchResults = array<SearchResult>();
            for (auto& query : names)
            {
                auto candidates = std::async([=]() { svc->searchProtocolCandidates(query, 5); });
                auto candidateInfos = candidates->map([=](auto protocol) mutable
                {
                    return (object{
                        object::pair{std:("id"), protocol["id"]}, 
                        object::pair{std:("slug"), protocol["slug"]}, 
                        object::pair{std:("name"), protocol["name"]}, 
                        object::pair{std:("symbol"), protocol["symbol"]}, 
                        object::pair{std:("category"), protocol["category"]}, 
                        object::pair{std:("chains"), protocol["chains"]}, 
                        object::pair{std:("url"), protocol["url"]}, 
                        object::pair{std:("logo"), protocol["logo"]}, 
                        object::pair{std:("tvl"), protocol["tvl"]}
                    });
                }
                );
                searchResults->push(object{
                    object::pair{std:("query"), std:("query")}, 
                    object::pair{std:("candidates"), candidateInfos}
                });
            }
            auto totalCandidates = searchResults->reduce([=](auto sum, auto r) mutable
            {
                return sum + r["candidates"]->get_length();
            }
            , 0);
            auto queriesWithNoResults = searchResults->filter([=](auto r) mutable
            {
                return r["candidates"]->get_length() == 0;
            }
            )->get_length();
            if (totalCandidates == 0) {
                auto errorMsg = std:("No protocols matched any of the provided names");
                logger->error(std:("[GET_PROTOCOL_SLUG] ") + errorMsg + string_empty);
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
            auto messageText = (queriesWithNoResults > 0) ? std:("Found ") + totalCandidates + std:(" candidate(s) for ") + searchResults->get_length() + std:(" search(es); ") + queriesWithNoResults + std:(" search(es) had no matches") : std:("Found ") + totalCandidates + std:(" candidate(s) for ") + searchResults->get_length() + std:(" search(es)");
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), messageText}, 
                    object::pair{std:("actions"), array<string>{ std:("GET_PROTOCOL_SLUG") }}, 
                    object::pair{std:("content"), searchResults}, 
                    object::pair{std:("source"), message->content->source}
                }); });
            }
            return as<any>(object{
                object::pair{std:("text"), messageText}, 
                object::pair{std:("success"), true}, 
                object::pair{std:("data"), searchResults}, 
                object::pair{std:("values"), searchResults}, 
                object::pair{std:("input"), inputParams}
            });
        }
        catch (const any& error)
        {
            auto msg = (is<Error>(error)) ? error->message : String(error);
            logger->error(std:("[GET_PROTOCOL_SLUG] Action failed: ") + msg + string_empty);
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std:("ACTION_STATE") }, true); });
            auto params = OR((composedState->data->actionParams), (object{}));
            auto failureInputParams = object{
                object::pair{std:("protocols"), params["protocols"]}
            };
            auto errorResult = as<any>(object{
                object::pair{std:("text"), std:("Failed to search for protocols: ") + msg + string_empty}, 
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
            object::pair{std:("text"), std:("What's Aave's TVL?")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Found 4 candidate(s) for 1 search(es)")}, 
            object::pair{std:("actions"), array<string>{ std:("GET_PROTOCOL_SLUG") }}, 
            object::pair{std:("content"), array<object>{ object{
                object::pair{std:("query"), std:("Aave")}, 
                object::pair{std:("candidates"), array<object>{ object{
                    object::pair{std:("id"), std:("2269")}, 
                    object::pair{std:("slug"), std:("aave-v3")}, 
                    object::pair{std:("name"), std:("Aave V3")}, 
                    object::pair{std:("symbol"), std:("AAVE")}, 
                    object::pair{std:("category"), std:("Lending")}, 
                    object::pair{std:("chains"), array<string>{ std:("Ethereum"), std:("Arbitrum"), std:("Polygon") }}, 
                    object::pair{std:("tvl"), 37488847110ll}
                }, object{
                    object::pair{std:("id"), std:("118")}, 
                    object::pair{std:("slug"), std:("aave-v2")}, 
                    object::pair{std:("name"), std:("Aave V2")}, 
                    object::pair{std:("symbol"), std:("AAVE")}, 
                    object::pair{std:("category"), std:("Lending")}, 
                    object::pair{std:("chains"), array<string>{ std:("Ethereum"), std:("Polygon") }}, 
                    object::pair{std:("tvl"), 250317261}
                } }}
            } }}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Aave V3 has $37.5B TVL across Ethereum, Arbitrum, and Polygon. Aave V2 has $250M TVL.")}, 
            object::pair{std:("actions"), array<string>{ std:("GET_PROTOCOL_TVL") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Compare Morpho and Curve TVL")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Found 6 candidate(s) for 2 search(es)")}, 
            object::pair{std:("actions"), array<string>{ std:("GET_PROTOCOL_SLUG") }}, 
            object::pair{std:("content"), array<object>{ object{
                object::pair{std:("query"), std:("Morpho")}, 
                object::pair{std:("candidates"), array<object>{ object{
                    object::pair{std:("id"), std:("2432")}, 
                    object::pair{std:("slug"), std:("morpho-v1")}, 
                    object::pair{std:("name"), std:("Morpho V1")}, 
                    object::pair{std:("symbol"), std:("MORPHO")}, 
                    object::pair{std:("category"), std:("Lending")}, 
                    object::pair{std:("chains"), array<string>{ std:("Ethereum") }}, 
                    object::pair{std:("tvl"), 8358689621ll}
                }, object{
                    object::pair{std:("id"), std:("2711")}, 
                    object::pair{std:("slug"), std:("morpho-v0-aavev3")}, 
                    object::pair{std:("name"), std:("Morpho V0 AaveV3")}, 
                    object::pair{std:("symbol"), std:("MORPHO")}, 
                    object::pair{std:("category"), std:("Lending")}, 
                    object::pair{std:("chains"), array<string>{ std:("Ethereum") }}, 
                    object::pair{std:("tvl"), 157908526}
                } }}
            }, object{
                object::pair{std:("query"), std:("Curve")}, 
                object::pair{std:("candidates"), array<object>{ object{
                    object::pair{std:("id"), std:("3")}, 
                    object::pair{std:("slug"), std:("curve-dex")}, 
                    object::pair{std:("name"), std:("Curve DEX")}, 
                    object::pair{std:("symbol"), std:("CRV")}, 
                    object::pair{std:("category"), std:("Dexs")}, 
                    object::pair{std:("chains"), array<string>{ std:("Ethereum") }}, 
                    object::pair{std:("tvl"), 2297116219ll}
                }, object{
                    object::pair{std:("id"), std:("3331")}, 
                    object::pair{std:("slug"), std:("curve-llamalend")}, 
                    object::pair{std:("name"), std:("Curve LlamaLend")}, 
                    object::pair{std:("symbol"), std:("CRV")}, 
                    object::pair{std:("category"), std:("Lending")}, 
                    object::pair{std:("chains"), array<string>{ std:("Ethereum") }}, 
                    object::pair{std:("tvl"), 88837892}
                } }}
            } }}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Morpho V1: $8.36B TVL. Curve DEX: $2.30B TVL. Morpho has 3.6x higher TVL.")}, 
            object::pair{std:("actions"), array<string>{ std:("GET_PROTOCOL_TVL") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Show me EigenLayer's TVL history")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Found 2 candidate(s) for 1 search(es)")}, 
            object::pair{std:("actions"), array<string>{ std:("GET_PROTOCOL_SLUG") }}, 
            object::pair{std:("content"), array<object>{ object{
                object::pair{std:("query"), std:("EigenLayer")}, 
                object::pair{std:("candidates"), array<object>{ object{
                    object::pair{std:("id"), std:("2442")}, 
                    object::pair{std:("slug"), std:("eigenlayer")}, 
                    object::pair{std:("name"), std:("EigenLayer")}, 
                    object::pair{std:("symbol"), std:("EIGEN")}, 
                    object::pair{std:("category"), std:("Restaking")}, 
                    object::pair{std:("chains"), array<string>{ std:("Ethereum") }}, 
                    object::pair{std:("tvl"), 16138339551ll}
                }, object{
                    object::pair{std:("id"), std:("3282")}, 
                    object::pair{std:("slug"), std:("eigenpie")}, 
                    object::pair{std:("name"), std:("Eigenpie")}, 
                    object::pair{std:("symbol"), std:("-")}, 
                    object::pair{std:("category"), std:("Liquid Restaking")}, 
                    object::pair{std:("chains"), array<string>{ std:("Ethereum") }}, 
                    object::pair{std:("tvl"), 11186759}
                } }}
            } }}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("EigenLayer TVL peaked at $18.2B in March 2024, currently at $16.1B (-11.5%).")}, 
            object::pair{std:("actions"), array<string>{ std:("GET_PROTOCOL_TVL_HISTORY") }}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
