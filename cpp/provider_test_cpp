#include "/home/runner/work/elizaos-cpp/elizaos-cpp/plugin-specification/core-plugin-v1/src/__tests__/provider.test.h"

any mockRuntime = as<any>(object{
    object::pair{std::string("getSetting"), vi->fn()->mockReturnValue(std::string("test-setting"))}, 
    object::pair{std::string("logger"), object{
        object::pair{std::string("info"), vi->fn()}, 
        object::pair{std::string("error"), vi->fn()}
    }}
});
any mockMessage = as<any>(object{
    object::pair{std::string("id"), std::string("00000000-0000-0000-0000-000000000001")}, 
    object::pair{std::string("roomId"), std::string("00000000-0000-0000-0000-000000000002")}, 
    object::pair{std::string("content"), object{
        object::pair{std::string("text"), std::string("Test message")}
    }}
});

void Main(void)
{
    describe(std::string("Provider adapter"), [=]() mutable
    {
        it(std::string("should convert from v2 provider to v1 provider correctly"), [=]() mutable
        {
            auto mockResult = object{
                object::pair{std::string("text"), std::string("Provider result text")}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("key1"), std::string("value1")}, 
                    object::pair{std::string("key2"), std::string("value2")}
                }}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("dataKey"), std::string("dataValue")}
                }}
            };
            auto providerV2 = object{
                object::pair{std::string("name"), std::string("testProvider")}, 
                object::pair{std::string("description"), std::string("Test provider description")}, 
                object::pair{std::string("get"), vi->fn()->mockResolvedValue(mockResult)}
            };
            auto providerV1 = fromV2Provider(providerV2);
            auto result = std::async([=]() { providerV1->get(mockRuntime, mockMessage); });
            expect(result)->toEqual(std::string("Provider result text"));
            expect(providerV2["get"])->toHaveBeenCalledWith(mockRuntime, mockMessage, undefined);
            expect(providerV1->name)->toBe(std::string("testProvider"));
            expect(providerV1->description)->toBe(std::string("Test provider description"));
        }
        );
        it(std::string("should convert from v1 provider to v2 provider correctly"), [=]() mutable
        {
            auto mockResult = object{
                object::pair{std::string("text"), std::string("Provider result text")}, 
                object::pair{std::string("key1"), std::string("value1")}, 
                object::pair{std::string("key2"), std::string("value2")}
            };
            auto providerV1 = object{
                object::pair{std::string("name"), std::string("v1Provider")}, 
                object::pair{std::string("description"), std::string("V1 provider test")}, 
                object::pair{std::string("get"), vi->fn()->mockResolvedValue(mockResult)}
            };
            auto providerV2 = toV2Provider(providerV1);
            auto result = std::async([=]() { providerV2->get(mockRuntime, mockMessage, object{
                object::pair{std::string("text"), string_empty}, 
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}
            }); });
            expect(result)->toEqual(utils::assign(object{
                , 
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), std::string("Provider result text")}
            }, mockResult));
            expect(providerV1->get)->toHaveBeenCalledWith(mockRuntime, mockMessage, object{
                object::pair{std::string("text"), string_empty}, 
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}
            });
            expect(providerV2->name)->toBe(std::string("v1Provider"));
            expect(providerV2->description)->toBe(std::string("V1 provider test"));
        }
        );
        it(std::string("should handle unnamed v1 providers properly"), [=]() mutable
        {
            auto unnamedProvider = object{
                object::pair{std::string("get"), vi->fn()->mockResolvedValue(object{
                    object::pair{std::string("text"), std::string("result")}
                })}
            };
            auto providerV2 = toV2Provider(unnamedProvider);
            expect(providerV2->name)->toBe(std::string("unnamed-provider"));
            expect(providerV2->description)->toBeUndefined();
        }
        );
        it(std::string("should handle state conversion when passing to v2 provider"), [=]() mutable
        {
            auto v2State = object{
                object::pair{std::string("values"), object{
                    object::pair{std::string("userId"), std::string("00000000-0000-0000-0000-000000000003")}, 
                    object::pair{std::string("walletBalance"), 100}
                }}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}
            };
            auto mockState = fromV2State(v2State);
            auto mockV2Provider = object{
                object::pair{std::string("name"), std::string("stateTestProvider")}, 
                object::pair{std::string("get"), vi->fn()->mockResolvedValue(object{
                    object::pair{std::string("text"), std::string("result")}
                })}
            };
            auto v1Provider = fromV2Provider(mockV2Provider);
            std::async([=]() { v1Provider->get(mockRuntime, mockMessage, mockState); });
            expect(mockV2Provider["get"])->toHaveBeenCalledWith(expect->anything(), expect->anything(), expect->objectContaining(object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}, 
                object::pair{std::string("userId"), std::string("00000000-0000-0000-0000-000000000003")}, 
                object::pair{std::string("walletBalance"), 100}
            }));
        }
        );
        it(std::string("should handle real-world provider example (TON wallet provider)"), [=]() mutable
        {
            auto mockTonWalletProviderV1 = object{
                object::pair{std::string("name"), std::string("tonWalletProvider")}, 
                object::pair{std::string("get"), vi->fn()->mockResolvedValue(object{
                    object::pair{std::string("text"), std::string("You have 10.5 TON in your wallet.")}, 
                    object::pair{std::string("walletAddress"), std::string("0x123abc")}, 
                    object::pair{std::string("walletBalance"), 10.5}, 
                    object::pair{std::string("tokenPrices"), object{
                        object::pair{std::string("TON"), 5.25}
                    }}
                })}
            };
            auto tonWalletProviderV2 = toV2Provider(mockTonWalletProviderV1);
            auto result = std::async([=]() { tonWalletProviderV2->get(mockRuntime, mockMessage, object{
                object::pair{std::string("text"), std::string("Check my wallet")}, 
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}
            }); });
            expect(result)->toEqual(object{
                object::pair{std::string("walletAddress"), std::string("0x123abc")}, 
                object::pair{std::string("walletBalance"), 10.5}, 
                object::pair{std::string("tokenPrices"), object{
                    object::pair{std::string("TON"), 5.25}
                }}, 
                object::pair{std::string("text"), std::string("You have 10.5 TON in your wallet.")}, 
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}
            });
            auto tonWalletProviderV1Again = fromV2Provider(tonWalletProviderV2);
            auto resultV1 = std::async([=]() { tonWalletProviderV1Again->get(mockRuntime, mockMessage); });
            expect(resultV1)->toEqual(std::string("You have 10.5 TON in your wallet."));
        }
        );
        it(std::string("should handle primitive results from V1 providers"), [=]() mutable
        {
            auto stringProvider = object{
                object::pair{std::string("name"), std::string("stringProvider")}, 
                object::pair{std::string("get"), vi->fn()->mockResolvedValue(std::string("Just a string result"))}
            };
            auto numberProvider = object{
                object::pair{std::string("name"), std::string("numberProvider")}, 
                object::pair{std::string("get"), vi->fn()->mockResolvedValue(42)}
            };
            auto stringProviderV2 = toV2Provider(stringProvider);
            auto numberProviderV2 = toV2Provider(numberProvider);
            auto stringResult = std::async([=]() { stringProviderV2->get(mockRuntime, mockMessage, object{}); });
            auto numberResult = std::async([=]() { numberProviderV2->get(mockRuntime, mockMessage, object{}); });
            expect(stringResult)->toEqual(object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), std::string("Just a string result")}
            });
            expect(numberResult)->toEqual(object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), std::string("42")}
            });
        }
        );
        it(std::string("should handle null or undefined results"), [=]() mutable
        {
            auto nullProvider = object{
                object::pair{std::string("name"), std::string("nullProvider")}, 
                object::pair{std::string("get"), vi->fn()->mockResolvedValue(nullptr)}
            };
            auto undefinedProvider = object{
                object::pair{std::string("name"), std::string("undefinedProvider")}, 
                object::pair{std::string("get"), vi->fn()->mockResolvedValue(undefined)}
            };
            auto nullProviderV2 = toV2Provider(nullProvider);
            auto undefinedProviderV2 = toV2Provider(undefinedProvider);
            auto nullResult = std::async([=]() { nullProviderV2->get(mockRuntime, mockMessage, object{}); });
            auto undefinedResult = std::async([=]() { undefinedProviderV2->get(mockRuntime, mockMessage, object{}); });
            expect(nullResult)->toEqual(object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}
            });
            expect(undefinedResult)->toEqual(object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}
            });
        }
        );
    }
    );
}

MAIN
