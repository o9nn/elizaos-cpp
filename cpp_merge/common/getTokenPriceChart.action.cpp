#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-coingecko/src/actions/getTokenPriceChart.action.h"

string formatMarketCap(double value)
{
    if (value >= 1000000000) return string_empty + (value / 1000000000)->toFixed(2) + std:("B");
    if (value >= 1000000) return string_empty + (value / 1000000)->toFixed(2) + std:("M");
    if (value >= 1000) return string_empty + (value / 1000)->toFixed(2) + std:("K");
    return value->toFixed(2);
};


std::shared_ptr<Action> getTokenPriceChartAction = object{
    object::pair{std:("name"), std:("GET_TOKEN_PRICE_CHART")}, 
    object::pair{std:("similes"), array<string>{ std:("TOKEN_CHART"), std:("PRICE_CHART"), std:("TOKEN_PRICE_HISTORY"), std:("PRICE_GRAPH"), std:("TOKEN_PERFORMANCE") }}, 
    object::pair{std:("description"), std:("Use this action when the user asks to see a price chart, graph, or price history for a token. When called successfully, this action automatically provides the token chart visualization in the chat with historical price data points, current price, and price change statistics.")}, 
    object::pair{std:("parameters"), object{
        object::pair{std:("token"), object{
            object::pair{std:("type"), std:("string")}, 
            object::pair{std:("description"), std:("Token symbol or contract address. Native tokens that can be used by symbol: ") + Object->keys(nativeTokenIds)->join(std:(", "))->toUpperCase() + std:(". For all other tokens, provide the contract address (e.g., '0x1bc0c42215582d5a085795f4badbac3ff36d1bcb'). Use GET_TOKEN_METADATA first to get the contract address for non-native tokens.")}, 
            object::pair{std:("required"), true}
        }}, 
        object::pair{std:("timeframe"), object{
            object::pair{std:("type"), std:("string")}, 
            object::pair{std:("description"), std:("Time period for the chart. Options: '1h', '24h', '7d', '30d', '90d', '1y'. Defaults to '24h'.")}, 
            object::pair{std:("required"), false}
        }}, 
        object::pair{std:("chain"), object{
            object::pair{std:("type"), std:("string")}, 
            object::pair{std:("description"), std:("Blockchain network for the token (e.g., 'base', 'ethereum', 'polygon', 'arbitrum', 'optimism'). Use GET_TOKEN_METADATA first to determine the correct chain for a specific token.")}, 
            object::pair{std:("required"), true}
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
                auto errorMsg = std:("Missing required parameter 'token'. Please specify which token to fetch price chart for. Native tokens (") + supportedNativeTokens + std:(") can be used by symbol. For all other tokens, provide the contract address. Use GET_TOKEN_METADATA first to get the contract address for non-native tokens.");
                logger->error(std:("[GET_TOKEN_PRICE_CHART] ") + errorMsg + string_empty);
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
            auto chain = params["chain"]["trim"]()["toLowerCase"]();
            if (!chain) {
                auto errorMsg = std:("Missing required parameter 'chain'. Please specify the blockchain network (e.g., 'base', 'ethereum', 'polygon'). Use GET_TOKEN_METADATA first to determine the correct chain for a specific token.");
                logger->error(std:("[GET_TOKEN_PRICE_CHART] ") + errorMsg + string_empty);
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
            auto timeframe = (OR((params["timeframe"]["trim"]()), (std:("24h"))))["toLowerCase"]();
            auto validTimeframes = array<string>{ std:("1h"), std:("24h"), std:("7d"), std:("30d"), std:("90d"), std:("1y") };
            if (!validTimeframes->includes(timeframe)) {
                auto errorMsg = std:("Invalid timeframe '") + timeframe + std:("'. Valid options: ") + validTimeframes->join(std:(", ")) + string_empty;
                logger->error(std:("[GET_TOKEN_PRICE_CHART] ") + errorMsg + string_empty);
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
            logger->info(std:("[GET_TOKEN_PRICE_CHART] Fetching price chart for ") + tokenRaw + std:(", timeframe: ") + timeframe + std:(", chain: ") + chain + string_empty);
            auto inputParams = object{
                object::pair{std:("token"), tokenRaw}, 
                object::pair{std:("timeframe"), std:("timeframe")}, 
                object::pair{std:("chain"), std:("chain")}
            };
            auto chartData = std::async([=]() { svc->getTokenPriceChart(tokenRaw, timeframe, chain); });
            auto priceChange = nullptr;
            if (chartData["data_points"]["length"] > 0) {
                auto firstPrice = const_(chartData["data_points"])[0]["price"];
                auto lastPrice = const_(chartData["data_points"])[chartData["data_points"]["length"] - 1]["price"];
                auto change = lastPrice - firstPrice;
                auto changePercent = (change / firstPrice) * 100;
                priceChange = object{
                    object::pair{std:("value"), change}, 
                    object::pair{std:("percentage"), changePercent}
                };
            }
            auto marketCapChange = nullptr;
            if (AND((chartData["market_cap_data_points"]), (chartData["market_cap_data_points"]["length"] > 0))) {
                auto firstMC = const_(chartData["market_cap_data_points"])[0]["marketCap"];
                auto lastMC = const_(chartData["market_cap_data_points"])[chartData["market_cap_data_points"]["length"] - 1]["marketCap"];
                auto change = lastMC - firstMC;
                auto changePercent = (change / firstMC) * 100;
                marketCapChange = object{
                    object::pair{std:("value"), change}, 
                    object::pair{std:("percentage"), changePercent}
                };
            }
            auto summary = std:("Price chart data for ") + (OR((chartData["token_symbol"]), (tokenRaw))) + std:(" over ") + timeframe + std:(":\
- Current Price: $") + (OR((chartData["current_price"]["toFixed"](6)), (std:("N/A")))) + std:("\
- Price Change: ") + (priceChange) ? any(string_empty + (priceChange["value"] >= 0) ? std:("+") : string_empty + std:("$") + priceChange["value"]->toFixed(6) + std:(" (") + (priceChange["percentage"] >= 0) ? std:("+") : string_empty + string_empty + priceChange["percentage"]->toFixed(2) + std:("%)")) (std:("N/A")) + std:("\
- Current Market Cap: ") + (chartData["current_market_cap"]) ? any(std:("$") + formatMarketCap(chartData["current_market_cap"]) + string_empty) (std:("N/A")) + std:("\
- Market Cap Change: ") + (marketCapChange) ? any(string_empty + (marketCapChange["value"] >= 0) ? std:("+") : string_empty + std:("$") + formatMarketCap(Math->abs(marketCapChange["value"])) + std:(" (") + (marketCapChange["percentage"] >= 0) ? std:("+") : string_empty + string_empty + marketCapChange["percentage"]->toFixed(2) + std:("%)")) (std:("N/A")) + std:("\
- Data Points: ") + chartData["data_points"]["length"] + std:(" price points\
- Timeframe: ") + chartData["timeframe"] + std:("\
\
Please analyze this price chart data and provide insights about the token's price movement, market cap trends, and any notable patterns you observe.");
            auto text = summary;
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), std:("text")}, 
                    object::pair{std:("actions"), array<string>{ std:("GET_TOKEN_PRICE_CHART") }}, 
                    object::pair{std:("content"), as<any>(utils::assign(object{
                        , 
                        object::pair{std:("price_change"), priceChange}, 
                        object::pair{std:("market_cap_change"), marketCapChange}
                    }, chartData))}, 
                    object::pair{std:("source"), message->content->source}
                }); });
            }
            return as<any>(object{
                object::pair{std:("text"), std:("text")}, 
                object::pair{std:("success"), true}, 
                object::pair{std:("data"), utils::assign(object{
                    , 
                    object::pair{std:("price_change"), priceChange}, 
                    object::pair{std:("market_cap_change"), marketCapChange}
                }, chartData)}, 
                object::pair{std:("values"), utils::assign(object{
                    , 
                    object::pair{std:("price_change"), priceChange}, 
                    object::pair{std:("market_cap_change"), marketCapChange}
                }, chartData)}, 
                object::pair{std:("input"), inputParams}
            });
        }
        catch (const any& error)
        {
            auto msg = (is<Error>(error)) ? error->message : String(error);
            logger->error(std:("[GET_TOKEN_PRICE_CHART] Action failed: ") + msg + string_empty);
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std:("ACTION_STATE") }, true); });
            auto params = OR((composedState->data->actionParams), (object{}));
            auto failureInputParams = object{
                object::pair{std:("token"), params["token"]}, 
                object::pair{std:("timeframe"), OR((params["timeframe"]), (std:("24h")))}, 
                object::pair{std:("chain"), params["chain"]}
            };
            auto errorText = std:("Failed to fetch token price chart: ") + msg + std:("\
\
Please check the following:\
1. **Token identifier**: Native tokens (") + Object->keys(nativeTokenIds)->join(std:(", "))->toUpperCase() + std:(") can be used by symbol. For all other tokens, you MUST provide the contract address. Use GET_TOKEN_METADATA first to get the contract address for non-native tokens.\
2. **Chain parameter** (REQUIRED): Provide the correct blockchain network:\
   | Chain        | Parameter   |\
   | ------------ | ----------- |\
   | **base**     | base        |\
   | **ethereum** | ethereum    |\
   | **polygon**  | polygon     |\
   | **arbitrum** | arbitrum    |\
   | **optimism** | optimism    |\
   \
3. **Timeframe**: Optional - '1h', '24h', '7d', '30d', '90d', or '1y' (default: '24h')\
\
 **Tip**: Use GET_TOKEN_METADATA action first to retrieve the correct chain and contract address for non-native tokens.\
\
Example: "Show me the price chart for BTC on ethereum over the last 7 days"\
Example: "Get the chart for 0x1bc0c42215582d5a085795f4badbac3ff36d1bcb on base for 30 days"");
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
            object::pair{std:("text"), std:("Show me the price chart for Bitcoin")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Price chart data for BTC over 24h...")}, 
            object::pair{std:("actions"), array<string>{ std:("GET_TOKEN_PRICE_CHART") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Can I see ETH price history for the past week?")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Price chart data for ETH over 7d...")}, 
            object::pair{std:("actions"), array<string>{ std:("GET_TOKEN_PRICE_CHART") }}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
