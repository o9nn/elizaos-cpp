#include "test-all-endpoints.h"

any testAllEndpointsAction = as<std::shared_ptr<Action>>(object{
    object::pair{std:("name"), std:("BIRDEYE_TEST_ALL_ENDPOINTS")}, 
    object::pair{std:("similes"), array<any>()}, 
    object::pair{std:("description"), std:("Test all Birdeye endpoints with sample data")}, 
    object::pair{std:("handler"), [=](auto runtime, auto _message, auto _state, auto _options, auto callback = undefined) mutable
    {
        try
        {
            elizaLogger->info(std:("Testing all endpoints"));
            std::async([=]() { waitFor(1000); });
            auto birdeyeProvider = std::make_shared<BirdeyeProvider>(runtime->cacheManager);
            auto sampleParams = object{
                object::pair{std:("token"), std:("EKpQGSJtjMFqKZ9KQanSqYXRcF8fBopzLHYxdM65zcjm")}, 
                object::pair{std:("address"), std:("MfDuWeqSHEqTFVYZ7LoexgAK9dxk7cy4DFJWjWMGVWa")}, 
                object::pair{std:("network"), std:("solana")}, 
                object::pair{std:("list_address"), std:("EKpQGSJtjMFqKZ9KQanSqYXRcF8fBopzLHYxdM65zcjm")}, 
                object::pair{std:("address_type"), std:("token")}, 
                object::pair{std:("type"), std:("1D")}, 
                object::pair{std:("tx_type"), std:("all")}, 
                object::pair{std:("sort_type"), std:("desc")}, 
                object::pair{std:("unixtime"), 1234567890}, 
                object::pair{std:("base_address"), std:("EKpQGSJtjMFqKZ9KQanSqYXRcF8fBopzLHYxdM65zcjm")}, 
                object::pair{std:("quote_address"), std:("EKpQGSJtjMFqKZ9KQanSqYXRcF8fBopzLHYxdM65zcjm")}, 
                object::pair{std:("time_to"), 1672531199}, 
                object::pair{std:("meme_platform_enabled"), true}, 
                object::pair{std:("time_frame"), std:("1D")}, 
                object::pair{std:("sort_by"), undefined}, 
                object::pair{std:("list_addresses"), std:("EKpQGSJtjMFqKZ9KQanSqYXRcF8fBopzLHYxdM65zcjm")}, 
                object::pair{std:("wallet"), std:("MfDuWeqSHEqTFVYZ7LoexgAK9dxk7cy4DFJWjWMGVWa")}, 
                object::pair{std:("token_address"), std:("EKpQGSJtjMFqKZ9KQanSqYXRcF8fBopzLHYxdM65zcjm")}, 
                object::pair{std:("pair"), std:("samplePair")}, 
                object::pair{std:("before_time"), 1672531199}, 
                object::pair{std:("after_time"), 1672331199}
            };
            elizaLogger->info(std:("fetchDefiSupportedNetworks"));
            std::async([=]() { birdeyeProvider->fetchDefiSupportedNetworks(); });
            elizaLogger->success(std:("fetchDefiSupportedNetworks: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std:("fetchDefiPrice"));
            std::async([=]() { birdeyeProvider->fetchDefiPrice(utils::assign(object{
            }, sampleParams)); });
            elizaLogger->success(std:("fetchDefiPrice: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std:("fetchDefiPriceMultiple"));
            std::async([=]() { birdeyeProvider->fetchDefiPriceMultiple(utils::assign(object{
            }, sampleParams)); });
            elizaLogger->success(std:("fetchDefiPriceMultiple: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std:("fetchDefiPriceMultiple_POST"));
            std::async([=]() { birdeyeProvider->fetchDefiPriceMultiple_POST(utils::assign(object{
            }, sampleParams)); });
            elizaLogger->success(std:("fetchDefiPriceMultiple_POST: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std:("fetchDefiPriceHistorical"));
            std::async([=]() { birdeyeProvider->fetchDefiPriceHistorical(utils::assign(object{
                , 
                object::pair{std:("address_type"), std:("token")}, 
                object::pair{std:("type"), std:("1D")}
            }, sampleParams)); });
            elizaLogger->success(std:("fetchDefiPriceHistorical: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std:("fetchDefiPriceHistoricalByUnixTime"));
            std::async([=]() { birdeyeProvider->fetchDefiPriceHistoricalByUnixTime(object{
                object::pair{std:("address"), sampleParams["token"]}
            }); });
            elizaLogger->success(std:("fetchDefiPriceHistoricalByUnixTime: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std:("fetchDefiTradesToken"));
            std::async([=]() { birdeyeProvider->fetchDefiTradesToken(object{
                object::pair{std:("address"), sampleParams["token"]}
            }); });
            elizaLogger->success(std:("fetchDefiTradesToken: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std:("fetchDefiTradesPair"));
            std::async([=]() { birdeyeProvider->fetchDefiTradesPair(object{
                object::pair{std:("address"), sampleParams["token"]}
            }); });
            elizaLogger->success(std:("fetchDefiTradesPair: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std:("fetchDefiTradesTokenSeekByTime"));
            std::async([=]() { birdeyeProvider->fetchDefiTradesTokenSeekByTime(object{
                object::pair{std:("address"), sampleParams["token"]}, 
                object::pair{std:("before_time"), sampleParams["before_time"]}
            }); });
            elizaLogger->success(std:("fetchDefiTradesTokenSeekByTime: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std:("fetchDefiTradesPairSeekByTime"));
            std::async([=]() { birdeyeProvider->fetchDefiTradesPairSeekByTime(object{
                object::pair{std:("address"), sampleParams["token"]}, 
                object::pair{std:("after_time"), sampleParams["after_time"]}
            }); });
            elizaLogger->success(std:("fetchDefiTradesPairSeekByTime: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std:("fetchDefiOHLCV"));
            std::async([=]() { birdeyeProvider->fetchDefiOHLCV(utils::assign(object{
                , 
                object::pair{std:("type"), std:("1D")}
            }, sampleParams)); });
            elizaLogger->success(std:("fetchDefiOHLCV: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std:("fetchDefiOHLCVPair"));
            std::async([=]() { birdeyeProvider->fetchDefiOHLCVPair(utils::assign(object{
                , 
                object::pair{std:("type"), std:("1D")}
            }, sampleParams)); });
            elizaLogger->success(std:("fetchDefiOHLCVPair: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std:("fetchDefiOHLCVBaseQuote"));
            std::async([=]() { birdeyeProvider->fetchDefiOHLCVBaseQuote(utils::assign(object{
                , 
                object::pair{std:("type"), std:("1D")}
            }, sampleParams)); });
            elizaLogger->success(std:("fetchDefiOHLCVBaseQuote: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std:("fetchDefiPriceVolume"));
            std::async([=]() { birdeyeProvider->fetchDefiPriceVolume(object{
                object::pair{std:("address"), sampleParams["token"]}
            }); });
            elizaLogger->success(std:("fetchDefiPriceVolume: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std:("fetchTokenList"));
            std::async([=]() { birdeyeProvider->fetchTokenList(utils::assign(object{
                , 
                object::pair{std:("sort_by"), std:("mc")}, 
                object::pair{std:("sort_type"), std:("desc")}
            }, sampleParams)); });
            elizaLogger->success(std:("fetchTokenList: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std:("fetchTokenSecurityByAddress"));
            std::async([=]() { birdeyeProvider->fetchTokenSecurityByAddress(utils::assign(object{
            }, sampleParams)); });
            elizaLogger->success(std:("fetchTokenSecurityByAddress: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std:("fetchTokenOverview"));
            std::async([=]() { birdeyeProvider->fetchTokenOverview(utils::assign(object{
            }, sampleParams)); });
            elizaLogger->success(std:("fetchTokenOverview: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std:("fetchTokenCreationInfo"));
            std::async([=]() { birdeyeProvider->fetchTokenCreationInfo(utils::assign(object{
            }, sampleParams)); });
            elizaLogger->success(std:("fetchTokenCreationInfo: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std:("fetchTokenTrending"));
            std::async([=]() { birdeyeProvider->fetchTokenTrending(utils::assign(object{
                , 
                object::pair{std:("sort_by"), std:("volume24hUSD")}, 
                object::pair{std:("sort_type"), std:("desc")}
            }, sampleParams)); });
            elizaLogger->success(std:("fetchTokenTrending: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std:("fetchTokenNewListing"));
            std::async([=]() { birdeyeProvider->fetchTokenNewListing(object{
                object::pair{std:("time_to"), ((std::make_shared<Date>()))->getTime()}, 
                object::pair{std:("meme_platform_enabled"), true}
            }); });
            elizaLogger->success(std:("fetchTokenNewListing: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std:("fetchTokenTopTraders"));
            std::async([=]() { birdeyeProvider->fetchTokenTopTraders(utils::assign(object{
                , 
                object::pair{std:("time_frame"), std:("24h")}, 
                object::pair{std:("sort_type"), std:("asc")}, 
                object::pair{std:("sort_by"), std:("volume")}
            }, sampleParams)); });
            elizaLogger->success(std:("fetchTokenTopTraders: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std:("fetchTokenAllMarketsList"));
            std::async([=]() { birdeyeProvider->fetchTokenAllMarketsList(utils::assign(object{
                , 
                object::pair{std:("time_frame"), std:("12H")}, 
                object::pair{std:("sort_type"), std:("asc")}, 
                object::pair{std:("sort_by"), std:("volume24h")}
            }, sampleParams)); });
            elizaLogger->success(std:("fetchTokenAllMarketsList: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std:("fetchTokenMetadataSingle"));
            std::async([=]() { birdeyeProvider->fetchTokenMetadataSingle(utils::assign(object{
            }, sampleParams)); });
            elizaLogger->success(std:("fetchTokenMetadataSingle: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std:("fetchTokenMarketData"));
            std::async([=]() { birdeyeProvider->fetchTokenMarketData(utils::assign(object{
            }, sampleParams)); });
            elizaLogger->success(std:("fetchTokenMarketData: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std:("fetchTokenTradeDataSingle"));
            std::async([=]() { birdeyeProvider->fetchTokenTradeDataSingle(utils::assign(object{
            }, sampleParams)); });
            elizaLogger->success(std:("fetchTokenTradeDataSingle: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std:("fetchTokenHolders"));
            std::async([=]() { birdeyeProvider->fetchTokenHolders(utils::assign(object{
            }, sampleParams)); });
            elizaLogger->success(std:("fetchTokenHolders: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std:("fetchTokenMintBurn"));
            std::async([=]() { birdeyeProvider->fetchTokenMintBurn(utils::assign(object{
                , 
                object::pair{std:("sort_by"), std:("block_time")}, 
                object::pair{std:("sort_type"), std:("desc")}, 
                object::pair{std:("type"), std:("all")}
            }, sampleParams)); });
            elizaLogger->success(std:("fetchTokenMintBurn: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std:("fetchWalletSupportedNetworks"));
            std::async([=]() { birdeyeProvider->fetchWalletSupportedNetworks(); });
            elizaLogger->success(std:("fetchWalletSupportedNetworks: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std:("fetchWalletPortfolio"));
            std::async([=]() { birdeyeProvider->fetchWalletPortfolio(utils::assign(object{
            }, sampleParams)); });
            elizaLogger->success(std:("fetchWalletPortfolio: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std:("fetchWalletTokenBalance"));
            std::async([=]() { birdeyeProvider->fetchWalletTokenBalance(utils::assign(object{
            }, sampleParams)); });
            elizaLogger->success(std:("fetchWalletTokenBalance: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std:("fetchWalletTransactionHistory"));
            std::async([=]() { birdeyeProvider->fetchWalletTransactionHistory(utils::assign(object{
            }, sampleParams)); });
            elizaLogger->success(std:("fetchWalletTransactionHistory: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std:("fetchWalletTransactionSimulate_POST"));
            std::async([=]() { birdeyeProvider->fetchWalletTransactionSimulate_POST(object{
                object::pair{std:("from"), sampleParams["token"]}, 
                object::pair{std:("to"), sampleParams["token"]}, 
                object::pair{std:("data"), JSON->stringify(object{
                    object::pair{std:("test"), std:("ok")}
                })}, 
                object::pair{std:("value"), std:("100000")}
            }); });
            elizaLogger->success(std:("fetchWalletTransactionSimulate_POST: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std:("fetchTraderGainersLosers"));
            std::async([=]() { birdeyeProvider->fetchTraderGainersLosers(utils::assign(object{
                , 
                object::pair{std:("type"), std:("today")}, 
                object::pair{std:("sort_type"), std:("asc")}
            }, sampleParams)); });
            elizaLogger->success(std:("fetchTraderGainersLosers: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std:("fetchTraderTransactionsSeek"));
            std::async([=]() { birdeyeProvider->fetchTraderTransactionsSeek(utils::assign(object{
                , 
                object::pair{std:("tx_type"), std:("all")}, 
                object::pair{std:("before_time"), undefined}
            }, sampleParams)); });
            elizaLogger->success(std:("fetchTraderTransactionsSeek: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std:("fetchPairOverviewSingle"));
            std::async([=]() { birdeyeProvider->fetchPairOverviewSingle(utils::assign(object{
            }, sampleParams)); });
            elizaLogger->success(std:("fetchPairOverviewSingle: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std:("fetchSearchTokenMarketData"));
            std::async([=]() { birdeyeProvider->fetchSearchTokenMarketData(utils::assign(object{
                , 
                object::pair{std:("sort_type"), std:("asc")}
            }, sampleParams)); });
            elizaLogger->success(std:("fetchSearchTokenMarketData: SUCCESS!"));
            std::async([=]() { waitFor(500); });
            elizaLogger->info(std:("All endpoints tested successfully"));
            callback(object{
                object::pair{std:("text"), std:("All endpoints tested successfully!")}
            });
            return true;
        }
        catch (const any& error)
        {
            console->error(std:("Error in testAllEndpointsAction:"), error["message"]);
            callback(object{
                object::pair{std:("text"), std:("Error: ") + error["message"] + string_empty}
            });
            return false;
        }
    }
    }, 
    object::pair{std:("validate"), [=](auto _runtime, auto message) mutable
    {
        return message->content->text->includes(std:("BIRDEYE_TEST_ALL_ENDPOINTS"));
    }
    }, 
    object::pair{std:("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std:("user"), std:("user")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I want you to BIRDEYE_TEST_ALL_ENDPOINTS")}, 
            object::pair{std:("action"), std:("BIRDEYE_TEST_ALL_ENDPOINTS")}
        }}
    } } })}
});

void Main(void)
{
}

MAIN
