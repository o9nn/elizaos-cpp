#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-coingecko/src/actions/getHistoricalPrice.action.h"

string formatMarketCap(double value)
{
    if (value >= 1000000000) return string_empty + (value / 1000000000)->toFixed(2) + std:("B");
    if (value >= 1000000) return string_empty + (value / 1000000)->toFixed(2) + std:("M");
    if (value >= 1000) return string_empty + (value / 1000)->toFixed(2) + std:("K");
    return value->toFixed(2);
};


string parseDateToApiFormat(string dateStr)
{
    auto normalized = dateStr->trim()->toLowerCase();
    if ((new RegExp(std:("^\d{2}-\d{2}-\d{4}")))->test(dateStr)) {
        return dateStr;
    }
    std::shared_ptr<Date> date;
    if (normalized == std:("today")) {
        date = std::make_shared<Date>();
    } else if (normalized == std:("yesterday")) {
        date = std::make_shared<Date>();
        date->setDate(date->getDate() - 1);
    } else if ((new RegExp(std:("^(\d+)\s*days?\s*ago")))->test(normalized)) {
        auto daysMatch = normalized->match((new RegExp(std:("^(\d+)\s*days?\s*ago"))));
        auto days = (daysMatch) ? any(parseInt((*const_(daysMatch))[1])) (0);
        date = std::make_shared<Date>();
        date->setDate(date->getDate() - days);
    } else if ((new RegExp(std:("^(\d+)\s*weeks?\s*ago")))->test(normalized)) {
        auto weeksMatch = normalized->match((new RegExp(std:("^(\d+)\s*weeks?\s*ago"))));
        auto weeks = (weeksMatch) ? any(parseInt((*const_(weeksMatch))[1])) (0);
        date = std::make_shared<Date>();
        date->setDate(date->getDate() - (weeks * 7));
    } else if ((new RegExp(std:("^(\d+)\s*months?\s*ago")))->test(normalized)) {
        auto monthsMatch = normalized->match((new RegExp(std:("^(\d+)\s*months?\s*ago"))));
        auto months = (monthsMatch) ? any(parseInt((*const_(monthsMatch))[1])) (0);
        date = std::make_shared<Date>();
        date->setMonth(date->getMonth() - months);
    } else if ((new RegExp(std:("^(\d+)\s*years?\s*ago")))->test(normalized)) {
        auto yearsMatch = normalized->match((new RegExp(std:("^(\d+)\s*years?\s*ago"))));
        auto years = (yearsMatch) ? any(parseInt((*const_(yearsMatch))[1])) (0);
        date = std::make_shared<Date>();
        date->setFullYear(date->getFullYear() - years);
    } else {
        date = std::make_shared<Date>(dateStr);
        if (isNaN(date->getTime())) {
            throw any(std::make_shared<Error>(std:("Unable to parse date: ") + dateStr + string_empty));
        }
    }
    auto day = String(date->getDate())->padStart(2, std:("0"));
    auto month = String(date->getMonth() + 1)->padStart(2, std:("0"));
    auto year = date->getFullYear();
    return string_empty + day + std:("-") + month + std:("-") + year + string_empty;
};


