#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-defillama/src/actions/getProtocolSlug.action.h"

std::shared_ptr<Action> getProtocolSlugAction = object{
    object::pair{std::string("name"), std::string("GET_PROTOCOL_SLUG")}, 
    object::pair{std::string("similes"), array<string>{ std::string("PROTOCOL_SLUG"), std::string("FIND_PROTOCOL"), std::string("SEARCH_PROTOCOL"), std::string("PROTOCOL_INFO"), std::string("DEFILLAMA_PROTOCOL_SLUG") }}, 
    object::pair{std::string("description"), std::string("Use this action to search for DeFi protocol slugs and basic information by protocol name or symbol. Returns protocol slugs needed for TVL history lookups.")}, 
    object::pair{std::string("parameters"), object{
        object::pair{std::string("protocols"), object{
            object::pair{std::string("type"), std::string("string")}, 
            object::pair{std::string("description"), std::string("Comma-separated list of DeFi protocol names or symbols to search for (e.g., 'Aave,Curve' or 'EIGEN,MORPHO')")}, 
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
                auto errorMsg = std::string("Missing required parameter 'protocols'. Please specify which DeFi protocol(s) to search for (e.g., 'Aave,Curve' or 'EIGEN,MORPHO').");
                logger->error(std::string("[GET_PROTOCOL_SLUG] ") + errorMsg + string_empty);
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
                logger->error(std::string("[GET_PROTOCOL_SLUG] ") + errorMsg + string_empty);
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
            logger->info(std::string("[GET_PROTOCOL_SLUG] Searching for protocols: ") + names->join(std::string(", ")) + string_empty);
            auto inputParams = object{
                object::pair{std::string("protocols"), protocolsRaw}
            };
            auto searchResults = array<SearchResult>();
            for (auto& query : names)
            {
                auto candidates = std::async([=]() { svc->searchProtocolCandidates(query, 5); });
                auto candidateInfos = candidates->map([=](auto protocol) mutable
                {
                    return (object{
                        object::pair{std::string("id"), protocol["id"]}, 
                        object::pair{std::string("slug"), protocol["slug"]}, 
                        object::pair{std::string("name"), protocol["name"]}, 
                        object::pair{std::string("symbol"), protocol["symbol"]}, 
                        object::pair{std::string("category"), protocol["category"]}, 
                        object::pair{std::string("chains"), protocol["chains"]}, 
                        object::pair{std::string("url"), protocol["url"]}, 
                        object::pair{std::string("logo"), protocol["logo"]}, 
                        object::pair{std::string("tvl"), protocol["tvl"]}
                    });
                }
                );
                searchResults->push(object{
                    object::pair{std::string("query"), std::string("query")}, 
                    object::pair{std::string("candidates"), candidateInfos}
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
                auto errorMsg = std::string("No protocols matched any of the provided names");
                logger->error(std::string("[GET_PROTOCOL_SLUG] ") + errorMsg + string_empty);
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
            auto messageText = (queriesWithNoResults > 0) ? std::string("Found ") + totalCandidates + std::string(" candidate(s) for ") + searchResults->get_length() + std::string(" search(es); ") + queriesWithNoResults + std::string(" search(es) had no matches") : std::string("Found ") + totalCandidates + std::string(" candidate(s) for ") + searchResults->get_length() + std::string(" search(es)");
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), messageText}, 
                    object::pair{std::string("actions"), array<string>{ std::string("GET_PROTOCOL_SLUG") }}, 
                    object::pair{std::string("content"), searchResults}, 
                    object::pair{std::string("source"), message->content->source}
                }); });
            }
            return as<any>(object{
                object::pair{std::string("text"), messageText}, 
                object::pair{std::string("success"), true}, 
                object::pair{std::string("data"), searchResults}, 
                object::pair{std::string("values"), searchResults}, 
                object::pair{std::string("input"), inputParams}
            });
        }
        catch (const any& error)
        {
            auto msg = (is<Error>(error)) ? error->message : String(error);
            logger->error(std::string("[GET_PROTOCOL_SLUG] Action failed: ") + msg + string_empty);
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std::string("ACTION_STATE") }, true); });
            auto params = OR((composedState->data->actionParams), (object{}));
            auto failureInputParams = object{
                object::pair{std::string("protocols"), params["protocols"]}
            };
            auto errorResult = as<any>(object{
                object::pair{std::string("text"), std::string("Failed to search for protocols: ") + msg + string_empty}, 
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
            object::pair{std::string("text"), std::string("What's Aave's TVL?")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Found 4 candidate(s) for 1 search(es)")}, 
            object::pair{std::string("actions"), array<string>{ std::string("GET_PROTOCOL_SLUG") }}, 
            object::pair{std::string("content"), array<object>{ object{
                object::pair{std::string("query"), std::string("Aave")}, 
                object::pair{std::string("candidates"), array<object>{ object{
                    object::pair{std::string("id"), std::string("2269")}, 
                    object::pair{std::string("slug"), std::string("aave-v3")}, 
                    object::pair{std::string("name"), std::string("Aave V3")}, 
                    object::pair{std::string("symbol"), std::string("AAVE")}, 
                    object::pair{std::string("category"), std::string("Lending")}, 
                    object::pair{std::string("chains"), array<string>{ std::string("Ethereum"), std::string("Arbitrum"), std::string("Polygon") }}, 
                    object::pair{std::string("tvl"), 37488847110ll}
                }, object{
                    object::pair{std::string("id"), std::string("118")}, 
                    object::pair{std::string("slug"), std::string("aave-v2")}, 
                    object::pair{std::string("name"), std::string("Aave V2")}, 
                    object::pair{std::string("symbol"), std::string("AAVE")}, 
                    object::pair{std::string("category"), std::string("Lending")}, 
                    object::pair{std::string("chains"), array<string>{ std::string("Ethereum"), std::string("Polygon") }}, 
                    object::pair{std::string("tvl"), 250317261}
                } }}
            } }}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Aave V3 has $37.5B TVL across Ethereum, Arbitrum, and Polygon. Aave V2 has $250M TVL.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("GET_PROTOCOL_TVL") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Compare Morpho and Curve TVL")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Found 6 candidate(s) for 2 search(es)")}, 
            object::pair{std::string("actions"), array<string>{ std::string("GET_PROTOCOL_SLUG") }}, 
            object::pair{std::string("content"), array<object>{ object{
                object::pair{std::string("query"), std::string("Morpho")}, 
                object::pair{std::string("candidates"), array<object>{ object{
                    object::pair{std::string("id"), std::string("2432")}, 
                    object::pair{std::string("slug"), std::string("morpho-v1")}, 
                    object::pair{std::string("name"), std::string("Morpho V1")}, 
                    object::pair{std::string("symbol"), std::string("MORPHO")}, 
                    object::pair{std::string("category"), std::string("Lending")}, 
                    object::pair{std::string("chains"), array<string>{ std::string("Ethereum") }}, 
                    object::pair{std::string("tvl"), 8358689621ll}
                }, object{
                    object::pair{std::string("id"), std::string("2711")}, 
                    object::pair{std::string("slug"), std::string("morpho-v0-aavev3")}, 
                    object::pair{std::string("name"), std::string("Morpho V0 AaveV3")}, 
                    object::pair{std::string("symbol"), std::string("MORPHO")}, 
                    object::pair{std::string("category"), std::string("Lending")}, 
                    object::pair{std::string("chains"), array<string>{ std::string("Ethereum") }}, 
                    object::pair{std::string("tvl"), 157908526}
                } }}
            }, object{
                object::pair{std::string("query"), std::string("Curve")}, 
                object::pair{std::string("candidates"), array<object>{ object{
                    object::pair{std::string("id"), std::string("3")}, 
                    object::pair{std::string("slug"), std::string("curve-dex")}, 
                    object::pair{std::string("name"), std::string("Curve DEX")}, 
                    object::pair{std::string("symbol"), std::string("CRV")}, 
                    object::pair{std::string("category"), std::string("Dexs")}, 
                    object::pair{std::string("chains"), array<string>{ std::string("Ethereum") }}, 
                    object::pair{std::string("tvl"), 2297116219ll}
                }, object{
                    object::pair{std::string("id"), std::string("3331")}, 
                    object::pair{std::string("slug"), std::string("curve-llamalend")}, 
                    object::pair{std::string("name"), std::string("Curve LlamaLend")}, 
                    object::pair{std::string("symbol"), std::string("CRV")}, 
                    object::pair{std::string("category"), std::string("Lending")}, 
                    object::pair{std::string("chains"), array<string>{ std::string("Ethereum") }}, 
                    object::pair{std::string("tvl"), 88837892}
                } }}
            } }}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Morpho V1: $8.36B TVL. Curve DEX: $2.30B TVL. Morpho has 3.6x higher TVL.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("GET_PROTOCOL_TVL") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Show me EigenLayer's TVL history")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Found 2 candidate(s) for 1 search(es)")}, 
            object::pair{std::string("actions"), array<string>{ std::string("GET_PROTOCOL_SLUG") }}, 
            object::pair{std::string("content"), array<object>{ object{
                object::pair{std::string("query"), std::string("EigenLayer")}, 
                object::pair{std::string("candidates"), array<object>{ object{
                    object::pair{std::string("id"), std::string("2442")}, 
                    object::pair{std::string("slug"), std::string("eigenlayer")}, 
                    object::pair{std::string("name"), std::string("EigenLayer")}, 
                    object::pair{std::string("symbol"), std::string("EIGEN")}, 
                    object::pair{std::string("category"), std::string("Restaking")}, 
                    object::pair{std::string("chains"), array<string>{ std::string("Ethereum") }}, 
                    object::pair{std::string("tvl"), 16138339551ll}
                }, object{
                    object::pair{std::string("id"), std::string("3282")}, 
                    object::pair{std::string("slug"), std::string("eigenpie")}, 
                    object::pair{std::string("name"), std::string("Eigenpie")}, 
                    object::pair{std::string("symbol"), std::string("-")}, 
                    object::pair{std::string("category"), std::string("Liquid Restaking")}, 
                    object::pair{std::string("chains"), array<string>{ std::string("Ethereum") }}, 
                    object::pair{std::string("tvl"), 11186759}
                } }}
            } }}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("EigenLayer TVL peaked at $18.2B in March 2024, currently at $16.1B (-11.5%).")}, 
            object::pair{std::string("actions"), array<string>{ std::string("GET_PROTOCOL_TVL_HISTORY") }}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
