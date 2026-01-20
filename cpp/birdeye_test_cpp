#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/plugin-birdeye/__tests__/birdeye.test.h"

void Main(void)
{
    describe(std::string("BirdeyeService Tests"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        shared<std::shared_ptr<BirdeyeService>> birdeyeService;
        shared<> originalFetch;
        shared PROVIDER_CONFIG = object{
            object::pair{std::string("BIRDEYE_API_KEY"), process->env->BIRDEYE_API_KEY}, 
            object::pair{std::string("BIRDEYE_API"), std::string("https://public-api.birdeye.so")}, 
            object::pair{std::string("TOKEN_ADDRESSES"), object{
                object::pair{std::string("SOL"), std::string("So11111111111111111111111111111111111111112")}, 
                object::pair{std::string("BTC"), std::string("3NZ9JMVBmGAqocybic2c7LQCJScmgsAZ6vQqTDzcqmJh")}, 
                object::pair{std::string("ETH"), std::string("7vfCXTUXx5WJV5JADk17DUJ4ksgau7utNKj4b963voxs")}
            }}
        };
        beforeEach([=]() mutable
        {
            originalFetch = global->fetch;
            global->fetch = vi->fn();
            mockRuntime = as<std::shared_ptr<IAgentRuntime>>(as<any>(object{
                object::pair{std::string("getSetting"), vi->fn([=](auto key) mutable
                {
                    if (key == std::string("BIRDEYE_API_KEY")) {
                        return process->env->BIRDEYE_API_KEY;
                    }
                    return PROVIDER_CONFIG["BIRDEYE_API_KEY"];
                }
                )}, 
                object::pair{std::string("getCache"), vi->fn()->mockResolvedValue(nullptr)}, 
                object::pair{std::string("setCache"), vi->fn()->mockResolvedValue(undefined)}, 
                object::pair{std::string("getService"), vi->fn()}
            }));
            birdeyeService = std::async([=]() { BirdeyeService::start(mockRuntime); });
        }
        );
        afterEach([=]() mutable
        {
            global->fetch = originalFetch;
            vi->restoreAllMocks();
        }
        );
        shared mockSuccessfulResponse = [=](auto data) mutable
        {
            (as<std::shared_ptr<vi::Mock>>(fetch))->mockResolvedValueOnce(object{
                object::pair{std::string("ok"), true}, 
                object::pair{std::string("json"), [=]() mutable
                {
                    return Promise->resolve(data);
                }
                }
            });
        };
        shared mockFailedResponse = [=](auto status) mutable
        {
            (as<std::shared_ptr<vi::Mock>>(fetch))->mockRejectedValueOnce(std::make_shared<Error>(std::string("HTTP error! status: ") + status + string_empty));
        };
        shared getEmptyMarketData = [=]() mutable
        {
            return (object{
                object::pair{std::string("price"), 0}, 
                object::pair{std::string("marketCap"), 0}, 
                object::pair{std::string("liquidity"), 0}, 
                object::pair{std::string("volume24h"), 0}, 
                object::pair{std::string("priceHistory"), array<any>()}
            });
        };
        describe(std::string("Service Initialization"), [=]() mutable
        {
            it(std::string("should initialize with API key from .env"), [=]() mutable
            {
                expect(birdeyeService->apiKey)->toBe(process->env->BIRDEYE_API_KEY);
            }
            );
            it(std::string("should warn when no API key is provided"), [=]() mutable
            {
                auto loggerSpy = vi->spyOn(logger, std::string("warn"));
                auto runtimeWithoutKey = as<std::shared_ptr<IAgentRuntime>>(as<any>(utils::assign(object{
                    , 
                    object::pair{std::string("getSetting"), vi->fn()->mockReturnValue(undefined)}
                }, mockRuntime)));
                std::async([=]() { BirdeyeService::start(runtimeWithoutKey); });
                expect(loggerSpy)->toHaveBeenCalledWith(std::string("no BIRDEYE_API_KEY set"));
                loggerSpy->mockRestore();
            }
            );
        }
        );
        describe(std::string("Token Market Data"), [=]() mutable
        {
            it(std::string("should fetch token market data successfully"), [=]() mutable
            {
                auto mockData = object{
                    object::pair{std::string("data"), object{
                        object::pair{std::string("price"), 1.23}, 
                        object::pair{std::string("market_cap"), 1000000}, 
                        object::pair{std::string("liquidity"), 500000}
                    }}
                };
                mockSuccessfulResponse(mockData);
                mockSuccessfulResponse(object{
                    object::pair{std::string("data"), object{
                        object::pair{std::string("volumeUSD"), 100000}
                    }}
                });
                mockSuccessfulResponse(object{
                    object::pair{std::string("data"), object{
                        object::pair{std::string("items"), array<object>{ object{
                            object::pair{std::string("value"), 1.2}
                        }, object{
                            object::pair{std::string("value"), 1.3}
                        } }}
                    }}
                });
                auto result = std::async([=]() { birdeyeService->getTokenMarketData(PROVIDER_CONFIG["TOKEN_ADDRESSES"]["SOL"]); });
                expect(result)->toEqual(object{
                    object::pair{std::string("price"), 1.23}, 
                    object::pair{std::string("marketCap"), 1000000}, 
                    object::pair{std::string("liquidity"), 500000}, 
                    object::pair{std::string("volume24h"), 100000}, 
                    object::pair{std::string("priceHistory"), array<double>{ 1.2, 1.3 }}
                });
                expect(fetch)->toHaveBeenCalledWith(expect->stringContaining(string_empty + PROVIDER_CONFIG["BIRDEYE_API"] + std::string("/defi/v3/token/market-data")), expect->any(Object));
            }
            );
            it(std::string("should handle failed token market data request"), [=]() mutable
            {
                mockFailedResponse(500);
                mockFailedResponse(500);
                mockFailedResponse(500);
                auto result = std::async([=]() { birdeyeService->getTokenMarketData(PROVIDER_CONFIG["TOKEN_ADDRESSES"]["SOL"]); });
                expect(result)->toEqual(getEmptyMarketData());
            }
            );
        }
        );
        describe(std::string("Multiple Tokens Market Data"), [=]() mutable
        {
            it(std::string("should fetch multiple tokens market data"), [=]() mutable
            {
                auto mockData = object{
                    object::pair{std::string("data"), object{
                        object::pair{PROVIDER_CONFIG["TOKEN_ADDRESSES"]["SOL"], object{
                            object::pair{std::string("value"), 1.23}, 
                            object::pair{std::string("priceInNative"), 1}, 
                            object::pair{std::string("liquidity"), 1000000}, 
                            object::pair{std::string("priceChange24h"), 5}
                        }}
                    }}
                };
                mockSuccessfulResponse(mockData);
                auto result = std::async([=]() { birdeyeService->getTokensMarketData(array<string>{ PROVIDER_CONFIG["TOKEN_ADDRESSES"]["SOL"] }); });
                expect(result)->toHaveProperty(PROVIDER_CONFIG["TOKEN_ADDRESSES"]["SOL"]);
                expect(fetch)->toHaveBeenCalledWith(expect->stringContaining(string_empty + PROVIDER_CONFIG["BIRDEYE_API"] + std::string("/defi/multi_price")), expect->any(Object));
            }
            );
        }
        );
        describe(std::string("Token Lookup"), [=]() mutable
        {
            it(std::string("should lookup token from cache"), [=]() mutable
            {
                auto mockCachedToken = object{
                    object::pair{std::string("priceUsd"), 1.23}, 
                    object::pair{std::string("priceSol"), 1}, 
                    object::pair{std::string("liquidity"), 1000000}, 
                    object::pair{std::string("priceChange24h"), 5}
                };
                mockRuntime->getCache = vi->fn()->mockResolvedValue(mockCachedToken);
                auto result = std::async([=]() { birdeyeService->lookupToken(std::string("solana"), PROVIDER_CONFIG["TOKEN_ADDRESSES"]["SOL"]); });
                expect(result)->toEqual(mockCachedToken);
                expect(fetch)->not->toHaveBeenCalled();
            }
            );
            it(std::string("should lookup token from API when not in cache"), [=]() mutable
            {
                mockRuntime->getCache = vi->fn()->mockResolvedValue(nullptr);
                auto mockData = object{
                    object::pair{std::string("data"), object{
                        object::pair{PROVIDER_CONFIG["TOKEN_ADDRESSES"]["SOL"], object{
                            object::pair{std::string("value"), 1.23}, 
                            object::pair{std::string("priceInNative"), 1}, 
                            object::pair{std::string("liquidity"), 1000000}, 
                            object::pair{std::string("priceChange24h"), 5}
                        }}
                    }}
                };
                mockSuccessfulResponse(mockData);
                auto result = std::async([=]() { birdeyeService->lookupToken(std::string("solana"), PROVIDER_CONFIG["TOKEN_ADDRESSES"]["SOL"]); });
                expect(result)->toBeDefined();
                expect(fetch)->toHaveBeenCalled();
            }
            );
        }
        );
        describe(std::string("Service Lifecycle"), [=]() mutable
        {
            it(std::string("should stop service and clear interval"), [=]() mutable
            {
                auto service = std::async([=]() { BirdeyeService::start(mockRuntime); });
                service->refreshInterval = setInterval([=]() mutable
                {
                }
                , 1000);
                std::async([=]() { service->stop(); });
                expect(service->refreshInterval)->toBeNull();
            }
            );
            it(std::string("should handle stop when service not found"), [=]() mutable
            {
                mockRuntime->getService = vi->fn()->mockReturnValue(nullptr);
                auto loggerSpy = vi->spyOn(logger, std::string("error"));
                std::async([=]() { BirdeyeService::stop(mockRuntime); });
                expect(loggerSpy)->toHaveBeenCalledWith(std::string("Birdeye not found"));
                loggerSpy->mockRestore();
            }
            );
        }
        );
    }
    );
}

MAIN