std::shared_ptr<Action> getHistoricalPriceAction = object{
    object::pair{std:("name"), std:("GET_HISTORICAL_PRICE")}, 
    object::pair{std:("similes"), array<string>{ std:("HISTORICAL_PRICE"), std:("PRICE_ON_DATE"), std:("PAST_PRICE"), std:("TOKEN_PRICE_HISTORY"), std:("PRICE_AT_DATE") }}, 
    object::pair{std:("description"), std:("Use this action when the user asks for a token's price on a specific date in the past. This action retrieves historical price data for any token (native or contract address) at a particular point in time. Returns the price, market cap, and trading volume for that date.")}, 
    object::pair{std:("parameters"), object{
        object::pair{std:("token"), object{
            object::pair{std:("type"), std:("string")}, 
            object::pair{std:("description"), std:("Token symbol or contract address. Native tokens that can be used by symbol: ") + Object->keys(nativeTokenIds)->join(std:(", "))->toUpperCase() + std:(". For all other tokens, provide the contract address (e.g., '0x833589fcd6edb6e08f4c7c32d4f71b54bda02913'). Use GET_TOKEN_METADATA first to get the contract address for non-native tokens.")}, 
            object::pair{std:("required"), true}
        }}, 
        object::pair{std:("date"), object{
            object::pair{std:("type"), std:("string")}, 
            object::pair{std:("description"), std:("Date for historical price. Accepts formats: 'dd-mm-yyyy' (e.g., '01-01-2024'), '2024-01-01', 'today', 'yesterday', '7 days ago', '2 weeks ago', '3 months ago', '1 year ago'.")}, 
            object::pair{std:("required"), true}
        }}, 
        object::pair{std:("chain"), object{
            object::pair{std:("type"), std:("string")}, 
            object::pair{std:("description"), std:("Blockchain network for the token (e.g., 'base', 'ethereum', 'polygon', 'arbitrum', 'optimism'). Required for contract addresses, optional for native tokens. Use GET_TOKEN_METADATA first to determine the correct chain.")}, 
            object::pair{std:("required"), false}
        }}
    }}, 
    object::pair{std:("validate"), [=](auto runtime) mutable
    {
        auto svc = as<any>(runtime->getService(CoinGeckoService::serviceType));
        if (!svc) {
            logger->error(std:("CoinGeckoService not available"));
            return false;
        }
        return true;
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto _state = undefined, auto _options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto svc = as<any>(runtime->getService(CoinGeckoService::serviceType));
            if (!svc) {
                throw any(std::make_shared<Error>(std:("CoinGeckoService not available")));
            }
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std:("ACTION_STATE") }, true); });
            auto params = OR((composedState->data->actionParams), (object{}));
            auto tokenRaw = params["token"]["trim"]();
            if (!tokenRaw) {
                auto supportedNativeTokens = Object->keys(nativeTokenIds)->join(std:(", "))->toUpperCase();
                auto errorMsg = std:("Missing required parameter 'token'. Please specify which token to fetch historical price for. Native tokens (") + supportedNativeTokens + std:(") can be used by symbol. For all other tokens, provide the contract address.");
                logger->error(std:("[GET_HISTORICAL_PRICE] ") + errorMsg + string_empty);
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
            auto dateRaw = params["date"]["trim"]();
            if (!dateRaw) {
                auto errorMsg = std:("Missing required parameter 'date'. Please specify the date for historical price (e.g., '01-01-2024', 'yesterday', '7 days ago').");
                logger->error(std:("[GET_HISTORICAL_PRICE] ") + errorMsg + string_empty);
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
            string apiDate;
            try
            {
                apiDate = parseDateToApiFormat(dateRaw);
            }
            catch (const any& err)
            {
                auto msg = (is<Error>(err)) ? err->message : String(err);
                auto errorMsg = std:("Invalid date format: ") + msg + std:(". Please use formats like 'dd-mm-yyyy', '2024-01-01', 'yesterday', '7 days ago', etc.");
                logger->error(std:("[GET_HISTORICAL_PRICE] ") + errorMsg + string_empty);
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
            auto chain = OR((params["chain"]["trim"]()["toLowerCase"]()), (std:("base")));
            logger->info(std:("[GET_HISTORICAL_PRICE] Fetching historical price for ") + tokenRaw + std:(" on ") + apiDate + std:(" (chain: ") + chain + std:(")"));
            auto inputParams = object{
                object::pair{std:("token"), tokenRaw}, 
                object::pair{std:("date"), dateRaw}, 
                object::pair{std:("chain"), std:("chain")}
            };
            auto historicalData = std::async([=]() { svc->getHistoricalPrice(tokenRaw, apiDate, chain); });
            auto tokenDisplay = (historicalData["token_name"]) ? string_empty + historicalData["token_name"] + std:(" (") + (OR((historicalData["token_symbol"]), (tokenRaw))) + std:(")") : (OR((historicalData["token_symbol"]), (tokenRaw)));
            auto summary = std:("Historical price data for ") + tokenDisplay + std:(" on ") + apiDate + std:(":\
- Token: ") + tokenDisplay + std:("\
- Date: ") + apiDate + std:("\
- Price: ") + (historicalData["price_usd"]) ? any(std:("$") + historicalData["price_usd"]->toLocaleString(undefined, object{
                object::pair{std:("minimumFractionDigits"), 2}, 
                object::pair{std:("maximumFractionDigits"), 6}
            }) + string_empty) (std:("N/A")) + std:("\
- Market Cap: ") + (historicalData["market_cap_usd"]) ? any(std:("$") + formatMarketCap(historicalData["market_cap_usd"]) + string_empty) (std:("N/A")) + std:("\
- 24h Volume: ") + (historicalData["total_volume_usd"]) ? any(std:("$") + formatMarketCap(historicalData["total_volume_usd"]) + string_empty) (std:("N/A")) + std:("\
- Chain: ") + historicalData["chain"] + std:("\
- CoinGecko ID: ") + historicalData["coin_id"] + std:("\
\
This historical price data shows the token's value on the specified date. You can use this to analyze price movements over time or compare with current prices.");
            auto text = summary;
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), std:("text")}, 
                    object::pair{std:("actions"), array<string>{ std:("GET_HISTORICAL_PRICE") }}, 
                    object::pair{std:("content"), as<Record<string, any>>(utils::assign(object{
                    }, historicalData))}, 
                    object::pair{std:("source"), message->content->source}
                }); });
            }
            return as<any>(object{
                object::pair{std:("text"), std:("text")}, 
                object::pair{std:("success"), true}, 
                object::pair{std:("data"), historicalData}, 
                object::pair{std:("values"), historicalData}, 
                object::pair{std:("input"), inputParams}
            });
        }
        catch (const any& error)
        {
            auto msg = (is<Error>(error)) ? error->message : String(error);
            logger->error(std:("[GET_HISTORICAL_PRICE] Action failed: ") + msg + string_empty);
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std:("ACTION_STATE") }, true); });
            auto params = OR((composedState->data->actionParams), (object{}));
            auto failureInputParams = object{
                object::pair{std:("token"), params["token"]}, 
                object::pair{std:("date"), params["date"]}, 
                object::pair{std:("chain"), OR((params["chain"]), (std:("base")))}
            };
            auto errorText = std:("Failed to fetch historical price: ") + msg + std:("\
\
Please check the following:\
1. **Token identifier**: Native tokens (") + Object->keys(nativeTokenIds)->join(std:(", "))->toUpperCase() + std:(") can be used by symbol. For all other tokens, you MUST provide the contract address.\
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
                object::pair{std:("text"), errorText}, 
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
            object::pair{std:("text"), std:("What was the price of Bitcoin on January 1st, 2024?")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Historical price data for Bitcoin (BTC) on 01-01-2024...")}, 
            object::pair{std:("actions"), array<string>{ std:("GET_HISTORICAL_PRICE") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Show me ETH price from 6 months ago")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Historical price data for Ethereum (ETH) on [date]...")}, 
            object::pair{std:("actions"), array<string>{ std:("GET_HISTORICAL_PRICE") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("What was MATIC worth on 15-06-2024?")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Historical price data for Polygon (MATIC) on 15-06-2024...")}, 
            object::pair{std:("actions"), array<string>{ std:("GET_HISTORICAL_PRICE") }}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
