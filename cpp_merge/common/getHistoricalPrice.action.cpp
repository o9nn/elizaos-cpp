#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-coingecko/src/actions/getHistoricalPrice.action.h"

string formatMarketCap(double value)
{
    if (value >= 1000000000) return string_empty + (value / 1000000000)->toFixed(2) + std::string("B");
    if (value >= 1000000) return string_empty + (value / 1000000)->toFixed(2) + std::string("M");
    if (value >= 1000) return string_empty + (value / 1000)->toFixed(2) + std::string("K");
    return value->toFixed(2);
};


string parseDateToApiFormat(string dateStr)
{
    auto normalized = dateStr->trim()->toLowerCase();
    if ((new RegExp(std::string("^\d{2}-\d{2}-\d{4}")))->test(dateStr)) {
        return dateStr;
    }
    std::shared_ptr<Date> date;
    if (normalized == std::string("today")) {
        date = std::make_shared<Date>();
    } else if (normalized == std::string("yesterday")) {
        date = std::make_shared<Date>();
        date->setDate(date->getDate() - 1);
    } else if ((new RegExp(std::string("^(\d+)\s*days?\s*ago")))->test(normalized)) {
        auto daysMatch = normalized->match((new RegExp(std::string("^(\d+)\s*days?\s*ago"))));
        auto days = (daysMatch) ? any(parseInt((*const_(daysMatch))[1])) : any(0);
        date = std::make_shared<Date>();
        date->setDate(date->getDate() - days);
    } else if ((new RegExp(std::string("^(\d+)\s*weeks?\s*ago")))->test(normalized)) {
        auto weeksMatch = normalized->match((new RegExp(std::string("^(\d+)\s*weeks?\s*ago"))));
        auto weeks = (weeksMatch) ? any(parseInt((*const_(weeksMatch))[1])) : any(0);
        date = std::make_shared<Date>();
        date->setDate(date->getDate() - (weeks * 7));
    } else if ((new RegExp(std::string("^(\d+)\s*months?\s*ago")))->test(normalized)) {
        auto monthsMatch = normalized->match((new RegExp(std::string("^(\d+)\s*months?\s*ago"))));
        auto months = (monthsMatch) ? any(parseInt((*const_(monthsMatch))[1])) : any(0);
        date = std::make_shared<Date>();
        date->setMonth(date->getMonth() - months);
    } else if ((new RegExp(std::string("^(\d+)\s*years?\s*ago")))->test(normalized)) {
        auto yearsMatch = normalized->match((new RegExp(std::string("^(\d+)\s*years?\s*ago"))));
        auto years = (yearsMatch) ? any(parseInt((*const_(yearsMatch))[1])) : any(0);
        date = std::make_shared<Date>();
        date->setFullYear(date->getFullYear() - years);
    } else {
        date = std::make_shared<Date>(dateStr);
        if (isNaN(date->getTime())) {
            throw any(std::make_shared<Error>(std::string("Unable to parse date: ") + dateStr + string_empty));
        }
    }
    auto day = String(date->getDate())->padStart(2, std::string("0"));
    auto month = String(date->getMonth() + 1)->padStart(2, std::string("0"));
    auto year = date->getFullYear();
    return string_empty + day + std::string("-") + month + std::string("-") + year + string_empty;
};


