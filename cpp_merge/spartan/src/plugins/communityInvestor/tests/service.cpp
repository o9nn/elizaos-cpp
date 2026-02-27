#include "service.hpp"

any testUserIdGlobalForService = asUUID(uuidv4());
std::function<any(any, any, any)> createFullMockComponentForSvcTest = [=](auto userId, auto profileData, auto runtime) mutable
{
    return (object{
        object::pair{std:("id"), asUUID(uuidv4())}, 
        object::pair{std:("entityId"), userId}, 
        object::pair{std:("agentId"), runtime->agentId}, 
        object::pair{std:("worldId"), runtime->agentId}, 
        object::pair{std:("roomId"), asUUID(uuidv4())}, 
        object::pair{std:("sourceEntityId"), runtime->agentId}, 
        object::pair{std:("type"), TRUST_MARKETPLACE_COMPONENT_TYPE}, 
        object::pair{std:("createdAt"), Date->now()}, 
        object::pair{std:("data"), profileData}
    });
};
std::function<any(string, double, any, any, any, double, any)> createMockRecForSvcTest = [=](auto id, auto timestamp, P2 type, auto conviction, auto userIdToSet, auto priceAtRec = undefined, auto metric = undefined) mutable
{
    return (object{
        object::pair{std:("id"), asUUID(uuidv4())}, 
        object::pair{std:("userId"), userIdToSet}, 
        object::pair{std:("messageId"), asUUID(uuidv4())}, 
        object::pair{std:("timestamp"), std:("timestamp")}, 
        object::pair{std:("tokenAddress"), std:("TEST_TOKEN_") + id + string_empty}, 
        object::pair{std:("chain"), SupportedChain->SOLANA}, 
        object::pair{std:("recommendationType"), type}, 
        object::pair{std:("conviction"), std:("conviction")}, 
        object::pair{std:("rawMessageQuote"), std:("This is a quote for ") + id + string_empty}, 
        object::pair{std:("priceAtRecommendation"), priceAtRec}, 
        object::pair{std:("metrics"), metric}, 
        object::pair{std:("processedForTradeDecision"), false}
    });
};
array<std::shared_ptr<TestCase>> calculateUserTrustScoreTestCases = array<std::shared_ptr<TestCase>>{ object{
    object::pair{std:("name"), std:("Service.calculateUserTrustScore: New user, score 0")}, 
    object::pair{std:("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        shared<any> createdComp;
        auto originalGetComponent = runtime->getComponent;
        auto originalCreateComponent = runtime->createComponent;
        {
            utils::finally __finally2937_3059([&]() mutable
            {
                runtime->getComponent = originalGetComponent;
                runtime->createComponent = originalCreateComponent;
            });
            try
            {
                runtime->getComponent = [=]() mutable
                {
                    return nullptr;
                };
                runtime->createComponent = [=](auto comp) mutable
                {
                    createdComp = comp;
                    return true;
                };
                std::async([=]() { service->calculateUserTrustScore(testUserIdGlobalForService, runtime); });
                if (!createdComp) throw any(std::make_shared<Error>(std:("createComponent was not called or did not set createdComp")));
                auto data = as<std::shared_ptr<UserTrustProfile>>(createdComp["data"]);
                if (OR((data->trustScore != 0), (data->recommendations->length != 0))) {
                    throw any(std::make_shared<Error>(std:("New user score expected 0, got ") + data->trustScore + string_empty));
                }
                logger->info(std:("Service.calculateUserTrustScore: New user score 0 - Passed"));
            }
            catch (...)
            {
                throw;
            }
        }
    }
    }
}, object{
    object::pair{std:("name"), std:("Service.calculateUserTrustScore: Single positive BUY recommendation")}, 
    object::pair{std:("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        shared recTimestamp = Date->now() - 10 * 24 * 60 * 60 * 1000;
        auto mockRec = createMockRecForSvcTest(std:("rec1"), recTimestamp, std:("BUY"), Conviction->HIGH, testUserIdGlobalForService, 10, object{
            object::pair{std:("evaluationTimestamp"), Date->now()}, 
            object::pair{std:("potentialProfitPercent"), 20}, 
            object::pair{std:("isScamOrRug"), false}
        });
        auto initialProfile = object{
            object::pair{std:("version"), std:("1.0.0")}, 
            object::pair{std:("userId"), testUserIdGlobalForService}, 
            object::pair{std:("trustScore"), 0}, 
            object::pair{std:("lastTrustScoreCalculationTimestamp"), Date->now() - 2 * 24 * 60 * 60 * 1000}, 
            object::pair{std:("recommendations"), array<any>{ mockRec }}
        };
        shared mockComp = createFullMockComponentForSvcTest(testUserIdGlobalForService, initialProfile, runtime);
        shared<any> updatedComp;
        auto originalGetComponent = runtime->getComponent;
        auto originalUpdateComponent = runtime->updateComponent;
        auto originalServiceGetTokenAPIData = (as<any>(service))["getTokenAPIData"]["bind"](service);
        auto originalServiceIsLikelyScamOrRug = (as<any>(service))["isLikelyScamOrRug"]["bind"](service);
        auto originalServiceEvalPerf = (as<any>(service))["evaluateRecommendationPerformance"]["bind"](service);
        {
            utils::finally __finally5730_6407([&]() mutable
            {
                runtime->getComponent = originalGetComponent;
                runtime->updateComponent = originalUpdateComponent;
                if (originalServiceGetTokenAPIData) (as<any>(service))["getTokenAPIData"] = originalServiceGetTokenAPIData; else (as<any>(service)).Delete("getTokenAPIData");
                if (originalServiceIsLikelyScamOrRug) (as<any>(service))["isLikelyScamOrRug"] = originalServiceIsLikelyScamOrRug; else (as<any>(service)).Delete("isLikelyScamOrRug");
                if (originalServiceEvalPerf) (as<any>(service))["evaluateRecommendationPerformance"] = originalServiceEvalPerf; else (as<any>(service)).Delete("evaluateRecommendationPerformance");
            });
            try
            {
                runtime->getComponent = [=]() mutable
                {
                    return mockComp;
                };
                runtime->updateComponent = [=](auto comp) mutable
                {
                    updatedComp = comp;
                };
                (as<any>(service))["getTokenAPIData"] = [=]() mutable
                {
                    return as<std::shared_ptr<TokenAPIData>>((object{
                        object::pair{std:("currentPrice"), 12}, 
                        object::pair{std:("priceHistory"), array<object>{ object{
                            object::pair{std:("timestamp"), recTimestamp}, 
                            object::pair{std:("price"), 10}
                        }, object{
                            object::pair{std:("timestamp"), Date->now()}, 
                            object::pair{std:("price"), 12}
                        } }}
                    }));
                };
                (as<any>(service))["isLikelyScamOrRug"] = [=]() mutable
                {
                    return false;
                };
                (as<any>(service))["evaluateRecommendationPerformance"] = [=]() mutable
                {
                    return (object{
                        object::pair{std:("evaluationTimestamp"), Date->now()}, 
                        object::pair{std:("potentialProfitPercent"), 20}, 
                        object::pair{std:("isScamOrRug"), false}
                    });
                };
                std::async([=]() { service->calculateUserTrustScore(testUserIdGlobalForService, runtime); });
                if (!updatedComp) throw any(std::make_shared<Error>(std:("updateComponent not called")));
                auto updatedData = as<std::shared_ptr<UserTrustProfile>>(updatedComp["data"]);
                if (Math->abs(updatedData->trustScore - 20) > 0.01) throw any(std::make_shared<Error>(std:("Expected score ~20.0, got ") + updatedData->trustScore + string_empty));
                logger->info(std:("Service.calculateUserTrustScore: Single positive BUY - Passed"));
            }
            catch (...)
            {
                throw;
            }
        }
    }
    }
}, object{
    object::pair{std:("name"), std:("Service.calculateUserTrustScore: Score clamping at +100")}, 
    object::pair{std:("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        auto recTimestamp = Date->now();
        auto recs = array<any>{ createMockRecForSvcTest(std:("superGood"), recTimestamp, std:("BUY"), Conviction->HIGH, testUserIdGlobalForService, 10, object{
            object::pair{std:("potentialProfitPercent"), 500}, 
            object::pair{std:("evaluationTimestamp"), recTimestamp}
        }) };
        auto initialProfile = object{
            object::pair{std:("version"), std:("1.0.0")}, 
            object::pair{std:("userId"), testUserIdGlobalForService}, 
            object::pair{std:("trustScore"), 0}, 
            object::pair{std:("lastTrustScoreCalculationTimestamp"), 0}, 
            object::pair{std:("recommendations"), recs}
        };
        shared mockComponent = createFullMockComponentForSvcTest(testUserIdGlobalForService, initialProfile, runtime);
        shared<any> updatedComp;
        auto originalGetComponent = runtime->getComponent;
        auto originalUpdateComponent = runtime->updateComponent;
        auto originalServiceEvalPerf = (as<any>(service))["evaluateRecommendationPerformance"]["bind"](service);
        auto originalServiceGetTokenAPIData = (as<any>(service))["getTokenAPIData"]["bind"](service);
        {
            utils::finally __finally8675_9169([&]() mutable
            {
                runtime->getComponent = originalGetComponent;
                runtime->updateComponent = originalUpdateComponent;
                if (originalServiceEvalPerf) (as<any>(service))["evaluateRecommendationPerformance"] = originalServiceEvalPerf; else (as<any>(service)).Delete("evaluateRecommendationPerformance");
                if (originalServiceGetTokenAPIData) (as<any>(service))["getTokenAPIData"] = originalServiceGetTokenAPIData; else (as<any>(service)).Delete("getTokenAPIData");
            });
            try
            {
                runtime->getComponent = [=]() mutable
                {
                    return mockComponent;
                };
                runtime->updateComponent = [=](auto comp) mutable
                {
                    updatedComp = comp;
                };
                (as<any>(service))["evaluateRecommendationPerformance"] = [=]() mutable
                {
                    return (object{
                        object::pair{std:("evaluationTimestamp"), Date->now()}, 
                        object::pair{std:("potentialProfitPercent"), 500}, 
                        object::pair{std:("isScamOrRug"), false}
                    });
                };
                (as<any>(service))["getTokenAPIData"] = [=]() mutable
                {
                    return as<std::shared_ptr<TokenAPIData>>((object{
                        object::pair{std:("currentPrice"), 60}
                    }));
                };
                std::async([=]() { service->calculateUserTrustScore(testUserIdGlobalForService, runtime); });
                if (!updatedComp) throw any(std::make_shared<Error>(std:("updateComponent not called")));
                auto updatedData = as<std::shared_ptr<UserTrustProfile>>(updatedComp["data"]);
                if (Math->abs(updatedData->trustScore - 100) > 0.01) throw any(std::make_shared<Error>(std:("Expected score clamped at 100, got ") + updatedData->trustScore + string_empty));
                logger->info(std:("Service.calculateUserTrustScore: Clamping at +100 - Passed"));
            }
            catch (...)
            {
                throw;
            }
        }
    }
    }
}, object{
    object::pair{std:("name"), std:("Service.calculateUserTrustScore: Recommendation with undefined metrics triggers re-evaluation")}, 
    object::pair{std:("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        shared recTimestamp = Date->now();
        auto recs = array<any>{ createMockRecForSvcTest(std:("needsEval"), recTimestamp, std:("BUY"), Conviction->MEDIUM, testUserIdGlobalForService, 10, undefined) };
        auto initialProfile = object{
            object::pair{std:("version"), std:("1.0.0")}, 
            object::pair{std:("userId"), testUserIdGlobalForService}, 
            object::pair{std:("trustScore"), 0}, 
            object::pair{std:("lastTrustScoreCalculationTimestamp"), 0}, 
            object::pair{std:("recommendations"), recs}
        };
        shared mockComponent = createFullMockComponentForSvcTest(testUserIdGlobalForService, initialProfile, runtime);
        shared<any> updatedComp;
        shared getTokenAPIDataCalled = false;
        shared evaluatePerformanceCalled = false;
        auto originalGetComponent = runtime->getComponent;
        auto originalUpdateComponent = runtime->updateComponent;
        auto originalServiceGetTokenAPIData = (as<any>(service))["getTokenAPIData"]["bind"](service);
        auto originalServiceEvalPerf = (as<any>(service))["evaluateRecommendationPerformance"]["bind"](service);
        {
            utils::finally __finally12129_12623([&]() mutable
            {
                runtime->getComponent = originalGetComponent;
                runtime->updateComponent = originalUpdateComponent;
                if (originalServiceGetTokenAPIData) (as<any>(service))["getTokenAPIData"] = originalServiceGetTokenAPIData; else (as<any>(service)).Delete("getTokenAPIData");
                if (originalServiceEvalPerf) (as<any>(service))["evaluateRecommendationPerformance"] = originalServiceEvalPerf; else (as<any>(service)).Delete("evaluateRecommendationPerformance");
            });
            try
            {
                runtime->getComponent = [=]() mutable
                {
                    return mockComponent;
                };
                runtime->updateComponent = [=](auto comp) mutable
                {
                    updatedComp = comp;
                };
                (as<any>(service))["getTokenAPIData"] = [=]() mutable
                {
                    getTokenAPIDataCalled = true;
                    return as<std::shared_ptr<TokenAPIData>>(object{
                        object::pair{std:("currentPrice"), 12}, 
                        object::pair{std:("priceHistory"), array<object>{ object{
                            object::pair{std:("timestamp"), recTimestamp}, 
                            object::pair{std:("price"), 10}
                        }, object{
                            object::pair{std:("timestamp"), Date->now()}, 
                            object::pair{std:("price"), 12}
                        } }}
                    });
                };
                (as<any>(service))["evaluateRecommendationPerformance"] = [=]() mutable
                {
                    evaluatePerformanceCalled = true;
                    return as<std::shared_ptr<RecommendationMetric>>(object{
                        object::pair{std:("evaluationTimestamp"), Date->now()}, 
                        object::pair{std:("potentialProfitPercent"), 20}, 
                        object::pair{std:("isScamOrRug"), false}
                    });
                };
                std::async([=]() { service->calculateUserTrustScore(testUserIdGlobalForService, runtime); });
                if (!updatedComp) throw any(std::make_shared<Error>(std:("updateComponent not called")));
                if (!getTokenAPIDataCalled) throw any(std::make_shared<Error>(std:("getTokenAPIData was not called for metric re-evaluation")));
                if (!evaluatePerformanceCalled) throw any(std::make_shared<Error>(std:("evaluateRecommendationPerformance was not called for metric re-evaluation")));
                auto updatedData = as<std::shared_ptr<UserTrustProfile>>(updatedComp["data"]);
                if (Math->abs(updatedData->trustScore - 20) > 0.01) throw any(std::make_shared<Error>(std:("Expected score ~20.0 after re-eval, got ") + updatedData->trustScore + string_empty));
                logger->info(std:("Service.calculateUserTrustScore: Undefined metrics re-evaluation - Passed"));
            }
            catch (...)
            {
                throw;
            }
        }
    }
    }
} };
array<std::shared_ptr<TestCase>> resolveTickerTestCases = array<std::shared_ptr<TestCase>>{ object{
    object::pair{std:("name"), std:("Service.resolveTicker: Known SOL ticker ($SOL)")}, 
    object::pair{std:("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        auto result = std::async([=]() { service->resolveTicker(std:("$SOL"), SupportedChain->SOLANA, array<any>()); });
        if (OR((result["address"] != std:("So11111111111111111111111111111111111111112")), (result["ticker"] != std:("SOL")))) {
            throw any(std::make_shared<Error>(std:("Unexpected result for $SOL: ") + JSON->stringify(result) + string_empty));
        }
        logger->info(std:("Service.resolveTicker: Known SOL - Passed"));
    }
    }
}, object{
    object::pair{std:("name"), std:("Service.resolveTicker: Unknown ticker, DexScreener finds it (SOL)")}, 
    object::pair{std:("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        auto originalDexscreenerClient = (as<any>(service))["dexscreenerClient"];
        {
            utils::finally __finally14537_14619([&]() mutable
            {
                (as<any>(service))["dexscreenerClient"] = originalDexscreenerClient;
            });
            try
            {
                (as<any>(service))["dexscreenerClient"] = object{
                    object::pair{std:("search"), [=](auto query) mutable
                    {
                        return (object{
                            object::pair{std:("pairs"), array<object>{ object{
                                object::pair{std:("baseToken"), object{
                                    object::pair{std:("address"), std:("DEX_FOUND_ADDR_SOL")}, 
                                    object::pair{std:("symbol"), query->replace(std:("$"), string_empty)}
                                }}, 
                                object::pair{std:("chainId"), std:("solana")}, 
                                object::pair{std:("liquidity"), object{
                                    object::pair{std:("usd"), 10000}
                                }}
                            } }}
                        });
                    }
                    }, 
                    object::pair{std:("searchForHighestLiquidityPair"), [=](auto query) mutable
                    {
                        return (object{
                            object::pair{std:("baseToken"), object{
                                object::pair{std:("address"), std:("DEX_FOUND_ADDR_SOL")}, 
                                object::pair{std:("symbol"), query->replace(std:("$"), string_empty)}
                            }}, 
                            object::pair{std:("chainId"), std:("solana")}, 
                            object::pair{std:("liquidity"), object{
                                object::pair{std:("usd"), 10000}
                            }}
                        });
                    }
                    }
                };
                auto result = std::async([=]() { service->resolveTicker(std:("$NEWCOINSOL"), SupportedChain->SOLANA, array<any>()); });
                if (OR((result["address"] != std:("DEX_FOUND_ADDR_SOL")), (result["ticker"] != std:("NEWCOINSOL")))) {
                    throw any(std::make_shared<Error>(std:("Unexpected result from DexScreener for SOL: ") + JSON->stringify(result) + string_empty));
                }
                logger->info(std:("Service.resolveTicker: DexScreener SOL find - Passed"));
            }
            catch (...)
            {
                throw;
            }
        }
    }
    }
}, object{
    object::pair{std:("name"), std:("Service.resolveTicker: Context message provides address for SOL")}, 
    object::pair{std:("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        auto testSolanaAddressInContext = std:("TESTCTXADRESSSOLANAbe891z3456789012345");
        auto contextMessages = array<std::shared_ptr<Memory>>{ as<std::shared_ptr<Memory>>(object{
            object::pair{std:("entityId"), asUUID(uuidv4())}, 
            object::pair{std:("roomId"), asUUID(uuidv4())}, 
            object::pair{std:("agentId"), runtime->agentId}, 
            object::pair{std:("content"), object{
                object::pair{std:("text"), std:("I heard $CTXTOKEN (") + testSolanaAddressInContext + std:(") is good.")}
            }}
        }) };
        auto result = std::async([=]() { service->resolveTicker(std:("$CTXTOKEN"), SupportedChain->SOLANA, contextMessages); });
        if (OR((result["address"] != testSolanaAddressInContext), (result["ticker"] != std:("CTXTOKEN")))) {
            throw any(std::make_shared<Error>(std:("Context resolution failed for $CTXTOKEN: expected ") + testSolanaAddressInContext + std:(", got ") + JSON->stringify(result) + string_empty));
        }
        logger->info(std:("Service.resolveTicker: Context message SOL - Passed"));
    }
    }
}, object{
    object::pair{std:("name"), std:("Service.resolveTicker: Returns null for unresolvable ticker")}, 
    object::pair{std:("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        auto originalDexscreenerClient = (as<any>(service))["dexscreenerClient"];
        {
            utils::finally __finally16342_16424([&]() mutable
            {
                (as<any>(service))["dexscreenerClient"] = originalDexscreenerClient;
            });
            try
            {
                (as<any>(service))["dexscreenerClient"] = object{
                    object::pair{std:("search"), [=]() mutable
                    {
                        return (object{
                            object::pair{std:("pairs"), array<any>()}
                        });
                    }
                    }
                };
                auto result = std::async([=]() { service->resolveTicker(std:("$NONEXISTENT"), SupportedChain->SOLANA, array<any>()); });
                if (result != nullptr) throw any(std::make_shared<Error>(std:("Expected null for unresolvable ticker, got ") + JSON->stringify(result) + string_empty));
                logger->info(std:("Service.resolveTicker: Unresolvable returns null - Passed"));
            }
            catch (...)
            {
                throw;
            }
        }
    }
    }
} };
array<std::shared_ptr<TestCase>> getTokenAPIDataTestCases = array<std::shared_ptr<TestCase>>{ object{
    object::pair{std:("name"), std:("Service.getTokenAPIData: Fetches SOL token data successfully")}, 
    object::pair{std:("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        auto tokenAddress = std:("TESTSOLADDR");
        auto originalBirdeyeClient = (as<any>(service))["birdeyeClient"];
        auto originalDexscreenerClient = (as<any>(service))["dexscreenerClient"];
        {
            utils::finally __finally18401_18547([&]() mutable
            {
                (as<any>(service))["birdeyeClient"] = originalBirdeyeClient;
                (as<any>(service))["dexscreenerClient"] = originalDexscreenerClient;
            });
            try
            {
                (as<any>(service))["birdeyeClient"] = object{
                    object::pair{std:("fetchTokenOverview"), [=]() mutable
                    {
                        return (object{
                            object::pair{std:("name"), std:("BirdEyeCoin")}, 
                            object::pair{std:("symbol"), std:("BEC")}, 
                            object::pair{std:("decimals"), 9}
                        });
                    }
                    }, 
                    object::pair{std:("fetchPrice"), [=]() mutable
                    {
                        return 15;
                    }
                    }, 
                    object::pair{std:("fetchTokenTradeData"), [=]() mutable
                    {
                        return as<std::shared_ptr<TokenTradeData>>(as<any>((object{
                            object::pair{std:("price"), 15}, 
                            object::pair{std:("history_24h_price"), 14}, 
                            object::pair{std:("volume_24h_usd"), 10000}, 
                            object::pair{std:("market"), 500000}, 
                            object::pair{std:("price_change_24h_percent"), 5}, 
                            object::pair{std:("unique_wallet_24h_change_percent"), 1}, 
                            object::pair{std:("volume_24h"), 1000}
                        })));
                    }
                    }, 
                    object::pair{std:("fetchTokenSecurity"), [=]() mutable
                    {
                        return as<any>((object{
                            object::pair{std:("top10HolderPercent"), 10}
                        }));
                    }
                    }
                };
                (as<any>(service))["dexscreenerClient"] = object{
                    object::pair{std:("search"), [=]() mutable
                    {
                        return (object{
                            object::pair{std:("schemaVersion"), std:("1.0.0")}, 
                            object::pair{std:("pairs"), array<object>{ object{
                                object::pair{std:("baseToken"), object{
                                    object::pair{std:("name"), std:("DexCoin")}, 
                                    object::pair{std:("symbol"), std:("DXC")}
                                }}, 
                                object::pair{std:("priceUsd"), std:("15.5")}, 
                                object::pair{std:("liquidity"), object{
                                    object::pair{std:("usd"), 50000}
                                }}, 
                                object::pair{std:("marketCap"), 500000}, 
                                object::pair{std:("chainId"), std:("solana")}
                            } }}
                        });
                    }
                    }
                };
                auto data = std::async([=]() { service->getTokenAPIData(tokenAddress, SupportedChain->SOLANA); });
                if (OR((OR((OR((!data), (data["name"] != std:("BirdEyeCoin")))), (data["currentPrice"] != 15))), (data["liquidity"] != 50000))) {
                    throw any(std::make_shared<Error>(std:("getTokenAPIData failed for SOL: ") + JSON->stringify(data) + string_empty));
                }
                logger->info(std:("Service.getTokenAPIData: SOL token data success - Passed"));
            }
            catch (...)
            {
                throw;
            }
        }
    }
    }
}, object{
    object::pair{std:("name"), std:("Service.getTokenAPIData: Returns null if all SOL API calls fail and DexScreener has no pair")}, 
    object::pair{std:("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        auto originalBirdeyeClient = (as<any>(service))["birdeyeClient"];
        auto originalDexscreenerClient = (as<any>(service))["dexscreenerClient"];
        auto originalLoggerError = logger->error;
        auto originalLoggerWarn = logger->warn;
        {
            utils::finally __finally20214_20446([&]() mutable
            {
                (as<any>(service))["birdeyeClient"] = originalBirdeyeClient;
                (as<any>(service))["dexscreenerClient"] = originalDexscreenerClient;
                logger->error = originalLoggerError;
                logger->warn = originalLoggerWarn;
            });
            try
            {
                logger->error = [=]() mutable
                {
                };
                logger->warn = [=]() mutable
                {
                };
                (as<any>(service))["birdeyeClient"] = object{
                    object::pair{std:("fetchTokenOverview"), [=]() mutable
                    {
                        throw any(std::make_shared<Error>(std:("Birdeye API Error")));
                    }
                    }, 
                    object::pair{std:("fetchPrice"), [=]() mutable
                    {
                        throw any(std::make_shared<Error>(std:("Birdeye API Error")));
                    }
                    }, 
                    object::pair{std:("fetchTokenTradeData"), [=]() mutable
                    {
                        throw any(std::make_shared<Error>(std:("Birdeye API Error")));
                    }
                    }, 
                    object::pair{std:("fetchTokenSecurity"), [=]() mutable
                    {
                        throw any(std::make_shared<Error>(std:("Birdeye API Error")));
                    }
                    }
                };
                (as<any>(service))["dexscreenerClient"] = object{
                    object::pair{std:("search"), [=]() mutable
                    {
                        return (object{
                            object::pair{std:("pairs"), array<any>()}
                        });
                    }
                    }, 
                    object::pair{std:("searchForHighestLiquidityPair"), [=]() mutable
                    {
                        return nullptr;
                    }
                    }
                };
                auto data = std::async([=]() { service->getTokenAPIData(std:("FAILSOLADDR"), SupportedChain->SOLANA); });
                if (data != nullptr) {
                    throw any(std::make_shared<Error>(std:("Expected null when all SOL APIs fail and Dexscreener finds no pair")));
                }
                logger->info(std:("Service.getTokenAPIData: SOL API failure correctly returns null - Passed"));
            }
            catch (...)
            {
                throw;
            }
        }
    }
    }
}, object{
    object::pair{std:("name"), std:("Service.getTokenAPIData: Fetches ETH token data using DexScreener")}, 
    object::pair{std:("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        (as<any>(service))["dexscreenerClient"] = object{
            object::pair{std:("search"), [=]() mutable
            {
                return (object{
                    object::pair{std:("pairs"), array<any>{ as<any>(object{
                        object::pair{std:("baseToken"), object{
                            object::pair{std:("name"), std:("EthCoin")}, 
                            object::pair{std:("symbol"), std:("ECN")}
                        }}, 
                        object::pair{std:("priceUsd"), std:("2000.0")}, 
                        object::pair{std:("liquidity"), object{
                            object::pair{std:("usd"), 100000}
                        }}, 
                        object::pair{std:("marketCap"), 200000000}, 
                        object::pair{std:("chainId"), std:("ethereum")}, 
                        object::pair{std:("priceChange"), object{
                            object::pair{std:("h24"), 5}, 
                            object::pair{std:("h6"), 2}, 
                            object::pair{std:("h1"), 1}, 
                            object::pair{std:("m5"), 0.1}
                        }}
                    }) }}
                });
            }
            }
        };
        (as<any>(service))["birdeyeClient"] = object{
            object::pair{std:("fetchTokenOverview"), [=]() mutable
            {
                throw any(std::make_shared<Error>(std:("Birdeye should not be called for ETH")));
            }
            }
        };
        auto data = std::async([=]() { service->getTokenAPIData(std:("TESTETHADDR"), SupportedChain->ETHEREUM); });
        if (OR((OR((!data), (data["name"] != std:("EthCoin")))), (data["currentPrice"] != 2000))) {
            throw any(std::make_shared<Error>(std:("getTokenAPIData failed for ETH: ") + JSON->stringify(data) + string_empty));
        }
        logger->info(std:("Service.getTokenAPIData: ETH token data success - Passed"));
    }
    }
}, object{
    object::pair{std:("name"), std:("Service.getTokenAPIData: Fetches BASE token data using DexScreener")}, 
    object::pair{std:("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        (as<any>(service))["dexscreenerClient"] = object{
            object::pair{std:("search"), [=]() mutable
            {
                return (object{
                    object::pair{std:("pairs"), array<any>{ as<any>(object{
                        object::pair{std:("baseToken"), object{
                            object::pair{std:("name"), std:("BaseCoin")}, 
                            object::pair{std:("symbol"), std:("BCN")}
                        }}, 
                        object::pair{std:("priceUsd"), std:("100.0")}, 
                        object::pair{std:("liquidity"), object{
                            object::pair{std:("usd"), 50000}
                        }}, 
                        object::pair{std:("marketCap"), 10000000}, 
                        object::pair{std:("chainId"), std:("base")}, 
                        object::pair{std:("priceChange"), object{
                            object::pair{std:("h24"), 2}, 
                            object::pair{std:("h6"), 1}, 
                            object::pair{std:("h1"), 0.5}, 
                            object::pair{std:("m5"), 0.05}
                        }}
                    }) }}
                });
            }
            }
        };
        auto data = std::async([=]() { service->getTokenAPIData(std:("TESTBASEADDR"), SupportedChain->BASE); });
        if (OR((OR((!data), (data["name"] != std:("BaseCoin")))), (data["currentPrice"] != 100))) {
            throw any(std::make_shared<Error>(std:("getTokenAPIData failed for BASE: ") + JSON->stringify(data) + string_empty));
        }
        logger->info(std:("Service.getTokenAPIData: BASE token data success - Passed"));
    }
    }
} };
array<std::shared_ptr<TestCase>> isLikelyScamOrRugTestCases = array<std::shared_ptr<TestCase>>{ object{
    object::pair{std:("name"), std:("Service.isLikelyScamOrRug: Flags severe price drop (>90%)")}, 
    object::pair{std:("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        auto recTimestamp = Date->now() - 10 * 60 * 1000;
        auto tokenData = object{
            object::pair{std:("currentPrice"), 1}, 
            object::pair{std:("priceHistory"), array<object>{ object{
                object::pair{std:("timestamp"), recTimestamp - 1000}, 
                object::pair{std:("price"), 100}
            }, object{
                object::pair{std:("timestamp"), recTimestamp}, 
                object::pair{std:("price"), 1}
            } }}, 
            object::pair{std:("liquidity"), 10000}, 
            object::pair{std:("marketCap"), 100000}
        };
        auto result = std::async([=]() { service->isLikelyScamOrRug(tokenData, recTimestamp - 2000); });
        if (!result) throw any(std::make_shared<Error>(std:("Severe price drop not flagged as scam/rug")));
        logger->info(std:("Service.isLikelyScamOrRug: Severe price drop - Passed"));
    }
    }
}, object{
    object::pair{std:("name"), std:("Service.isLikelyScamOrRug: Flags based on isKnownScam field")}, 
    object::pair{std:("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        auto tokenData = object{
            object::pair{std:("isKnownScam"), true}, 
            object::pair{std:("currentPrice"), 10}, 
            object::pair{std:("name"), std:("ScamCoin")}, 
            object::pair{std:("symbol"), std:("SCM")}
        };
        auto result = std::async([=]() { service->isLikelyScamOrRug(tokenData, Date->now()); });
        if (!result) throw any(std::make_shared<Error>(std:("Known scam not flagged.")));
        logger->info(std:("Service.isLikelyScamOrRug: Known scam field - Passed"));
    }
    }
}, object{
    object::pair{std:("name"), std:("Service.isLikelyScamOrRug: Flags critical liquidity (<$500)")}, 
    object::pair{std:("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        auto tokenData = object{
            object::pair{std:("currentPrice"), 1}, 
            object::pair{std:("liquidity"), 400}, 
            object::pair{std:("marketCap"), 10000}, 
            object::pair{std:("name"), std:("LowLiq")}, 
            object::pair{std:("symbol"), std:("LLQ")}
        };
        auto result = std::async([=]() { service->isLikelyScamOrRug(tokenData, Date->now()); });
        if (!result) throw any(std::make_shared<Error>(std:("Critical liquidity not flagged.")));
        logger->info(std:("Service.isLikelyScamOrRug: Critical liquidity - Passed"));
    }
    }
}, object{
    object::pair{std:("name"), std:("Service.isLikelyScamOrRug: Flags very low liquidity ratio (<0.5%)")}, 
    object::pair{std:("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        auto tokenData = object{
            object::pair{std:("currentPrice"), 1}, 
            object::pair{std:("liquidity"), 400}, 
            object::pair{std:("marketCap"), 100000}, 
            object::pair{std:("name"), std:("LowRatio")}, 
            object::pair{std:("symbol"), std:("LRT")}
        };
        auto result = std::async([=]() { service->isLikelyScamOrRug(tokenData, Date->now()); });
        if (!result) throw any(std::make_shared<Error>(std:("Very low liquidity ratio not flagged as scam/rug")));
        logger->info(std:("Service.isLikelyScamOrRug: Very low liquidity ratio - Passed"));
    }
    }
}, object{
    object::pair{std:("name"), std:("Service.isLikelyScamOrRug: Not flagged for healthy token")}, 
    object::pair{std:("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        auto tokenData = object{
            object::pair{std:("currentPrice"), 100}, 
            object::pair{std:("liquidity"), 500000}, 
            object::pair{std:("marketCap"), 5000000}, 
            object::pair{std:("priceHistory"), array<object>{ object{
                object::pair{std:("timestamp"), Date->now() - 1000}, 
                object::pair{std:("price"), 95}
            }, object{
                object::pair{std:("timestamp"), Date->now()}, 
                object::pair{std:("price"), 100}
            } }}, 
            object::pair{std:("name"), std:("Healthy")}, 
            object::pair{std:("symbol"), std:("HLT")}
        };
        auto result = std::async([=]() { service->isLikelyScamOrRug(tokenData, Date->now() - 2000); });
        if (result) throw any(std::make_shared<Error>(std:("Healthy token incorrectly flagged as scam/rug")));
        logger->info(std:("Service.isLikelyScamOrRug: Healthy token - Passed"));
    }
    }
} };
array<std::shared_ptr<TestCase>> evaluatePerfTestCases = array<std::shared_ptr<TestCase>>{ object{
    object::pair{std:("name"), std:("Service.evaluatePerf: BUY rec profit, not scam")}, 
    object::pair{std:("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        auto originalIsLikelyScamOrRug = (as<any>(service))["isLikelyScamOrRug"]["bind"](service);
        {
            utils::finally __finally27690_27869([&]() mutable
            {
                if (originalIsLikelyScamOrRug) (as<any>(service))["isLikelyScamOrRug"] = originalIsLikelyScamOrRug; else (as<any>(service)).Delete("isLikelyScamOrRug");
            });
            try
            {
                auto recTimestamp = Date->now() - 60 * 60 * 1000;
                auto baseRec = object{
                    object::pair{std:("id"), asUUID(uuidv4())}, 
                    object::pair{std:("userId"), testUserIdGlobalForService}, 
                    object::pair{std:("messageId"), asUUID(uuidv4())}, 
                    object::pair{std:("timestamp"), recTimestamp}, 
                    object::pair{std:("tokenAddress"), std:("ADDR_PROFIT")}, 
                    object::pair{std:("chain"), SupportedChain->SOLANA}, 
                    object::pair{std:("conviction"), Conviction->HIGH}, 
                    object::pair{std:("rawMessageQuote"), std:("quote")}, 
                    object::pair{std:("priceAtRecommendation"), 10}
                };
                auto tokenData = object{
                    object::pair{std:("currentPrice"), 15}, 
                    object::pair{std:("priceHistory"), array<object>{ object{
                        object::pair{std:("timestamp"), recTimestamp}, 
                        object::pair{std:("price"), 10}
                    }, object{
                        object::pair{std:("timestamp"), Date->now()}, 
                        object::pair{std:("price"), 15}
                    } }}, 
                    object::pair{std:("name"), std:("ProfitCoin")}, 
                    object::pair{std:("symbol"), std:("PFT")}
                };
                (as<any>(service))["isLikelyScamOrRug"] = [=]() mutable
                {
                    return false;
                };
                auto metrics = std::async([=]() { service->evaluateRecommendationPerformance(as<std::shared_ptr<Recommendation>>(utils::assign(object{
                    , 
                    object::pair{std:("recommendationType"), std:("BUY")}
                }, baseRec)), tokenData); });
                if (Math->abs((OR((metrics->potentialProfitPercent), (0))) - 50) > 0.01) throw any(std::make_shared<Error>(std:("BUY profit mismatch")));
                if (metrics->isScamOrRug) throw any(std::make_shared<Error>(std:("Incorrectly flagged as scam")));
                logger->info(std:("Service.evaluatePerf: BUY profit - Passed"));
            }
            catch (...)
            {
                throw;
            }
        }
    }
    }
}, object{
    object::pair{std:("name"), std:("Service.evaluatePerf: BUY rec for rugged token results in -99 profit percent")}, 
    object::pair{std:("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        auto originalIsLikelyScamOrRug = (as<any>(service))["isLikelyScamOrRug"]["bind"](service);
        {
            utils::finally __finally29493_29672([&]() mutable
            {
                if (originalIsLikelyScamOrRug) (as<any>(service))["isLikelyScamOrRug"] = originalIsLikelyScamOrRug; else (as<any>(service)).Delete("isLikelyScamOrRug");
            });
            try
            {
                auto recTimestamp = Date->now() - 60 * 60 * 1000;
                auto baseRec = object{
                    object::pair{std:("id"), asUUID(uuidv4())}, 
                    object::pair{std:("userId"), testUserIdGlobalForService}, 
                    object::pair{std:("messageId"), asUUID(uuidv4())}, 
                    object::pair{std:("timestamp"), recTimestamp}, 
                    object::pair{std:("tokenAddress"), std:("ADDR_RUG_BUY")}, 
                    object::pair{std:("chain"), SupportedChain->SOLANA}, 
                    object::pair{std:("conviction"), Conviction->HIGH}, 
                    object::pair{std:("rawMessageQuote"), std:("quote")}, 
                    object::pair{std:("priceAtRecommendation"), 10}
                };
                auto tokenData = object{
                    object::pair{std:("currentPrice"), 0.1}, 
                    object::pair{std:("priceHistory"), array<object>{ object{
                        object::pair{std:("timestamp"), recTimestamp}, 
                        object::pair{std:("price"), 10}
                    }, object{
                        object::pair{std:("timestamp"), Date->now()}, 
                        object::pair{std:("price"), 0.1}
                    } }}, 
                    object::pair{std:("name"), std:("RugCoin")}, 
                    object::pair{std:("symbol"), std:("RUG")}
                };
                (as<any>(service))["isLikelyScamOrRug"] = [=]() mutable
                {
                    return true;
                };
                auto metrics = std::async([=]() { service->evaluateRecommendationPerformance(as<std::shared_ptr<Recommendation>>(utils::assign(object{
                    , 
                    object::pair{std:("recommendationType"), std:("BUY")}
                }, baseRec)), tokenData); });
                if (metrics->potentialProfitPercent != -99) throw any(std::make_shared<Error>(std:("BUY scam profit mismatch, expected -99")));
                if (!metrics->isScamOrRug) throw any(std::make_shared<Error>(std:("Not flagged as scam when it should be")));
                logger->info(std:("Service.evaluatePerf: BUY rec rugged - Passed"));
            }
            catch (...)
            {
                throw;
            }
        }
    }
    }
}, object{
    object::pair{std:("name"), std:("Service.evaluatePerf: SELL rec, avoided loss correctly (price dropped)")}, 
    object::pair{std:("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        auto originalIsLikelyScamOrRug = (as<any>(service))["isLikelyScamOrRug"]["bind"](service);
        {
            utils::finally __finally31232_31411([&]() mutable
            {
                if (originalIsLikelyScamOrRug) (as<any>(service))["isLikelyScamOrRug"] = originalIsLikelyScamOrRug; else (as<any>(service)).Delete("isLikelyScamOrRug");
            });
            try
            {
                auto recTimestamp = Date->now() - 60 * 60 * 1000;
                auto baseRec = object{
                    object::pair{std:("id"), asUUID(uuidv4())}, 
                    object::pair{std:("userId"), testUserIdGlobalForService}, 
                    object::pair{std:("messageId"), asUUID(uuidv4())}, 
                    object::pair{std:("timestamp"), recTimestamp}, 
                    object::pair{std:("tokenAddress"), std:("ADDR_AVOID_LOSS")}, 
                    object::pair{std:("chain"), SupportedChain->SOLANA}, 
                    object::pair{std:("conviction"), Conviction->MEDIUM}, 
                    object::pair{std:("rawMessageQuote"), std:("sell this")}, 
                    object::pair{std:("priceAtRecommendation"), 100}
                };
                auto tokenData = object{
                    object::pair{std:("currentPrice"), 20}, 
                    object::pair{std:("priceHistory"), array<object>{ object{
                        object::pair{std:("timestamp"), recTimestamp}, 
                        object::pair{std:("price"), 100}
                    }, object{
                        object::pair{std:("timestamp"), Date->now()}, 
                        object::pair{std:("price"), 20}
                    } }}, 
                    object::pair{std:("name"), std:("DropCoin")}, 
                    object::pair{std:("symbol"), std:("DRP")}
                };
                (as<any>(service))["isLikelyScamOrRug"] = [=]() mutable
                {
                    return false;
                };
                auto metrics = std::async([=]() { service->evaluateRecommendationPerformance(as<std::shared_ptr<Recommendation>>(utils::assign(object{
                    , 
                    object::pair{std:("recommendationType"), std:("SELL")}
                }, baseRec)), tokenData); });
                if (Math->abs((OR((metrics->avoidedLossPercent), (0))) - 80) > 0.01) throw any(std::make_shared<Error>(std:("SELL avoided loss mismatch. Expected 80%")));
                logger->info(std:("Service.evaluatePerf: SELL avoided loss - Passed"));
            }
            catch (...)
            {
                throw;
            }
        }
    }
    }
}, object{
    object::pair{std:("name"), std:("Service.evaluatePerf: SELL rec, missed gain (price pumped)")}, 
    object::pair{std:("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        auto originalIsLikelyScamOrRug = (as<any>(service))["isLikelyScamOrRug"]["bind"](service);
        {
            utils::finally __finally32966_33145([&]() mutable
            {
                if (originalIsLikelyScamOrRug) (as<any>(service))["isLikelyScamOrRug"] = originalIsLikelyScamOrRug; else (as<any>(service)).Delete("isLikelyScamOrRug");
            });
            try
            {
                auto recTimestamp = Date->now() - 60 * 60 * 1000;
                auto baseRec = object{
                    object::pair{std:("id"), asUUID(uuidv4())}, 
                    object::pair{std:("userId"), testUserIdGlobalForService}, 
                    object::pair{std:("messageId"), asUUID(uuidv4())}, 
                    object::pair{std:("timestamp"), recTimestamp}, 
                    object::pair{std:("tokenAddress"), std:("ADDR_MISSED_GAIN")}, 
                    object::pair{std:("chain"), SupportedChain->SOLANA}, 
                    object::pair{std:("conviction"), Conviction->MEDIUM}, 
                    object::pair{std:("rawMessageQuote"), std:("sell this now")}, 
                    object::pair{std:("priceAtRecommendation"), 50}
                };
                auto tokenData = object{
                    object::pair{std:("currentPrice"), 100}, 
                    object::pair{std:("priceHistory"), array<object>{ object{
                        object::pair{std:("timestamp"), recTimestamp}, 
                        object::pair{std:("price"), 50}
                    }, object{
                        object::pair{std:("timestamp"), Date->now()}, 
                        object::pair{std:("price"), 100}
                    } }}, 
                    object::pair{std:("name"), std:("PumpCoin")}, 
                    object::pair{std:("symbol"), std:("PMP")}
                };
                (as<any>(service))["isLikelyScamOrRug"] = [=]() mutable
                {
                    return false;
                };
                auto metrics = std::async([=]() { service->evaluateRecommendationPerformance(as<std::shared_ptr<Recommendation>>(utils::assign(object{
                    , 
                    object::pair{std:("recommendationType"), std:("SELL")}
                }, baseRec)), tokenData); });
                if (Math->abs((OR((metrics->avoidedLossPercent), (0))) - -100) > 0.01) throw any(std::make_shared<Error>(std:("SELL missed gain mismatch. Expected -100%")));
                logger->info(std:("Service.evaluatePerf: SELL missed gain - Passed"));
            }
            catch (...)
            {
                throw;
            }
        }
    }
    }
}, object{
    object::pair{std:("name"), std:("Service.evaluatePerf: SELL rec, correctly identified scam (avoidedLossPercent = 99)")}, 
    object::pair{std:("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        auto originalIsLikelyScamOrRug = (as<any>(service))["isLikelyScamOrRug"]["bind"](service);
        {
            utils::finally __finally34627_34806([&]() mutable
            {
                if (originalIsLikelyScamOrRug) (as<any>(service))["isLikelyScamOrRug"] = originalIsLikelyScamOrRug; else (as<any>(service)).Delete("isLikelyScamOrRug");
            });
            try
            {
                auto recTimestamp = Date->now() - 60 * 60 * 1000;
                auto baseRec = object{
                    object::pair{std:("id"), asUUID(uuidv4())}, 
                    object::pair{std:("userId"), testUserIdGlobalForService}, 
                    object::pair{std:("messageId"), asUUID(uuidv4())}, 
                    object::pair{std:("timestamp"), recTimestamp}, 
                    object::pair{std:("tokenAddress"), std:("ADDR_RUG_SELL")}, 
                    object::pair{std:("chain"), SupportedChain->SOLANA}, 
                    object::pair{std:("conviction"), Conviction->HIGH}, 
                    object::pair{std:("rawMessageQuote"), std:("quote")}, 
                    object::pair{std:("priceAtRecommendation"), 10}
                };
                auto tokenData = object{
                    object::pair{std:("currentPrice"), 0.1}, 
                    object::pair{std:("name"), std:("SellScam")}, 
                    object::pair{std:("symbol"), std:("SSC")}
                };
                (as<any>(service))["isLikelyScamOrRug"] = [=]() mutable
                {
                    return true;
                };
                auto metrics = std::async([=]() { service->evaluateRecommendationPerformance(as<std::shared_ptr<Recommendation>>(utils::assign(object{
                    , 
                    object::pair{std:("recommendationType"), std:("SELL")}
                }, baseRec)), tokenData); });
                if (metrics->avoidedLossPercent != 99) throw any(std::make_shared<Error>(std:("SELL scam avoidedLossPercent mismatch, expected 99")));
                if (!metrics->isScamOrRug) throw any(std::make_shared<Error>(std:("Not flagged as scam for SELL when it should be")));
                logger->info(std:("Service.evaluatePerf: SELL rec rugged, correct ID - Passed"));
            }
            catch (...)
            {
                throw;
            }
        }
    }
    }
} };
array<std::shared_ptr<TestCase>> getLeaderboardDataTestCases = array<std::shared_ptr<TestCase>>{ object{
    object::pair{std:("name"), std:("Service.getLeaderboardData: Returns sorted entries with ranks")}, 
    object::pair{std:("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        shared user1Id = asUUID(uuidv4());
        shared user2Id = asUUID(uuidv4());
        shared user3Id = asUUID(uuidv4());
        (as<any>(runtime->getAgents)) = [=]() mutable
        {
            return array<object>{ object{
                object::pair{std:("id"), user1Id}, 
                object::pair{std:("names"), array<string>{ std:("UserA") }}, 
                object::pair{std:("metadata"), object{}}
            }, object{
                object::pair{std:("id"), user2Id}, 
                object::pair{std:("names"), array<string>{ std:("UserB") }}, 
                object::pair{std:("metadata"), object{}}
            }, object{
                object::pair{std:("id"), user3Id}, 
                object::pair{std:("names"), array<string>{ std:("UserC") }}, 
                object::pair{std:("metadata"), object{}}
            } };
        };
        shared user1Comp = createFullMockComponentForSvcTest(user1Id, object{
            object::pair{std:("version"), std:("1.0.0")}, 
            object::pair{std:("userId"), user1Id}, 
            object::pair{std:("trustScore"), 75}, 
            object::pair{std:("recommendations"), array<any>()}, 
            object::pair{std:("lastTrustScoreCalculationTimestamp"), Date->now()}
        }, runtime);
        shared user2Comp = createFullMockComponentForSvcTest(user2Id, object{
            object::pair{std:("version"), std:("1.0.0")}, 
            object::pair{std:("userId"), user2Id}, 
            object::pair{std:("trustScore"), 90}, 
            object::pair{std:("recommendations"), array<any>()}, 
            object::pair{std:("lastTrustScoreCalculationTimestamp"), Date->now()}
        }, runtime);
        shared user3Comp = createFullMockComponentForSvcTest(user3Id, object{
            object::pair{std:("version"), std:("1.0.0")}, 
            object::pair{std:("userId"), user3Id}, 
            object::pair{std:("trustScore"), 80}, 
            object::pair{std:("recommendations"), array<any>()}, 
            object::pair{std:("lastTrustScoreCalculationTimestamp"), Date->now()}
        }, runtime);
        (as<any>(runtime->getComponent)) = [=](auto entityId, auto type, auto worldId = undefined) mutable
        {
            if (AND((worldId), (worldId != runtime->agentId))) return nullptr;
            if (type != TRUST_MARKETPLACE_COMPONENT_TYPE) return nullptr;
            if (entityId == user1Id) return user1Comp;
            if (entityId == user2Id) return user2Comp;
            if (entityId == user3Id) return user3Comp;
            return nullptr;
        };
        (as<any>(runtime->getEntityById)) = [=](auto id) mutable
        {
            if (id == user1Id) return object{
                object::pair{std:("id"), user1Id}, 
                object::pair{std:("names"), array<string>{ std:("UserA") }}, 
                object::pair{std:("agentId"), runtime->agentId}
            };
            if (id == user2Id) return object{
                object::pair{std:("id"), user2Id}, 
                object::pair{std:("names"), array<string>{ std:("UserB") }}, 
                object::pair{std:("agentId"), runtime->agentId}
            };
            if (id == user3Id) return object{
                object::pair{std:("id"), user3Id}, 
                object::pair{std:("names"), array<string>{ std:("UserC") }}, 
                object::pair{std:("agentId"), runtime->agentId}
            };
            return nullptr;
        };
        auto leaderboard = std::async([=]() { service->getLeaderboardData(runtime); });
        if (leaderboard->get_length() != 3) throw any(std::make_shared<Error>(std:("Expected 3 leaderboard entries, got ") + leaderboard->get_length() + string_empty));
        if (OR((OR((const_(leaderboard)[0]->userId != user2Id), (const_(leaderboard)[0]->rank != 1))), (const_(leaderboard)[0]->trustScore != 90))) throw any(std::make_shared<Error>(std:("Leaderboard error for rank 1: ") + JSON->stringify(const_(leaderboard)[0]) + string_empty));
        if (OR((OR((const_(leaderboard)[1]->userId != user3Id), (const_(leaderboard)[1]->rank != 2))), (const_(leaderboard)[1]->trustScore != 80))) throw any(std::make_shared<Error>(std:("Leaderboard error for rank 2: ") + JSON->stringify(const_(leaderboard)[1]) + string_empty));
        if (OR((OR((const_(leaderboard)[2]->userId != user1Id), (const_(leaderboard)[2]->rank != 3))), (const_(leaderboard)[2]->trustScore != 75))) throw any(std::make_shared<Error>(std:("Leaderboard error for rank 3: ") + JSON->stringify(const_(leaderboard)[2]) + string_empty));
        logger->info(std:("Service.getLeaderboardData: Sorted entries - Passed"));
    }
    }
}, object{
    object::pair{std:("name"), std:("Service.getLeaderboardData: Handles empty agents list")}, 
    object::pair{std:("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        (as<any>(runtime->getAgents)) = [=]() mutable
        {
            return array<any>();
        };
        auto leaderboard = std::async([=]() { service->getLeaderboardData(runtime); });
        if (leaderboard->get_length() != 0) throw any(std::make_shared<Error>(std:("Expected 0 leaderboard entries for no agents")));
        logger->info(std:("Service.getLeaderboardData: Empty agents list - Passed"));
    }
    }
}, object{
    object::pair{std:("name"), std:("Service.getLeaderboardData: Handles agents with no trust profile component")}, 
    object::pair{std:("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        shared user1Id = asUUID(uuidv4());
        (as<any>(runtime->getAgents)) = [=]() mutable
        {
            return array<object>{ object{
                object::pair{std:("id"), user1Id}, 
                object::pair{std:("names"), array<string>{ std:("UserA") }}, 
                object::pair{std:("metadata"), object{}}
            } };
        };
        (as<any>(runtime->getComponent)) = [=]() mutable
        {
            return nullptr;
        };
        (as<any>(runtime->getEntityById)) = [=](auto id) mutable
        {
            return (object{
                object::pair{std:("id"), std:("id")}, 
                object::pair{std:("names"), array<string>{ std:("UserA") }}, 
                object::pair{std:("agentId"), runtime->agentId}
            });
        };
        auto leaderboard = std::async([=]() { service->getLeaderboardData(runtime); });
        if (leaderboard->get_length() != 0) throw any(std::make_shared<Error>(std:("Expected 0 leaderboard entries when no profiles found")));
        logger->info(std:("Service.getLeaderboardData: No profiles found - Passed"));
    }
    }
} };
std::shared_ptr<TestSuite> serviceTestSuite = object{
    object::pair{std:("name"), std:("CommunityInvestorService Tests (ElizaOS Runner Format)")}, 
    object::pair{std:("tests"), array<any>{ calculateUserTrustScoreTestCases, resolveTickerTestCases, getTokenAPIDataTestCases, isLikelyScamOrRugTestCases, evaluatePerfTestCases, getLeaderboardDataTestCases }}
};

void Main(void)
{
}

MAIN
