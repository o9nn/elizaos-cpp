#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-web-search/src/actions/webSearch.h"

string MaxTokens(string data, double maxTokens)
{
    return (data->get_length() > maxTokens) ? data->slice(0, maxTokens) : data;
};


double DEFAULT_MAX_WEB_SEARCH_CHARS = 16000;
std::shared_ptr<Action> webSearch = object{
    object::pair{std::string("name"), std::string("WEB_SEARCH")}, 
    object::pair{std::string("similes"), array<string>{ std::string("SEARCH_WEB"), std::string("INTERNET_SEARCH"), std::string("LOOKUP"), std::string("QUERY_WEB"), std::string("FIND_ONLINE"), std::string("SEARCH_ENGINE"), std::string("WEB_LOOKUP"), std::string("ONLINE_SEARCH"), std::string("FIND_INFORMATION") }}, 
    object::pair{std::string("suppressInitialMessage"), true}, 
    object::pair{std::string("description"), std::string("Search the web using Tavily. Supports general web search and finance topics (crypto/DeFi/markets). Use when other actions/providers can't provide accurate or current info.\
\
") + std::string("IMPORTANT - Result Quality Check:\
") + std::string("- If search returns off-topic or poor results, RETRY with parameter adjustments in the SAME round\
") + std::string("- Try: topic='finance' for crypto/markets, source filter (theblock.com, coindesk.com), broader time_range, advanced search_depth, or rephrased query\
") + std::string("- For crypto/DeFi content: use topic='finance' + source from [theblock.com, coindesk.com, decrypt.co, dlnews.com]\
") + std::string("- Don't give up after one attempt if results are clearly irrelevant")}, 
    object::pair{std::string("parameters"), object{
        object::pair{std::string("query"), object{
            object::pair{std::string("type"), std::string("string")}, 
            object::pair{std::string("description"), std::string("The search query to look up on the web")}, 
            object::pair{std::string("required"), true}
        }}, 
        object::pair{std::string("topic"), object{
            object::pair{std::string("type"), std::string("string")}, 
            object::pair{std::string("description"), std::string("Search topic: 'general' for web search, 'finance' for financial/crypto/DeFi content. Defaults to 'general'.")}, 
            object::pair{std::string("required"), false}
        }}, 
        object::pair{std::string("source"), object{
            object::pair{std::string("type"), std::string("string")}, 
            object::pair{std::string("description"), std::string("Specific source domain to limit results (e.g., 'bloomberg.com', 'reuters.com'). Uses site: operator.")}, 
            object::pair{std::string("required"), false}
        }}, 
        object::pair{std::string("max_results"), object{
            object::pair{std::string("type"), std::string("number")}, 
            object::pair{std::string("description"), std::string("Maximum number of results to return (1-20). Defaults to 5.")}, 
            object::pair{std::string("required"), false}
        }}, 
        object::pair{std::string("search_depth"), object{
            object::pair{std::string("type"), std::string("string")}, 
            object::pair{std::string("description"), std::string("Search depth: 'basic' for quick results or 'advanced' for comprehensive search. Defaults to 'basic'.")}, 
            object::pair{std::string("required"), false}
        }}, 
        object::pair{std::string("time_range"), object{
            object::pair{std::string("type"), std::string("string")}, 
            object::pair{std::string("description"), std::string("Time range filter: 'day', 'week', 'month', 'year' (or 'd', 'w', 'm', 'y')")}, 
            object::pair{std::string("required"), false}
        }}, 
        object::pair{std::string("start_date"), object{
            object::pair{std::string("type"), std::string("string")}, 
            object::pair{std::string("description"), std::string("Start date filter in YYYY-MM-DD format (returns results after this date)")}, 
            object::pair{std::string("required"), false}
        }}, 
        object::pair{std::string("end_date"), object{
            object::pair{std::string("type"), std::string("string")}, 
            object::pair{std::string("description"), std::string("End date filter in YYYY-MM-DD format (returns results before this date)")}, 
            object::pair{std::string("required"), false}
        }}
    }}, 
    object::pair{std::string("validate"), [=](auto runtime, auto _message, auto _state = undefined) mutable
    {
        try
        {
            auto service = runtime->getService<std::shared_ptr<TavilyService>>(std::string("TAVILY"));
            return !!service;
        }
        catch (const any& err)
        {
            logger->warn(std::string("TavilyService not available:"), (as<std::shared_ptr<Error>>(err))->message);
            return false;
        }
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto _state, auto _options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto tavilyService = runtime->getService<std::shared_ptr<TavilyService>>(std::string("TAVILY"));
            if (!tavilyService) {
                throw any(std::make_shared<Error>(std::string("TavilyService not initialized")));
            }
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std::string("ACTION_STATE") }, true); });
            auto params = OR((OR((composedState->data->actionParams), (composedState->data->webSearch))), (object{}));
            auto query = params["query"]["trim"]();
            if (!query) {
                auto errorMsg = std::string("Missing required parameter 'query'. Please specify what to search for.");
                logger->error(std::string("[WEB_SEARCH] ") + errorMsg + string_empty);
                auto emptyResult = object{
                    object::pair{std::string("text"), errorMsg}, 
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("error"), std::string("missing_required_parameter")}
                };
                if (callback) {
                    callback(object{
                        object::pair{std::string("text"), emptyResult->text}, 
                        object::pair{std::string("content"), object{
                            object::pair{std::string("error"), std::string("missing_required_parameter")}, 
                            object::pair{std::string("details"), errorMsg}
                        }}
                    });
                }
                return emptyResult;
            }
            auto source = params["source"]["trim"]();
            auto topic = (params["topic"] == std::string("finance")) ? std::string("finance") : std::string("general");
            auto maxResults = (params["max_results"]) ? any(Math->min(Math->max(1, params["max_results"]), 20)) : any(5);
            auto searchDepth = (params["search_depth"] == std::string("advanced")) ? std::string("advanced") : std::string("basic");
            auto enhancedQuery = query;
            if (source) {
                enhancedQuery = string_empty + query + std::string(" site:") + source + string_empty;
                logger->info(std::string("[WEB_SEARCH] Searching with source filter: ") + source + string_empty);
            }
            logger->info(std::string("[WEB_SEARCH] Searching for: "") + enhancedQuery + std::string("" (topic: ") + topic + std::string(")"));
            auto inputParams = object{
                object::pair{std::string("query"), std::string("query")}, 
                object::pair{std::string("topic"), std::string("topic")}, 
                object::pair{std::string("source"), std::string("source")}, 
                object::pair{std::string("max_results"), maxResults}, 
                object::pair{std::string("search_depth"), searchDepth}, 
                object::pair{std::string("time_range"), params["time_range"]}, 
                object::pair{std::string("start_date"), params["start_date"]}, 
                object::pair{std::string("end_date"), params["end_date"]}
            };
            auto searchResponse = std::async([=]() { tavilyService->search(enhancedQuery, object{
                object::pair{std::string("topic"), std::string("topic")}, 
                object::pair{std::string("max_results"), maxResults}, 
                object::pair{std::string("search_depth"), searchDepth}, 
                object::pair{std::string("time_range"), params["time_range"]}, 
                object::pair{std::string("start_date"), params["start_date"]}, 
                object::pair{std::string("end_date"), params["end_date"]}, 
                object::pair{std::string("include_answer"), true}, 
                object::pair{std::string("include_images"), false}
            }); });
            if (AND((searchResponse), (searchResponse->results->length))) {
                auto responseList = (searchResponse->answer) ? any(string_empty + searchResponse->answer + string_empty + (AND((Array->isArray(searchResponse->results)), (searchResponse->results->length > 0))) ? any(std::string("\
\
For more details, you can check out these resources:\
") + searchResponse->results->map([=](auto result, auto index) mutable
                {
                    return string_empty + (index + 1) + std::string(". [") + result["title"] + std::string("](") + result["url"] + std::string(")");
                }
                )->join(std::string("\
")) + string_empty) : any(string_empty) + string_empty) : any(string_empty);
                auto result = as<any>(object{
                    object::pair{std::string("text"), MaxTokens(responseList, DEFAULT_MAX_WEB_SEARCH_CHARS)}, 
                    object::pair{std::string("success"), true}, 
                    object::pair{std::string("data"), searchResponse}, 
                    object::pair{std::string("input"), inputParams}
                });
                if (callback) {
                    callback(object{
                        object::pair{std::string("text"), result->text}, 
                        object::pair{std::string("actions"), array<string>{ std::string("WEB_SEARCH") }}, 
                        object::pair{std::string("data"), result->data}
                    });
                }
                return result;
            }
            auto noResult = as<any>(object{
                object::pair{std::string("text"), std::string("I couldn't find relevant results for that query.")}, 
                object::pair{std::string("success"), false}, 
                object::pair{std::string("input"), inputParams}
            });
            if (callback) {
                callback(object{
                    object::pair{std::string("text"), noResult->text}
                });
            }
            return noResult;
        }
        catch (const any& error)
        {
            auto errMsg = (is<Error>(error)) ? error->message : String(error);
            logger->error(std::string("[WEB_SEARCH] Action failed: ") + errMsg + string_empty);
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std::string("ACTION_STATE") }, true); });
            auto params = OR((OR((composedState->data->actionParams), (composedState->data->webSearch))), (object{}));
            auto failureInputParams = object{
                object::pair{std::string("query"), params["query"]}, 
                object::pair{std::string("topic"), params["topic"]}, 
                object::pair{std::string("source"), params["source"]}, 
                object::pair{std::string("max_results"), params["max_results"]}, 
                object::pair{std::string("search_depth"), params["search_depth"]}, 
                object::pair{std::string("time_range"), params["time_range"]}, 
                object::pair{std::string("start_date"), params["start_date"]}, 
                object::pair{std::string("end_date"), params["end_date"]}
            };
            auto errorResult = as<any>(object{
                object::pair{std::string("text"), std::string("Web search failed: ") + errMsg + string_empty}, 
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), errMsg}, 
                object::pair{std::string("input"), failureInputParams}
            });
            if (callback) {
                callback(object{
                    object::pair{std::string("text"), errorResult->text}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("error"), std::string("web_search_failed")}, 
                        object::pair{std::string("details"), errMsg}
                    }}
                });
            }
            return errorResult;
        }
    }
    }, 
    object::pair{std::string("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Latest Aave news")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Let me search for Aave news from crypto sources:")}, 
            object::pair{std::string("action"), std::string("WEB_SEARCH")}, 
            object::pair{std::string("actionParams"), object{
                object::pair{std::string("query"), std::string("Aave protocol")}, 
                object::pair{std::string("topic"), std::string("finance")}, 
                object::pair{std::string("source"), std::string("theblock.com")}, 
                object::pair{std::string("time_range"), std::string("week")}
            }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Find the latest news about SpaceX launches.")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Here is the latest news about SpaceX launches:")}, 
            object::pair{std::string("action"), std::string("WEB_SEARCH")}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Can you find details about the iPhone 16 release?")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Here are the details I found about the iPhone 16 release:")}, 
            object::pair{std::string("action"), std::string("WEB_SEARCH")}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("What is the schedule for the next FIFA World Cup?")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Here is the schedule for the next FIFA World Cup:")}, 
            object::pair{std::string("action"), std::string("WEB_SEARCH")}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Check the latest stock price of Tesla.")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Here is the latest stock price of Tesla I found:")}, 
            object::pair{std::string("action"), std::string("WEB_SEARCH")}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("What are the current trending movies in the US?")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Here are the current trending movies in the US:")}, 
            object::pair{std::string("action"), std::string("WEB_SEARCH")}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("What is the latest score in the NBA finals?")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Here is the latest score from the NBA finals:")}, 
            object::pair{std::string("action"), std::string("WEB_SEARCH")}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("When is the next Apple keynote event?")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Here is the information about the next Apple keynote event:")}, 
            object::pair{std::string("action"), std::string("WEB_SEARCH")}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
