#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/communityInvestor/tests/trustScore.h"

any testUserIdGlobalTrustScore = asUUID(uuidv4());
any testWorldId = asUUID(uuidv4());
std::function<any(any, any, any)> createFullMockComponentForTrustScore = [=](auto userId, auto profileData, auto runtime) mutable
{
    return (object{
        object::pair{std::string("id"), asUUID(uuidv4())}, 
        object::pair{std::string("entityId"), userId}, 
        object::pair{std::string("agentId"), runtime->agentId}, 
        object::pair{std::string("worldId"), testWorldId}, 
        object::pair{std::string("roomId"), asUUID(uuidv4())}, 
        object::pair{std::string("sourceEntityId"), runtime->agentId}, 
        object::pair{std::string("type"), TRUST_MARKETPLACE_COMPONENT_TYPE}, 
        object::pair{std::string("createdAt"), Date->now()}, 
        object::pair{std::string("data"), profileData}
    });
};
std::function<any(string, double, any, any, any, double)> createRecForTrustScore = [=](auto id, auto timestamp, P2 type, auto conviction, auto metric, auto priceAtRec = undefined) mutable
{
    return (object{
        object::pair{std::string("id"), asUUID(uuidv4())}, 
        object::pair{std::string("userId"), testUserIdGlobalTrustScore}, 
        object::pair{std::string("messageId"), asUUID(uuidv4())}, 
        object::pair{std::string("timestamp"), std::string("timestamp")}, 
        object::pair{std::string("tokenAddress"), std::string("TOKEN_ADDR_") + id + string_empty}, 
        object::pair{std::string("chain"), SupportedChain->SOLANA}, 
        object::pair{std::string("recommendationType"), type}, 
        object::pair{std::string("conviction"), std::string("conviction")}, 
        object::pair{std::string("rawMessageQuote"), std::string("Quote for ") + id + string_empty}, 
        object::pair{std::string("priceAtRecommendation"), priceAtRec}, 
        object::pair{std::string("metrics"), metric}, 
        object::pair{std::string("processedForTradeDecision"), true}
    });
};
array<std::shared_ptr<TestCase>> recencyWeightTests = array<std::shared_ptr<TestCase>>{ object{
    object::pair{std::string("name"), std::string("TrustScore.Recency: 1.0 for very recent")}, 
    object::pair{std::string("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        auto weight = service->getRecencyWeight(Date->now());
        if (Math->abs(weight - 1) > 0.015) {
            throw any(std::make_shared<Error>(std::string("Recency Weight: Expected ~1.0, got ") + weight + string_empty));
        }
        logger->info(std::string("TrustScore.Recency: 1.0 - Passed"));
    }
    }
}, object{
    object::pair{std::string("name"), std::string("TrustScore.Recency: 0.1 for older than RECENCY_WEIGHT_MONTHS")}, 
    object::pair{std::string("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        auto recencyMonths = 6;
        auto sixMonthsInMillis = recencyMonths * 30.44 * 24 * 60 * 60 * 1000;
        auto weight = service->getRecencyWeight(Date->now() - sixMonthsInMillis - 1000);
        if (Math->abs(weight - 0.1) > 0.01) {
            throw any(std::make_shared<Error>(std::string("Recency Weight: Expected ~0.1, got ") + weight + string_empty));
        }
        logger->info(std::string("TrustScore.Recency: 0.1 older - Passed"));
    }
    }
} };
array<std::shared_ptr<TestCase>> convictionWeightTests = array<std::shared_ptr<TestCase>>{ object{
    object::pair{std::string("name"), std::string("TrustScore.Conviction: Correct weights per level")}, 
    object::pair{std::string("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        if (service->getConvictionWeight(Conviction->NONE) != 0.25) throw any(std::make_shared<Error>(std::string("NONE weight mismatch")));
        if (service->getConvictionWeight(Conviction->LOW) != 0.5) throw any(std::make_shared<Error>(std::string("LOW weight mismatch")));
        if (service->getConvictionWeight(Conviction->MEDIUM) != 1) throw any(std::make_shared<Error>(std::string("MEDIUM mismatch")));
        if (service->getConvictionWeight(Conviction->HIGH) != 1.5) throw any(std::make_shared<Error>(std::string("HIGH mismatch")));
        logger->info(std::string("TrustScore.Conviction: Weights - Passed"));
    }
    }
} };
array<std::shared_ptr<TestCase>> calculateScoreLogicTests = array<std::shared_ptr<TestCase>>{ object{
    object::pair{std::string("name"), std::string("TrustScore.Calc: New user, no recs, score 0")}, 
    object::pair{std::string("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        shared createdCompData = nullptr;
        auto originalGetComponent = runtime->getComponent;
        auto originalCreateComponent = runtime->createComponent;
        {
            utils::finally __finally4573_4695([&]() mutable
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
                    createdCompData = as<std::shared_ptr<UserTrustProfile>>(comp->data);
                    return true;
                };
                std::async([=]() { service->calculateUserTrustScore(testUserIdGlobalTrustScore, runtime, testWorldId); });
                if (!createdCompData) throw any(std::make_shared<Error>(std::string("createComponent was not effectively called")));
                if (OR((createdCompData["trustScore"] != 0), ((OR((createdCompData["recommendations"]), (array<any>())))["length"] != 0))) {
                    throw any(std::make_shared<Error>(std::string("New user score expected 0, got ") + createdCompData["trustScore"] + string_empty));
                }
                logger->info(std::string("TrustScore.Calc: New user - Passed"));
            }
            catch (...)
            {
                throw;
            }
        }
    }
    }
}, object{
    object::pair{std::string("name"), std::string("TrustScore.Calc: Single good BUY (+50% perf, recent, high conv)")}, 
    object::pair{std::string("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        shared recTimestamp = Date->now() - 1 * 24 * 60 * 60 * 1000;
        auto basePerformance = 50;
        auto recs = array<any>{ createRecForTrustScore(std::string("goodBuy"), recTimestamp, std::string("BUY"), Conviction->HIGH, object{
            object::pair{std::string("potentialProfitPercent"), basePerformance}, 
            object::pair{std::string("evaluationTimestamp"), recTimestamp}, 
            object::pair{std::string("isScamOrRug"), false}
        }, 10) };
        auto initialProfile = object{
            object::pair{std::string("version"), std::string("1.0.0")}, 
            object::pair{std::string("userId"), testUserIdGlobalTrustScore}, 
            object::pair{std::string("trustScore"), 0}, 
            object::pair{std::string("lastTrustScoreCalculationTimestamp"), 0}, 
            object::pair{std::string("recommendations"), recs}
        };
        shared initialComponent = createFullMockComponentForTrustScore(testUserIdGlobalTrustScore, initialProfile, runtime);
        shared updatedCompData = nullptr;
        auto originalGetComponent = runtime->getComponent;
        auto originalUpdateComponent = runtime->updateComponent;
        auto originalServiceGetTokenAPIData = (as<any>(service))["getTokenAPIData"]["bind"](service);
        auto originalServiceIsLikelyScamOrRug = (as<any>(service))["isLikelyScamOrRug"]["bind"](service);
        {
            utils::finally __finally7291_7771([&]() mutable
            {
                runtime->getComponent = originalGetComponent;
                runtime->updateComponent = originalUpdateComponent;
                if (originalServiceGetTokenAPIData) (as<any>(service))["getTokenAPIData"] = originalServiceGetTokenAPIData; else (as<any>(service)).Delete("getTokenAPIData");
                if (originalServiceIsLikelyScamOrRug) (as<any>(service))["isLikelyScamOrRug"] = originalServiceIsLikelyScamOrRug; else (as<any>(service)).Delete("isLikelyScamOrRug");
            });
            try
            {
                runtime->getComponent = [=]() mutable
                {
                    return initialComponent;
                };
                runtime->updateComponent = [=](auto comp) mutable
                {
                    updatedCompData = as<std::shared_ptr<UserTrustProfile>>(comp->data);
                };
                (as<any>(service))["getTokenAPIData"] = [=]() mutable
                {
                    return as<std::shared_ptr<TokenAPIData>>((object{
                        object::pair{std::string("currentPrice"), 15}, 
                        object::pair{std::string("priceHistory"), array<object>{ object{
                            object::pair{std::string("timestamp"), recTimestamp}, 
                            object::pair{std::string("price"), 10}
                        }, object{
                            object::pair{std::string("timestamp"), Date->now()}, 
                            object::pair{std::string("price"), 15}
                        } }}, 
                        object::pair{std::string("name"), std::string("TestCoin")}, 
                        object::pair{std::string("symbol"), std::string("TST")}
                    }));
                };
                (as<any>(service))["isLikelyScamOrRug"] = [=]() mutable
                {
                    return false;
                };
                std::async([=]() { service->calculateUserTrustScore(testUserIdGlobalTrustScore, runtime, testWorldId); });
                if (!updatedCompData) throw any(std::make_shared<Error>(std::string("updateComponent was not effectively called")));
                auto expectedFinalScore = Math->max(-100, Math->min(100, basePerformance));
                if (Math->abs(updatedCompData["trustScore"] - expectedFinalScore) > 0.01) {
                    throw any(std::make_shared<Error>(std::string("Expected score ") + expectedFinalScore->toFixed(2) + std::string(", got ") + updatedCompData["trustScore"]["toFixed"](2) + string_empty));
                }
                logger->info(std::string("TrustScore.Calc: Single good BUY - Passed"));
            }
            catch (...)
            {
                throw;
            }
        }
    }
    }
}, object{
    object::pair{std::string("name"), std::string("TrustScore.Calc: Single scam BUY (-100 perf, recent, med conv)")}, 
    object::pair{std::string("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        auto recTimestamp = Date->now() - 2 * 24 * 60 * 60 * 1000;
        auto recs = array<any>{ createRecForTrustScore(std::string("scamBuy"), recTimestamp, std::string("BUY"), Conviction->MEDIUM, object{
            object::pair{std::string("potentialProfitPercent"), -99}, 
            object::pair{std::string("isScamOrRug"), true}, 
            object::pair{std::string("evaluationTimestamp"), recTimestamp}
        }, 10) };
        auto initialProfile = object{
            object::pair{std::string("version"), std::string("1.0.0")}, 
            object::pair{std::string("userId"), testUserIdGlobalTrustScore}, 
            object::pair{std::string("trustScore"), 0}, 
            object::pair{std::string("lastTrustScoreCalculationTimestamp"), 0}, 
            object::pair{std::string("recommendations"), recs}
        };
        shared initialComponent = createFullMockComponentForTrustScore(testUserIdGlobalTrustScore, initialProfile, runtime);
        shared updatedCompData = nullptr;
        auto originalGetComponent = runtime->getComponent;
        auto originalUpdateComponent = runtime->updateComponent;
        auto originalServiceGetTokenAPIData = (as<any>(service))["getTokenAPIData"]["bind"](service);
        auto originalServiceIsLikelyScamOrRug = (as<any>(service))["isLikelyScamOrRug"]["bind"](service);
        {
            utils::finally __finally9983_10463([&]() mutable
            {
                runtime->getComponent = originalGetComponent;
                runtime->updateComponent = originalUpdateComponent;
                if (originalServiceGetTokenAPIData) (as<any>(service))["getTokenAPIData"] = originalServiceGetTokenAPIData; else (as<any>(service)).Delete("getTokenAPIData");
                if (originalServiceIsLikelyScamOrRug) (as<any>(service))["isLikelyScamOrRug"] = originalServiceIsLikelyScamOrRug; else (as<any>(service)).Delete("isLikelyScamOrRug");
            });
            try
            {
                runtime->getComponent = [=]() mutable
                {
                    return initialComponent;
                };
                runtime->updateComponent = [=](auto comp) mutable
                {
                    updatedCompData = as<std::shared_ptr<UserTrustProfile>>(comp->data);
                };
                (as<any>(service))["getTokenAPIData"] = [=]() mutable
                {
                    return as<std::shared_ptr<TokenAPIData>>((object{
                        object::pair{std::string("currentPrice"), 0.01}, 
                        object::pair{std::string("name"), std::string("ScamCoin")}, 
                        object::pair{std::string("symbol"), std::string("SCM")}
                    }));
                };
                (as<any>(service))["isLikelyScamOrRug"] = [=]() mutable
                {
                    return true;
                };
                std::async([=]() { service->calculateUserTrustScore(testUserIdGlobalTrustScore, runtime, testWorldId); });
                if (!updatedCompData) throw any(std::make_shared<Error>(std::string("updateComponent was not effectively called for scam BUY")));
                if (Math->abs(updatedCompData["trustScore"] - -100) > 0.01) {
                    throw any(std::make_shared<Error>(std::string("Expected score ~-100 for scam BUY, got ") + updatedCompData["trustScore"]["toFixed"](2) + string_empty));
                }
                logger->info(std::string("TrustScore.Calc: Single scam BUY - Passed"));
            }
            catch (...)
            {
                throw;
            }
        }
    }
    }
}, object{
    object::pair{std::string("name"), std::string("TrustScore.Calc: Score clamps at +100")}, 
    object::pair{std::string("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        auto recs = array<any>{ createRecForTrustScore(std::string("superGood"), Date->now(), std::string("BUY"), Conviction->HIGH, object{
            object::pair{std::string("potentialProfitPercent"), 500}, 
            object::pair{std::string("evaluationTimestamp"), Date->now()}, 
            object::pair{std::string("isScamOrRug"), false}
        }, 1) };
        auto initialProfile = object{
            object::pair{std::string("version"), std::string("1.0.0")}, 
            object::pair{std::string("userId"), testUserIdGlobalTrustScore}, 
            object::pair{std::string("trustScore"), 0}, 
            object::pair{std::string("lastTrustScoreCalculationTimestamp"), 0}, 
            object::pair{std::string("recommendations"), recs}
        };
        shared initialComponent = createFullMockComponentForTrustScore(testUserIdGlobalTrustScore, initialProfile, runtime);
        shared updatedCompData = nullptr;
        auto originalGetComponent = runtime->getComponent;
        auto originalUpdateComponent = runtime->updateComponent;
        auto originalServiceGetTokenAPIData = (as<any>(service))["getTokenAPIData"]["bind"](service);
        auto originalServiceIsLikelyScamOrRug = (as<any>(service))["isLikelyScamOrRug"]["bind"](service);
        {
            utils::finally __finally12552_13032([&]() mutable
            {
                runtime->getComponent = originalGetComponent;
                runtime->updateComponent = originalUpdateComponent;
                if (originalServiceGetTokenAPIData) (as<any>(service))["getTokenAPIData"] = originalServiceGetTokenAPIData; else (as<any>(service)).Delete("getTokenAPIData");
                if (originalServiceIsLikelyScamOrRug) (as<any>(service))["isLikelyScamOrRug"] = originalServiceIsLikelyScamOrRug; else (as<any>(service)).Delete("isLikelyScamOrRug");
            });
            try
            {
                runtime->getComponent = [=]() mutable
                {
                    return initialComponent;
                };
                runtime->updateComponent = [=](auto comp) mutable
                {
                    updatedCompData = as<std::shared_ptr<UserTrustProfile>>(comp->data);
                };
                (as<any>(service))["getTokenAPIData"] = [=]() mutable
                {
                    return as<std::shared_ptr<TokenAPIData>>((object{
                        object::pair{std::string("currentPrice"), 6}, 
                        object::pair{std::string("name"), std::string("SuperCoin")}, 
                        object::pair{std::string("symbol"), std::string("SUP")}
                    }));
                };
                (as<any>(service))["isLikelyScamOrRug"] = [=]() mutable
                {
                    return false;
                };
                std::async([=]() { service->calculateUserTrustScore(testUserIdGlobalTrustScore, runtime, testWorldId); });
                if (!updatedCompData) throw any(std::make_shared<Error>(std::string("updateComponent was not effectively called")));
                if (Math->abs(updatedCompData["trustScore"] - 100) > 0.01) {
                    throw any(std::make_shared<Error>(std::string("Expected score clamped at 100, got ") + updatedCompData["trustScore"]["toFixed"](2) + string_empty));
                }
                logger->info(std::string("TrustScore.Calc: Clamping at +100 - Passed"));
            }
            catch (...)
            {
                throw;
            }
        }
    }
    }
}, object{
    object::pair{std::string("name"), std::string("TrustScore.Calc: Score clamps at -100")}, 
    object::pair{std::string("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        auto recs = array<any>{ createRecForTrustScore(std::string("superBad"), Date->now(), std::string("BUY"), Conviction->HIGH, object{
            object::pair{std::string("potentialProfitPercent"), -500}, 
            object::pair{std::string("evaluationTimestamp"), Date->now()}, 
            object::pair{std::string("isScamOrRug"), false}
        }, 1) };
        auto initialProfile = object{
            object::pair{std::string("version"), std::string("1.0.0")}, 
            object::pair{std::string("userId"), testUserIdGlobalTrustScore}, 
            object::pair{std::string("trustScore"), 0}, 
            object::pair{std::string("lastTrustScoreCalculationTimestamp"), 0}, 
            object::pair{std::string("recommendations"), recs}
        };
        shared initialComponent = createFullMockComponentForTrustScore(testUserIdGlobalTrustScore, initialProfile, runtime);
        shared updatedCompData = nullptr;
        auto originalGetComponent = runtime->getComponent;
        auto originalUpdateComponent = runtime->updateComponent;
        auto originalServiceGetTokenAPIData = (as<any>(service))["getTokenAPIData"]["bind"](service);
        auto originalServiceIsLikelyScamOrRug = (as<any>(service))["isLikelyScamOrRug"]["bind"](service);
        {
            utils::finally __finally15129_15609([&]() mutable
            {
                runtime->getComponent = originalGetComponent;
                runtime->updateComponent = originalUpdateComponent;
                if (originalServiceGetTokenAPIData) (as<any>(service))["getTokenAPIData"] = originalServiceGetTokenAPIData; else (as<any>(service)).Delete("getTokenAPIData");
                if (originalServiceIsLikelyScamOrRug) (as<any>(service))["isLikelyScamOrRug"] = originalServiceIsLikelyScamOrRug; else (as<any>(service)).Delete("isLikelyScamOrRug");
            });
            try
            {
                runtime->getComponent = [=]() mutable
                {
                    return initialComponent;
                };
                runtime->updateComponent = [=](auto comp) mutable
                {
                    updatedCompData = as<std::shared_ptr<UserTrustProfile>>(comp->data);
                };
                (as<any>(service))["getTokenAPIData"] = [=]() mutable
                {
                    return as<std::shared_ptr<TokenAPIData>>((object{
                        object::pair{std::string("currentPrice"), 0.01}, 
                        object::pair{std::string("name"), std::string("SuperBadCoin")}, 
                        object::pair{std::string("symbol"), std::string("SBD")}
                    }));
                };
                (as<any>(service))["isLikelyScamOrRug"] = [=]() mutable
                {
                    return false;
                };
                std::async([=]() { service->calculateUserTrustScore(testUserIdGlobalTrustScore, runtime, testWorldId); });
                if (!updatedCompData) throw any(std::make_shared<Error>(std::string("updateComponent was not effectively called")));
                if (Math->abs(updatedCompData["trustScore"] - -100) > 0.01) {
                    throw any(std::make_shared<Error>(std::string("Expected score clamped at -100, got ") + updatedCompData["trustScore"]["toFixed"](2) + string_empty));
                }
                logger->info(std::string("TrustScore.Calc: Clamping at -100 - Passed"));
            }
            catch (...)
            {
                throw;
            }
        }
    }
    }
}, object{
    object::pair{std::string("name"), std::string("TrustScore.Calc: Metric re-evaluation due to interval")}, 
    object::pair{std::string("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        auto METRIC_REFRESH_INTERVAL = const_((as<any>(service)))[std::string("METRIC_REFRESH_INTERVAL")];
        auto oldEvalTimestamp = Date->now() - METRIC_REFRESH_INTERVAL * 2;
        shared recTimestamp = Date->now() - METRIC_REFRESH_INTERVAL * 3;
        auto recs = array<any>{ createRecForTrustScore(std::string("needsReEval"), recTimestamp, std::string("BUY"), Conviction->MEDIUM, object{
            object::pair{std::string("potentialProfitPercent"), 5}, 
            object::pair{std::string("evaluationTimestamp"), oldEvalTimestamp}, 
            object::pair{std::string("isScamOrRug"), false}
        }, 10) };
        auto initialProfile = object{
            object::pair{std::string("version"), std::string("1.0.0")}, 
            object::pair{std::string("userId"), testUserIdGlobalTrustScore}, 
            object::pair{std::string("trustScore"), 0}, 
            object::pair{std::string("recommendations"), recs}, 
            object::pair{std::string("lastTrustScoreCalculationTimestamp"), 0}
        };
        shared initialComponent = createFullMockComponentForTrustScore(testUserIdGlobalTrustScore, initialProfile, runtime);
        shared updatedCompData = nullptr;
        shared getTokenAPIDataCalledCount = 0;
        auto originalGetComponent = runtime->getComponent;
        auto originalUpdateComponent = runtime->updateComponent;
        auto originalServiceGetTokenAPIData = (as<any>(service))["getTokenAPIData"]["bind"](service);
        auto originalServiceIsLikelyScamOrRug = (as<any>(service))["isLikelyScamOrRug"]["bind"](service);
        {
            utils::finally __finally18404_18884([&]() mutable
            {
                runtime->getComponent = originalGetComponent;
                runtime->updateComponent = originalUpdateComponent;
                if (originalServiceGetTokenAPIData) (as<any>(service))["getTokenAPIData"] = originalServiceGetTokenAPIData; else (as<any>(service)).Delete("getTokenAPIData");
                if (originalServiceIsLikelyScamOrRug) (as<any>(service))["isLikelyScamOrRug"] = originalServiceIsLikelyScamOrRug; else (as<any>(service)).Delete("isLikelyScamOrRug");
            });
            try
            {
                runtime->getComponent = [=]() mutable
                {
                    return initialComponent;
                };
                runtime->updateComponent = [=](auto comp) mutable
                {
                    updatedCompData = as<std::shared_ptr<UserTrustProfile>>(comp->data);
                };
                (as<any>(service))["getTokenAPIData"] = [=]() mutable
                {
                    getTokenAPIDataCalledCount++;
                    return as<std::shared_ptr<TokenAPIData>>(object{
                        object::pair{std::string("currentPrice"), 12}, 
                        object::pair{std::string("priceHistory"), array<object>{ object{
                            object::pair{std::string("timestamp"), recTimestamp}, 
                            object::pair{std::string("price"), 10}
                        }, object{
                            object::pair{std::string("timestamp"), Date->now()}, 
                            object::pair{std::string("price"), 12}
                        } }}, 
                        object::pair{std::string("name"), std::string("ReevalCoin")}, 
                        object::pair{std::string("symbol"), std::string("REV")}
                    });
                };
                (as<any>(service))["isLikelyScamOrRug"] = [=]() mutable
                {
                    return false;
                };
                std::async([=]() { service->calculateUserTrustScore(testUserIdGlobalTrustScore, runtime, testWorldId); });
                if (!updatedCompData) throw any(std::make_shared<Error>(std::string("updateComponent was not effectively called")));
                if (getTokenAPIDataCalledCount == 0) throw any(std::make_shared<Error>(std::string("getTokenAPIData was NOT called for re-eval")));
                if (Math->abs((OR((const_(updatedCompData["recommendations"])[0]["metrics"]["potentialProfitPercent"]), (0))) - 20) > 0.01) {
                    throw any(std::make_shared<Error>(std::string("Metric not re-evaluated correctly to 20%")));
                }
                logger->info(std::string("TrustScore.Calc: Metric re-evaluation due to interval - Passed"));
            }
            catch (...)
            {
                throw;
            }
        }
    }
    }
}, object{
    object::pair{std::string("name"), std::string("TrustScore.Calc: Fresh metric NOT re-evaluated")}, 
    object::pair{std::string("fn"), [=](auto runtime) mutable
    {
        auto service = std::make_shared<CommunityInvestorService>(runtime);
        auto METRIC_REFRESH_INTERVAL = const_((as<any>(service)))[std::string("METRIC_REFRESH_INTERVAL")];
        auto evalTimestampWithinInterval = Date->now() - METRIC_REFRESH_INTERVAL / 2;
        auto recTimestamp = Date->now() - METRIC_REFRESH_INTERVAL / 3;
        auto recsFreshMetrics = array<any>{ createRecForTrustScore(std::string("freshMetrics"), recTimestamp, std::string("BUY"), Conviction->HIGH, object{
            object::pair{std::string("potentialProfitPercent"), 15}, 
            object::pair{std::string("evaluationTimestamp"), evalTimestampWithinInterval}, 
            object::pair{std::string("isScamOrRug"), false}
        }, 10) };
        auto initialProfileFreshMetrics = object{
            object::pair{std::string("version"), std::string("1.0.0")}, 
            object::pair{std::string("userId"), testUserIdGlobalTrustScore}, 
            object::pair{std::string("trustScore"), 0}, 
            object::pair{std::string("recommendations"), recsFreshMetrics}, 
            object::pair{std::string("lastTrustScoreCalculationTimestamp"), 0}
        };
        shared mockCompFreshMetrics = createFullMockComponentForTrustScore(testUserIdGlobalTrustScore, initialProfileFreshMetrics, runtime);
        shared updatedCompData = nullptr;
        shared getTokenAPIDataCalledCount = 0;
        auto originalGetComponent = runtime->getComponent;
        auto originalUpdateComponent = runtime->updateComponent;
        auto originalServiceGetTokenAPIData = (as<any>(service))["getTokenAPIData"]["bind"](service);
        {
            utils::finally __finally21435_21732([&]() mutable
            {
                runtime->getComponent = originalGetComponent;
                runtime->updateComponent = originalUpdateComponent;
                if (originalServiceGetTokenAPIData) (as<any>(service))["getTokenAPIData"] = originalServiceGetTokenAPIData; else (as<any>(service)).Delete("getTokenAPIData");
            });
            try
            {
                runtime->getComponent = [=]() mutable
                {
                    return mockCompFreshMetrics;
                };
                runtime->updateComponent = [=](auto comp) mutable
                {
                    updatedCompData = as<std::shared_ptr<UserTrustProfile>>(comp->data);
                };
                (as<any>(service))["getTokenAPIData"] = [=]() mutable
                {
                    getTokenAPIDataCalledCount++;
                    return as<std::shared_ptr<TokenAPIData>>(object{});
                };
                std::async([=]() { service->calculateUserTrustScore(testUserIdGlobalTrustScore, runtime, testWorldId); });
                if (!updatedCompData) throw any(std::make_shared<Error>(std::string("updateComponent was not effectively called (freshMetrics)")));
                if (getTokenAPIDataCalledCount > 0) throw any(std::make_shared<Error>(std::string("getTokenAPIData WAS called for fresh metric, but should not have been.")));
                if (Math->abs((OR((const_(updatedCompData["recommendations"])[0]["metrics"]["potentialProfitPercent"]), (0))) - 15) > 0.01) {
                    throw any(std::make_shared<Error>(std::string("Existing fresh metric was incorrectly changed")));
                }
                logger->info(std::string("TrustScore.Calc: Fresh metric NOT re-evaluated - Passed"));
            }
            catch (...)
            {
                throw;
            }
        }
    }
    }
} };
std::shared_ptr<TestSuite> trustScoreTestSuite = object{
    object::pair{std::string("name"), std::string("Trust Score Logic Tests (Runtime Format)")}, 
    object::pair{std::string("tests"), array<any>{ recencyWeightTests, convictionWeightTests, calculateScoreLogicTests }}
};

void Main(void)
{
}

MAIN
