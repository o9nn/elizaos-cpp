#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/communityInvestor/tests/service.h"

any testUserIdGlobalForService = asUUID(uuidv4());
std::function<any(any, any, any)> createFullMockComponentForSvcTest = [=](auto userId, auto profileData, auto runtime) mutable
{
    return (object{
        object::pair{std::string("id"), asUUID(uuidv4())}, 
        object::pair{std::string("entityId"), userId}, 
        object::pair{std::string("agentId"), runtime->agentId}, 
        object::pair{std::string("worldId"), runtime->agentId}, 
        object::pair{std::string("roomId"), asUUID(uuidv4())}, 
        object::pair{std::string("sourceEntityId"), runtime->agentId}, 
        object::pair{std::string("type"), TRUST_MARKETPLACE_COMPONENT_TYPE}, 
        object::pair{std::string("createdAt"), Date->now()}, 
        object::pair{std::string("data"), profileData}
    });
};
std::function<any(string, double, any, any, any, double, any)> createMockRecForSvcTest = [=](auto id, auto timestamp, P2 type, auto conviction, auto userIdToSet, auto priceAtRec = undefined, auto metric = undefined) mutable
{
    return (object{
        object::pair{std::string("id"), asUUID(uuidv4())}, 
        object::pair{std::string("userId"), userIdToSet}, 
        object::pair{std::string("messageId"), asUUID(uuidv4())}, 
        object::pair{std::string("timestamp"), std::string("timestamp")}, 
        object::pair{std::string("tokenAddress"), std::string("TEST_TOKEN_") + id + string_empty}, 
        object::pair{std::string("chain"), SupportedChain->SOLANA}, 
        object::pair{std::string("recommendationType"), type}, 
        object::pair{std::string("conviction"), std::string("conviction")}, 
        object::pair{std::string("rawMessageQuote"), std::string("This is a quote for ") + id + string_empty}, 
        object::pair{std::string("priceAtRecommendation"), priceAtRec}, 
        object::pair{std::string("metrics"), metric}, 
        object::pair{std::string("processedForTradeDecision"), false}
    });
};
array<std::shared_ptr<TestCase>> calculateUserTrustScoreTestCases = array<std::shared_ptr<TestCase>>{ object{
    object::pair{std::string("name"), std::string("Service.calculateUserTrustScore: New user, score 0")}, 
    object::pair{std::string("fn"), [=](auto runtime) mutable
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
                if (!createdComp) throw any(std::make_shared<Error>(std::string("createComponent was not called or did not set createdComp")));
                auto data = as<std::shared_ptr<UserTrustProfile>>(createdComp["data"]);
                if (OR((data->trustScore != 0), (data->recommendations->length != 0))) {
                    throw any(std::make_shared<Error>(std::string("New user score expected 0, got ") + data->trustScore + string_empty));
                }
                logger->info(std::string("Service.calculateUserTrustScore: New user score 0 - Passed"));
            }
            catch (...)
            {
                throw;
            }
        }
    }
    }
}, object{
    object::pair{std::string("name"), std::string("Service.calculateUserTrustScore: Single positive BUY recommendation")}, 
    object::pair{std::string("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        shared recTimestamp = Date->now() - 10 * 24 * 60 * 60 * 1000;
        auto mockRec = createMockRecForSvcTest(std::string("rec1"), recTimestamp, std::string("BUY"), Conviction->HIGH, testUserIdGlobalForService, 10, object{
            object::pair{std::string("evaluationTimestamp"), Date->now()}, 
            object::pair{std::string("potentialProfitPercent"), 20}, 
            object::pair{std::string("isScamOrRug"), false}
        });
        auto initialProfile = object{
            object::pair{std::string("version"), std::string("1.0.0")}, 
            object::pair{std::string("userId"), testUserIdGlobalForService}, 
            object::pair{std::string("trustScore"), 0}, 
            object::pair{std::string("lastTrustScoreCalculationTimestamp"), Date->now() - 2 * 24 * 60 * 60 * 1000}, 
            object::pair{std::string("recommendations"), array<any>{ mockRec }}
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
                        object::pair{std::string("currentPrice"), 12}, 
                        object::pair{std::string("priceHistory"), array<object>{ object{
                            object::pair{std::string("timestamp"), recTimestamp}, 
                            object::pair{std::string("price"), 10}
                        }, object{
                            object::pair{std::string("timestamp"), Date->now()}, 
                            object::pair{std::string("price"), 12}
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
                        object::pair{std::string("evaluationTimestamp"), Date->now()}, 
                        object::pair{std::string("potentialProfitPercent"), 20}, 
                        object::pair{std::string("isScamOrRug"), false}
                    });
                };
                std::async([=]() { service->calculateUserTrustScore(testUserIdGlobalForService, runtime); });
                if (!updatedComp) throw any(std::make_shared<Error>(std::string("updateComponent not called")));
                auto updatedData = as<std::shared_ptr<UserTrustProfile>>(updatedComp["data"]);
                if (Math->abs(updatedData->trustScore - 20) > 0.01) throw any(std::make_shared<Error>(std::string("Expected score ~20.0, got ") + updatedData->trustScore + string_empty));
                logger->info(std::string("Service.calculateUserTrustScore: Single positive BUY - Passed"));
            }
            catch (...)
            {
                throw;
            }
        }
    }
    }
}, object{
    object::pair{std::string("name"), std::string("Service.calculateUserTrustScore: Score clamping at +100")}, 
    object::pair{std::string("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        auto recTimestamp = Date->now();
        auto recs = array<any>{ createMockRecForSvcTest(std::string("superGood"), recTimestamp, std::string("BUY"), Conviction->HIGH, testUserIdGlobalForService, 10, object{
            object::pair{std::string("potentialProfitPercent"), 500}, 
            object::pair{std::string("evaluationTimestamp"), recTimestamp}
        }) };
        auto initialProfile = object{
            object::pair{std::string("version"), std::string("1.0.0")}, 
            object::pair{std::string("userId"), testUserIdGlobalForService}, 
            object::pair{std::string("trustScore"), 0}, 
            object::pair{std::string("lastTrustScoreCalculationTimestamp"), 0}, 
            object::pair{std::string("recommendations"), recs}
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
                        object::pair{std::string("evaluationTimestamp"), Date->now()}, 
                        object::pair{std::string("potentialProfitPercent"), 500}, 
                        object::pair{std::string("isScamOrRug"), false}
                    });
                };
                (as<any>(service))["getTokenAPIData"] = [=]() mutable
                {
                    return as<std::shared_ptr<TokenAPIData>>((object{
                        object::pair{std::string("currentPrice"), 60}
                    }));
                };
                std::async([=]() { service->calculateUserTrustScore(testUserIdGlobalForService, runtime); });
                if (!updatedComp) throw any(std::make_shared<Error>(std::string("updateComponent not called")));
                auto updatedData = as<std::shared_ptr<UserTrustProfile>>(updatedComp["data"]);
                if (Math->abs(updatedData->trustScore - 100) > 0.01) throw any(std::make_shared<Error>(std::string("Expected score clamped at 100, got ") + updatedData->trustScore + string_empty));
                logger->info(std::string("Service.calculateUserTrustScore: Clamping at +100 - Passed"));
            }
            catch (...)
            {
                throw;
            }
        }
    }
    }
}, object{
    object::pair{std::string("name"), std::string("Service.calculateUserTrustScore: Recommendation with undefined metrics triggers re-evaluation")}, 
    object::pair{std::string("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        shared recTimestamp = Date->now();
        auto recs = array<any>{ createMockRecForSvcTest(std::string("needsEval"), recTimestamp, std::string("BUY"), Conviction->MEDIUM, testUserIdGlobalForService, 10, undefined) };
        auto initialProfile = object{
            object::pair{std::string("version"), std::string("1.0.0")}, 
            object::pair{std::string("userId"), testUserIdGlobalForService}, 
            object::pair{std::string("trustScore"), 0}, 
            object::pair{std::string("lastTrustScoreCalculationTimestamp"), 0}, 
            object::pair{std::string("recommendations"), recs}
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
                        object::pair{std::string("currentPrice"), 12}, 
                        object::pair{std::string("priceHistory"), array<object>{ object{
                            object::pair{std::string("timestamp"), recTimestamp}, 
                            object::pair{std::string("price"), 10}
                        }, object{
                            object::pair{std::string("timestamp"), Date->now()}, 
                            object::pair{std::string("price"), 12}
                        } }}
                    });
                };
                (as<any>(service))["evaluateRecommendationPerformance"] = [=]() mutable
                {
                    evaluatePerformanceCalled = true;
                    return as<std::shared_ptr<RecommendationMetric>>(object{
                        object::pair{std::string("evaluationTimestamp"), Date->now()}, 
                        object::pair{std::string("potentialProfitPercent"), 20}, 
                        object::pair{std::string("isScamOrRug"), false}
                    });
                };
                std::async([=]() { service->calculateUserTrustScore(testUserIdGlobalForService, runtime); });
                if (!updatedComp) throw any(std::make_shared<Error>(std::string("updateComponent not called")));
                if (!getTokenAPIDataCalled) throw any(std::make_shared<Error>(std::string("getTokenAPIData was not called for metric re-evaluation")));
                if (!evaluatePerformanceCalled) throw any(std::make_shared<Error>(std::string("evaluateRecommendationPerformance was not called for metric re-evaluation")));
                auto updatedData = as<std::shared_ptr<UserTrustProfile>>(updatedComp["data"]);
                if (Math->abs(updatedData->trustScore - 20) > 0.01) throw any(std::make_shared<Error>(std::string("Expected score ~20.0 after re-eval, got ") + updatedData->trustScore + string_empty));
                logger->info(std::string("Service.calculateUserTrustScore: Undefined metrics re-evaluation - Passed"));
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
    object::pair{std::string("name"), std::string("Service.resolveTicker: Known SOL ticker ($SOL)")}, 
    object::pair{std::string("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        auto result = std::async([=]() { service->resolveTicker(std::string("$SOL"), SupportedChain->SOLANA, array<any>()); });
        if (OR((result["address"] != std::string("So11111111111111111111111111111111111111112")), (result["ticker"] != std::string("SOL")))) {
            throw any(std::make_shared<Error>(std::string("Unexpected result for $SOL: ") + JSON->stringify(result) + string_empty));
        }
        logger->info(std::string("Service.resolveTicker: Known SOL - Passed"));
    }
    }
}, object{
    object::pair{std::string("name"), std::string("Service.resolveTicker: Unknown ticker, DexScreener finds it (SOL)")}, 
    object::pair{std::string("fn"), [=](auto runtime) mutable
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
                    object::pair{std::string("search"), [=](auto query) mutable
                    {
                        return (object{
                            object::pair{std::string("pairs"), array<object>{ object{
                                object::pair{std::string("baseToken"), object{
                                    object::pair{std::string("address"), std::string("DEX_FOUND_ADDR_SOL")}, 
                                    object::pair{std::string("symbol"), query->replace(std::string("$"), string_empty)}
                                }}, 
                                object::pair{std::string("chainId"), std::string("solana")}, 
                                object::pair{std::string("liquidity"), object{
                                    object::pair{std::string("usd"), 10000}
                                }}
                            } }}
                        });
                    }
                    }, 
                    object::pair{std::string("searchForHighestLiquidityPair"), [=](auto query) mutable
                    {
                        return (object{
                            object::pair{std::string("baseToken"), object{
                                object::pair{std::string("address"), std::string("DEX_FOUND_ADDR_SOL")}, 
                                object::pair{std::string("symbol"), query->replace(std::string("$"), string_empty)}
                            }}, 
                            object::pair{std::string("chainId"), std::string("solana")}, 
                            object::pair{std::string("liquidity"), object{
                                object::pair{std::string("usd"), 10000}
                            }}
                        });
                    }
                    }
                };
                auto result = std::async([=]() { service->resolveTicker(std::string("$NEWCOINSOL"), SupportedChain->SOLANA, array<any>()); });
                if (OR((result["address"] != std::string("DEX_FOUND_ADDR_SOL")), (result["ticker"] != std::string("NEWCOINSOL")))) {
                    throw any(std::make_shared<Error>(std::string("Unexpected result from DexScreener for SOL: ") + JSON->stringify(result) + string_empty));
                }
                logger->info(std::string("Service.resolveTicker: DexScreener SOL find - Passed"));
            }
            catch (...)
            {
                throw;
            }
        }
    }
    }
}, object{
    object::pair{std::string("name"), std::string("Service.resolveTicker: Context message provides address for SOL")}, 
    object::pair{std::string("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        auto testSolanaAddressInContext = std::string("TESTCTXADRESSSOLANAbe891z3456789012345");
        auto contextMessages = array<std::shared_ptr<Memory>>{ as<std::shared_ptr<Memory>>(object{
            object::pair{std::string("entityId"), asUUID(uuidv4())}, 
            object::pair{std::string("roomId"), asUUID(uuidv4())}, 
            object::pair{std::string("agentId"), runtime->agentId}, 
            object::pair{std::string("content"), object{
                object::pair{std::string("text"), std::string("I heard $CTXTOKEN (") + testSolanaAddressInContext + std::string(") is good.")}
            }}
        }) };
        auto result = std::async([=]() { service->resolveTicker(std::string("$CTXTOKEN"), SupportedChain->SOLANA, contextMessages); });
        if (OR((result["address"] != testSolanaAddressInContext), (result["ticker"] != std::string("CTXTOKEN")))) {
            throw any(std::make_shared<Error>(std::string("Context resolution failed for $CTXTOKEN: expected ") + testSolanaAddressInContext + std::string(", got ") + JSON->stringify(result) + string_empty));
        }
        logger->info(std::string("Service.resolveTicker: Context message SOL - Passed"));
    }
    }
}, object{
    object::pair{std::string("name"), std::string("Service.resolveTicker: Returns null for unresolvable ticker")}, 
    object::pair{std::string("fn"), [=](auto runtime) mutable
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
                    object::pair{std::string("search"), [=]() mutable
                    {
                        return (object{
                            object::pair{std::string("pairs"), array<any>()}
                        });
                    }
                    }
                };
                auto result = std::async([=]() { service->resolveTicker(std::string("$NONEXISTENT"), SupportedChain->SOLANA, array<any>()); });
                if (result != nullptr) throw any(std::make_shared<Error>(std::string("Expected null for unresolvable ticker, got ") + JSON->stringify(result) + string_empty));
                logger->info(std::string("Service.resolveTicker: Unresolvable returns null - Passed"));
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
    object::pair{std::string("name"), std::string("Service.getTokenAPIData: Fetches SOL token data successfully")}, 
    object::pair{std::string("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        auto tokenAddress = std::string("TESTSOLADDR");
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
                    object::pair{std::string("fetchTokenOverview"), [=]() mutable
                    {
                        return (object{
                            object::pair{std::string("name"), std::string("BirdEyeCoin")}, 
                            object::pair{std::string("symbol"), std::string("BEC")}, 
                            object::pair{std::string("decimals"), 9}
                        });
                    }
                    }, 
                    object::pair{std::string("fetchPrice"), [=]() mutable
                    {
                        return 15;
                    }
                    }, 
                    object::pair{std::string("fetchTokenTradeData"), [=]() mutable
                    {
                        return as<std::shared_ptr<TokenTradeData>>(as<any>((object{
                            object::pair{std::string("price"), 15}, 
                            object::pair{std::string("history_24h_price"), 14}, 
                            object::pair{std::string("volume_24h_usd"), 10000}, 
                            object::pair{std::string("market"), 500000}, 
                            object::pair{std::string("price_change_24h_percent"), 5}, 
                            object::pair{std::string("unique_wallet_24h_change_percent"), 1}, 
                            object::pair{std::string("volume_24h"), 1000}
                        })));
                    }
                    }, 
                    object::pair{std::string("fetchTokenSecurity"), [=]() mutable
                    {
                        return as<any>((object{
                            object::pair{std::string("top10HolderPercent"), 10}
                        }));
                    }
                    }
                };
                (as<any>(service))["dexscreenerClient"] = object{
                    object::pair{std::string("search"), [=]() mutable
                    {
                        return (object{
                            object::pair{std::string("schemaVersion"), std::string("1.0.0")}, 
                            object::pair{std::string("pairs"), array<object>{ object{
                                object::pair{std::string("baseToken"), object{
                                    object::pair{std::string("name"), std::string("DexCoin")}, 
                                    object::pair{std::string("symbol"), std::string("DXC")}
                                }}, 
                                object::pair{std::string("priceUsd"), std::string("15.5")}, 
                                object::pair{std::string("liquidity"), object{
                                    object::pair{std::string("usd"), 50000}
                                }}, 
                                object::pair{std::string("marketCap"), 500000}, 
                                object::pair{std::string("chainId"), std::string("solana")}
                            } }}
                        });
                    }
                    }
                };
                auto data = std::async([=]() { service->getTokenAPIData(tokenAddress, SupportedChain->SOLANA); });
                if (OR((OR((OR((!data), (data["name"] != std::string("BirdEyeCoin")))), (data["currentPrice"] != 15))), (data["liquidity"] != 50000))) {
                    throw any(std::make_shared<Error>(std::string("getTokenAPIData failed for SOL: ") + JSON->stringify(data) + string_empty));
                }
                logger->info(std::string("Service.getTokenAPIData: SOL token data success - Passed"));
            }
            catch (...)
            {
                throw;
            }
        }
    }
    }
}, object{
    object::pair{std::string("name"), std::string("Service.getTokenAPIData: Returns null if all SOL API calls fail and DexScreener has no pair")}, 
    object::pair{std::string("fn"), [=](auto runtime) mutable
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
                    object::pair{std::string("fetchTokenOverview"), [=]() mutable
                    {
                        throw any(std::make_shared<Error>(std::string("Birdeye API Error")));
                    }
                    }, 
                    object::pair{std::string("fetchPrice"), [=]() mutable
                    {
                        throw any(std::make_shared<Error>(std::string("Birdeye API Error")));
                    }
                    }, 
                    object::pair{std::string("fetchTokenTradeData"), [=]() mutable
                    {
                        throw any(std::make_shared<Error>(std::string("Birdeye API Error")));
                    }
                    }, 
                    object::pair{std::string("fetchTokenSecurity"), [=]() mutable
                    {
                        throw any(std::make_shared<Error>(std::string("Birdeye API Error")));
                    }
                    }
                };
                (as<any>(service))["dexscreenerClient"] = object{
                    object::pair{std::string("search"), [=]() mutable
                    {
                        return (object{
                            object::pair{std::string("pairs"), array<any>()}
                        });
                    }
                    }, 
                    object::pair{std::string("searchForHighestLiquidityPair"), [=]() mutable
                    {
                        return nullptr;
                    }
                    }
                };
                auto data = std::async([=]() { service->getTokenAPIData(std::string("FAILSOLADDR"), SupportedChain->SOLANA); });
                if (data != nullptr) {
                    throw any(std::make_shared<Error>(std::string("Expected null when all SOL APIs fail and Dexscreener finds no pair")));
                }
                logger->info(std::string("Service.getTokenAPIData: SOL API failure correctly returns null - Passed"));
            }
            catch (...)
            {
                throw;
            }
        }
    }
    }
}, object{
    object::pair{std::string("name"), std::string("Service.getTokenAPIData: Fetches ETH token data using DexScreener")}, 
    object::pair{std::string("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        (as<any>(service))["dexscreenerClient"] = object{
            object::pair{std::string("search"), [=]() mutable
            {
                return (object{
                    object::pair{std::string("pairs"), array<any>{ as<any>(object{
                        object::pair{std::string("baseToken"), object{
                            object::pair{std::string("name"), std::string("EthCoin")}, 
                            object::pair{std::string("symbol"), std::string("ECN")}
                        }}, 
                        object::pair{std::string("priceUsd"), std::string("2000.0")}, 
                        object::pair{std::string("liquidity"), object{
                            object::pair{std::string("usd"), 100000}
                        }}, 
                        object::pair{std::string("marketCap"), 200000000}, 
                        object::pair{std::string("chainId"), std::string("ethereum")}, 
                        object::pair{std::string("priceChange"), object{
                            object::pair{std::string("h24"), 5}, 
                            object::pair{std::string("h6"), 2}, 
                            object::pair{std::string("h1"), 1}, 
                            object::pair{std::string("m5"), 0.1}
                        }}
                    }) }}
                });
            }
            }
        };
        (as<any>(service))["birdeyeClient"] = object{
            object::pair{std::string("fetchTokenOverview"), [=]() mutable
            {
                throw any(std::make_shared<Error>(std::string("Birdeye should not be called for ETH")));
            }
            }
        };
        auto data = std::async([=]() { service->getTokenAPIData(std::string("TESTETHADDR"), SupportedChain->ETHEREUM); });
        if (OR((OR((!data), (data["name"] != std::string("EthCoin")))), (data["currentPrice"] != 2000))) {
            throw any(std::make_shared<Error>(std::string("getTokenAPIData failed for ETH: ") + JSON->stringify(data) + string_empty));
        }
        logger->info(std::string("Service.getTokenAPIData: ETH token data success - Passed"));
    }
    }
}, object{
    object::pair{std::string("name"), std::string("Service.getTokenAPIData: Fetches BASE token data using DexScreener")}, 
    object::pair{std::string("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        (as<any>(service))["dexscreenerClient"] = object{
            object::pair{std::string("search"), [=]() mutable
            {
                return (object{
                    object::pair{std::string("pairs"), array<any>{ as<any>(object{
                        object::pair{std::string("baseToken"), object{
                            object::pair{std::string("name"), std::string("BaseCoin")}, 
                            object::pair{std::string("symbol"), std::string("BCN")}
                        }}, 
                        object::pair{std::string("priceUsd"), std::string("100.0")}, 
                        object::pair{std::string("liquidity"), object{
                            object::pair{std::string("usd"), 50000}
                        }}, 
                        object::pair{std::string("marketCap"), 10000000}, 
                        object::pair{std::string("chainId"), std::string("base")}, 
                        object::pair{std::string("priceChange"), object{
                            object::pair{std::string("h24"), 2}, 
                            object::pair{std::string("h6"), 1}, 
                            object::pair{std::string("h1"), 0.5}, 
                            object::pair{std::string("m5"), 0.05}
                        }}
                    }) }}
                });
            }
            }
        };
        auto data = std::async([=]() { service->getTokenAPIData(std::string("TESTBASEADDR"), SupportedChain->BASE); });
        if (OR((OR((!data), (data["name"] != std::string("BaseCoin")))), (data["currentPrice"] != 100))) {
            throw any(std::make_shared<Error>(std::string("getTokenAPIData failed for BASE: ") + JSON->stringify(data) + string_empty));
        }
        logger->info(std::string("Service.getTokenAPIData: BASE token data success - Passed"));
    }
    }
} };
array<std::shared_ptr<TestCase>> isLikelyScamOrRugTestCases = array<std::shared_ptr<TestCase>>{ object{
    object::pair{std::string("name"), std::string("Service.isLikelyScamOrRug: Flags severe price drop (>90%)")}, 
    object::pair{std::string("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        auto recTimestamp = Date->now() - 10 * 60 * 1000;
        auto tokenData = object{
            object::pair{std::string("currentPrice"), 1}, 
            object::pair{std::string("priceHistory"), array<object>{ object{
                object::pair{std::string("timestamp"), recTimestamp - 1000}, 
                object::pair{std::string("price"), 100}
            }, object{
                object::pair{std::string("timestamp"), recTimestamp}, 
                object::pair{std::string("price"), 1}
            } }}, 
            object::pair{std::string("liquidity"), 10000}, 
            object::pair{std::string("marketCap"), 100000}
        };
        auto result = std::async([=]() { service->isLikelyScamOrRug(tokenData, recTimestamp - 2000); });
        if (!result) throw any(std::make_shared<Error>(std::string("Severe price drop not flagged as scam/rug")));
        logger->info(std::string("Service.isLikelyScamOrRug: Severe price drop - Passed"));
    }
    }
}, object{
    object::pair{std::string("name"), std::string("Service.isLikelyScamOrRug: Flags based on isKnownScam field")}, 
    object::pair{std::string("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        auto tokenData = object{
            object::pair{std::string("isKnownScam"), true}, 
            object::pair{std::string("currentPrice"), 10}, 
            object::pair{std::string("name"), std::string("ScamCoin")}, 
            object::pair{std::string("symbol"), std::string("SCM")}
        };
        auto result = std::async([=]() { service->isLikelyScamOrRug(tokenData, Date->now()); });
        if (!result) throw any(std::make_shared<Error>(std::string("Known scam not flagged.")));
        logger->info(std::string("Service.isLikelyScamOrRug: Known scam field - Passed"));
    }
    }
}, object{
    object::pair{std::string("name"), std::string("Service.isLikelyScamOrRug: Flags critical liquidity (<$500)")}, 
    object::pair{std::string("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        auto tokenData = object{
            object::pair{std::string("currentPrice"), 1}, 
            object::pair{std::string("liquidity"), 400}, 
            object::pair{std::string("marketCap"), 10000}, 
            object::pair{std::string("name"), std::string("LowLiq")}, 
            object::pair{std::string("symbol"), std::string("LLQ")}
        };
        auto result = std::async([=]() { service->isLikelyScamOrRug(tokenData, Date->now()); });
        if (!result) throw any(std::make_shared<Error>(std::string("Critical liquidity not flagged.")));
        logger->info(std::string("Service.isLikelyScamOrRug: Critical liquidity - Passed"));
    }
    }
}, object{
    object::pair{std::string("name"), std::string("Service.isLikelyScamOrRug: Flags very low liquidity ratio (<0.5%)")}, 
    object::pair{std::string("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        auto tokenData = object{
            object::pair{std::string("currentPrice"), 1}, 
            object::pair{std::string("liquidity"), 400}, 
            object::pair{std::string("marketCap"), 100000}, 
            object::pair{std::string("name"), std::string("LowRatio")}, 
            object::pair{std::string("symbol"), std::string("LRT")}
        };
        auto result = std::async([=]() { service->isLikelyScamOrRug(tokenData, Date->now()); });
        if (!result) throw any(std::make_shared<Error>(std::string("Very low liquidity ratio not flagged as scam/rug")));
        logger->info(std::string("Service.isLikelyScamOrRug: Very low liquidity ratio - Passed"));
    }
    }
}, object{
    object::pair{std::string("name"), std::string("Service.isLikelyScamOrRug: Not flagged for healthy token")}, 
    object::pair{std::string("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        auto tokenData = object{
            object::pair{std::string("currentPrice"), 100}, 
            object::pair{std::string("liquidity"), 500000}, 
            object::pair{std::string("marketCap"), 5000000}, 
            object::pair{std::string("priceHistory"), array<object>{ object{
                object::pair{std::string("timestamp"), Date->now() - 1000}, 
                object::pair{std::string("price"), 95}
            }, object{
                object::pair{std::string("timestamp"), Date->now()}, 
                object::pair{std::string("price"), 100}
            } }}, 
            object::pair{std::string("name"), std::string("Healthy")}, 
            object::pair{std::string("symbol"), std::string("HLT")}
        };
        auto result = std::async([=]() { service->isLikelyScamOrRug(tokenData, Date->now() - 2000); });
        if (result) throw any(std::make_shared<Error>(std::string("Healthy token incorrectly flagged as scam/rug")));
        logger->info(std::string("Service.isLikelyScamOrRug: Healthy token - Passed"));
    }
    }
} };
array<std::shared_ptr<TestCase>> evaluatePerfTestCases = array<std::shared_ptr<TestCase>>{ object{
    object::pair{std::string("name"), std::string("Service.evaluatePerf: BUY rec profit, not scam")}, 
    object::pair{std::string("fn"), [=](auto runtime) mutable
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
                    object::pair{std::string("id"), asUUID(uuidv4())}, 
                    object::pair{std::string("userId"), testUserIdGlobalForService}, 
                    object::pair{std::string("messageId"), asUUID(uuidv4())}, 
                    object::pair{std::string("timestamp"), recTimestamp}, 
                    object::pair{std::string("tokenAddress"), std::string("ADDR_PROFIT")}, 
                    object::pair{std::string("chain"), SupportedChain->SOLANA}, 
                    object::pair{std::string("conviction"), Conviction->HIGH}, 
                    object::pair{std::string("rawMessageQuote"), std::string("quote")}, 
                    object::pair{std::string("priceAtRecommendation"), 10}
                };
                auto tokenData = object{
                    object::pair{std::string("currentPrice"), 15}, 
                    object::pair{std::string("priceHistory"), array<object>{ object{
                        object::pair{std::string("timestamp"), recTimestamp}, 
                        object::pair{std::string("price"), 10}
                    }, object{
                        object::pair{std::string("timestamp"), Date->now()}, 
                        object::pair{std::string("price"), 15}
                    } }}, 
                    object::pair{std::string("name"), std::string("ProfitCoin")}, 
                    object::pair{std::string("symbol"), std::string("PFT")}
                };
                (as<any>(service))["isLikelyScamOrRug"] = [=]() mutable
                {
                    return false;
                };
                auto metrics = std::async([=]() { service->evaluateRecommendationPerformance(as<std::shared_ptr<Recommendation>>(utils::assign(object{
                    , 
                    object::pair{std::string("recommendationType"), std::string("BUY")}
                }, baseRec)), tokenData); });
                if (Math->abs((OR((metrics->potentialProfitPercent), (0))) - 50) > 0.01) throw any(std::make_shared<Error>(std::string("BUY profit mismatch")));
                if (metrics->isScamOrRug) throw any(std::make_shared<Error>(std::string("Incorrectly flagged as scam")));
                logger->info(std::string("Service.evaluatePerf: BUY profit - Passed"));
            }
            catch (...)
            {
                throw;
            }
        }
    }
    }
}, object{
    object::pair{std::string("name"), std::string("Service.evaluatePerf: BUY rec for rugged token results in -99 profit percent")}, 
    object::pair{std::string("fn"), [=](auto runtime) mutable
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
                    object::pair{std::string("id"), asUUID(uuidv4())}, 
                    object::pair{std::string("userId"), testUserIdGlobalForService}, 
                    object::pair{std::string("messageId"), asUUID(uuidv4())}, 
                    object::pair{std::string("timestamp"), recTimestamp}, 
                    object::pair{std::string("tokenAddress"), std::string("ADDR_RUG_BUY")}, 
                    object::pair{std::string("chain"), SupportedChain->SOLANA}, 
                    object::pair{std::string("conviction"), Conviction->HIGH}, 
                    object::pair{std::string("rawMessageQuote"), std::string("quote")}, 
                    object::pair{std::string("priceAtRecommendation"), 10}
                };
                auto tokenData = object{
                    object::pair{std::string("currentPrice"), 0.1}, 
                    object::pair{std::string("priceHistory"), array<object>{ object{
                        object::pair{std::string("timestamp"), recTimestamp}, 
                        object::pair{std::string("price"), 10}
                    }, object{
                        object::pair{std::string("timestamp"), Date->now()}, 
                        object::pair{std::string("price"), 0.1}
                    } }}, 
                    object::pair{std::string("name"), std::string("RugCoin")}, 
                    object::pair{std::string("symbol"), std::string("RUG")}
                };
                (as<any>(service))["isLikelyScamOrRug"] = [=]() mutable
                {
                    return true;
                };
                auto metrics = std::async([=]() { service->evaluateRecommendationPerformance(as<std::shared_ptr<Recommendation>>(utils::assign(object{
                    , 
                    object::pair{std::string("recommendationType"), std::string("BUY")}
                }, baseRec)), tokenData); });
                if (metrics->potentialProfitPercent != -99) throw any(std::make_shared<Error>(std::string("BUY scam profit mismatch, expected -99")));
                if (!metrics->isScamOrRug) throw any(std::make_shared<Error>(std::string("Not flagged as scam when it should be")));
                logger->info(std::string("Service.evaluatePerf: BUY rec rugged - Passed"));
            }
            catch (...)
            {
                throw;
            }
        }
    }
    }
}, object{
    object::pair{std::string("name"), std::string("Service.evaluatePerf: SELL rec, avoided loss correctly (price dropped)")}, 
    object::pair{std::string("fn"), [=](auto runtime) mutable
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
                    object::pair{std::string("id"), asUUID(uuidv4())}, 
                    object::pair{std::string("userId"), testUserIdGlobalForService}, 
                    object::pair{std::string("messageId"), asUUID(uuidv4())}, 
                    object::pair{std::string("timestamp"), recTimestamp}, 
                    object::pair{std::string("tokenAddress"), std::string("ADDR_AVOID_LOSS")}, 
                    object::pair{std::string("chain"), SupportedChain->SOLANA}, 
                    object::pair{std::string("conviction"), Conviction->MEDIUM}, 
                    object::pair{std::string("rawMessageQuote"), std::string("sell this")}, 
                    object::pair{std::string("priceAtRecommendation"), 100}
                };
                auto tokenData = object{
                    object::pair{std::string("currentPrice"), 20}, 
                    object::pair{std::string("priceHistory"), array<object>{ object{
                        object::pair{std::string("timestamp"), recTimestamp}, 
                        object::pair{std::string("price"), 100}
                    }, object{
                        object::pair{std::string("timestamp"), Date->now()}, 
                        object::pair{std::string("price"), 20}
                    } }}, 
                    object::pair{std::string("name"), std::string("DropCoin")}, 
                    object::pair{std::string("symbol"), std::string("DRP")}
                };
                (as<any>(service))["isLikelyScamOrRug"] = [=]() mutable
                {
                    return false;
                };
                auto metrics = std::async([=]() { service->evaluateRecommendationPerformance(as<std::shared_ptr<Recommendation>>(utils::assign(object{
                    , 
                    object::pair{std::string("recommendationType"), std::string("SELL")}
                }, baseRec)), tokenData); });
                if (Math->abs((OR((metrics->avoidedLossPercent), (0))) - 80) > 0.01) throw any(std::make_shared<Error>(std::string("SELL avoided loss mismatch. Expected 80%")));
                logger->info(std::string("Service.evaluatePerf: SELL avoided loss - Passed"));
            }
            catch (...)
            {
                throw;
            }
        }
    }
    }
}, object{
    object::pair{std::string("name"), std::string("Service.evaluatePerf: SELL rec, missed gain (price pumped)")}, 
    object::pair{std::string("fn"), [=](auto runtime) mutable
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
                    object::pair{std::string("id"), asUUID(uuidv4())}, 
                    object::pair{std::string("userId"), testUserIdGlobalForService}, 
                    object::pair{std::string("messageId"), asUUID(uuidv4())}, 
                    object::pair{std::string("timestamp"), recTimestamp}, 
                    object::pair{std::string("tokenAddress"), std::string("ADDR_MISSED_GAIN")}, 
                    object::pair{std::string("chain"), SupportedChain->SOLANA}, 
                    object::pair{std::string("conviction"), Conviction->MEDIUM}, 
                    object::pair{std::string("rawMessageQuote"), std::string("sell this now")}, 
                    object::pair{std::string("priceAtRecommendation"), 50}
                };
                auto tokenData = object{
                    object::pair{std::string("currentPrice"), 100}, 
                    object::pair{std::string("priceHistory"), array<object>{ object{
                        object::pair{std::string("timestamp"), recTimestamp}, 
                        object::pair{std::string("price"), 50}
                    }, object{
                        object::pair{std::string("timestamp"), Date->now()}, 
                        object::pair{std::string("price"), 100}
                    } }}, 
                    object::pair{std::string("name"), std::string("PumpCoin")}, 
                    object::pair{std::string("symbol"), std::string("PMP")}
                };
                (as<any>(service))["isLikelyScamOrRug"] = [=]() mutable
                {
                    return false;
                };
                auto metrics = std::async([=]() { service->evaluateRecommendationPerformance(as<std::shared_ptr<Recommendation>>(utils::assign(object{
                    , 
                    object::pair{std::string("recommendationType"), std::string("SELL")}
                }, baseRec)), tokenData); });
                if (Math->abs((OR((metrics->avoidedLossPercent), (0))) - -100) > 0.01) throw any(std::make_shared<Error>(std::string("SELL missed gain mismatch. Expected -100%")));
                logger->info(std::string("Service.evaluatePerf: SELL missed gain - Passed"));
            }
            catch (...)
            {
                throw;
            }
        }
    }
    }
}, object{
    object::pair{std::string("name"), std::string("Service.evaluatePerf: SELL rec, correctly identified scam (avoidedLossPercent = 99)")}, 
    object::pair{std::string("fn"), [=](auto runtime) mutable
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
                    object::pair{std::string("id"), asUUID(uuidv4())}, 
                    object::pair{std::string("userId"), testUserIdGlobalForService}, 
                    object::pair{std::string("messageId"), asUUID(uuidv4())}, 
                    object::pair{std::string("timestamp"), recTimestamp}, 
                    object::pair{std::string("tokenAddress"), std::string("ADDR_RUG_SELL")}, 
                    object::pair{std::string("chain"), SupportedChain->SOLANA}, 
                    object::pair{std::string("conviction"), Conviction->HIGH}, 
                    object::pair{std::string("rawMessageQuote"), std::string("quote")}, 
                    object::pair{std::string("priceAtRecommendation"), 10}
                };
                auto tokenData = object{
                    object::pair{std::string("currentPrice"), 0.1}, 
                    object::pair{std::string("name"), std::string("SellScam")}, 
                    object::pair{std::string("symbol"), std::string("SSC")}
                };
                (as<any>(service))["isLikelyScamOrRug"] = [=]() mutable
                {
                    return true;
                };
                auto metrics = std::async([=]() { service->evaluateRecommendationPerformance(as<std::shared_ptr<Recommendation>>(utils::assign(object{
                    , 
                    object::pair{std::string("recommendationType"), std::string("SELL")}
                }, baseRec)), tokenData); });
                if (metrics->avoidedLossPercent != 99) throw any(std::make_shared<Error>(std::string("SELL scam avoidedLossPercent mismatch, expected 99")));
                if (!metrics->isScamOrRug) throw any(std::make_shared<Error>(std::string("Not flagged as scam for SELL when it should be")));
                logger->info(std::string("Service.evaluatePerf: SELL rec rugged, correct ID - Passed"));
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
    object::pair{std::string("name"), std::string("Service.getLeaderboardData: Returns sorted entries with ranks")}, 
    object::pair{std::string("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        shared user1Id = asUUID(uuidv4());
        shared user2Id = asUUID(uuidv4());
        shared user3Id = asUUID(uuidv4());
        (as<any>(runtime->getAgents)) = [=]() mutable
        {
            return array<object>{ object{
                object::pair{std::string("id"), user1Id}, 
                object::pair{std::string("names"), array<string>{ std::string("UserA") }}, 
                object::pair{std::string("metadata"), object{}}
            }, object{
                object::pair{std::string("id"), user2Id}, 
                object::pair{std::string("names"), array<string>{ std::string("UserB") }}, 
                object::pair{std::string("metadata"), object{}}
            }, object{
                object::pair{std::string("id"), user3Id}, 
                object::pair{std::string("names"), array<string>{ std::string("UserC") }}, 
                object::pair{std::string("metadata"), object{}}
            } };
        };
        shared user1Comp = createFullMockComponentForSvcTest(user1Id, object{
            object::pair{std::string("version"), std::string("1.0.0")}, 
            object::pair{std::string("userId"), user1Id}, 
            object::pair{std::string("trustScore"), 75}, 
            object::pair{std::string("recommendations"), array<any>()}, 
            object::pair{std::string("lastTrustScoreCalculationTimestamp"), Date->now()}
        }, runtime);
        shared user2Comp = createFullMockComponentForSvcTest(user2Id, object{
            object::pair{std::string("version"), std::string("1.0.0")}, 
            object::pair{std::string("userId"), user2Id}, 
            object::pair{std::string("trustScore"), 90}, 
            object::pair{std::string("recommendations"), array<any>()}, 
            object::pair{std::string("lastTrustScoreCalculationTimestamp"), Date->now()}
        }, runtime);
        shared user3Comp = createFullMockComponentForSvcTest(user3Id, object{
            object::pair{std::string("version"), std::string("1.0.0")}, 
            object::pair{std::string("userId"), user3Id}, 
            object::pair{std::string("trustScore"), 80}, 
            object::pair{std::string("recommendations"), array<any>()}, 
            object::pair{std::string("lastTrustScoreCalculationTimestamp"), Date->now()}
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
                object::pair{std::string("id"), user1Id}, 
                object::pair{std::string("names"), array<string>{ std::string("UserA") }}, 
                object::pair{std::string("agentId"), runtime->agentId}
            };
            if (id == user2Id) return object{
                object::pair{std::string("id"), user2Id}, 
                object::pair{std::string("names"), array<string>{ std::string("UserB") }}, 
                object::pair{std::string("agentId"), runtime->agentId}
            };
            if (id == user3Id) return object{
                object::pair{std::string("id"), user3Id}, 
                object::pair{std::string("names"), array<string>{ std::string("UserC") }}, 
                object::pair{std::string("agentId"), runtime->agentId}
            };
            return nullptr;
        };
        auto leaderboard = std::async([=]() { service->getLeaderboardData(runtime); });
        if (leaderboard->get_length() != 3) throw any(std::make_shared<Error>(std::string("Expected 3 leaderboard entries, got ") + leaderboard->get_length() + string_empty));
        if (OR((OR((const_(leaderboard)[0]->userId != user2Id), (const_(leaderboard)[0]->rank != 1))), (const_(leaderboard)[0]->trustScore != 90))) throw any(std::make_shared<Error>(std::string("Leaderboard error for rank 1: ") + JSON->stringify(const_(leaderboard)[0]) + string_empty));
        if (OR((OR((const_(leaderboard)[1]->userId != user3Id), (const_(leaderboard)[1]->rank != 2))), (const_(leaderboard)[1]->trustScore != 80))) throw any(std::make_shared<Error>(std::string("Leaderboard error for rank 2: ") + JSON->stringify(const_(leaderboard)[1]) + string_empty));
        if (OR((OR((const_(leaderboard)[2]->userId != user1Id), (const_(leaderboard)[2]->rank != 3))), (const_(leaderboard)[2]->trustScore != 75))) throw any(std::make_shared<Error>(std::string("Leaderboard error for rank 3: ") + JSON->stringify(const_(leaderboard)[2]) + string_empty));
        logger->info(std::string("Service.getLeaderboardData: Sorted entries - Passed"));
    }
    }
}, object{
    object::pair{std::string("name"), std::string("Service.getLeaderboardData: Handles empty agents list")}, 
    object::pair{std::string("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        (as<any>(runtime->getAgents)) = [=]() mutable
        {
            return array<any>();
        };
        auto leaderboard = std::async([=]() { service->getLeaderboardData(runtime); });
        if (leaderboard->get_length() != 0) throw any(std::make_shared<Error>(std::string("Expected 0 leaderboard entries for no agents")));
        logger->info(std::string("Service.getLeaderboardData: Empty agents list - Passed"));
    }
    }
}, object{
    object::pair{std::string("name"), std::string("Service.getLeaderboardData: Handles agents with no trust profile component")}, 
    object::pair{std::string("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        shared user1Id = asUUID(uuidv4());
        (as<any>(runtime->getAgents)) = [=]() mutable
        {
            return array<object>{ object{
                object::pair{std::string("id"), user1Id}, 
                object::pair{std::string("names"), array<string>{ std::string("UserA") }}, 
                object::pair{std::string("metadata"), object{}}
            } };
        };
        (as<any>(runtime->getComponent)) = [=]() mutable
        {
            return nullptr;
        };
        (as<any>(runtime->getEntityById)) = [=](auto id) mutable
        {
            return (object{
                object::pair{std::string("id"), std::string("id")}, 
                object::pair{std::string("names"), array<string>{ std::string("UserA") }}, 
                object::pair{std::string("agentId"), runtime->agentId}
            });
        };
        auto leaderboard = std::async([=]() { service->getLeaderboardData(runtime); });
        if (leaderboard->get_length() != 0) throw any(std::make_shared<Error>(std::string("Expected 0 leaderboard entries when no profiles found")));
        logger->info(std::string("Service.getLeaderboardData: No profiles found - Passed"));
    }
    }
} };
std::shared_ptr<TestSuite> serviceTestSuite = object{
    object::pair{std::string("name"), std::string("CommunityInvestorService Tests (ElizaOS Runner Format)")}, 
    object::pair{std::string("tests"), array<any>{ calculateUserTrustScoreTestCases, resolveTickerTestCases, getTokenAPIDataTestCases, isLikelyScamOrRugTestCases, evaluatePerfTestCases, getLeaderboardDataTestCases }}
};

void Main(void)
{
}

MAIN
