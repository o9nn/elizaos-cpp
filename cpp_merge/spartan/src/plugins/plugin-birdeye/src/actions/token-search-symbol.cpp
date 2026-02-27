#include "token-search-symbol.h"

string SYMBOL_SEARCH_MODE = std:("strict");
any tokenSearchSymbolAction = as<std::shared_ptr<Action>>(object{
    object::pair{std:("name"), std:("TOKEN_SEARCH_SYMBOL")}, 
    object::pair{std:("similes"), array<string>{ std:("SEARCH_TOKEN_SYMBOL"), std:("FIND_TOKEN_SYMBOL"), std:("LOOKUP_TOKEN_SYMBOL"), std:("CHECK_TOKEN_SYMBOL"), std:("GET_TOKEN_BY_SYMBOL"), std:("SYMBOL_SEARCH"), std:("SYMBOL_LOOKUP"), std:("SYMBOL_CHECK"), std:("TOKEN_SYMBOL_INFO"), std:("TOKEN_SYMBOL_DETAILS"), std:("TOKEN_SYMBOL_LOOKUP"), std:("TOKEN_SYMBOL_SEARCH"), std:("TOKEN_SYMBOL_CHECK"), std:("TOKEN_SYMBOL_QUERY"), std:("TOKEN_SYMBOL_FIND"), std:("GET_TOKEN_INFO"), std:("TOKEN_INFO"), std:("TOKEN_REPORT"), std:("TOKEN_ANALYSIS"), std:("TOKEN_OVERVIEW"), std:("TOKEN_SUMMARY"), std:("TOKEN_INSIGHT"), std:("TOKEN_DATA"), std:("TOKEN_STATS"), std:("TOKEN_METRICS"), std:("TOKEN_PROFILE"), std:("TOKEN_REVIEW"), std:("TOKEN_CHECK"), std:("TOKEN_LOOKUP"), std:("TOKEN_FIND"), std:("TOKEN_DISCOVER"), std:("TOKEN_EXPLORE") }}, 
    object::pair{std:("description"), std:("Search for detailed token information including security and trade data by symbol")}, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto _state, auto _options, auto callback = undefined) mutable
    {
        try
        {
            shared provider = std::make_shared<BirdeyeProvider>(runtime);
            shared symbols = extractSymbols(message->content->text, SYMBOL_SEARCH_MODE);
            elizaLogger->info(std:("Searching Birdeye provider for ") + symbols->get_length() + std:(" symbols"));
            auto results = std::async([=]() { Promise->all(symbols->map([=](auto symbol) mutable
            {
                return provider->fetchSearchTokenMarketData(object{
                    object::pair{std:("keyword"), symbol}, 
                    object::pair{std:("sort_by"), std:("volume_24h_usd")}, 
                    object::pair{std:("sort_type"), std:("desc")}, 
                    object::pair{std:("chain"), std:("all")}, 
                    object::pair{std:("limit"), 5}
                });
            }
            )); });
            auto validResults = as<array<array<std::shared_ptr<TokenResult>>>>(results->map([=](auto r, auto i) mutable
            {
                return r->data->items->filter([=](auto item) mutable
                {
                    return AND((item["type"] == std:("token")), (item["result"]));
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
            auto completeResults = std:("I performed a search for the token symbols you requested and found the following results (for more details search by contract address):\
\
") + validResults->map([=](auto result, auto i) mutable
            {
                return string_empty + formatTokenSummary(const_(symbols)[i], i, result) + string_empty;
            }
            )->join(std:("\
")) + string_empty;
            callback(object{
                object::pair{std:("text"), completeResults}
            });
            return true;
        }
        catch (const any& error)
        {
            console->error(std:("Error in searchTokens handler:"), error["message"]);
            callback(object{
                object::pair{std:("text"), std:("Error: ") + error["message"] + string_empty}
            });
            return false;
        }
    }
    }, 
    object::pair{std:("validate"), [=](auto _runtime, auto message) mutable
    {
        auto symbols = extractSymbols(message->content->text, SYMBOL_SEARCH_MODE);
        return symbols->get_length() > 0;
    }
    }, 
    object::pair{std:("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std:("user"), std:("user")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Search for $SOL and $ETH")}, 
            object::pair{std:("action"), std:("SEARCH_TOKENS")}
        }}
    }, object{
        object::pair{std:("user"), std:("user")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Find information about $BTC")}, 
            object::pair{std:("action"), std:("TOKEN_SEARCH")}
        }}
    }, object{
        object::pair{std:("user"), std:("user")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Look up $WETH token")}, 
            object::pair{std:("action"), std:("LOOKUP_TOKENS")}
        }}
    }, object{
        object::pair{std:("user"), std:("user")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Tell me about SOL")}, 
            object::pair{std:("action"), std:("CHECK_TOKEN")}
        }}
    }, object{
        object::pair{std:("user"), std:("user")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Give me details on $ADA")}, 
            object::pair{std:("action"), std:("TOKEN_DETAILS")}
        }}
    }, object{
        object::pair{std:("user"), std:("user")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("What can you tell me about $DOGE?")}, 
            object::pair{std:("action"), std:("TOKEN_INFO")}
        }}
    }, object{
        object::pair{std:("user"), std:("user")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I need a report on $XRP")}, 
            object::pair{std:("action"), std:("TOKEN_REPORT")}
        }}
    }, object{
        object::pair{std:("user"), std:("user")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Analyze $BNB for me")}, 
            object::pair{std:("action"), std:("TOKEN_ANALYSIS")}
        }}
    }, object{
        object::pair{std:("user"), std:("user")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Overview of $LTC")}, 
            object::pair{std:("action"), std:("TOKEN_OVERVIEW")}
        }}
    } } })}
});
std::function<string(string, double, array<std::shared_ptr<TokenResult>>)> formatTokenSummary = [=](auto symbol, auto _index, auto tokens) mutable
{
    return tokens->map([=](auto token, auto i) mutable
    {
        auto output = string_empty;
        if (i == 0) {
            output += std:("Search Results for ") + symbol + std:(":\
\
");
        }
        output += std:("Search Result #") + (tokens->get_length() > 0) ? any(i + 1) (string_empty) + std:(":\
");
        output += std:("🔖 Symbol: $") + token->symbol->toUpperCase() + std:("\
");
        output += std:("🔗 Address: ") + token->address + std:("\
");
        output += std:("🌐 Network: ") + token->network->toUpperCase() + std:("\
");
        output += std:("💵 Price: ") + formatPrice(token->price) + std:(" (") + formatPercentChange(token->price_change_24h_percent) + std:(")\
");
        output += std:("💸 Volume (24h USD): ") + formatValue(token->volume_24h_usd) + std:("\
");
        output += (token->market_cap) ? any(std:("💰 Market Cap: ") + formatValue(token->market_cap) + std:("\
")) (string_empty);
        output += (token->fdv) ? any(std:("🌊 FDV: ") + formatValue(token->fdv) + std:("\
")) (string_empty);
        return output;
    }
    )->join(std:("\
"));
};

void Main(void)
{
}

MAIN