std::shared_ptr<Action> getHistoricalPriceAction = object{
    object::pair{std::string("name"), std::string("GET_HISTORICAL_PRICE")}, 
    object::pair{std::string("similes"), array<string>{ std::string("HISTORICAL_PRICE"), std::string("PRICE_ON_DATE"), std::string("PAST_PRICE"), std::string("TOKEN_PRICE_HISTORY"), std::string("PRICE_AT_DATE") }}, 
    object::pair{std::string("description"), std::string("Use this action when the user asks for a token's price on a specific date in the past. This action retrieves historical price data for any token (native or contract address) at a particular point in time. Returns the price, market cap, and trading volume for that date.")}, 
    object::pair{std::string("parameters"), object{
        object::pair{std::string("token"), object{
            object::pair{std::string("type"), std::string("string")}, 
            object::pair{std::string("description"), std::string("Token symbol or contract address. Native tokens that can be used by symbol: ") + Object->keys(nativeTokenIds)->join(std::string(", "))->toUpperCase() + std::string(". For all other tokens, provide the contract address (e.g., '0x833589fcd6edb6e08f4c7c32d4f71b54bda02913'). Use GET_TOKEN_METADATA first to get the contract address for non-native tokens.")}, 
            object::pair{std::string("required"), true}
        }}, 
        object::pair{std::string("date"), object{
            object::pair{std::string("type"), std::string("string")}, 
            object::pair{std::string("description"), std::string("Date for historical price. Accepts formats: 'dd-mm-yyyy' (e.g., '01-01-2024'), '2024-01-01', 'today', 'yesterday', '7 days ago', '2 weeks ago', '3 months ago', '1 year ago'.")}, 
            object::pair{std::string("required"), true}
        }}, 
        object::pair{std::string("chain"), object{
            object::pair{std::string("type"), std::string("string")}, 
            object::pair{std::string("description"), std::string("Blockchain network for the token (e.g., 'base', 'ethereum', 'polygon', 'arbitrum', 'optimism'). Required for contract addresses, optional for native tokens. Use GET_TOKEN_METADATA first to determine the correct chain.")}, 
            object::pair{std::string("required"), false}
        }}
    }}, 
    object::pair{std::string("validate"), [=](auto runtime) mutable
    {
        auto svc = as<any>(runtime->getService(CoinGeckoService::serviceType));
        if (!svc) {
            logger->error(std::string("CoinGeckoService not available"));
            return false;
        }
        return true;
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto _state = undefined, auto _options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto svc = as<any>(runtime->getService(CoinGeckoService::serviceType));
            if (!svc) {
                throw any(std::make_shared<Error>(std::string("CoinGeckoService not available")));
            }
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std::string("ACTION_STATE") }, true); });
            auto params = OR((composedState->data->actionParams), (object{}));
            auto tokenRaw = params["token"]["trim"]();
            if (!tokenRaw) {
                auto supportedNativeTokens = Object->keys(nativeTokenIds)->join(std::string(", "))->toUpperCase();
                auto errorMsg = std::string("Missing required parameter 'token'. Please specify which token to fetch historical price for. Native tokens (") + supportedNativeTokens + std::string(") can be used by symbol. For all other tokens, provide the contract address.");
                logger->error(std::string("[GET_HISTORICAL_PRICE] ") + errorMsg + string_empty);
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
            auto dateRaw = params["date"]["trim"]();
            if (!dateRaw) {
                auto errorMsg = std::string("Missing required parameter 'date'. Please specify the date for historical price (e.g., '01-01-2024', 'yesterday', '7 days ago').");
                logger->error(std::string("[GET_HISTORICAL_PRICE] ") + errorMsg + string_empty);
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
            string apiDate;
            try
            {
                apiDate = parseDateToApiFormat(dateRaw);
            }
            catch (const any& err)
            {
                auto msg = (is<Error>(err)) ? err->message : String(err);
                auto errorMsg = std::string("Invalid date format: ") + msg + std::string(". Please use formats like 'dd-mm-yyyy', '2024-01-01', 'yesterday', '7 days ago', etc.");
                logger->error(std::string("[GET_HISTORICAL_PRICE] ") + errorMsg + string_empty);
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
            auto chain = OR((params["chain"]["trim"]()["toLowerCase"]()), (std::string("base")));
            logger->info(std::string("[GET_HISTORICAL_PRICE] Fetching historical price for ") + tokenRaw + std::string(" on ") + apiDate + std::string(" (chain: ") + chain + std::string(")"));
            auto inputParams = object{
                object::pair{std::string("token"), tokenRaw}, 
                object::pair{std::string("date"), dateRaw}, 
                object::pair{std::string("chain"), std::string("chain")}
            };
            auto historicalData = std::async([=]() { svc->getHistoricalPrice(tokenRaw, apiDate, chain); });
            auto tokenDisplay = (historicalData["token_name"]) ? string_empty + historicalData["token_name"] + std::string(" (") + (OR((historicalData["token_symbol"]), (tokenRaw))) + std::string(")") : (OR((historicalData["token_symbol"]), (tokenRaw)));
            auto summary = std::string("Historical price data for ") + tokenDisplay + std::string(" on ") + apiDate + std::string(":\
- Token: ") + tokenDisplay + std::string("\
- Date: ") + apiDate + std::string("\
- Price: ") + (historicalData["price_usd"]) ? any(std::string("$") + historicalData["price_usd"]->toLocaleString(undefined, object{
                object::pair{std::string("minimumFractionDigits"), 2}, 
                object::pair{std::string("maximumFractionDigits"), 6}
            }) + string_empty) : any(std::string("N/A")) + std::string("\
- Market Cap: ") + (historicalData["market_cap_usd"]) ? any(std::string("$") + formatMarketCap(historicalData["market_cap_usd"]) + string_empty) : any(std::string("N/A")) + std::string("\
- 24h Volume: ") + (historicalData["total_volume_usd"]) ? any(std::string("$") + formatMarketCap(historicalData["total_volume_usd"]) + string_empty) : any(std::string("N/A")) + std::string("\
- Chain: ") + historicalData["chain"] + std::string("\
- CoinGecko ID: ") + historicalData["coin_id"] + std::string("\
\
This historical price data shows the token's value on the specified date. You can use this to analyze price movements over time or compare with current prices.");
            auto text = summary;
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("text")}, 
                    object::pair{std::string("actions"), array<string>{ std::string("GET_HISTORICAL_PRICE") }}, 
                    object::pair{std::string("content"), as<Record<string, any>>(utils::assign(object{
                    }, historicalData))}, 
                    object::pair{std::string("source"), message->content->source}
                }); });
            }
            return as<any>(object{
                object::pair{std::string("text"), std::string("text")}, 
                object::pair{std::string("success"), true}, 
                object::pair{std::string("data"), historicalData}, 
                object::pair{std::string("values"), historicalData}, 
                object::pair{std::string("input"), inputParams}
            });
        }
        catch (const any& error)
        {
            auto msg = (is<Error>(error)) ? error->message : String(error);
            logger->error(std::string("[GET_HISTORICAL_PRICE] Action failed: ") + msg + string_empty);
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std::string("ACTION_STATE") }, true); });
            auto params = OR((composedState->data->actionParams), (object{}));
            auto failureInputParams = object{
                object::pair{std::string("token"), params["token"]}, 
                object::pair{std::string("date"), params["date"]}, 
                object::pair{std::string("chain"), OR((params["chain"]), (std::string("base")))}
            };
            auto errorText = std::string("Failed to fetch historical price: ") + msg + std::string("\
\
Please check the following:\
1. **Token identifier**: Native tokens (") + Object->keys(nativeTokenIds)->join(std::string(", "))->toUpperCase() + std::string(") can be used by symbol. For all other tokens, you MUST provide the contract address.\
2. **Date format**: Use formats like 'dd-mm-yyyy', '2024-01-01', 'yesterday', '7 days ago', '2 weeks ago', '3 months ago', or '1 year ago'.\
3. **Chain parameter**: Provide the correct blockchain network for contract addresses:\
   | Chain        | Parameter   |\
   | ------------ | ----------- |\
   | **base**     | base        |\
   | **ethereum** | ethereum    |\
   | **polygon**  | polygon     |\
   | **arbitrum** | arbitrum    |\
   | **optimism** | optimism    |\
   \
4. **Historical data availability**: CoinGecko may not have historical data for very new tokens or dates before the token was listed.\
\
**Tip**: Use GET_TOKEN_METADATA action first to retrieve the correct chain and contract address for non-native tokens.\
\
Example: "What was the price of BTC on January 1st, 2024?"\
Example: "Get historical price for ETH 6 months ago"\
Example: "Show me the price of 0x833589fcd6edb6e08f4c7c32d4f71b54bda02913 on base on 01-09-2024"");
            auto errorResult = as<any>(object{
                object::pair{std::string("text"), errorText}, 
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
            object::pair{std::string("text"), std::string("What was the price of Bitcoin on January 1st, 2024?")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Historical price data for Bitcoin (BTC) on 01-01-2024...")}, 
            object::pair{std::string("actions"), array<string>{ std::string("GET_HISTORICAL_PRICE") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Show me ETH price from 6 months ago")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Historical price data for Ethereum (ETH) on [date]...")}, 
            object::pair{std::string("actions"), array<string>{ std::string("GET_HISTORICAL_PRICE") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("What was MATIC worth on 15-06-2024?")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Historical price data for Polygon (MATIC) on 15-06-2024...")}, 
            object::pair{std::string("actions"), array<string>{ std::string("GET_HISTORICAL_PRICE") }}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
