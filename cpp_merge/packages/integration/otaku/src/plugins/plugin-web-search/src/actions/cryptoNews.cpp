#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-web-search/src/actions/cryptoNews.h"

string MaxTokens(string data, double maxTokens)
{
    return (data->get_length() > maxTokens) ? data->slice(0, maxTokens) : data;
};


double DEFAULT_MAX_CRYPTO_NEWS_CHARS = 20000;
object CRYPTO_NEWS_SOURCES = as<std::shared_ptr<const>>(object{
    object::pair{std::string("theblock"), std::string("theblock.co")}, 
    object::pair{std::string("coindesk"), std::string("coindesk.com")}, 
    object::pair{std::string("decrypt"), std::string("decrypt.co")}, 
    object::pair{std::string("dlnews"), std::string("dlnews.com")}, 
    object::pair{std::string("coinbureau"), std::string("coinbureau.com")}, 
    object::pair{std::string("cointelegraph"), std::string("cointelegraph.com")}, 
    object::pair{std::string("blockworks"), std::string("blockworks.co")}
});
std::shared_ptr<Action> cryptoNews = as<std::shared_ptr<Action>>(object{
    object::pair{std::string("name"), std::string("CRYPTO_NEWS")}, 
    object::pair{std::string("similes"), array<string>{ std::string("BLOCKCHAIN_NEWS"), std::string("DEFI_NEWS"), std::string("CRYPTO_UPDATES"), std::string("WEB3_NEWS"), std::string("CRYPTOCURRENCY_NEWS"), std::string("GET_CRYPTO_NEWS"), std::string("LATEST_CRYPTO"), std::string("CRYPTO_HEADLINES") }}, 
    object::pair{std::string("suppressInitialMessage"), true}, 
    object::pair{std::string("description"), std::string("Search for cryptocurrency, blockchain, DeFi, and Web3 news from reputable crypto-focused sources.\
\
") + std::string("**CoinDesk API** (when configured with COINDESK_API_KEY):\
") + std::string("- Direct access to CoinDesk's news database (100+ articles available per query)\
") + std::string("- Rich filtering: categories (markets/tech/policy/defi/nft/layer-2/regulation), keywords, authors, tags\
") + std::string("- Full metadata: title, summary, optional body, publish dates, authors, thumbnails\
") + std::string("- Date range filtering with automatic time_range conversion\
") + std::string("- Sorted by relevance or publish date\
\
") + std::string("**Tavily Fallback** (for other sources or when CoinDesk unavailable):\
") + std::string("- Uses finance topic for crypto-focused results\
") + std::string("- Site filtering for TheBlock, Decrypt, DL News, Coinbureau, Cointelegraph, Blockworks\
") + std::string("- Up to 20 results with answer synthesis")}, 
    object::pair{std::string("parameters"), object{
        object::pair{std::string("query"), object{
            object::pair{std::string("type"), std::string("string")}, 
            object::pair{std::string("description"), std::string("The crypto/blockchain/DeFi news query (e.g., 'Aave', 'Ethereum merge', 'DeFi hacks')")}, 
            object::pair{std::string("required"), true}
        }}, 
        object::pair{std::string("source"), object{
            object::pair{std::string("type"), std::string("string")}, 
            object::pair{std::string("description"), std::string("Specific crypto news source: 'theblock', 'coindesk', 'decrypt', 'dlnews', 'coinbureau', 'cointelegraph', 'blockworks'. Leave empty to search all sources.")}, 
            object::pair{std::string("required"), false}
        }}, 
        object::pair{std::string("categories"), object{
            object::pair{std::string("type"), std::string("array")}, 
            object::pair{std::string("description"), std::string("CoinDesk categories to filter by: 'markets', 'tech', 'policy', 'defi', 'nft', 'layer-2', 'regulation'. Only applies when using CoinDesk API.")}, 
            object::pair{std::string("required"), false}
        }}, 
        object::pair{std::string("time_range"), object{
            object::pair{std::string("type"), std::string("string")}, 
            object::pair{std::string("description"), std::string("Time range filter: 'day', 'week', 'month', 'year' (or 'd', 'w', 'm', 'y'). Defaults to 'week' for recent news.")}, 
            object::pair{std::string("required"), false}
        }}, 
        object::pair{std::string("max_results"), object{
            object::pair{std::string("type"), std::string("number")}, 
            object::pair{std::string("description"), std::string("Maximum number of results (1-100 for CoinDesk API, 1-20 for Tavily). Defaults to 10.")}, 
            object::pair{std::string("required"), false}
        }}, 
        object::pair{std::string("search_depth"), object{
            object::pair{std::string("type"), std::string("string")}, 
            object::pair{std::string("description"), std::string("Search depth: 'basic' or 'advanced'. Only applies to Tavily fallback. Defaults to 'basic'.")}, 
            object::pair{std::string("required"), false}
        }}, 
        object::pair{std::string("include_body"), object{
            object::pair{std::string("type"), std::string("boolean")}, 
            object::pair{std::string("description"), std::string("Include full article body in response (CoinDesk API only). Defaults to false for performance.")}, 
            object::pair{std::string("required"), false}
        }}
    }}, 
    object::pair{std::string("validate"), [=](auto runtime, auto _message, auto _state = undefined) mutable
    {
        try
        {
            auto coindeskService = runtime->getService<std::shared_ptr<CoinDeskService>>(std::string("COINDESK_NEWS"));
            auto tavilyService = runtime->getService<std::shared_ptr<TavilyService>>(std::string("TAVILY"));
            return !!(OR((coindeskService), (tavilyService)));
        }
        catch (const any& err)
        {
            logger->warn(std::string("No news service available:"), (as<std::shared_ptr<Error>>(err))->message);
            return false;
        }
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto _state, auto _options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std::string("ACTION_STATE") }, true); });
            auto params = OR((OR((composedState->data->actionParams), (composedState->data->cryptoNews))), (object{}));
            auto query = params["query"]["trim"]();
            if (!query) {
                auto errorMsg = std::string("Missing required parameter 'query'. Please specify what crypto news to search for.");
                logger->error(std::string("[CRYPTO_NEWS] ") + errorMsg + string_empty);
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
            auto sourceKey = params["source"]["toLowerCase"]()["trim"]();
            auto timeRange = OR((params["time_range"]), (std::string("week")));
            auto maxResults = (params["max_results"]) ? any(Math->min(Math->max(1, params["max_results"]), 100)) : any(10);
            auto searchDepth = (params["search_depth"] == std::string("advanced")) ? std::string("advanced") : std::string("basic");
            shared categories = params["categories"];
            shared includeBody = params["include_body"] == true;
            auto inputParams = object{
                object::pair{std::string("query"), std::string("query")}, 
                object::pair{std::string("source"), sourceKey}, 
                object::pair{std::string("categories"), std::string("categories")}, 
                object::pair{std::string("time_range"), timeRange}, 
                object::pair{std::string("max_results"), maxResults}, 
                object::pair{std::string("search_depth"), searchDepth}, 
                object::pair{std::string("include_body"), includeBody}
            };
            any startDate;
            any endDate;
            if (timeRange) {
                auto now = std::make_shared<Date>();
                endDate = const_(now->toISOString()->split(std::string("T")))[0];
                static switch_type __switch6521_7428 = {
                    { any(std::string("day")), 1 },
                    { any(std::string("d")), 2 },
                    { any(std::string("week")), 3 },
                    { any(std::string("w")), 4 },
                    { any(std::string("month")), 5 },
                    { any(std::string("m")), 6 },
                    { any(std::string("year")), 7 },
                    { any(std::string("y")), 8 }
                };
                switch (__switch6521_7428[timeRange])
                {
                case 1:
                case 2:
                    startDate = const_(((std::make_shared<Date>(now->getTime() - 24 * 60 * 60 * 1000)))->toISOString()->split(std::string("T")))[0];
                    break;
                case 3:
                case 4:
                    startDate = const_(((std::make_shared<Date>(now->getTime() - 7 * 24 * 60 * 60 * 1000)))->toISOString()->split(std::string("T")))[0];
                    break;
                case 5:
                case 6:
                    startDate = const_(((std::make_shared<Date>(now->getTime() - 30 * 24 * 60 * 60 * 1000)))->toISOString()->split(std::string("T")))[0];
                    break;
                case 7:
                case 8:
                    startDate = const_(((std::make_shared<Date>(now->getTime() - 365 * 24 * 60 * 60 * 1000)))->toISOString()->split(std::string("T")))[0];
                    break;
                }
            }
            auto coindeskService = runtime->getService<std::shared_ptr<CoinDeskService>>(std::string("COINDESK_NEWS"));
            if (AND((AND((coindeskService), (coindeskService->isConfigured()))), ((OR((!sourceKey), (sourceKey == std::string("coindesk"))))))) {
                logger->info(std::string("[CRYPTO_NEWS] Using CoinDesk API for: ") + query + string_empty);
                auto coindeskResponse = std::async([=]() { coindeskService->searchNews(object{
                    object::pair{std::string("query"), std::string("query")}, 
                    object::pair{std::string("limit"), maxResults}, 
                    object::pair{std::string("categories"), std::string("categories")}, 
                    object::pair{std::string("startDate"), std::string("startDate")}, 
                    object::pair{std::string("endDate"), std::string("endDate")}, 
                    object::pair{std::string("sortBy"), std::string("published")}, 
                    object::pair{std::string("sortOrder"), std::string("desc")}, 
                    object::pair{std::string("includeSummary"), true}, 
                    object::pair{std::string("includeBody"), std::string("includeBody")}, 
                    object::pair{std::string("includeThumbnail"), true}
                }); });
                if (AND((coindeskResponse->success), (coindeskResponse->data->articles->length))) {
                    auto articles = coindeskResponse->data->articles;
                    auto responseText = std::string("**CoinDesk News Results** (") + articles->length + std::string(" articles)\
\
");
                    responseText += articles->map([=](auto article, auto index) mutable
                    {
                        auto parts = array<string>{ std::string("**") + (index + 1) + std::string(". ") + article["title"] + std::string("**") };
                        if (article["summary"]) parts->push(string_empty + article["summary"] + string_empty);
                        if (article["categories"]["length"]) parts->push(std::string("*Categories: ") + article["categories"]["join"](std::string(", ")) + std::string("*"));
                        if (article["authors"]["length"]) parts->push(std::string("*By: ") + article["authors"]["join"](std::string(", ")) + std::string("*"));
                        if (article["publishedAt"]) {
                            auto pubDate = std::make_shared<Date>(article["publishedAt"]);
                            parts->push(std::string("*Published: ") + pubDate->toLocaleDateString() + std::string(" at ") + pubDate->toLocaleTimeString() + std::string("*"));
                        }
                        parts->push(std::string("[Read full article](") + article["url"] + std::string(")"));
                        if (AND((includeBody), (article["body"]))) {
                            parts->push(std::string("\
") + article["body"]["substring"](0, 500) + string_empty + (article["body"]["length"] > 500) ? std::string("...") : string_empty + string_empty);
                        }
                        return parts->join(std::string("\
"));
                    }
                    )->join(std::string("\
\
---\
\
"));
                    auto result = as<any>(object{
                        object::pair{std::string("text"), MaxTokens(responseText, DEFAULT_MAX_CRYPTO_NEWS_CHARS)}, 
                        object::pair{std::string("success"), true}, 
                        object::pair{std::string("data"), object{
                            object::pair{std::string("articles"), std::string("articles")}, 
                            object::pair{std::string("source"), std::string("coindesk-api")}, 
                            object::pair{std::string("total"), coindeskResponse->data->total}
                        }}, 
                        object::pair{std::string("input"), inputParams}
                    });
                    if (callback) {
                        callback(object{
                            object::pair{std::string("text"), result->text}, 
                            object::pair{std::string("actions"), array<string>{ std::string("CRYPTO_NEWS") }}, 
                            object::pair{std::string("data"), result->data}
                        });
                    }
                    return result;
                }
                logger->warn(std::string("[CRYPTO_NEWS] CoinDesk API returned no results, falling back to Tavily"));
            }
            auto tavilyService = runtime->getService<std::shared_ptr<TavilyService>>(std::string("TAVILY"));
            if (!tavilyService) {
                throw any(std::make_shared<Error>(std::string("No news service available (CoinDesk or Tavily)")));
            }
            auto sourceDomain = (AND((sourceKey), (in(sourceKey, CRYPTO_NEWS_SOURCES)))) ? any(const_(CRYPTO_NEWS_SOURCES)[as<any>(sourceKey)]) : any(nullptr);
            auto enhancedQuery = query;
            if (sourceDomain) {
                enhancedQuery = string_empty + query + std::string(" site:") + sourceDomain + string_empty;
                logger->info(std::string("[CRYPTO_NEWS] Using Tavily with source filter: ") + sourceKey + string_empty);
            } else {
                logger->info(std::string("[CRYPTO_NEWS] Using Tavily for all crypto sources"));
            }
            auto searchResponse = std::async([=]() { tavilyService->search(enhancedQuery, object{
                object::pair{std::string("topic"), std::string("finance")}, 
                object::pair{std::string("max_results"), maxResults}, 
                object::pair{std::string("search_depth"), searchDepth}, 
                object::pair{std::string("time_range"), timeRange}, 
                object::pair{std::string("include_answer"), true}, 
                object::pair{std::string("include_images"), false}
            }); });
            if (AND((searchResponse), (searchResponse->results->length))) {
                auto responseList = (searchResponse->answer) ? any(string_empty + searchResponse->answer + string_empty + (AND((Array->isArray(searchResponse->results)), (searchResponse->results->length > 0))) ? any(std::string("\
\
Sources:\
") + searchResponse->results->map([=](auto result, auto index) mutable
                {
                    return string_empty + (index + 1) + std::string(". [") + result["title"] + std::string("](") + result["url"] + std::string(")");
                }
                )->join(std::string("\
")) + string_empty) : any(string_empty) + string_empty) : any(string_empty);
                auto result = as<any>(object{
                    object::pair{std::string("text"), MaxTokens(responseList, DEFAULT_MAX_CRYPTO_NEWS_CHARS)}, 
                    object::pair{std::string("success"), true}, 
                    object::pair{std::string("data"), searchResponse}, 
                    object::pair{std::string("input"), inputParams}
                });
                if (callback) {
                    callback(object{
                        object::pair{std::string("text"), result->text}, 
                        object::pair{std::string("actions"), array<string>{ std::string("CRYPTO_NEWS") }}, 
                        object::pair{std::string("data"), result->data}
                    });
                }
                return result;
            }
            auto noResult = as<any>(object{
                object::pair{std::string("text"), (sourceDomain) ? std::string("No crypto news found from ") + sourceKey + std::string(" for "") + query + std::string("". Try removing source filter or adjusting time range.") : std::string("No crypto news found for "") + query + std::string("". Try different keywords or broader time range.")}, 
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
            logger->error(std::string("[CRYPTO_NEWS] Action failed: ") + errMsg + string_empty);
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std::string("ACTION_STATE") }, true); });
            auto params = OR((OR((composedState->data->actionParams), (composedState->data->cryptoNews))), (object{}));
            auto failureInputParams = object{
                object::pair{std::string("query"), params["query"]}, 
                object::pair{std::string("source"), params["source"]}, 
                object::pair{std::string("categories"), params["categories"]}, 
                object::pair{std::string("time_range"), params["time_range"]}, 
                object::pair{std::string("max_results"), params["max_results"]}, 
                object::pair{std::string("search_depth"), params["search_depth"]}, 
                object::pair{std::string("include_body"), params["include_body"]}
            };
            auto errorResult = as<any>(object{
                object::pair{std::string("text"), std::string("Crypto news search failed: ") + errMsg + string_empty}, 
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), errMsg}, 
                object::pair{std::string("input"), failureInputParams}
            });
            if (callback) {
                callback(object{
                    object::pair{std::string("text"), errorResult->text}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("error"), std::string("crypto_news_failed")}, 
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
            object::pair{std::string("text"), std::string("Searching crypto sources for Aave news:")}, 
            object::pair{std::string("action"), std::string("CRYPTO_NEWS")}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("What's happening with Ethereum today?")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Checking latest Ethereum news:")}, 
            object::pair{std::string("action"), std::string("CRYPTO_NEWS")}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Any DeFi news from The Block?")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Searching The Block for DeFi news:")}, 
            object::pair{std::string("action"), std::string("CRYPTO_NEWS")}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("CoinDesk news on Bitcoin")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Fetching Bitcoin news from CoinDesk:")}, 
            object::pair{std::string("action"), std::string("CRYPTO_NEWS")}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Show me DeFi policy news from this month")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Searching DeFi policy news:")}, 
            object::pair{std::string("action"), std::string("CRYPTO_NEWS")}, 
            object::pair{std::string("actionParams"), object{
                object::pair{std::string("query"), std::string("DeFi")}, 
                object::pair{std::string("categories"), array<string>{ std::string("policy"), std::string("defi"), std::string("regulation") }}, 
                object::pair{std::string("time_range"), std::string("month")}, 
                object::pair{std::string("max_results"), 15}
            }}
        }}
    } } }}
});

void Main(void)
{
}

MAIN
