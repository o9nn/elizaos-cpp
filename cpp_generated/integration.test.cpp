#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-experience/tests/integration.test.h"

string detectDomain(string text)
{
    auto domains = object{
        object::pair{std::string("shell"), array<string>{ std::string("command"), std::string("terminal"), std::string("bash"), std::string("shell"), std::string("execute"), std::string("script") }}, 
        object::pair{std::string("coding"), array<string>{ std::string("code"), std::string("function"), std::string("variable"), std::string("syntax"), std::string("programming"), std::string("debug") }}, 
        object::pair{std::string("system"), array<string>{ std::string("file"), std::string("directory"), std::string("process"), std::string("memory"), std::string("cpu"), std::string("system") }}, 
        object::pair{std::string("network"), array<string>{ std::string("http"), std::string("api"), std::string("request"), std::string("response"), std::string("url"), std::string("network") }}, 
        object::pair{std::string("data"), array<string>{ std::string("json"), std::string("csv"), std::string("database"), std::string("query"), std::string("data") }}
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
    return std::string("general");
};


std::function<any()> tuuid = [=]() mutable
{
    return as<std::shared_ptr<UUID>>(uuidv4());
};
std::shared_ptr<Provider> mockRAGProvider = object{
    object::pair{std::string("name"), std::string("experienceRAG")}, 
    object::pair{std::string("description"), std::string("Mock RAG provider")}, 
    object::pair{std::string("get"), vi->fn()}
};
std::shared_ptr<Provider> mockRecentProvider = object{
    object::pair{std::string("name"), std::string("recentExperiences")}, 
    object::pair{std::string("description"), std::string("Mock recent experiences provider")}, 
    object::pair{std::string("get"), vi->fn()}
};
any mockRuntime = as<std::shared_ptr<IAgentRuntime>>(as<any>(object{
    object::pair{std::string("agentId"), tuuid()}, 
    object::pair{std::string("getService"), vi->fn()}, 
    object::pair{std::string("useModel"), vi->fn()}, 
    object::pair{std::string("emitEvent"), vi->fn()}, 
    object::pair{std::string("providers"), array<any>{ mockRAGProvider, mockRecentProvider }}
}));
std::function<any(string, any)> createMockMessage = [=](auto text, auto entityId = undefined) mutable
{
    return (object{
        object::pair{std::string("id"), tuuid()}, 
        object::pair{std::string("agentId"), mockRuntime->agentId}, 
        object::pair{std::string("entityId"), OR((entityId), (mockRuntime->agentId))}, 
        object::pair{std::string("roomId"), tuuid()}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("text")}
        }}, 
        object::pair{std::string("createdAt"), Date->now()}, 
        object::pair{std::string("embedding"), array<any>()}
    });
};
std::function<any(any)> createMockState = [=](auto overrides = object{}) mutable
{
    return (utils::assign(object{
        object::pair{std::string("values"), OR((overrides->values), (object{}))}, 
        object::pair{std::string("data"), OR((overrides->data), (object{}))}, 
        object::pair{std::string("text"), OR((overrides->text), (string_empty))}
    }, overrides));
};

