#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-web-search/src/actions/webSearch.h"

string MaxTokens(string data, double maxTokens)
{
    return (data->get_length() > maxTokens) ? data->slice(0, maxTokens) : data;
};


double DEFAULT_MAX_WEB_SEARCH_CHARS = 16000;
std::shared_ptr<Action> webSearch = object{
    object::pair{std:("name"), std:("WEB_SEARCH")}, 
    object::pair{std:("similes"), array<string>{ std:("SEARCH_WEB"), std:("INTERNET_SEARCH"), std:("LOOKUP"), std:("QUERY_WEB"), std:("FIND_ONLINE"), std:("SEARCH_ENGINE"), std:("WEB_LOOKUP"), std:("ONLINE_SEARCH"), std:("FIND_INFORMATION") }}, 
    object::pair{std:("suppressInitialMessage"), true}, 
    object::pair{std:("description"), std:("Search the web using Tavily. Supports general web search and finance topics (crypto/DeFi/markets). Use when other actions/providers can't provide accurate or current info.\
\
") + std:("IMPORTANT - Result Quality Check:\
") + std:("- If search returns off-topic or poor results, RETRY with parameter adjustments in the SAME round\
") + std:("- Try: topic='finance' for crypto/markets, source filter (theblock.com, coindesk.com), broader time_range, advanced search_depth, or rephrased query\
") + std:("- For crypto/DeFi content: use topic='finance' + source from [theblock.com, coindesk.com, decrypt.co, dlnews.com]\
") + std:("- Don't give up after one attempt if results are clearly irrelevant")}, 
    object::pair{std:("parameters"), object{
        object::pair{std:("query"), object{
            object::pair{std:("type"), std:("string")}, 
            object::pair{std:("description"), std:("The search query to look up on the web")}, 
            object::pair{std:("required"), true}
        }}, 
        object::pair{std:("topic"), object{
            object::pair{std:("type"), std:("string")}, 
            object::pair{std:("description"), std:("Search topic: 'general' for web search, 'finance' for financial/crypto/DeFi content. Defaults to 'general'.")}, 
            object::pair{std:("required"), false}
        }}, 
        object::pair{std:("source"), object{
            object::pair{std:("type"), std:("string")}, 
            object::pair{std:("description"), std:("Specific source domain to limit results (e.g., 'bloomberg.com', 'reuters.com'). Uses site: operator.")}, 
            object::pair{std:("required"), false}
        }}, 
        object::pair{std:("max_results"), object{
            object::pair{std:("type"), std:("number")}, 
            object::pair{std:("description"), std:("Maximum number of results to return (1-20). Defaults to 5.")}, 
            object::pair{std:("required"), false}
        }}, 
        object::pair{std:("search_depth"), object{
            object::pair{std:("type"), std:("string")}, 
            object::pair{std:("description"), std:("Search depth: 'basic' for quick results or 'advanced' for comprehensive search. Defaults to 'basic'.")}, 
            object::pair{std:("required"), false}
        }}, 
        object::pair{std:("time_range"), object{
            object::pair{std:("type"), std:("string")}, 
            object::pair{std:("description"), std:("Time range filter: 'day', 'week', 'month', 'year' (or 'd', 'w', 'm', 'y')")}, 
            object::pair{std:("required"), false}
        }}, 
        object::pair{std:("start_date"), object{
            object::pair{std:("type"), std:("string")}, 
            object::pair{std:("description"), std:("Start date filter in YYYY-MM-DD format (returns results after this date)")}, 
            object::pair{std:("required"), false}
        }}, 
        object::pair{std:("end_date"), object{
            object::pair{std:("type"), std:("string")}, 
            object::pair{std:("description"), std:("End date filter in YYYY-MM-DD format (returns results before this date)")}, 
            object::pair{std:("required"), false}
        }}
    }}, 
    object::pair{std:("validate"), [=](auto runtime, auto _message, auto _state = undefined) mutable
    {
        try
        {
            auto service = runtime->getService<std::shared_ptr<TavilyService>>(std:("TAVILY"));
            return !!service;
        }
        catch (const any& err)
        {
            logger->warn(std:("TavilyService not available:"), (as<std::shared_ptr<Error>>(err))->message);
            return false;
        }
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto _state, auto _options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto tavilyService = runtime->getService<std::shared_ptr<TavilyService>>(std:("TAVILY"));
            if (!tavilyService) {
                throw any(std::make_shared<Error>(std:("TavilyService not initialized")));
            }
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std:("ACTION_STATE") }, true); });
            auto params = OR((OR((composedState->data->actionParams), (composedState->data->webSearch))), (object{}));
            auto query = params["query"]["trim"]();
            if (!query) {
                auto errorMsg = std:("Missing required parameter 'query'. Please specify what to search for.");
                logger->error(std:("[WEB_SEARCH] ") + errorMsg + string_empty);
                auto emptyResult = object{
                    object::pair{std:("text"), errorMsg}, 
                    object::pair{std:("success"), false}, 
                    object::pair{std:("error"), std:("missing_required_parameter")}
                };
                if (callback) {
                    callback(object{
                        object::pair{std:("text"), emptyResult->text}, 
                        object::pair{std:("content"), object{
                            object::pair{std:("error"), std:("missing_required_parameter")}, 
                            object::pair{std:("details"), errorMsg}
                        }}
                    });
                }
                return emptyResult;
            }
            auto source = params["source"]["trim"]();
            auto topic = (params["topic"] == std:("finance")) ? std:("finance") : std:("general");
            auto maxResults = (params["max_results"]) ? any(Math->min(Math->max(1, params["max_results"]), 20)) (5);
            auto searchDepth = (params["search_depth"] == std:("advanced")) ? std:("advanced") : std:("basic");
            auto enhancedQuery = query;
            if (source) {
                enhancedQuery = string_empty + query + std:(" site:") + source + string_empty;
                logger->info(std:("[WEB_SEARCH] Searching with source filter: ") + source + string_empty);
            }
            logger->info(std:("[WEB_SEARCH] Searching for: "") + enhancedQuery + std:("" (topic: ") + topic + std:(")"));
            auto inputParams = object{
                object::pair{std:("query"), std:("query")}, 
                object::pair{std:("topic"), std:("topic")}, 
                object::pair{std:("source"), std:("source")}, 
                object::pair{std:("max_results"), maxResults}, 
                object::pair{std:("search_depth"), searchDepth}, 
                object::pair{std:("time_range"), params["time_range"]}, 
                object::pair{std:("start_date"), params["start_date"]}, 
                object::pair{std:("end_date"), params["end_date"]}
            };
            auto searchResponse = std::async([=]() { tavilyService->search(enhancedQuery, object{
                object::pair{std:("topic"), std:("topic")}, 
                object::pair{std:("max_results"), maxResults}, 
                object::pair{std:("search_depth"), searchDepth}, 
                object::pair{std:("time_range"), params["time_range"]}, 
                object::pair{std:("start_date"), params["start_date"]}, 
                object::pair{std:("end_date"), params["end_date"]}, 
                object::pair{std:("include_answer"), true}, 
                object::pair{std:("include_images"), false}
            }); });
            if (AND((searchResponse), (searchResponse->results->length))) {
                auto responseList = (searchResponse->answer) ? any(string_empty + searchResponse->answer + string_empty + (AND((Array->isArray(searchResponse->results)), (searchResponse->results->length > 0))) ? any(std:("\
\
For more details, you can check out these resources:\
") + searchResponse->results->map([=](auto result, auto index) mutable
                {
                    return string_empty + (index + 1) + std:(". [") + result["title"] + std:("](") + result["url"] + std:(")");
                }
                )->join(std:("\
")) + string_empty) (string_empty) + string_empty) (string_empty);
                auto result = as<any>(object{
                    object::pair{std:("text"), MaxTokens(responseList, DEFAULT_MAX_WEB_SEARCH_CHARS)}, 
                    object::pair{std:("success"), true}, 
                    object::pair{std:("data"), searchResponse}, 
                    object::pair{std:("input"), inputParams}
                });
                if (callback) {
                    callback(object{
                        object::pair{std:("text"), result->text}, 
                        object::pair{std:("actions"), array<string>{ std:("WEB_SEARCH") }}, 
                        object::pair{std:("data"), result->data}
                    });
                }
                return result;
            }
            auto noResult = as<any>(object{
                object::pair{std:("text"), std:("I couldn't find relevant results for that query.")}, 
                object::pair{std:("success"), false}, 
                object::pair{std:("input"), inputParams}
            });
            if (callback) {
                callback(object{
                    object::pair{std:("text"), noResult->text}
                });
            }
            return noResult;
        }
        catch (const any& error)
        {
            auto errMsg = (is<Error>(error)) ? error->message : String(error);
            logger->error(std:("[WEB_SEARCH] Action failed: ") + errMsg + string_empty);
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std:("ACTION_STATE") }, true); });
            auto params = OR((OR((composedState->data->actionParams), (composedState->data->webSearch))), (object{}));
            auto failureInputParams = object{
                object::pair{std:("query"), params["query"]}, 
                object::pair{std:("topic"), params["topic"]}, 
                object::pair{std:("source"), params["source"]}, 
                object::pair{std:("max_results"), params["max_results"]}, 
                object::pair{std:("search_depth"), params["search_depth"]}, 
                object::pair{std:("time_range"), params["time_range"]}, 
                object::pair{std:("start_date"), params["start_date"]}, 
                object::pair{std:("end_date"), params["end_date"]}
            };
            auto errorResult = as<any>(object{
                object::pair{std:("text"), std:("Web search failed: ") + errMsg + string_empty}, 
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), errMsg}, 
                object::pair{std:("input"), failureInputParams}
            });
            if (callback) {
                callback(object{
                    object::pair{std:("text"), errorResult->text}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("error"), std:("web_search_failed")}, 
                        object::pair{std:("details"), errMsg}
                    }}
                });
            }
            return errorResult;
        }
    }
    }, 
    object::pair{std:("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Latest Aave news")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Let me search for Aave news from crypto sources:")}, 
            object::pair{std:("action"), std:("WEB_SEARCH")}, 
            object::pair{std:("actionParams"), object{
                object::pair{std:("query"), std:("Aave protocol")}, 
                object::pair{std:("topic"), std:("finance")}, 
                object::pair{std:("source"), std:("theblock.com")}, 
                object::pair{std:("time_range"), std:("week")}
            }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Find the latest news about SpaceX launches.")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Here is the latest news about SpaceX launches:")}, 
            object::pair{std:("action"), std:("WEB_SEARCH")}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Can you find details about the iPhone 16 release?")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Here are the details I found about the iPhone 16 release:")}, 
            object::pair{std:("action"), std:("WEB_SEARCH")}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("What is the schedule for the next FIFA World Cup?")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Here is the schedule for the next FIFA World Cup:")}, 
            object::pair{std:("action"), std:("WEB_SEARCH")}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Check the latest stock price of Tesla.")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Here is the latest stock price of Tesla I found:")}, 
            object::pair{std:("action"), std:("WEB_SEARCH")}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("What are the current trending movies in the US?")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Here are the current trending movies in the US:")}, 
            object::pair{std:("action"), std:("WEB_SEARCH")}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("What is the latest score in the NBA finals?")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Here is the latest score from the NBA finals:")}, 
            object::pair{std:("action"), std:("WEB_SEARCH")}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("When is the next Apple keynote event?")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Here is the information about the next Apple keynote event:")}, 
            object::pair{std:("action"), std:("WEB_SEARCH")}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
