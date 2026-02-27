#include "provider.test.h"

any mockRuntime = as<any>(object{
    object::pair{std:("getSetting"), vi->fn()->mockReturnValue(std:("test-setting"))}, 
    object::pair{std:("logger"), object{
        object::pair{std:("info"), vi->fn()}, 
        object::pair{std:("error"), vi->fn()}
    }}
});
any mockMessage = as<any>(object{
    object::pair{std:("id"), std:("00000000-0000-0000-0000-000000000001")}, 
    object::pair{std:("roomId"), std:("00000000-0000-0000-0000-000000000002")}, 
    object::pair{std:("content"), object{
        object::pair{std:("text"), std:("Test message")}
    }}
});

void Main(void)
{
    describe(std:("Provider adapter"), [=]() mutable
    {
        it(std:("should convert from v2 provider to v1 provider correctly"), [=]() mutable
        {
            auto mockResult = object{
                object::pair{std:("text"), std:("Provider result text")}, 
                object::pair{std:("values"), object{
                    object::pair{std:("key1"), std:("value1")}, 
                    object::pair{std:("key2"), std:("value2")}
                }}, 
                object::pair{std:("data"), object{
                    object::pair{std:("dataKey"), std:("dataValue")}
                }}
            };
            auto providerV2 = object{
                object::pair{std:("name"), std:("testProvider")}, 
                object::pair{std:("description"), std:("Test provider description")}, 
                object::pair{std:("get"), vi->fn()->mockResolvedValue(mockResult)}
            };
            auto providerV1 = fromV2Provider(providerV2);
            auto result = std::async([=]() { providerV1->get(mockRuntime, mockMessage); });
            expect(result)->toEqual(std:("Provider result text"));
            expect(providerV2["get"])->toHaveBeenCalledWith(mockRuntime, mockMessage, undefined);
            expect(providerV1->name)->toBe(std:("testProvider"));
            expect(providerV1->description)->toBe(std:("Test provider description"));
        }
        );
        it(std:("should convert from v1 provider to v2 provider correctly"), [=]() mutable
        {
            auto mockResult = object{
                object::pair{std:("text"), std:("Provider result text")}, 
                object::pair{std:("key1"), std:("value1")}, 
                object::pair{std:("key2"), std:("value2")}
            };
            auto providerV1 = object{
                object::pair{std:("name"), std:("v1Provider")}, 
                object::pair{std:("description"), std:("V1 provider test")}, 
                object::pair{std:("get"), vi->fn()->mockResolvedValue(mockResult)}
            };
            auto providerV2 = toV2Provider(providerV1);
            auto result = std::async([=]() { providerV2->get(mockRuntime, mockMessage, object{
                object::pair{std:("text"), string_empty}, 
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}
            }); });
            expect(result)->toEqual(utils::assign(object{
                , 
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), std:("Provider result text")}
            }, mockResult));
            expect(providerV1->get)->toHaveBeenCalledWith(mockRuntime, mockMessage, object{
                object::pair{std:("text"), string_empty}, 
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}
            });
            expect(providerV2->name)->toBe(std:("v1Provider"));
            expect(providerV2->description)->toBe(std:("V1 provider test"));
        }
        );
        it(std:("should handle unnamed v1 providers properly"), [=]() mutable
        {
            auto unnamedProvider = object{
                object::pair{std:("get"), vi->fn()->mockResolvedValue(object{
                    object::pair{std:("text"), std:("result")}
                })}
            };
            auto providerV2 = toV2Provider(unnamedProvider);
            expect(providerV2->name)->toBe(std:("unnamed-provider"));
            expect(providerV2->description)->toBeUndefined();
        }
        );
        it(std:("should handle state conversion when passing to v2 provider"), [=]() mutable
        {
            auto v2State = object{
                object::pair{std:("values"), object{
                    object::pair{std:("userId"), std:("00000000-0000-0000-0000-000000000003")}, 
                    object::pair{std:("walletBalance"), 100}
                }}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
            auto mockState = fromV2State(v2State);
            auto mockV2Provider = object{
                object::pair{std:("name"), std:("stateTestProvider")}, 
                object::pair{std:("get"), vi->fn()->mockResolvedValue(object{
                    object::pair{std:("text"), std:("result")}
                })}
            };
            auto v1Provider = fromV2Provider(mockV2Provider);
            std::async([=]() { v1Provider->get(mockRuntime, mockMessage, mockState); });
            expect(mockV2Provider["get"])->toHaveBeenCalledWith(expect->anything(), expect->anything(), expect->objectContaining(object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}, 
                object::pair{std:("userId"), std:("00000000-0000-0000-0000-000000000003")}, 
                object::pair{std:("walletBalance"), 100}
            }));
        }
        );
        it(std:("should handle real-world provider example (TON wallet provider)"), [=]() mutable
        {
            auto mockTonWalletProviderV1 = object{
                object::pair{std:("name"), std:("tonWalletProvider")}, 
                object::pair{std:("get"), vi->fn()->mockResolvedValue(object{
                    object::pair{std:("text"), std:("You have 10.5 TON in your wallet.")}, 
                    object::pair{std:("walletAddress"), std:("0x123abc")}, 
                    object::pair{std:("walletBalance"), 10.5}, 
                    object::pair{std:("tokenPrices"), object{
                        object::pair{std:("TON"), 5.25}
                    }}
                })}
            };
            auto tonWalletProviderV2 = toV2Provider(mockTonWalletProviderV1);
            auto result = std::async([=]() { tonWalletProviderV2->get(mockRuntime, mockMessage, object{
                object::pair{std:("text"), std:("Check my wallet")}, 
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}
            }); });
            expect(result)->toEqual(object{
                object::pair{std:("walletAddress"), std:("0x123abc")}, 
                object::pair{std:("walletBalance"), 10.5}, 
                object::pair{std:("tokenPrices"), object{
                    object::pair{std:("TON"), 5.25}
                }}, 
                object::pair{std:("text"), std:("You have 10.5 TON in your wallet.")}, 
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}
            });
            auto tonWalletProviderV1Again = fromV2Provider(tonWalletProviderV2);
            auto resultV1 = std::async([=]() { tonWalletProviderV1Again->get(mockRuntime, mockMessage); });
            expect(resultV1)->toEqual(std:("You have 10.5 TON in your wallet."));
        }
        );
        it(std:("should handle primitive results from V1 providers"), [=]() mutable
        {
            auto stringProvider = object{
                object::pair{std:("name"), std:("stringProvider")}, 
                object::pair{std:("get"), vi->fn()->mockResolvedValue(std:("Just a string result"))}
            };
            auto numberProvider = object{
                object::pair{std:("name"), std:("numberProvider")}, 
                object::pair{std:("get"), vi->fn()->mockResolvedValue(42)}
            };
            auto stringProviderV2 = toV2Provider(stringProvider);
            auto numberProviderV2 = toV2Provider(numberProvider);
            auto stringResult = std::async([=]() { stringProviderV2->get(mockRuntime, mockMessage, object{}); });
            auto numberResult = std::async([=]() { numberProviderV2->get(mockRuntime, mockMessage, object{}); });
            expect(stringResult)->toEqual(object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), std:("Just a string result")}
            });
            expect(numberResult)->toEqual(object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), std:("42")}
            });
        }
        );
        it(std:("should handle null or undefined results"), [=]() mutable
        {
            auto nullProvider = object{
                object::pair{std:("name"), std:("nullProvider")}, 
                object::pair{std:("get"), vi->fn()->mockResolvedValue(nullptr)}
            };
            auto undefinedProvider = object{
                object::pair{std:("name"), std:("undefinedProvider")}, 
                object::pair{std:("get"), vi->fn()->mockResolvedValue(undefined)}
            };
            auto nullProviderV2 = toV2Provider(nullProvider);
            auto undefinedProviderV2 = toV2Provider(undefinedProvider);
            auto nullResult = std::async([=]() { nullProviderV2->get(mockRuntime, mockMessage, object{}); });
            auto undefinedResult = std::async([=]() { undefinedProviderV2->get(mockRuntime, mockMessage, object{}); });
            expect(nullResult)->toEqual(object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            });
            expect(undefinedResult)->toEqual(object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            });
        }
        );
    }
    );
}

MAIN
