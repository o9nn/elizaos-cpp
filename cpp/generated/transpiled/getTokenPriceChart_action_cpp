#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-coingecko/src/actions/getTokenPriceChart.action.h"

string formatMarketCap(double value)
{
    if (value >= 1000000000) return string_empty + (value / 1000000000)->toFixed(2) + std::string("B");
    if (value >= 1000000) return string_empty + (value / 1000000)->toFixed(2) + std::string("M");
    if (value >= 1000) return string_empty + (value / 1000)->toFixed(2) + std::string("K");
    return value->toFixed(2);
};


std::shared_ptr<Action> getTokenPriceChartAction = object{
    object::pair{std::string("name"), std::string("GET_TOKEN_PRICE_CHART")}, 
    object::pair{std::string("similes"), array<string>{ std::string("TOKEN_CHART"), std::string("PRICE_CHART"), std::string("TOKEN_PRICE_HISTORY"), std::string("PRICE_GRAPH"), std::string("TOKEN_PERFORMANCE") }}, 
    object::pair{std::string("description"), std::string("Use this action when the user asks to see a price chart, graph, or price history for a token. When called successfully, this action automatically provides the token chart visualization in the chat with historical price data points, current price, and price change statistics.")}, 
    object::pair{std::string("parameters"), object{
        object::pair{std::string("token"), object{
            object::pair{std::string("type"), std::string("string")}, 
            object::pair{std::string("description"), std::string("Token symbol or contract address. Native tokens that can be used by symbol: ") + Object->keys(nativeTokenIds)->join(std::string(", "))->toUpperCase() + std::string(". For all other tokens, provide the contract address (e.g., '0x1bc0c42215582d5a085795f4badbac3ff36d1bcb'). Use GET_TOKEN_METADATA first to get the contract address for non-native tokens.")}, 
            object::pair{std::string("required"), true}
        }}, 
        object::pair{std::string("timeframe"), object{
            object::pair{std::string("type"), std::string("string")}, 
            object::pair{std::string("description"), std::string("Time period for the chart. Options: '1h', '24h', '7d', '30d', '90d', '1y'. Defaults to '24h'.")}, 
            object::pair{std::string("required"), false}
        }}, 
        object::pair{std::string("chain"), object{
            object::pair{std::string("type"), std::string("string")}, 
            object::pair{std::string("description"), std::string("Blockchain network for the token (e.g., 'base', 'ethereum', 'polygon', 'arbitrum', 'optimism'). Use GET_TOKEN_METADATA first to determine the correct chain for a specific token.")}, 
            object::pair{std::string("required"), true}
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
                auto errorMsg = std::string("Missing required parameter 'token'. Please specify which token to fetch price chart for. Native tokens (") + supportedNativeTokens + std::string(") can be used by symbol. For all other tokens, provide the contract address. Use GET_TOKEN_METADATA first to get the contract address for non-native tokens.");
                logger->error(std::string("[GET_TOKEN_PRICE_CHART] ") + errorMsg + string_empty);
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
            auto chain = params["chain"]["trim"]()["toLowerCase"]();
            if (!chain) {
                auto errorMsg = std::string("Missing required parameter 'chain'. Please specify the blockchain network (e.g., 'base', 'ethereum', 'polygon'). Use GET_TOKEN_METADATA first to determine the correct chain for a specific token.");
                logger->error(std::string("[GET_TOKEN_PRICE_CHART] ") + errorMsg + string_empty);
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
            auto timeframe = (OR((params["timeframe"]["trim"]()), (std::string("24h"))))["toLowerCase"]();
            auto validTimeframes = array<string>{ std::string("1h"), std::string("24h"), std::string("7d"), std::string("30d"), std::string("90d"), std::string("1y") };
            if (!validTimeframes->includes(timeframe)) {
                auto errorMsg = std::string("Invalid timeframe '") + timeframe + std::string("'. Valid options: ") + validTimeframes->join(std::string(", ")) + string_empty;
                logger->error(std::string("[GET_TOKEN_PRICE_CHART] ") + errorMsg + string_empty);
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
            logger->info(std::string("[GET_TOKEN_PRICE_CHART] Fetching price chart for ") + tokenRaw + std::string(", timeframe: ") + timeframe + std::string(", chain: ") + chain + string_empty);
            auto inputParams = object{
                object::pair{std::string("token"), tokenRaw}, 
                object::pair{std::string("timeframe"), std::string("timeframe")}, 
                object::pair{std::string("chain"), std::string("chain")}
            };
            auto chartData = std::async([=]() { svc->getTokenPriceChart(tokenRaw, timeframe, chain); });
            auto priceChange = nullptr;
            if (chartData["data_points"]["length"] > 0) {
                auto firstPrice = const_(chartData["data_points"])[0]["price"];
                auto lastPrice = const_(chartData["data_points"])[chartData["data_points"]["length"] - 1]["price"];
                auto change = lastPrice - firstPrice;
                auto changePercent = (change / firstPrice) * 100;
                priceChange = object{
                    object::pair{std::string("value"), change}, 
                    object::pair{std::string("percentage"), changePercent}
                };
            }
            auto marketCapChange = nullptr;
            if (AND((chartData["market_cap_data_points"]), (chartData["market_cap_data_points"]["length"] > 0))) {
                auto firstMC = const_(chartData["market_cap_data_points"])[0]["marketCap"];
                auto lastMC = const_(chartData["market_cap_data_points"])[chartData["market_cap_data_points"]["length"] - 1]["marketCap"];
                auto change = lastMC - firstMC;
                auto changePercent = (change / firstMC) * 100;
                marketCapChange = object{
                    object::pair{std::string("value"), change}, 
                    object::pair{std::string("percentage"), changePercent}
                };
            }
            auto summary = std::string("Price chart data for ") + (OR((chartData["token_symbol"]), (tokenRaw))) + std::string(" over ") + timeframe + std::string(":\
- Current Price: $") + (OR((chartData["current_price"]["toFixed"](6)), (std::string("N/A")))) + std::string("\
- Price Change: ") + (priceChange) ? any(string_empty + (priceChange["value"] >= 0) ? std::string("+") : string_empty + std::string("$") + priceChange["value"]->toFixed(6) + std::string(" (") + (priceChange["percentage"] >= 0) ? std::string("+") : string_empty + string_empty + priceChange["percentage"]->toFixed(2) + std::string("%)")) : any(std::string("N/A")) + std::string("\
- Current Market Cap: ") + (chartData["current_market_cap"]) ? any(std::string("$") + formatMarketCap(chartData["current_market_cap"]) + string_empty) : any(std::string("N/A")) + std::string("\
- Market Cap Change: ") + (marketCapChange) ? any(string_empty + (marketCapChange["value"] >= 0) ? std::string("+") : string_empty + std::string("$") + formatMarketCap(Math->abs(marketCapChange["value"])) + std::string(" (") + (marketCapChange["percentage"] >= 0) ? std::string("+") : string_empty + string_empty + marketCapChange["percentage"]->toFixed(2) + std::string("%)")) : any(std::string("N/A")) + std::string("\
- Data Points: ") + chartData["data_points"]["length"] + std::string(" price points\
- Timeframe: ") + chartData["timeframe"] + std::string("\
\
Please analyze this price chart data and provide insights about the token's price movement, market cap trends, and any notable patterns you observe.");
            auto text = summary;
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("text")}, 
                    object::pair{std::string("actions"), array<string>{ std::string("GET_TOKEN_PRICE_CHART") }}, 
                    object::pair{std::string("content"), as<any>(utils::assign(object{
                        , 
                        object::pair{std::string("price_change"), priceChange}, 
                        object::pair{std::string("market_cap_change"), marketCapChange}
                    }, chartData))}, 
                    object::pair{std::string("source"), message->content->source}
                }); });
            }
            return as<any>(object{
                object::pair{std::string("text"), std::string("text")}, 
                object::pair{std::string("success"), true}, 
                object::pair{std::string("data"), utils::assign(object{
                    , 
                    object::pair{std::string("price_change"), priceChange}, 
                    object::pair{std::string("market_cap_change"), marketCapChange}
                }, chartData)}, 
                object::pair{std::string("values"), utils::assign(object{
                    , 
                    object::pair{std::string("price_change"), priceChange}, 
                    object::pair{std::string("market_cap_change"), marketCapChange}
                }, chartData)}, 
                object::pair{std::string("input"), inputParams}
            });
        }
        catch (const any& error)
        {
            auto msg = (is<Error>(error)) ? error->message : String(error);
            logger->error(std::string("[GET_TOKEN_PRICE_CHART] Action failed: ") + msg + string_empty);
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std::string("ACTION_STATE") }, true); });
            auto params = OR((composedState->data->actionParams), (object{}));
            auto failureInputParams = object{
                object::pair{std::string("token"), params["token"]}, 
                object::pair{std::string("timeframe"), OR((params["timeframe"]), (std::string("24h")))}, 
                object::pair{std::string("chain"), params["chain"]}
            };
            auto errorText = std::string("Failed to fetch token price chart: ") + msg + std::string("\
\
Please check the following:\
1. **Token identifier**: Native tokens (") + Object->keys(nativeTokenIds)->join(std::string(", "))->toUpperCase() + std::string(") can be used by symbol. For all other tokens, you MUST provide the contract address. Use GET_TOKEN_METADATA first to get the contract address for non-native tokens.\
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
            object::pair{std::string("text"), std::string("Show me the price chart for Bitcoin")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Price chart data for BTC over 24h...")}, 
            object::pair{std::string("actions"), array<string>{ std::string("GET_TOKEN_PRICE_CHART") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Can I see ETH price history for the past week?")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Price chart data for ETH over 7d...")}, 
            object::pair{std::string("actions"), array<string>{ std::string("GET_TOKEN_PRICE_CHART") }}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
