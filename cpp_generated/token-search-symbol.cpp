#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/plugin-birdeye/src/actions/token-search-symbol.h"

string SYMBOL_SEARCH_MODE = std::string("strict");
any tokenSearchSymbolAction = as<std::shared_ptr<Action>>(object{
    object::pair{std::string("name"), std::string("TOKEN_SEARCH_SYMBOL")}, 
    object::pair{std::string("similes"), array<string>{ std::string("SEARCH_TOKEN_SYMBOL"), std::string("FIND_TOKEN_SYMBOL"), std::string("LOOKUP_TOKEN_SYMBOL"), std::string("CHECK_TOKEN_SYMBOL"), std::string("GET_TOKEN_BY_SYMBOL"), std::string("SYMBOL_SEARCH"), std::string("SYMBOL_LOOKUP"), std::string("SYMBOL_CHECK"), std::string("TOKEN_SYMBOL_INFO"), std::string("TOKEN_SYMBOL_DETAILS"), std::string("TOKEN_SYMBOL_LOOKUP"), std::string("TOKEN_SYMBOL_SEARCH"), std::string("TOKEN_SYMBOL_CHECK"), std::string("TOKEN_SYMBOL_QUERY"), std::string("TOKEN_SYMBOL_FIND"), std::string("GET_TOKEN_INFO"), std::string("TOKEN_INFO"), std::string("TOKEN_REPORT"), std::string("TOKEN_ANALYSIS"), std::string("TOKEN_OVERVIEW"), std::string("TOKEN_SUMMARY"), std::string("TOKEN_INSIGHT"), std::string("TOKEN_DATA"), std::string("TOKEN_STATS"), std::string("TOKEN_METRICS"), std::string("TOKEN_PROFILE"), std::string("TOKEN_REVIEW"), std::string("TOKEN_CHECK"), std::string("TOKEN_LOOKUP"), std::string("TOKEN_FIND"), std::string("TOKEN_DISCOVER"), std::string("TOKEN_EXPLORE") }}, 
    object::pair{std::string("description"), std::string("Search for detailed token information including security and trade data by symbol")}, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto _state, auto _options, auto callback = undefined) mutable
    {
        try
        {
            shared provider = std::make_shared<BirdeyeProvider>(runtime);
            shared symbols = extractSymbols(message->content->text, SYMBOL_SEARCH_MODE);
            elizaLogger->info(std::string("Searching Birdeye provider for ") + symbols->get_length() + std::string(" symbols"));
            auto results = std::async([=]() { Promise->all(symbols->map([=](auto symbol) mutable
            {
                return provider->fetchSearchTokenMarketData(object{
                    object::pair{std::string("keyword"), symbol}, 
                    object::pair{std::string("sort_by"), std::string("volume_24h_usd")}, 
                    object::pair{std::string("sort_type"), std::string("desc")}, 
                    object::pair{std::string("chain"), std::string("all")}, 
                    object::pair{std::string("limit"), 5}
                });
            }
            )); });
            auto validResults = as<array<array<std::shared_ptr<TokenResult>>>>(results->map([=](auto r, auto i) mutable
            {
                return r->data->items->filter([=](auto item) mutable
                {
                    return AND((item["type"] == std::string("token")), (item["result"]));
                }
                )->flatMap([=](auto item) mutable
                {
                    return (as<array<std::shared_ptr<TokenResult>>>(item["result"]))->filter([=](auto r) mutable
                    {
                        return r->symbol->toLowerCase() == const_(symbols)[i]->toLowerCase();
                    }
                    );
                }
                );
            }
            ));
            if (validResults->get_length() == 0) {
                return true;
            }
            auto completeResults = std::string("I performed a search for the token symbols you requested and found the following results (for more details search by contract address):\
\
") + validResults->map([=](auto result, auto i) mutable
            {
                return string_empty + formatTokenSummary(const_(symbols)[i], i, result) + string_empty;
            }
            )->join(std::string("\
")) + string_empty;
            callback(object{
                object::pair{std::string("text"), completeResults}
            });
            return true;
        }
        catch (const any& error)
        {
            console->error(std::string("Error in searchTokens handler:"), error["message"]);
            callback(object{
                object::pair{std::string("text"), std::string("Error: ") + error["message"] + string_empty}
            });
            return false;
        }
    }
    }, 
    object::pair{std::string("validate"), [=](auto _runtime, auto message) mutable
    {
        auto symbols = extractSymbols(message->content->text, SYMBOL_SEARCH_MODE);
        return symbols->get_length() > 0;
    }
    }, 
    object::pair{std::string("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std::string("user"), std::string("user")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Search for $SOL and $ETH")}, 
            object::pair{std::string("action"), std::string("SEARCH_TOKENS")}
        }}
    }, object{
        object::pair{std::string("user"), std::string("user")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Find information about $BTC")}, 
            object::pair{std::string("action"), std::string("TOKEN_SEARCH")}
        }}
    }, object{
        object::pair{std::string("user"), std::string("user")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Look up $WETH token")}, 
            object::pair{std::string("action"), std::string("LOOKUP_TOKENS")}
        }}
    }, object{
        object::pair{std::string("user"), std::string("user")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Tell me about SOL")}, 
            object::pair{std::string("action"), std::string("CHECK_TOKEN")}
        }}
    }, object{
        object::pair{std::string("user"), std::string("user")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Give me details on $ADA")}, 
            object::pair{std::string("action"), std::string("TOKEN_DETAILS")}
        }}
    }, object{
        object::pair{std::string("user"), std::string("user")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("What can you tell me about $DOGE?")}, 
            object::pair{std::string("action"), std::string("TOKEN_INFO")}
        }}
    }, object{
        object::pair{std::string("user"), std::string("user")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I need a report on $XRP")}, 
            object::pair{std::string("action"), std::string("TOKEN_REPORT")}
        }}
    }, object{
        object::pair{std::string("user"), std::string("user")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Analyze $BNB for me")}, 
            object::pair{std::string("action"), std::string("TOKEN_ANALYSIS")}
        }}
    }, object{
        object::pair{std::string("user"), std::string("user")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Overview of $LTC")}, 
            object::pair{std::string("action"), std::string("TOKEN_OVERVIEW")}
        }}
    } } })}
});
std::function<string(string, double, array<std::shared_ptr<TokenResult>>)> formatTokenSummary = [=](auto symbol, auto _index, auto tokens) mutable
{
    return tokens->map([=](auto token, auto i) mutable
    {
        auto output = string_empty;
        if (i == 0) {
            output += std::string("Search Results for ") + symbol + std::string(":\
\
");
        }
        output += std::string("Search Result #") + (tokens->get_length() > 0) ? any(i + 1) : any(string_empty) + std::string(":\
");
        output += std::string("🔖 Symbol: $") + token->symbol->toUpperCase() + std::string("\
");
        output += std::string("🔗 Address: ") + token->address + std::string("\
");
        output += std::string("🌐 Network: ") + token->network->toUpperCase() + std::string("\
");
        output += std::string("💵 Price: ") + formatPrice(token->price) + std::string(" (") + formatPercentChange(token->price_change_24h_percent) + std::string(")\
");
        output += std::string("💸 Volume (24h USD): ") + formatValue(token->volume_24h_usd) + std::string("\
");
        output += (token->market_cap) ? any(std::string("💰 Market Cap: ") + formatValue(token->market_cap) + std::string("\
")) : any(string_empty);
        output += (token->fdv) ? any(std::string("🌊 FDV: ") + formatValue(token->fdv) + std::string("\
")) : any(string_empty);
        return output;
    }
    )->join(std::string("\
"));
};

void Main(void)
{
}

MAIN
