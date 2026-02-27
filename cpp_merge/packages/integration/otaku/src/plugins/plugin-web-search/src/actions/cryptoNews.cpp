#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-web-search/src/actions/cryptoNews.h"

string MaxTokens(string data, double maxTokens)
{
    return (data->get_length() > maxTokens) ? data->slice(0, maxTokens) : data;
};


double DEFAULT_MAX_CRYPTO_NEWS_CHARS = 20000;
object CRYPTO_NEWS_SOURCES = as<std::shared_ptr<const>>(object{
    object::pair{std:("theblock"), std:("theblock.co")}, 
    object::pair{std:("coindesk"), std:("coindesk.com")}, 
    object::pair{std:("decrypt"), std:("decrypt.co")}, 
    object::pair{std:("dlnews"), std:("dlnews.com")}, 
    object::pair{std:("coinbureau"), std:("coinbureau.com")}, 
    object::pair{std:("cointelegraph"), std:("cointelegraph.com")}, 
    object::pair{std:("blockworks"), std:("blockworks.co")}
});
std::shared_ptr<Action> cryptoNews = as<std::shared_ptr<Action>>(object{
    object::pair{std:("name"), std:("CRYPTO_NEWS")}, 
    object::pair{std:("similes"), array<string>{ std:("BLOCKCHAIN_NEWS"), std:("DEFI_NEWS"), std:("CRYPTO_UPDATES"), std:("WEB3_NEWS"), std:("CRYPTOCURRENCY_NEWS"), std:("GET_CRYPTO_NEWS"), std:("LATEST_CRYPTO"), std:("CRYPTO_HEADLINES") }}, 
    object::pair{std:("suppressInitialMessage"), true}, 
    object::pair{std:("description"), std:("Search for cryptocurrency, blockchain, DeFi, and Web3 news from reputable crypto-focused sources.\
\
") + std:("**CoinDesk API** (when configured with COINDESK_API_KEY):\
") + std:("- Direct access to CoinDesk's news database (100+ articles available per query)\
") + std:("- Rich filtering: categories (markets/tech/policy/defi/nft/layer-2/regulation), keywords, authors, tags\
") + std:("- Full metadata: title, summary, optional body, publish dates, authors, thumbnails\
") + std:("- Date range filtering with automatic time_range conversion\
") + std:("- Sorted by relevance or publish date\
\
") + std:("**Tavily Fallback** (for other sources or when CoinDesk unavailable):\
") + std:("- Uses finance topic for crypto-focused results\
") + std:("- Site filtering for TheBlock, Decrypt, DL News, Coinbureau, Cointelegraph, Blockworks\
") + std:("- Up to 20 results with answer synthesis")}, 
    object::pair{std:("parameters"), object{
        object::pair{std:("query"), object{
            object::pair{std:("type"), std:("string")}, 
            object::pair{std:("description"), std:("The crypto/blockchain/DeFi news query (e.g., 'Aave', 'Ethereum merge', 'DeFi hacks')")}, 
            object::pair{std:("required"), true}
        }}, 
        object::pair{std:("source"), object{
            object::pair{std:("type"), std:("string")}, 
            object::pair{std:("description"), std:("Specific crypto news source: 'theblock', 'coindesk', 'decrypt', 'dlnews', 'coinbureau', 'cointelegraph', 'blockworks'. Leave empty to search all sources.")}, 
            object::pair{std:("required"), false}
        }}, 
        object::pair{std:("categories"), object{
            object::pair{std:("type"), std:("array")}, 
            object::pair{std:("description"), std:("CoinDesk categories to filter by: 'markets', 'tech', 'policy', 'defi', 'nft', 'layer-2', 'regulation'. Only applies when using CoinDesk API.")}, 
            object::pair{std:("required"), false}
        }}, 
        object::pair{std:("time_range"), object{
            object::pair{std:("type"), std:("string")}, 
            object::pair{std:("description"), std:("Time range filter: 'day', 'week', 'month', 'year' (or 'd', 'w', 'm', 'y'). Defaults to 'week' for recent news.")}, 
            object::pair{std:("required"), false}
        }}, 
        object::pair{std:("max_results"), object{
            object::pair{std:("type"), std:("number")}, 
            object::pair{std:("description"), std:("Maximum number of results (1-100 for CoinDesk API, 1-20 for Tavily). Defaults to 10.")}, 
            object::pair{std:("required"), false}
        }}, 
        object::pair{std:("search_depth"), object{
            object::pair{std:("type"), std:("string")}, 
            object::pair{std:("description"), std:("Search depth: 'basic' or 'advanced'. Only applies to Tavily fallback. Defaults to 'basic'.")}, 
            object::pair{std:("required"), false}
        }}, 
        object::pair{std:("include_body"), object{
            object::pair{std:("type"), std:("boolean")}, 
            object::pair{std:("description"), std:("Include full article body in response (CoinDesk API only). Defaults to false for performance.")}, 
            object::pair{std:("required"), false}
        }}
    }}, 
    object::pair{std:("validate"), [=](auto runtime, auto _message, auto _state = undefined) mutable
    {
        try
        {
            auto coindeskService = runtime->getService<std::shared_ptr<CoinDeskService>>(std:("COINDESK_NEWS"));
            auto tavilyService = runtime->getService<std::shared_ptr<TavilyService>>(std:("TAVILY"));
            return !!(OR((coindeskService), (tavilyService)));
        }
        catch (const any& err)
        {
            logger->warn(std:("No news service available:"), (as<std::shared_ptr<Error>>(err))->message);
            return false;
        }
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto _state, auto _options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std:("ACTION_STATE") }, true); });
            auto params = OR((OR((composedState->data->actionParams), (composedState->data->cryptoNews))), (object{}));
            auto query = params["query"]["trim"]();
            if (!query) {
                auto errorMsg = std:("Missing required parameter 'query'. Please specify what crypto news to search for.");
                logger->error(std:("[CRYPTO_NEWS] ") + errorMsg + string_empty);
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
            auto sourceKey = params["source"]["toLowerCase"]()["trim"]();
            auto timeRange = OR((params["time_range"]), (std:("week")));
            auto maxResults = (params["max_results"]) ? any(Math->min(Math->max(1, params["max_results"]), 100)) (10);
            auto searchDepth = (params["search_depth"] == std:("advanced")) ? std:("advanced") : std:("basic");
            shared categories = params["categories"];
            shared includeBody = params["include_body"] == true;
            auto inputParams = object{
                object::pair{std:("query"), std:("query")}, 
                object::pair{std:("source"), sourceKey}, 
                object::pair{std:("categories"), std:("categories")}, 
                object::pair{std:("time_range"), timeRange}, 
                object::pair{std:("max_results"), maxResults}, 
                object::pair{std:("search_depth"), searchDepth}, 
                object::pair{std:("include_body"), includeBody}
            };
            any startDate;
            any endDate;
            if (timeRange) {
                auto now = std::make_shared<Date>();
                endDate = const_(now->toISOString()->split(std:("T")))[0];
                static switch_type __switch6521_7428 = {
                    { any(std:("day")), 1 },
                    { any(std:("d")), 2 },
                    { any(std:("week")), 3 },
                    { any(std:("w")), 4 },
                    { any(std:("month")), 5 },
                    { any(std:("m")), 6 },
                    { any(std:("year")), 7 },
                    { any(std:("y")), 8 }
                };
                switch (__switch6521_7428[timeRange])
                {
                case 1:
                case 2:
                    startDate = const_(((std::make_shared<Date>(now->getTime() - 24 * 60 * 60 * 1000)))->toISOString()->split(std:("T")))[0];
                    break;
                case 3:
                case 4:
                    startDate = const_(((std::make_shared<Date>(now->getTime() - 7 * 24 * 60 * 60 * 1000)))->toISOString()->split(std:("T")))[0];
                    break;
                case 5:
                case 6:
                    startDate = const_(((std::make_shared<Date>(now->getTime() - 30 * 24 * 60 * 60 * 1000)))->toISOString()->split(std:("T")))[0];
                    break;
                case 7:
                case 8:
                    startDate = const_(((std::make_shared<Date>(now->getTime() - 365 * 24 * 60 * 60 * 1000)))->toISOString()->split(std:("T")))[0];
                    break;
                }
            }
            auto coindeskService = runtime->getService<std::shared_ptr<CoinDeskService>>(std:("COINDESK_NEWS"));
            if (AND((AND((coindeskService), (coindeskService->isConfigured()))), ((OR((!sourceKey), (sourceKey == std:("coindesk"))))))) {
                logger->info(std:("[CRYPTO_NEWS] Using CoinDesk API for: ") + query + string_empty);
                auto coindeskResponse = std::async([=]() { coindeskService->searchNews(object{
                    object::pair{std:("query"), std:("query")}, 
                    object::pair{std:("limit"), maxResults}, 
                    object::pair{std:("categories"), std:("categories")}, 
                    object::pair{std:("startDate"), std:("startDate")}, 
                    object::pair{std:("endDate"), std:("endDate")}, 
                    object::pair{std:("sortBy"), std:("published")}, 
                    object::pair{std:("sortOrder"), std:("desc")}, 
                    object::pair{std:("includeSummary"), true}, 
                    object::pair{std:("includeBody"), std:("includeBody")}, 
                    object::pair{std:("includeThumbnail"), true}
                }); });
                if (AND((coindeskResponse->success), (coindeskResponse->data->articles->length))) {
                    auto articles = coindeskResponse->data->articles;
                    auto responseText = std:("**CoinDesk News Results** (") + articles->length + std:(" articles)\
\
");
                    responseText += articles->map([=](auto article, auto index) mutable
                    {
                        auto parts = array<string>{ std:("**") + (index + 1) + std:(". ") + article["title"] + std:("**") };
                        if (article["summary"]) parts->push(string_empty + article["summary"] + string_empty);
                        if (article["categories"]["length"]) parts->push(std:("*Categories: ") + article["categories"]["join"](std:(", ")) + std:("*"));
                        if (article["authors"]["length"]) parts->push(std:("*By: ") + article["authors"]["join"](std:(", ")) + std:("*"));
                        if (article["publishedAt"]) {
                            auto pubDate = std::make_shared<Date>(article["publishedAt"]);
                            parts->push(std:("*Published: ") + pubDate->toLocaleDateString() + std:(" at ") + pubDate->toLocaleTimeString() + std:("*"));
                        }
                        parts->push(std:("[Read full article](") + article["url"] + std:(")"));
                        if (AND((includeBody), (article["body"]))) {
                            parts->push(std:("\
") + article["body"]["substring"](0, 500) + string_empty + (article["body"]["length"] > 500) ? std:("...") : string_empty + string_empty);
                        }
                        return parts->join(std:("\
"));
                    }
                    )->join(std:("\
\
---\
\
"));
                    auto result = as<any>(object{
                        object::pair{std:("text"), MaxTokens(responseText, DEFAULT_MAX_CRYPTO_NEWS_CHARS)}, 
                        object::pair{std:("success"), true}, 
                        object::pair{std:("data"), object{
                            object::pair{std:("articles"), std:("articles")}, 
                            object::pair{std:("source"), std:("coindesk-api")}, 
                            object::pair{std:("total"), coindeskResponse->data->total}
                        }}, 
                        object::pair{std:("input"), inputParams}
                    });
                    if (callback) {
                        callback(object{
                            object::pair{std:("text"), result->text}, 
                            object::pair{std:("actions"), array<string>{ std:("CRYPTO_NEWS") }}, 
                            object::pair{std:("data"), result->data}
                        });
                    }
                    return result;
                }
                logger->warn(std:("[CRYPTO_NEWS] CoinDesk API returned no results, falling back to Tavily"));
            }
            auto tavilyService = runtime->getService<std::shared_ptr<TavilyService>>(std:("TAVILY"));
            if (!tavilyService) {
                throw any(std::make_shared<Error>(std:("No news service available (CoinDesk or Tavily)")));
            }
            auto sourceDomain = (AND((sourceKey), (in(sourceKey, CRYPTO_NEWS_SOURCES)))) ? any(const_(CRYPTO_NEWS_SOURCES)[as<any>(sourceKey)]) (nullptr);
            auto enhancedQuery = query;
            if (sourceDomain) {
                enhancedQuery = string_empty + query + std:(" site:") + sourceDomain + string_empty;
                logger->info(std:("[CRYPTO_NEWS] Using Tavily with source filter: ") + sourceKey + string_empty);
            } else {
                logger->info(std:("[CRYPTO_NEWS] Using Tavily for all crypto sources"));
            }
            auto searchResponse = std::async([=]() { tavilyService->search(enhancedQuery, object{
                object::pair{std:("topic"), std:("finance")}, 
                object::pair{std:("max_results"), maxResults}, 
                object::pair{std:("search_depth"), searchDepth}, 
                object::pair{std:("time_range"), timeRange}, 
                object::pair{std:("include_answer"), true}, 
                object::pair{std:("include_images"), false}
            }); });
            if (AND((searchResponse), (searchResponse->results->length))) {
                auto responseList = (searchResponse->answer) ? any(string_empty + searchResponse->answer + string_empty + (AND((Array->isArray(searchResponse->results)), (searchResponse->results->length > 0))) ? any(std:("\
\
Sources:\
") + searchResponse->results->map([=](auto result, auto index) mutable
                {
                    return string_empty + (index + 1) + std:(". [") + result["title"] + std:("](") + result["url"] + std:(")");
                }
                )->join(std:("\
")) + string_empty) (string_empty) + string_empty) (string_empty);
                auto result = as<any>(object{
                    object::pair{std:("text"), MaxTokens(responseList, DEFAULT_MAX_CRYPTO_NEWS_CHARS)}, 
                    object::pair{std:("success"), true}, 
                    object::pair{std:("data"), searchResponse}, 
                    object::pair{std:("input"), inputParams}
                });
                if (callback) {
                    callback(object{
                        object::pair{std:("text"), result->text}, 
                        object::pair{std:("actions"), array<string>{ std:("CRYPTO_NEWS") }}, 
                        object::pair{std:("data"), result->data}
                    });
                }
                return result;
            }
            auto noResult = as<any>(object{
                object::pair{std:("text"), (sourceDomain) ? std:("No crypto news found from ") + sourceKey + std:(" for "") + query + std:("". Try removing source filter or adjusting time range.") : std:("No crypto news found for "") + query + std:("". Try different keywords or broader time range.")}, 
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
            logger->error(std:("[CRYPTO_NEWS] Action failed: ") + errMsg + string_empty);
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std:("ACTION_STATE") }, true); });
            auto params = OR((OR((composedState->data->actionParams), (composedState->data->cryptoNews))), (object{}));
            auto failureInputParams = object{
                object::pair{std:("query"), params["query"]}, 
                object::pair{std:("source"), params["source"]}, 
                object::pair{std:("categories"), params["categories"]}, 
                object::pair{std:("time_range"), params["time_range"]}, 
                object::pair{std:("max_results"), params["max_results"]}, 
                object::pair{std:("search_depth"), params["search_depth"]}, 
                object::pair{std:("include_body"), params["include_body"]}
            };
            auto errorResult = as<any>(object{
                object::pair{std:("text"), std:("Crypto news search failed: ") + errMsg + string_empty}, 
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), errMsg}, 
                object::pair{std:("input"), failureInputParams}
            });
            if (callback) {
                callback(object{
                    object::pair{std:("text"), errorResult->text}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("error"), std:("crypto_news_failed")}, 
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
            object::pair{std:("text"), std:("Searching crypto sources for Aave news:")}, 
            object::pair{std:("action"), std:("CRYPTO_NEWS")}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("What's happening with Ethereum today?")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Checking latest Ethereum news:")}, 
            object::pair{std:("action"), std:("CRYPTO_NEWS")}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Any DeFi news from The Block?")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Searching The Block for DeFi news:")}, 
            object::pair{std:("action"), std:("CRYPTO_NEWS")}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("CoinDesk news on Bitcoin")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Fetching Bitcoin news from CoinDesk:")}, 
            object::pair{std:("action"), std:("CRYPTO_NEWS")}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Show me DeFi policy news from this month")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Searching DeFi policy news:")}, 
            object::pair{std:("action"), std:("CRYPTO_NEWS")}, 
            object::pair{std:("actionParams"), object{
                object::pair{std:("query"), std:("DeFi")}, 
                object::pair{std:("categories"), array<string>{ std:("policy"), std:("defi"), std:("regulation") }}, 
                object::pair{std:("time_range"), std:("month")}, 
                object::pair{std:("max_results"), 15}
            }}
        }}
    } } }}
});

void Main(void)
{
}

MAIN