void Main(void)
{
    describe(std::string("Experience Plugin Integration"), [=]() mutable
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
                    object::pair{std::string("data"), object{
                        object::pair{std::string("experiences"), array<any>()}, 
                        object::pair{std::string("keyLearnings"), array<any>()}
                    }}, 
                    object::pair{std::string("text"), std::string("Default mock RAG response")}
                });
            }
            );
            (as<std::shared_ptr<jest::Mock>>(mockRecentProvider->get))->mockImplementation([=]() mutable
            {
                return (object{
                    object::pair{std::string("data"), object{
                        object::pair{std::string("experiences"), array<any>()}, 
                        object::pair{std::string("patterns"), array<any>()}, 
                        object::pair{std::string("stats"), object{
                            object::pair{std::string("averageConfidence"), 0.7}, 
                            object::pair{std::string("total"), 0}
                        }}
                    }}, 
                    object::pair{std::string("text"), std::string("Default mock Recent response")}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("count"), 0}
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
        describe(std::string("Plugin Structure"), [=]() mutable
        {
            it(std::string("should export all required components"), [=]() mutable
            {
                expect(experiencePlugin->name)->toBe(std::string("experience"));
                expect(experiencePlugin->description)->toContain(std::string("experiences"));
                expect(experiencePlugin->services)->toHaveLength(1);
                expect(experiencePlugin->providers)->toHaveLength(2);
                expect(experiencePlugin->evaluators)->toHaveLength(1);
            }
            );
            it(std::string("should have correct service type"), [=]() mutable
            {
                expect(ExperienceService::serviceType)->toBe(std::string("EXPERIENCE"));
            }
            );
            it(std::string("should have all required providers"), [=]() mutable
            {
                auto providerNames = OR((experiencePlugin->providers->map([=](auto p) mutable
                {
                    return p["name"];
                }
                )), (array<any>()));
                expect(providerNames)->toContain(std::string("experienceRAG"));
                expect(providerNames)->toContain(std::string("recentExperiences"));
            }
            );
            it(std::string("should have experience evaluator"), [=]() mutable
            {
                auto evaluatorNames = OR((experiencePlugin->evaluators->map([=](auto e) mutable
                {
                    return e["name"];
                }
                )), (array<any>()));
                expect(evaluatorNames)->toContain(std::string("EXPERIENCE_EVALUATOR"));
            }
            );
        }
        );
        describe(std::string("End-to-End Experience Flow"), [=]() mutable
        {
            it(std::string("should record, query, and analyze experiences"), [=]() mutable
            {
                auto successExperience = std::async([=]() { experienceService->recordExperience(object{
                    object::pair{std::string("type"), ExperienceType::SUCCESS}, 
                    object::pair{std::string("outcome"), OutcomeType::POSITIVE}, 
                    object::pair{std::string("context"), std::string("Shell command execution")}, 
                    object::pair{std::string("action"), std::string("execute_ls")}, 
                    object::pair{std::string("result"), std::string("Listed directory contents successfully")}, 
                    object::pair{std::string("learning"), std::string("ls command works well for directory listing")}, 
                    object::pair{std::string("domain"), std::string("shell")}, 
                    object::pair{std::string("tags"), array<string>{ std::string("shell"), std::string("command"), std::string("ls") }}, 
                    object::pair{std::string("confidence"), 0.9}, 
                    object::pair{std::string("importance"), 0.7}
                }); });
                expect(successExperience->id)->toBeDefined();
                expect(successExperience->type)->toBe(ExperienceType::SUCCESS);
                auto failureExperience = std::async([=]() { experienceService->recordExperience(object{
                    object::pair{std::string("type"), ExperienceType::FAILURE}, 
                    object::pair{std::string("outcome"), OutcomeType::NEGATIVE}, 
                    object::pair{std::string("context"), std::string("Shell command execution")}, 
                    object::pair{std::string("action"), std::string("execute_rm")}, 
                    object::pair{std::string("result"), std::string("Permission denied")}, 
                    object::pair{std::string("learning"), std::string("rm command requires proper permissions")}, 
                    object::pair{std::string("domain"), std::string("shell")}, 
                    object::pair{std::string("tags"), array<string>{ std::string("shell"), std::string("command"), std::string("rm"), std::string("permissions") }}, 
                    object::pair{std::string("confidence"), 0.8}, 
                    object::pair{std::string("importance"), 0.9}
                }); });
                expect(failureExperience->id)->toBeDefined();
                expect(failureExperience->type)->toBe(ExperienceType::FAILURE);
                auto shellExperiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std::string("domain"), std::string("shell")}
                }); });
                expect(shellExperiences)->toHaveLength(2);
                expect(shellExperiences->every([=](auto e) mutable
                {
                    return e->domain == std::string("shell");
                }
                ))->toBe(true);
                auto positiveExperiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std::string("outcome"), OutcomeType::POSITIVE}
                }); });
                expect(positiveExperiences)->toHaveLength(1);
                expect(const_(positiveExperiences)[0]->outcome)->toBe(OutcomeType::POSITIVE);
                auto similarExperiences = std::async([=]() { experienceService->findSimilarExperiences(std::string("shell command execution"), 5); });
                expect(similarExperiences->get_length())->toBeGreaterThan(0);
                expect(similarExperiences->every([=](auto e) mutable
                {
                    return e->domain == std::string("shell");
                }
                ))->toBe(true);
                auto analysis = std::async([=]() { experienceService->analyzeExperiences(std::string("shell")); });
                expect(analysis->frequency)->toBe(2);
                expect(analysis->reliability)->toBeGreaterThan(0);
                expect(analysis->recommendations)->toBeDefined();
            }
            );
            it(std::string("should handle experience corrections and contradictions"), [=]() mutable
            {
                std::async([=]() { experienceService->recordExperience(object{
                    object::pair{std::string("type"), ExperienceType::SUCCESS}, 
                    object::pair{std::string("outcome"), OutcomeType::POSITIVE}, 
                    object::pair{std::string("context"), std::string("API call")}, 
                    object::pair{std::string("action"), std::string("call_api")}, 
                    object::pair{std::string("result"), std::string("API responded successfully")}, 
                    object::pair{std::string("learning"), std::string("API is reliable and fast")}, 
                    object::pair{std::string("domain"), std::string("network")}, 
                    object::pair{std::string("confidence"), 0.8}, 
                    object::pair{std::string("importance"), 0.6}
                }); });
                std::async([=]() { experienceService->recordExperience(object{
                    object::pair{std::string("type"), ExperienceType::FAILURE}, 
                    object::pair{std::string("outcome"), OutcomeType::NEGATIVE}, 
                    object::pair{std::string("context"), std::string("API call")}, 
                    object::pair{std::string("action"), std::string("call_api")}, 
                    object::pair{std::string("result"), std::string("API timeout")}, 
                    object::pair{std::string("learning"), std::string("API can be unreliable under load")}, 
                    object::pair{std::string("domain"), std::string("network")}, 
                    object::pair{std::string("confidence"), 0.9}, 
                    object::pair{std::string("importance"), 0.8}, 
                    object::pair{std::string("previousBelief"), std::string("API is reliable and fast")}, 
                    object::pair{std::string("correctedBelief"), std::string("API reliability depends on load conditions")}
                }); });
                auto apiExperiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std::string("domain"), std::string("network")}
                }); });
                auto outcomes = std::make_shared<Set>(apiExperiences->map([=](auto e) mutable
                {
                    return e->outcome;
                }
                ));
                expect(outcomes->size)->toBeGreaterThan(1);
            }
            );
            it(std::string("should track access patterns and importance"), [=]() mutable
            {
                auto experience = std::async([=]() { experienceService->recordExperience(object{
                    object::pair{std::string("type"), ExperienceType::LEARNING}, 
                    object::pair{std::string("outcome"), OutcomeType::NEUTRAL}, 
                    object::pair{std::string("context"), std::string("Learning test")}, 
                    object::pair{std::string("action"), std::string("test_learning")}, 
                    object::pair{std::string("result"), std::string("Knowledge gained")}, 
                    object::pair{std::string("learning"), std::string("Testing access patterns")}, 
                    object::pair{std::string("domain"), std::string("testing")}, 
                    object::pair{std::string("confidence"), 0.7}, 
                    object::pair{std::string("importance"), 0.5}
                }); });
                expect(experience->accessCount)->toBe(0);
                std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std::string("domain"), std::string("testing")}
                }); });
                std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std::string("domain"), std::string("testing")}
                }); });
                std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std::string("domain"), std::string("testing")}
                }); });
                auto updatedExperiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std::string("domain"), std::string("testing")}
                }); });
                expect(const_(updatedExperiences)[0]->accessCount)->toBeGreaterThan(0);
                expect(const_(updatedExperiences)[0]->lastAccessedAt)->toBeDefined();
            }
            );
        }
        );
        describe(std::string("Experience Evaluator Integration"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                (as<std::shared_ptr<jest::Mock>>(mockRAGProvider->get))->mockImplementation([=](auto runtime, auto message, auto state) mutable
                {
                    auto query = OR((state["query"]["toLowerCase"]()), (string_empty));
                    auto experiences = array<object>{ object{
                        object::pair{std::string("id"), tuuid()}, 
                        object::pair{std::string("learning"), std::string("Generic RAG for evaluator")}
                    } };
                    auto keyLearnings = (query["startsWith"](std::string("domain:"))) ? array<string>{ std::string("Learning for ") + query + string_empty } : array<any>();
                    return object{
                        object::pair{std::string("data"), object{
                            object::pair{std::string("experiences"), std::string("experiences")}, 
                            object::pair{std::string("keyLearnings"), std::string("keyLearnings")}
                        }}, 
                        object::pair{std::string("text"), std::string("Evaluator RAG response")}
                    };
                }
                );
                (as<std::shared_ptr<jest::Mock>>(mockRecentProvider->get))->mockImplementation([=](auto runtime, auto message, auto state) mutable
                {
                    return (object{
                        object::pair{std::string("data"), object{
                            object::pair{std::string("experiences"), array<any>()}, 
                            object::pair{std::string("patterns"), (state["includePatterns"]) ? array<object>{ object{
                                object::pair{std::string("description"), std::string("Pattern for evaluator")}, 
                                object::pair{std::string("frequency"), 3}, 
                                object::pair{std::string("significance"), std::string("medium")}
                            } } : array<any>()}, 
                            object::pair{std::string("stats"), object{
                                object::pair{std::string("averageConfidence"), 0.8}, 
                                object::pair{std::string("total"), 5}
                            }}
                        }}, 
                        object::pair{std::string("text"), std::string("Evaluator Recent response")}, 
                        object::pair{std::string("values"), object{
                            object::pair{std::string("count"), 0}
                        }}
                    });
                }
                );
            }
            );
            it(std::string("should validate agent messages"), [=]() mutable
            {
                auto agentMessage = createMockMessage(std::string("Agent message"), mockRuntime->agentId);
                auto userMessage = createMockMessage(std::string("User message"), tuuid());
                expect(std::async([=]() { experienceEvaluator->validate(mockRuntime, agentMessage, mockState); }))->toBe(true);
                expect(std::async([=]() { experienceEvaluator->validate(mockRuntime, userMessage, mockState); }))->toBe(false);
            }
            );
            it(std::string("should detect and record discoveries using providers"), [=]() mutable
            {
                auto discoveryText = std::string("I found that the system has jq installed for JSON processing");
                auto message = createMockMessage(discoveryText);
                mockState->recentMessagesData = array<any>();
                (as<std::shared_ptr<jest::Mock>>(mockRAGProvider->get))->mockResolvedValueOnce(object{
                    object::pair{std::string("data"), object{
                        object::pair{std::string("experiences"), array<any>()}, 
                        object::pair{std::string("keyLearnings"), array<any>()}
                    }}, 
                    object::pair{std::string("text"), std::string("Initial RAG for discovery")}
                });
                (as<std::shared_ptr<jest::Mock>>(mockRAGProvider->get))->mockResolvedValueOnce(object{
                    object::pair{std::string("data"), object{
                        object::pair{std::string("experiences"), array<any>()}, 
                        object::pair{std::string("keyLearnings"), array<string>{ std::string("Key learning for system domain via jq discovery") }}
                    }}, 
                    object::pair{std::string("text"), std::string("Domain RAG for discovery")}
                });
                std::async([=]() { experienceEvaluator->handler(mockRuntime, message, mockState); });
                expect(mockRAGProvider->get)->toHaveBeenCalledTimes(2);
                expect(mockRAGProvider->get)->toHaveBeenNthCalledWith(1, mockRuntime, message, expect->objectContaining(object{
                    object::pair{std::string("query"), discoveryText->toLowerCase()}
                }));
                expect(mockRAGProvider->get)->toHaveBeenNthCalledWith(2, mockRuntime, message, expect->objectContaining(object{
                    object::pair{std::string("query"), std::string("domain:system")}
                }));
                auto experiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std::string("type"), ExperienceType::DISCOVERY}
                }); });
                expect(experiences->get_length())->toBeGreaterThan(0);
                expect(const_(experiences)[0]->learning)->toContain(std::string("jq"));
                auto learningExperiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std::string("type"), ExperienceType::LEARNING}, 
                    object::pair{std::string("domain"), std::string("system")}
                }); });
                expect(learningExperiences->get_length())->toBeGreaterThan(0);
                expect(const_(learningExperiences)[0]->learning)->toContain(std::string("Key learning for system domain via jq discovery"));
            }
            );
            it(std::string("should use pattern detection from recentExperiences provider"), [=]() mutable
            {
                auto message = createMockMessage(std::string("Agent message that does not trigger other specific detections"));
                mockState->recentMessagesData = array<any>{ createMockMessage(std::string("p1")), createMockMessage(std::string("p2")), createMockMessage(std::string("p3")) };
                (as<std::shared_ptr<jest::Mock>>(mockRecentProvider->get))->mockResolvedValueOnce(object{
                    object::pair{std::string("data"), object{
                        object::pair{std::string("experiences"), array<any>()}, 
                        object::pair{std::string("patterns"), array<object>{ object{
                            object::pair{std::string("description"), std::string("Test pattern from recent for pattern detection test")}, 
                            object::pair{std::string("frequency"), 5}, 
                            object::pair{std::string("significance"), std::string("high")}
                        } }}, 
                        object::pair{std::string("stats"), object{
                            object::pair{std::string("averageConfidence"), 0.85}, 
                            object::pair{std::string("total"), 10}
                        }}
                    }}, 
                    object::pair{std::string("text"), std::string("Recent experiences for pattern detection test")}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("count"), 3}
                    }}
                });
                std::async([=]() { experienceEvaluator->handler(mockRuntime, message, mockState); });
                expect(mockRecentProvider->get)->toHaveBeenCalledWith(mockRuntime, message, expect->objectContaining(object{
                    object::pair{std::string("includePatterns"), true}
                }));
                auto experiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std::string("type"), ExperienceType::VALIDATION}
                }); });
                expect(experiences->get_length())->toBeGreaterThan(0);
                expect(const_(experiences)[0]->learning)->toContain(std::string("Test pattern from recent for pattern detection test"));
            }
            );
            it(std::string("should handle provider errors gracefully and record a learning experience"), [=]() mutable
            {
                auto message = createMockMessage(std::string("Test message causing RAG provider error"));
                mockState->recentMessagesData = array<any>();
                (as<std::shared_ptr<jest::Mock>>(mockRAGProvider->get))->mockRejectedValueOnce(std::make_shared<Error>(std::string("Isolated RAG error")));
                (as<std::shared_ptr<jest::Mock>>(mockRecentProvider->get))->mockResolvedValueOnce(object{
                    object::pair{std::string("data"), object{
                        object::pair{std::string("experiences"), array<any>()}, 
                        object::pair{std::string("patterns"), array<any>()}, 
                        object::pair{std::string("stats"), nullptr}
                    }}, 
                    object::pair{std::string("text"), std::string("Recent provider benign response during RAG error")}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("count"), 0}
                    }}
                });
                std::async([=]() { experienceEvaluator->handler(mockRuntime, message, mockState); });
                auto learningExperiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std::string("type"), ExperienceType::LEARNING}, 
                    object::pair{std::string("domain"), std::string("system")}
                }); });
                expect(learningExperiences->get_length())->toBeGreaterThan(0);
                expect(const_(learningExperiences)[0]->learning)->toContain(std::string("An error occurred in experience evaluator: Isolated RAG error"));
            }
            );
        }
        );
        describe(std::string("Provider Integration"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                vi->clearAllMocks();
                experienceService = std::make_shared<ExperienceService>(mockRuntime);
                mockRuntime->getService = vi->fn()->mockReturnValue(experienceService);
                std::async([=]() { experienceService->recordExperience(object{
                    object::pair{std::string("type"), ExperienceType::SUCCESS}, 
                    object::pair{std::string("outcome"), OutcomeType::POSITIVE}, 
                    object::pair{std::string("context"), std::string("File ops success")}, 
                    object::pair{std::string("action"), std::string("mkfile")}, 
                    object::pair{std::string("result"), std::string("Created")}, 
                    object::pair{std::string("learning"), std::string("mkfile works")}, 
                    object::pair{std::string("domain"), std::string("system")}
                }); });
                std::async([=]() { experienceService->recordExperience(object{
                    object::pair{std::string("type"), ExperienceType::DISCOVERY}, 
                    object::pair{std::string("outcome"), OutcomeType::POSITIVE}, 
                    object::pair{std::string("context"), std::string("Tool discovery")}, 
                    object::pair{std::string("action"), std::string("findtool")}, 
                    object::pair{std::string("result"), std::string("Found it")}, 
                    object::pair{std::string("learning"), std::string("new tool available")}, 
                    object::pair{std::string("domain"), std::string("system")}
                }); });
            }
            );
            it(std::string("should provide relevant experiences via RAG provider"), [=]() mutable
            {
                auto ragProvider = experiencePlugin->providers->find([=](auto p) mutable
                {
                    return p["name"] == std::string("experienceRAG");
                }
                );
                expect(ragProvider)->toBeDefined();
                auto message = createMockMessage(std::string("system file ops"));
                auto expectedExperiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std::string("domain"), std::string("system")}
                }); });
                (as<std::shared_ptr<jest::Mock>>(mockRAGProvider->get))->mockResolvedValueOnce(object{
                    object::pair{std::string("data"), object{
                        object::pair{std::string("experiences"), expectedExperiences}, 
                        object::pair{std::string("keyLearnings"), array<string>{ std::string("RAG Learning for file ops") }}
                    }}
                });
                auto result = std::async([=]() { ragProvider->get(mockRuntime, message, mockState); });
                expect(result->data)->toBeDefined();
                expect(result->data->experiences)->toEqual(expectedExperiences);
                expect(result->text)->toContain(std::string("Found 2 relevant experiences"));
                expect(result->text)->toContain(std::string("mkfile works"));
                expect(result->text)->toContain(std::string("new tool available"));
            }
            );
            it(std::string("should provide recent experiences with statistics"), [=]() mutable
            {
                auto recentProvider = experiencePlugin->providers->find([=](auto p) mutable
                {
                    return p["name"] == std::string("recentExperiences");
                }
                );
                expect(recentProvider)->toBeDefined();
                auto message = createMockMessage(string_empty);
                auto recentExperiencesData = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std::string("limit"), 2}
                }); });
                (as<std::shared_ptr<jest::Mock>>(mockRecentProvider->get))->mockResolvedValueOnce(object{
                    object::pair{std::string("data"), object{
                        object::pair{std::string("experiences"), recentExperiencesData}, 
                        object::pair{std::string("patterns"), array<any>()}, 
                        object::pair{std::string("stats"), object{
                            object::pair{std::string("averageConfidence"), 0.85}, 
                            object::pair{std::string("total"), recentExperiencesData->get_length()}
                        }}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("count"), recentExperiencesData->get_length()}
                    }}
                });
                auto result = std::async([=]() { recentProvider->get(mockRuntime, message, mockState); });
                expect(result->data)->toBeDefined();
                expect(result->data->experiences)->toEqual(recentExperiencesData);
                expect(result->values->count)->toBe(recentExperiencesData->get_length());
                expect(result->data->stats->total)->toBe(recentExperiencesData->get_length());
                expect(result->text)->toContain(std::string("Recent ") + recentExperiencesData->get_length() + std::string(" experiences"));
                expect(result->text)->toContain(std::string("mkfile works"));
                expect(result->text)->toContain(std::string("Statistics"));
            }
            );
        }
        );
        describe(std::string("Memory Management"), [=]() mutable
        {
            it(std::string("should handle large numbers of experiences efficiently"), [=]() mutable
            {
                (as<any>(experienceService))["maxExperiences"] = 10;
                for (auto i = 0; i < 15; i++)
                {
                    std::async([=]() { experienceService->recordExperience(object{
                        object::pair{std::string("type"), ExperienceType::LEARNING}, 
                        object::pair{std::string("context"), std::string("Ctx ") + i + string_empty}, 
                        object::pair{std::string("action"), std::string("act_") + i + string_empty}, 
                        object::pair{std::string("result"), std::string("Res ") + i + string_empty}, 
                        object::pair{std::string("learning"), std::string("Learn ") + i + string_empty}, 
                        object::pair{std::string("importance"), (i < 5) ? 0.1 : 0.9}
                    }); });
                }
                auto allExperiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std::string("limit"), 20}
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
            it(std::string("should handle embedding generation failures gracefully"), [=]() mutable
            {
                (as<std::shared_ptr<jest::Mock>>(mockRuntime->useModel))->mockRejectedValueOnce(std::make_shared<Error>(std::string("Embedding model fail")));
                shared experience = std::async([=]() { experienceService->recordExperience(object{
                    object::pair{std::string("learning"), std::string("No embedding")}
                }); });
                expect(experience->id)->toBeDefined();
                expect(experience->embedding)->toBeUndefined();
                auto experiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std::string("domain"), std::string("general")}
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
        describe(std::string("Error Handling"), [=]() mutable
        {
            it(std::string("should handle service unavailability gracefully"), [=]() mutable
            {
                auto mockRuntimeNoService = as<std::shared_ptr<IAgentRuntime>>(as<any>(utils::assign(object{
                    , 
                    object::pair{std::string("getService"), vi->fn()->mockReturnValue(nullptr)}
                }, mockRuntime)));
                auto ragProvider = experiencePlugin->providers->find([=](auto p) mutable
                {
                    return p["name"] == std::string("experienceRAG");
                }
                );
                auto result = std::async([=]() { ragProvider->get(mockRuntimeNoService, createMockMessage(std::string("q")), createMockState()); });
                expect(result->data->experiences)->toEqual(array<any>());
                expect(result->text)->toContain(std::string("not available"));
            }
            );
            it(std::string("should handle malformed queries gracefully"), [=]() mutable
            {
                auto experiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std::string("invalidField"), std::string("invalid")}
                }); });
                expect(Array->isArray(experiences))->toBe(true);
            }
            );
            it(std::string("should handle concurrent access safely"), [=]() mutable
            {
                auto promises = array<any>();
                for (auto i = 0; i < 10; i++)
                {
                    promises->push(experienceService->recordExperience(object{
                        object::pair{std::string("type"), ExperienceType::LEARNING}, 
                        object::pair{std::string("outcome"), OutcomeType::NEUTRAL}, 
                        object::pair{std::string("context"), std::string("Concurrent context ") + i + string_empty}, 
                        object::pair{std::string("action"), std::string("concurrent_action_") + i + string_empty}, 
                        object::pair{std::string("result"), std::string("Concurrent result ") + i + string_empty}, 
                        object::pair{std::string("learning"), std::string("Concurrent learning ") + i + string_empty}, 
                        object::pair{std::string("domain"), std::string("concurrent")}
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
                    object::pair{std::string("domain"), std::string("concurrent")}
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
