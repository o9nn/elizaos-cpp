#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-experience/tests/integration.test.h"

string detectDomain(string text)
{
    auto domains = object{
        object::pair{std:("shell"), array<string>{ std:("command"), std:("terminal"), std:("bash"), std:("shell"), std:("execute"), std:("script") }}, 
        object::pair{std:("coding"), array<string>{ std:("code"), std:("function"), std:("variable"), std:("syntax"), std:("programming"), std:("debug") }}, 
        object::pair{std:("system"), array<string>{ std:("file"), std:("directory"), std:("process"), std:("memory"), std:("cpu"), std:("system") }}, 
        object::pair{std:("network"), array<string>{ std:("http"), std:("api"), std:("request"), std:("response"), std:("url"), std:("network") }}, 
        object::pair{std:("data"), array<string>{ std:("json"), std:("csv"), std:("database"), std:("query"), std:("data") }}
    };
    shared lowerText = text->toLowerCase();
    for (auto& [domain, keywords] : Object->entries(domains))
    {
        if (keywords->some([=](auto keyword) mutable
        {
            return lowerText->includes(keyword);
        }
        )) {
            return domain;
        }
    }
    return std:("general");
};


std::function<any()> tuuid = [=]() mutable
{
    return as<std::shared_ptr<UUID>>(uuidv4());
};
std::shared_ptr<Provider> mockRAGProvider = object{
    object::pair{std:("name"), std:("experienceRAG")}, 
    object::pair{std:("description"), std:("Mock RAG provider")}, 
    object::pair{std:("get"), vi->fn()}
};
std::shared_ptr<Provider> mockRecentProvider = object{
    object::pair{std:("name"), std:("recentExperiences")}, 
    object::pair{std:("description"), std:("Mock recent experiences provider")}, 
    object::pair{std:("get"), vi->fn()}
};
any mockRuntime = as<std::shared_ptr<IAgentRuntime>>(as<any>(object{
    object::pair{std:("agentId"), tuuid()}, 
    object::pair{std:("getService"), vi->fn()}, 
    object::pair{std:("useModel"), vi->fn()}, 
    object::pair{std:("emitEvent"), vi->fn()}, 
    object::pair{std:("providers"), array<any>{ mockRAGProvider, mockRecentProvider }}
}));
std::function<any(string, any)> createMockMessage = [=](auto text, auto entityId = undefined) mutable
{
    return (object{
        object::pair{std:("id"), tuuid()}, 
        object::pair{std:("agentId"), mockRuntime->agentId}, 
        object::pair{std:("entityId"), OR((entityId), (mockRuntime->agentId))}, 
        object::pair{std:("roomId"), tuuid()}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("text")}
        }}, 
        object::pair{std:("createdAt"), Date->now()}, 
        object::pair{std:("embedding"), array<any>()}
    });
};
std::function<any(any)> createMockState = [=](auto overrides = object{}) mutable
{
    return (utils::assign(object{
        object::pair{std:("values"), OR((overrides->values), (object{}))}, 
        object::pair{std:("data"), OR((overrides->data), (object{}))}, 
        object::pair{std:("text"), OR((overrides->text), (string_empty))}
    }, overrides));
};

