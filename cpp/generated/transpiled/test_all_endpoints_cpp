#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/plugin-birdeye/src/actions/test-all-endpoints.h"

any testAllEndpointsAction = as<std::shared_ptr<Action>>(object{
    object::pair{std::string("name"), std::string("BIRDEYE_TEST_ALL_ENDPOINTS")}, 
    object::pair{std::string("similes"), array<any>()}, 
    object::pair{std::string("description"), std::string("Test all Birdeye endpoints with sample data")}, 
    object::pair{std::string("handler"), [=](auto runtime, auto _message, auto _state, auto _options, auto callback = undefined) mutable
    {
        try
        {
            elizaLogger->info(std::string("Testing all endpoints"));
            std::async([=]() { waitFor(1000); });
            auto birdeyeProvider = std::make_shared<BirdeyeProvider>(runtime->cacheManager);
            auto sampleParams = object{
                object::pair{std::string("token"), std::string("EKpQGSJtjMFqKZ9KQanSqYXRcF8fBopzLHYxdM65zcjm")}, 
                object::pair{std::string("address"), std::string("MfDuWeqSHEqTFVYZ7LoexgAK9dxk7cy4DFJWjWMGVWa")}, 
                object::pair{std::string("network"), std::string("solana")}, 
                object::pair{std::string("list_address"), std::string("EKpQGSJtjMFqKZ9KQanSqYXRcF8fBopzLHYxdM65zcjm")}, 
                object::pair{std::string("address_type"), std::string("token")}, 
                object::pair{std::string("type"), std::string("1D")}, 
                object::pair{std::string("tx_type"), std::string("all")}, 
                object::pair{std::string("sort_type"), std::string("desc")}, 
                object::pair{std::string("unixtime"), 1234567890}, 
                object::pair{std::string("base_address"), std::string("EKpQGSJtjMFqKZ9KQanSqYXRcF8fBopzLHYxdM65zcjm")}, 
                object::pair{std::string("quote_address"), std::string("EKpQGSJtjMFqKZ9KQanSqYXRcF8fBopzLHYxdM65zcjm")}, 
                object::pair{std::string("time_to"), 1672531199}, 
                object::pair{std::string("meme_platform_enabled"), true}, 
                object::pair{std::string("time_frame"), std::string("1D")}, 
                object::pair{std::string("sort_by"), undefined}, 
                object::pair{std::string("list_addresses"), std::string("EKpQGSJtjMFqKZ9KQanSqYXRcF8fBopzLHYxdM65zcjm")}, 
                object::pair{std::string("wallet"), std::string("MfDuWeqSHEqTFVYZ7LoexgAK9dxk7cy4DFJWjWMGVWa")}, 
                object::pair{std::string("token_address"), std::string("EKpQGSJtjMFqKZ9KQanSqYXRcF8fBopzLHYxdM65zcjm")}, 
                object::pair{std::string("pair"), std::string("samplePair")}, 
                object::pair{std::string("before_time"), 1672531199}, 
                object::pair{std::string("after_time"), 1672331199}
            };
            elizaLogger->info(std::string("fetchDefiSupportedNetworks"));
            std::async([=]() { birdeyeProvider->fetchDefiSupportedNetworks(); });
            elizaLogger->success(std::string("fetchDefiSupportedNetworks: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std::string("fetchDefiPrice"));
            std::async([=]() { birdeyeProvider->fetchDefiPrice(utils::assign(object{
            }, sampleParams)); });
            elizaLogger->success(std::string("fetchDefiPrice: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std::string("fetchDefiPriceMultiple"));
            std::async([=]() { birdeyeProvider->fetchDefiPriceMultiple(utils::assign(object{
            }, sampleParams)); });
            elizaLogger->success(std::string("fetchDefiPriceMultiple: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std::string("fetchDefiPriceMultiple_POST"));
            std::async([=]() { birdeyeProvider->fetchDefiPriceMultiple_POST(utils::assign(object{
            }, sampleParams)); });
            elizaLogger->success(std::string("fetchDefiPriceMultiple_POST: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std::string("fetchDefiPriceHistorical"));
            std::async([=]() { birdeyeProvider->fetchDefiPriceHistorical(utils::assign(object{
                , 
                object::pair{std::string("address_type"), std::string("token")}, 
                object::pair{std::string("type"), std::string("1D")}
            }, sampleParams)); });
            elizaLogger->success(std::string("fetchDefiPriceHistorical: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std::string("fetchDefiPriceHistoricalByUnixTime"));
            std::async([=]() { birdeyeProvider->fetchDefiPriceHistoricalByUnixTime(object{
                object::pair{std::string("address"), sampleParams["token"]}
            }); });
            elizaLogger->success(std::string("fetchDefiPriceHistoricalByUnixTime: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std::string("fetchDefiTradesToken"));
            std::async([=]() { birdeyeProvider->fetchDefiTradesToken(object{
                object::pair{std::string("address"), sampleParams["token"]}
            }); });
            elizaLogger->success(std::string("fetchDefiTradesToken: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std::string("fetchDefiTradesPair"));
            std::async([=]() { birdeyeProvider->fetchDefiTradesPair(object{
                object::pair{std::string("address"), sampleParams["token"]}
            }); });
            elizaLogger->success(std::string("fetchDefiTradesPair: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std::string("fetchDefiTradesTokenSeekByTime"));
            std::async([=]() { birdeyeProvider->fetchDefiTradesTokenSeekByTime(object{
                object::pair{std::string("address"), sampleParams["token"]}, 
                object::pair{std::string("before_time"), sampleParams["before_time"]}
            }); });
            elizaLogger->success(std::string("fetchDefiTradesTokenSeekByTime: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std::string("fetchDefiTradesPairSeekByTime"));
            std::async([=]() { birdeyeProvider->fetchDefiTradesPairSeekByTime(object{
                object::pair{std::string("address"), sampleParams["token"]}, 
                object::pair{std::string("after_time"), sampleParams["after_time"]}
            }); });
            elizaLogger->success(std::string("fetchDefiTradesPairSeekByTime: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std::string("fetchDefiOHLCV"));
            std::async([=]() { birdeyeProvider->fetchDefiOHLCV(utils::assign(object{
                , 
                object::pair{std::string("type"), std::string("1D")}
            }, sampleParams)); });
            elizaLogger->success(std::string("fetchDefiOHLCV: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std::string("fetchDefiOHLCVPair"));
            std::async([=]() { birdeyeProvider->fetchDefiOHLCVPair(utils::assign(object{
                , 
                object::pair{std::string("type"), std::string("1D")}
            }, sampleParams)); });
            elizaLogger->success(std::string("fetchDefiOHLCVPair: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std::string("fetchDefiOHLCVBaseQuote"));
            std::async([=]() { birdeyeProvider->fetchDefiOHLCVBaseQuote(utils::assign(object{
                , 
                object::pair{std::string("type"), std::string("1D")}
            }, sampleParams)); });
            elizaLogger->success(std::string("fetchDefiOHLCVBaseQuote: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std::string("fetchDefiPriceVolume"));
            std::async([=]() { birdeyeProvider->fetchDefiPriceVolume(object{
                object::pair{std::string("address"), sampleParams["token"]}
            }); });
            elizaLogger->success(std::string("fetchDefiPriceVolume: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std::string("fetchTokenList"));
            std::async([=]() { birdeyeProvider->fetchTokenList(utils::assign(object{
                , 
                object::pair{std::string("sort_by"), std::string("mc")}, 
                object::pair{std::string("sort_type"), std::string("desc")}
            }, sampleParams)); });
            elizaLogger->success(std::string("fetchTokenList: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std::string("fetchTokenSecurityByAddress"));
            std::async([=]() { birdeyeProvider->fetchTokenSecurityByAddress(utils::assign(object{
            }, sampleParams)); });
            elizaLogger->success(std::string("fetchTokenSecurityByAddress: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std::string("fetchTokenOverview"));
            std::async([=]() { birdeyeProvider->fetchTokenOverview(utils::assign(object{
            }, sampleParams)); });
            elizaLogger->success(std::string("fetchTokenOverview: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std::string("fetchTokenCreationInfo"));
            std::async([=]() { birdeyeProvider->fetchTokenCreationInfo(utils::assign(object{
            }, sampleParams)); });
            elizaLogger->success(std::string("fetchTokenCreationInfo: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std::string("fetchTokenTrending"));
            std::async([=]() { birdeyeProvider->fetchTokenTrending(utils::assign(object{
                , 
                object::pair{std::string("sort_by"), std::string("volume24hUSD")}, 
                object::pair{std::string("sort_type"), std::string("desc")}
            }, sampleParams)); });
            elizaLogger->success(std::string("fetchTokenTrending: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std::string("fetchTokenNewListing"));
            std::async([=]() { birdeyeProvider->fetchTokenNewListing(object{
                object::pair{std::string("time_to"), ((std::make_shared<Date>()))->getTime()}, 
                object::pair{std::string("meme_platform_enabled"), true}
            }); });
            elizaLogger->success(std::string("fetchTokenNewListing: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std::string("fetchTokenTopTraders"));
            std::async([=]() { birdeyeProvider->fetchTokenTopTraders(utils::assign(object{
                , 
                object::pair{std::string("time_frame"), std::string("24h")}, 
                object::pair{std::string("sort_type"), std::string("asc")}, 
                object::pair{std::string("sort_by"), std::string("volume")}
            }, sampleParams)); });
            elizaLogger->success(std::string("fetchTokenTopTraders: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std::string("fetchTokenAllMarketsList"));
            std::async([=]() { birdeyeProvider->fetchTokenAllMarketsList(utils::assign(object{
                , 
                object::pair{std::string("time_frame"), std::string("12H")}, 
                object::pair{std::string("sort_type"), std::string("asc")}, 
                object::pair{std::string("sort_by"), std::string("volume24h")}
            }, sampleParams)); });
            elizaLogger->success(std::string("fetchTokenAllMarketsList: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std::string("fetchTokenMetadataSingle"));
            std::async([=]() { birdeyeProvider->fetchTokenMetadataSingle(utils::assign(object{
            }, sampleParams)); });
            elizaLogger->success(std::string("fetchTokenMetadataSingle: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std::string("fetchTokenMarketData"));
            std::async([=]() { birdeyeProvider->fetchTokenMarketData(utils::assign(object{
            }, sampleParams)); });
            elizaLogger->success(std::string("fetchTokenMarketData: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std::string("fetchTokenTradeDataSingle"));
            std::async([=]() { birdeyeProvider->fetchTokenTradeDataSingle(utils::assign(object{
            }, sampleParams)); });
            elizaLogger->success(std::string("fetchTokenTradeDataSingle: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std::string("fetchTokenHolders"));
            std::async([=]() { birdeyeProvider->fetchTokenHolders(utils::assign(object{
            }, sampleParams)); });
            elizaLogger->success(std::string("fetchTokenHolders: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std::string("fetchTokenMintBurn"));
            std::async([=]() { birdeyeProvider->fetchTokenMintBurn(utils::assign(object{
                , 
                object::pair{std::string("sort_by"), std::string("block_time")}, 
                object::pair{std::string("sort_type"), std::string("desc")}, 
                object::pair{std::string("type"), std::string("all")}
            }, sampleParams)); });
            elizaLogger->success(std::string("fetchTokenMintBurn: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std::string("fetchWalletSupportedNetworks"));
            std::async([=]() { birdeyeProvider->fetchWalletSupportedNetworks(); });
            elizaLogger->success(std::string("fetchWalletSupportedNetworks: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std::string("fetchWalletPortfolio"));
            std::async([=]() { birdeyeProvider->fetchWalletPortfolio(utils::assign(object{
            }, sampleParams)); });
            elizaLogger->success(std::string("fetchWalletPortfolio: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std::string("fetchWalletTokenBalance"));
            std::async([=]() { birdeyeProvider->fetchWalletTokenBalance(utils::assign(object{
            }, sampleParams)); });
            elizaLogger->success(std::string("fetchWalletTokenBalance: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std::string("fetchWalletTransactionHistory"));
            std::async([=]() { birdeyeProvider->fetchWalletTransactionHistory(utils::assign(object{
            }, sampleParams)); });
            elizaLogger->success(std::string("fetchWalletTransactionHistory: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std::string("fetchWalletTransactionSimulate_POST"));
            std::async([=]() { birdeyeProvider->fetchWalletTransactionSimulate_POST(object{
                object::pair{std::string("from"), sampleParams["token"]}, 
                object::pair{std::string("to"), sampleParams["token"]}, 
                object::pair{std::string("data"), JSON->stringify(object{
                    object::pair{std::string("test"), std::string("ok")}
                })}, 
                object::pair{std::string("value"), std::string("100000")}
            }); });
            elizaLogger->success(std::string("fetchWalletTransactionSimulate_POST: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std::string("fetchTraderGainersLosers"));
            std::async([=]() { birdeyeProvider->fetchTraderGainersLosers(utils::assign(object{
                , 
                object::pair{std::string("type"), std::string("today")}, 
                object::pair{std::string("sort_type"), std::string("asc")}
            }, sampleParams)); });
            elizaLogger->success(std::string("fetchTraderGainersLosers: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std::string("fetchTraderTransactionsSeek"));
            std::async([=]() { birdeyeProvider->fetchTraderTransactionsSeek(utils::assign(object{
                , 
                object::pair{std::string("tx_type"), std::string("all")}, 
                object::pair{std::string("before_time"), undefined}
            }, sampleParams)); });
            elizaLogger->success(std::string("fetchTraderTransactionsSeek: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std::string("fetchPairOverviewSingle"));
            std::async([=]() { birdeyeProvider->fetchPairOverviewSingle(utils::assign(object{
            }, sampleParams)); });
            elizaLogger->success(std::string("fetchPairOverviewSingle: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std::string("fetchSearchTokenMarketData"));
            std::async([=]() { birdeyeProvider->fetchSearchTokenMarketData(utils::assign(object{
                , 
                object::pair{std::string("sort_type"), std::string("asc")}
            }, sampleParams)); });
            elizaLogger->success(std::string("fetchSearchTokenMarketData: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std::string("All endpoints tested successfully"));
            callback(object{
                object::pair{std::string("text"), std::string("All endpoints tested successfully!")}
            });
            return true;
        }
        catch (const any& error)
        {
            console->error(std::string("Error in testAllEndpointsAction:"), error["message"]);
            callback(object{
                object::pair{std::string("text"), std::string("Error: ") + error["message"] + string_empty}
            });
            return false;
        }
    }
    }, 
    object::pair{std::string("validate"), [=](auto _runtime, auto message) mutable
    {
        return message->content->text->includes(std::string("BIRDEYE_TEST_ALL_ENDPOINTS"));
    }
    }, 
    object::pair{std::string("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std::string("user"), std::string("user")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I want you to BIRDEYE_TEST_ALL_ENDPOINTS")}, 
            object::pair{std::string("action"), std::string("BIRDEYE_TEST_ALL_ENDPOINTS")}
        }}
    } } })}
});

void Main(void)
{
}

MAIN