void Main(void)
{
    describe(std:("Experience Plugin Integration"), [=]() mutable
    {
        shared<std::shared_ptr<ExperienceService>> experienceService;
        shared<std::shared_ptr<State>> mockState;
        beforeEach([=]() mutable
        {
            vi->clearAllMocks();
            experienceService = std::make_shared<ExperienceService>(mockRuntime);
            mockState = createMockState();
            mockRuntime->useModel = vi->fn()->mockResolvedValue(array<double>{ 0.1, 0.2, 0.3, 0.4, 0.5 });
            mockRuntime->getService = vi->fn()->mockReturnValue(experienceService);
            (as<std::shared_ptr<jest::Mock>>(mockRAGProvider->get))->mockImplementation([=]() mutable
            {
                return (object{
                    object::pair{std:("data"), object{
                        object::pair{std:("experiences"), array<any>()}, 
                        object::pair{std:("keyLearnings"), array<any>()}
                    }}, 
                    object::pair{std:("text"), std:("Default mock RAG response")}
                });
            }
            );
            (as<std::shared_ptr<jest::Mock>>(mockRecentProvider->get))->mockImplementation([=]() mutable
            {
                return (object{
                    object::pair{std:("data"), object{
                        object::pair{std:("experiences"), array<any>()}, 
                        object::pair{std:("patterns"), array<any>()}, 
                        object::pair{std:("stats"), object{
                            object::pair{std:("averageConfidence"), 0.7}, 
                            object::pair{std:("total"), 0}
                        }}
                    }}, 
                    object::pair{std:("text"), std:("Default mock Recent response")}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("count"), 0}
                    }}
                });
            }
            );
        }
        );
        afterEach([=]() mutable
        {
            std::async([=]() { experienceService->stop(); });
        }
        );
        describe(std:("Plugin Structure"), [=]() mutable
        {
            it(std:("should all required components"), [=]() mutable
            {
                expect(experiencePlugin->name)->toBe(std:("experience"));
                expect(experiencePlugin->description)->toContain(std:("experiences"));
                expect(experiencePlugin->services)->toHaveLength(1);
                expect(experiencePlugin->providers)->toHaveLength(2);
                expect(experiencePlugin->evaluators)->toHaveLength(1);
            }
            );
            it(std:("should have correct service type"), [=]() mutable
            {
                expect(ExperienceService::serviceType)->toBe(std:("EXPERIENCE"));
            }
            );
            it(std:("should have all required providers"), [=]() mutable
            {
                auto providerNames = OR((experiencePlugin->providers->map([=](auto p) mutable
                {
                    return p["name"];
                }
                )), (array<any>()));
                expect(providerNames)->toContain(std:("experienceRAG"));
                expect(providerNames)->toContain(std:("recentExperiences"));
            }
            );
            it(std:("should have experience evaluator"), [=]() mutable
            {
                auto evaluatorNames = OR((experiencePlugin->evaluators->map([=](auto e) mutable
                {
                    return e["name"];
                }
                )), (array<any>()));
                expect(evaluatorNames)->toContain(std:("EXPERIENCE_EVALUATOR"));
            }
            );
        }
        );
        describe(std:("End-to-End Experience Flow"), [=]() mutable
        {
            it(std:("should record, query, and analyze experiences"), [=]() mutable
            {
                auto successExperience = std::async([=]() { experienceService->recordExperience(object{
                    object::pair{std:("type"), ExperienceType::SUCCESS}, 
                    object::pair{std:("outcome"), OutcomeType::POSITIVE}, 
                    object::pair{std:("context"), std:("Shell command execution")}, 
                    object::pair{std:("action"), std:("execute_ls")}, 
                    object::pair{std:("result"), std:("Listed directory contents successfully")}, 
                    object::pair{std:("learning"), std:("ls command works well for directory listing")}, 
                    object::pair{std:("domain"), std:("shell")}, 
                    object::pair{std:("tags"), array<string>{ std:("shell"), std:("command"), std:("ls") }}, 
                    object::pair{std:("confidence"), 0.9}, 
                    object::pair{std:("importance"), 0.7}
                }); });
                expect(successExperience->id)->toBeDefined();
                expect(successExperience->type)->toBe(ExperienceType::SUCCESS);
                auto failureExperience = std::async([=]() { experienceService->recordExperience(object{
                    object::pair{std:("type"), ExperienceType::FAILURE}, 
                    object::pair{std:("outcome"), OutcomeType::NEGATIVE}, 
                    object::pair{std:("context"), std:("Shell command execution")}, 
                    object::pair{std:("action"), std:("execute_rm")}, 
                    object::pair{std:("result"), std:("Permission denied")}, 
                    object::pair{std:("learning"), std:("rm command requires proper permissions")}, 
                    object::pair{std:("domain"), std:("shell")}, 
                    object::pair{std:("tags"), array<string>{ std:("shell"), std:("command"), std:("rm"), std:("permissions") }}, 
                    object::pair{std:("confidence"), 0.8}, 
                    object::pair{std:("importance"), 0.9}
                }); });
                expect(failureExperience->id)->toBeDefined();
                expect(failureExperience->type)->toBe(ExperienceType::FAILURE);
                auto shellExperiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std:("domain"), std:("shell")}
                }); });
                expect(shellExperiences)->toHaveLength(2);
                expect(shellExperiences->every([=](auto e) mutable
                {
                    return e->domain == std:("shell");
                }
                ))->toBe(true);
                auto positiveExperiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std:("outcome"), OutcomeType::POSITIVE}
                }); });
                expect(positiveExperiences)->toHaveLength(1);
                expect(const_(positiveExperiences)[0]->outcome)->toBe(OutcomeType::POSITIVE);
                auto similarExperiences = std::async([=]() { experienceService->findSimilarExperiences(std:("shell command execution"), 5); });
                expect(similarExperiences->get_length())->toBeGreaterThan(0);
                expect(similarExperiences->every([=](auto e) mutable
                {
                    return e->domain == std:("shell");
                }
                ))->toBe(true);
                auto analysis = std::async([=]() { experienceService->analyzeExperiences(std:("shell")); });
                expect(analysis->frequency)->toBe(2);
                expect(analysis->reliability)->toBeGreaterThan(0);
                expect(analysis->recommendations)->toBeDefined();
            }
            );
            it(std:("should handle experience corrections and contradictions"), [=]() mutable
            {
                std::async([=]() { experienceService->recordExperience(object{
                    object::pair{std:("type"), ExperienceType::SUCCESS}, 
                    object::pair{std:("outcome"), OutcomeType::POSITIVE}, 
                    object::pair{std:("context"), std:("API call")}, 
                    object::pair{std:("action"), std:("call_api")}, 
                    object::pair{std:("result"), std:("API responded successfully")}, 
                    object::pair{std:("learning"), std:("API is reliable and fast")}, 
                    object::pair{std:("domain"), std:("network")}, 
                    object::pair{std:("confidence"), 0.8}, 
                    object::pair{std:("importance"), 0.6}
                }); });
                std::async([=]() { experienceService->recordExperience(object{
                    object::pair{std:("type"), ExperienceType::FAILURE}, 
                    object::pair{std:("outcome"), OutcomeType::NEGATIVE}, 
                    object::pair{std:("context"), std:("API call")}, 
                    object::pair{std:("action"), std:("call_api")}, 
                    object::pair{std:("result"), std:("API timeout")}, 
                    object::pair{std:("learning"), std:("API can be unreliable under load")}, 
                    object::pair{std:("domain"), std:("network")}, 
                    object::pair{std:("confidence"), 0.9}, 
                    object::pair{std:("importance"), 0.8}, 
                    object::pair{std:("previousBelief"), std:("API is reliable and fast")}, 
                    object::pair{std:("correctedBelief"), std:("API reliability depends on load conditions")}
                }); });
                auto apiExperiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std:("domain"), std:("network")}
                }); });
                auto outcomes = std::make_shared<Set>(apiExperiences->map([=](auto e) mutable
                {
                    return e->outcome;
                }
                ));
                expect(outcomes->size)->toBeGreaterThan(1);
            }
            );
            it(std:("should track access patterns and importance"), [=]() mutable
            {
                auto experience = std::async([=]() { experienceService->recordExperience(object{
                    object::pair{std:("type"), ExperienceType::LEARNING}, 
                    object::pair{std:("outcome"), OutcomeType::NEUTRAL}, 
                    object::pair{std:("context"), std:("Learning test")}, 
                    object::pair{std:("action"), std:("test_learning")}, 
                    object::pair{std:("result"), std:("Knowledge gained")}, 
                    object::pair{std:("learning"), std:("Testing access patterns")}, 
                    object::pair{std:("domain"), std:("testing")}, 
                    object::pair{std:("confidence"), 0.7}, 
                    object::pair{std:("importance"), 0.5}
                }); });
                expect(experience->accessCount)->toBe(0);
                std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std:("domain"), std:("testing")}
                }); });
                std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std:("domain"), std:("testing")}
                }); });
                std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std:("domain"), std:("testing")}
                }); });
                auto updatedExperiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std:("domain"), std:("testing")}
                }); });
                expect(const_(updatedExperiences)[0]->accessCount)->toBeGreaterThan(0);
                expect(const_(updatedExperiences)[0]->lastAccessedAt)->toBeDefined();
            }
            );
        }
        );
        describe(std:("Experience Evaluator Integration"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                (as<std::shared_ptr<jest::Mock>>(mockRAGProvider->get))->mockImplementation([=](auto runtime, auto message, auto state) mutable
                {
                    auto query = OR((state["query"]["toLowerCase"]()), (string_empty));
                    auto experiences = array<object>{ object{
                        object::pair{std:("id"), tuuid()}, 
                        object::pair{std:("learning"), std:("Generic RAG for evaluator")}
                    } };
                    auto keyLearnings = (query["startsWith"](std:("domain:"))) ? array<string>{ std:("Learning for ") + query + string_empty } : array<any>();
                    return object{
                        object::pair{std:("data"), object{
                            object::pair{std:("experiences"), std:("experiences")}, 
                            object::pair{std:("keyLearnings"), std:("keyLearnings")}
                        }}, 
                        object::pair{std:("text"), std:("Evaluator RAG response")}
                    };
                }
                );
                (as<std::shared_ptr<jest::Mock>>(mockRecentProvider->get))->mockImplementation([=](auto runtime, auto message, auto state) mutable
                {
                    return (object{
                        object::pair{std:("data"), object{
                            object::pair{std:("experiences"), array<any>()}, 
                            object::pair{std:("patterns"), (state["includePatterns"]) ? array<object>{ object{
                                object::pair{std:("description"), std:("Pattern for evaluator")}, 
                                object::pair{std:("frequency"), 3}, 
                                object::pair{std:("significance"), std:("medium")}
                            } } : array<any>()}, 
                            object::pair{std:("stats"), object{
                                object::pair{std:("averageConfidence"), 0.8}, 
                                object::pair{std:("total"), 5}
                            }}
                        }}, 
                        object::pair{std:("text"), std:("Evaluator Recent response")}, 
                        object::pair{std:("values"), object{
                            object::pair{std:("count"), 0}
                        }}
                    });
                }
                );
            }
            );
            it(std:("should validate agent messages"), [=]() mutable
            {
                auto agentMessage = createMockMessage(std:("Agent message"), mockRuntime->agentId);
                auto userMessage = createMockMessage(std:("User message"), tuuid());
                expect(std::async([=]() { experienceEvaluator->validate(mockRuntime, agentMessage, mockState); }))->toBe(true);
                expect(std::async([=]() { experienceEvaluator->validate(mockRuntime, userMessage, mockState); }))->toBe(false);
            }
            );
            it(std:("should detect and record discoveries using providers"), [=]() mutable
            {
                auto discoveryText = std:("I found that the system has jq installed for JSON processing");
                auto message = createMockMessage(discoveryText);
                mockState->recentMessagesData = array<any>();
                (as<std::shared_ptr<jest::Mock>>(mockRAGProvider->get))->mockResolvedValueOnce(object{
                    object::pair{std:("data"), object{
                        object::pair{std:("experiences"), array<any>()}, 
                        object::pair{std:("keyLearnings"), array<any>()}
                    }}, 
                    object::pair{std:("text"), std:("Initial RAG for discovery")}
                });
                (as<std::shared_ptr<jest::Mock>>(mockRAGProvider->get))->mockResolvedValueOnce(object{
                    object::pair{std:("data"), object{
                        object::pair{std:("experiences"), array<any>()}, 
                        object::pair{std:("keyLearnings"), array<string>{ std:("Key learning for system domain via jq discovery") }}
                    }}, 
                    object::pair{std:("text"), std:("Domain RAG for discovery")}
                });
                std::async([=]() { experienceEvaluator->handler(mockRuntime, message, mockState); });
                expect(mockRAGProvider->get)->toHaveBeenCalledTimes(2);
                expect(mockRAGProvider->get)->toHaveBeenNthCalledWith(1, mockRuntime, message, expect->objectContaining(object{
                    object::pair{std:("query"), discoveryText->toLowerCase()}
                }));
                expect(mockRAGProvider->get)->toHaveBeenNthCalledWith(2, mockRuntime, message, expect->objectContaining(object{
                    object::pair{std:("query"), std:("domain:system")}
                }));
                auto experiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std:("type"), ExperienceType::DISCOVERY}
                }); });
                expect(experiences->get_length())->toBeGreaterThan(0);
                expect(const_(experiences)[0]->learning)->toContain(std:("jq"));
                auto learningExperiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std:("type"), ExperienceType::LEARNING}, 
                    object::pair{std:("domain"), std:("system")}
                }); });
                expect(learningExperiences->get_length())->toBeGreaterThan(0);
                expect(const_(learningExperiences)[0]->learning)->toContain(std:("Key learning for system domain via jq discovery"));
            }
            );
            it(std:("should use pattern detection from recentExperiences provider"), [=]() mutable
            {
                auto message = createMockMessage(std:("Agent message that does not trigger other specific detections"));
                mockState->recentMessagesData = array<any>{ createMockMessage(std:("p1")), createMockMessage(std:("p2")), createMockMessage(std:("p3")) };
                (as<std::shared_ptr<jest::Mock>>(mockRecentProvider->get))->mockResolvedValueOnce(object{
                    object::pair{std:("data"), object{
                        object::pair{std:("experiences"), array<any>()}, 
                        object::pair{std:("patterns"), array<object>{ object{
                            object::pair{std:("description"), std:("Test pattern from recent for pattern detection test")}, 
                            object::pair{std:("frequency"), 5}, 
                            object::pair{std:("significance"), std:("high")}
                        } }}, 
                        object::pair{std:("stats"), object{
                            object::pair{std:("averageConfidence"), 0.85}, 
                            object::pair{std:("total"), 10}
                        }}
                    }}, 
                    object::pair{std:("text"), std:("Recent experiences for pattern detection test")}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("count"), 3}
                    }}
                });
                std::async([=]() { experienceEvaluator->handler(mockRuntime, message, mockState); });
                expect(mockRecentProvider->get)->toHaveBeenCalledWith(mockRuntime, message, expect->objectContaining(object{
                    object::pair{std:("includePatterns"), true}
                }));
                auto experiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std:("type"), ExperienceType::VALIDATION}
                }); });
                expect(experiences->get_length())->toBeGreaterThan(0);
                expect(const_(experiences)[0]->learning)->toContain(std:("Test pattern from recent for pattern detection test"));
            }
            );
            it(std:("should handle provider errors gracefully and record a learning experience"), [=]() mutable
            {
                auto message = createMockMessage(std:("Test message causing RAG provider error"));
                mockState->recentMessagesData = array<any>();
                (as<std::shared_ptr<jest::Mock>>(mockRAGProvider->get))->mockRejectedValueOnce(std::make_shared<Error>(std:("Isolated RAG error")));
                (as<std::shared_ptr<jest::Mock>>(mockRecentProvider->get))->mockResolvedValueOnce(object{
                    object::pair{std:("data"), object{
                        object::pair{std:("experiences"), array<any>()}, 
                        object::pair{std:("patterns"), array<any>()}, 
                        object::pair{std:("stats"), nullptr}
                    }}, 
                    object::pair{std:("text"), std:("Recent provider benign response during RAG error")}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("count"), 0}
                    }}
                });
                std::async([=]() { experienceEvaluator->handler(mockRuntime, message, mockState); });
                auto learningExperiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std:("type"), ExperienceType::LEARNING}, 
                    object::pair{std:("domain"), std:("system")}
                }); });
                expect(learningExperiences->get_length())->toBeGreaterThan(0);
                expect(const_(learningExperiences)[0]->learning)->toContain(std:("An error occurred in experience evaluator: Isolated RAG error"));
            }
            );
        }
        );
        describe(std:("Provider Integration"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                vi->clearAllMocks();
                experienceService = std::make_shared<ExperienceService>(mockRuntime);
                mockRuntime->getService = vi->fn()->mockReturnValue(experienceService);
                std::async([=]() { experienceService->recordExperience(object{
                    object::pair{std:("type"), ExperienceType::SUCCESS}, 
                    object::pair{std:("outcome"), OutcomeType::POSITIVE}, 
                    object::pair{std:("context"), std:("File ops success")}, 
                    object::pair{std:("action"), std:("mkfile")}, 
                    object::pair{std:("result"), std:("Created")}, 
                    object::pair{std:("learning"), std:("mkfile works")}, 
                    object::pair{std:("domain"), std:("system")}
                }); });
                std::async([=]() { experienceService->recordExperience(object{
                    object::pair{std:("type"), ExperienceType::DISCOVERY}, 
                    object::pair{std:("outcome"), OutcomeType::POSITIVE}, 
                    object::pair{std:("context"), std:("Tool discovery")}, 
                    object::pair{std:("action"), std:("findtool")}, 
                    object::pair{std:("result"), std:("Found it")}, 
                    object::pair{std:("learning"), std:("new tool available")}, 
                    object::pair{std:("domain"), std:("system")}
                }); });
            }
            );
            it(std:("should provide relevant experiences via RAG provider"), [=]() mutable
            {
                auto ragProvider = experiencePlugin->providers->find([=](auto p) mutable
                {
                    return p["name"] == std:("experienceRAG");
                }
                );
                expect(ragProvider)->toBeDefined();
                auto message = createMockMessage(std:("system file ops"));
                auto expectedExperiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std:("domain"), std:("system")}
                }); });
                (as<std::shared_ptr<jest::Mock>>(mockRAGProvider->get))->mockResolvedValueOnce(object{
                    object::pair{std:("data"), object{
                        object::pair{std:("experiences"), expectedExperiences}, 
                        object::pair{std:("keyLearnings"), array<string>{ std:("RAG Learning for file ops") }}
                    }}
                });
                auto result = std::async([=]() { ragProvider->get(mockRuntime, message, mockState); });
                expect(result->data)->toBeDefined();
                expect(result->data->experiences)->toEqual(expectedExperiences);
                expect(result->text)->toContain(std:("Found 2 relevant experiences"));
                expect(result->text)->toContain(std:("mkfile works"));
                expect(result->text)->toContain(std:("new tool available"));
            }
            );
            it(std:("should provide recent experiences with statistics"), [=]() mutable
            {
                auto recentProvider = experiencePlugin->providers->find([=](auto p) mutable
                {
                    return p["name"] == std:("recentExperiences");
                }
                );
                expect(recentProvider)->toBeDefined();
                auto message = createMockMessage(string_empty);
                auto recentExperiencesData = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std:("limit"), 2}
                }); });
                (as<std::shared_ptr<jest::Mock>>(mockRecentProvider->get))->mockResolvedValueOnce(object{
                    object::pair{std:("data"), object{
                        object::pair{std:("experiences"), recentExperiencesData}, 
                        object::pair{std:("patterns"), array<any>()}, 
                        object::pair{std:("stats"), object{
                            object::pair{std:("averageConfidence"), 0.85}, 
                            object::pair{std:("total"), recentExperiencesData->get_length()}
                        }}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("count"), recentExperiencesData->get_length()}
                    }}
                });
                auto result = std::async([=]() { recentProvider->get(mockRuntime, message, mockState); });
                expect(result->data)->toBeDefined();
                expect(result->data->experiences)->toEqual(recentExperiencesData);
                expect(result->values->count)->toBe(recentExperiencesData->get_length());
                expect(result->data->stats->total)->toBe(recentExperiencesData->get_length());
                expect(result->text)->toContain(std:("Recent ") + recentExperiencesData->get_length() + std:(" experiences"));
                expect(result->text)->toContain(std:("mkfile works"));
                expect(result->text)->toContain(std:("Statistics"));
            }
            );
        }
        );
        describe(std:("Memory Management"), [=]() mutable
        {
            it(std:("should handle large numbers of experiences efficiently"), [=]() mutable
            {
                (as<any>(experienceService))["maxExperiences"] = 10;
                for (auto i = 0; i < 15; i++)
                {
                    std::async([=]() { experienceService->recordExperience(object{
                        object::pair{std:("type"), ExperienceType::LEARNING}, 
                        object::pair{std:("context"), std:("Ctx ") + i + string_empty}, 
                        object::pair{std:("action"), std:("act_") + i + string_empty}, 
                        object::pair{std:("result"), std:("Res ") + i + string_empty}, 
                        object::pair{std:("learning"), std:("Learn ") + i + string_empty}, 
                        object::pair{std:("importance"), (i < 5) ? 0.1 : 0.9}
                    }); });
                }
                auto allExperiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std:("limit"), 20}
                }); });
                expect(allExperiences->get_length())->toBeLessThanOrEqual(10);
                auto highImportanceCount = allExperiences->filter([=](auto e) mutable
                {
                    return e->importance > 0.5;
                }
                )->get_length();
                expect(highImportanceCount)->toBeGreaterThanOrEqual(5);
            }
            );
            it(std:("should handle embedding generation failures gracefully"), [=]() mutable
            {
                (as<std::shared_ptr<jest::Mock>>(mockRuntime->useModel))->mockRejectedValueOnce(std::make_shared<Error>(std:("Embedding model fail")));
                shared experience = std::async([=]() { experienceService->recordExperience(object{
                    object::pair{std:("learning"), std:("No embedding")}
                }); });
                expect(experience->id)->toBeDefined();
                expect(experience->embedding)->toBeUndefined();
                auto experiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std:("domain"), std:("general")}
                }); });
                expect(experiences->some([=](auto e) mutable
                {
                    return e->id == experience->id;
                }
                ))->toBe(true);
            }
            );
        }
        );
        describe(std:("Error Handling"), [=]() mutable
        {
            it(std:("should handle service unavailability gracefully"), [=]() mutable
            {
                auto mockRuntimeNoService = as<std::shared_ptr<IAgentRuntime>>(as<any>(utils::assign(object{
                    , 
                    object::pair{std:("getService"), vi->fn()->mockReturnValue(nullptr)}
                }, mockRuntime)));
                auto ragProvider = experiencePlugin->providers->find([=](auto p) mutable
                {
                    return p["name"] == std:("experienceRAG");
                }
                );
                auto result = std::async([=]() { ragProvider->get(mockRuntimeNoService, createMockMessage(std:("q")), createMockState()); });
                expect(result->data->experiences)->toEqual(array<any>());
                expect(result->text)->toContain(std:("not available"));
            }
            );
            it(std:("should handle malformed queries gracefully"), [=]() mutable
            {
                auto experiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std:("invalidField"), std:("invalid")}
                }); });
                expect(Array->isArray(experiences))->toBe(true);
            }
            );
            it(std:("should handle concurrent access safely"), [=]() mutable
            {
                auto promises = array<any>();
                for (auto i = 0; i < 10; i++)
                {
                    promises->push(experienceService->recordExperience(object{
                        object::pair{std:("type"), ExperienceType::LEARNING}, 
                        object::pair{std:("outcome"), OutcomeType::NEUTRAL}, 
                        object::pair{std:("context"), std:("Concurrent context ") + i + string_empty}, 
                        object::pair{std:("action"), std:("concurrent_action_") + i + string_empty}, 
                        object::pair{std:("result"), std:("Concurrent result ") + i + string_empty}, 
                        object::pair{std:("learning"), std:("Concurrent learning ") + i + string_empty}, 
                        object::pair{std:("domain"), std:("concurrent")}
                    }));
                }
                auto results = std::async([=]() { Promise->all(promises); });
                expect(results)->toHaveLength(10);
                expect(results->every([=](auto r) mutable
                {
                    return r["id"];
                }
                ))->toBe(true);
                auto allExperiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std:("domain"), std:("concurrent")}
                }); });
                expect(allExperiences)->toHaveLength(10);
            }
            );
        }
        );
    }
    );
}

MAIN
