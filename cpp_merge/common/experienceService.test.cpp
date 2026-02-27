#include "experienceService.test.h"

any mockRuntime = as<std::shared_ptr<IAgentRuntime>>(as<any>(object{
    object::pair{std:("agentId"), as<std::shared_ptr<const>>(std:("test-agent-123"))}, 
    object::pair{std:("getService"), vi->fn()}, 
    object::pair{std:("useModel"), vi->fn()}, 
    object::pair{std:("emitEvent"), vi->fn()}
}));

void Main(void)
{
    describe(std:("ExperienceService"), [=]() mutable
    {
        shared<std::shared_ptr<ExperienceService>> experienceService;
        beforeEach([=]() mutable
        {
            vi->clearAllMocks();
            experienceService = std::make_shared<ExperienceService>(mockRuntime);
            mockRuntime->useModel = vi->fn()->mockResolvedValue(array<double>{ 0.1, 0.2, 0.3, 0.4, 0.5 });
        }
        );
        afterEach([=]() mutable
        {
            std::async([=]() { experienceService->stop(); });
        }
        );
        describe(std:("recordExperience"), [=]() mutable
        {
            it(std:("should record a basic experience"), [=]() mutable
            {
                auto experienceData = object{
                    object::pair{std:("type"), ExperienceType::SUCCESS}, 
                    object::pair{std:("outcome"), OutcomeType::POSITIVE}, 
                    object::pair{std:("context"), std:("Testing context")}, 
                    object::pair{std:("action"), std:("test_action")}, 
                    object::pair{std:("result"), std:("Test successful")}, 
                    object::pair{std:("learning"), std:("Testing works well")}, 
                    object::pair{std:("domain"), std:("testing")}, 
                    object::pair{std:("tags"), array<string>{ std:("test"), std:("success") }}, 
                    object::pair{std:("confidence"), 0.8}, 
                    object::pair{std:("importance"), 0.7}
                };
                auto experience = std::async([=]() { experienceService->recordExperience(experienceData); });
                expect(experience->id)->toBeDefined();
                expect(experience->type)->toBe(ExperienceType::SUCCESS);
                expect(experience->outcome)->toBe(OutcomeType::POSITIVE);
                expect(experience->learning)->toBe(std:("Testing works well"));
                expect(experience->confidence)->toBe(0.8);
                expect(experience->importance)->toBe(0.7);
                expect(experience->domain)->toBe(std:("testing"));
                expect(experience->tags)->toEqual(array<string>{ std:("test"), std:("success") });
                expect(experience->agentId)->toBe(std:("test-agent-123"));
                expect(experience->createdAt)->toBeDefined();
                expect(experience->accessCount)->toBe(0);
            }
            );
            it(std:("should generate embeddings for experiences"), [=]() mutable
            {
                auto experienceData = object{
                    object::pair{std:("type"), ExperienceType::LEARNING}, 
                    object::pair{std:("outcome"), OutcomeType::NEUTRAL}, 
                    object::pair{std:("context"), std:("Learning context")}, 
                    object::pair{std:("action"), std:("learn_something")}, 
                    object::pair{std:("result"), std:("Knowledge gained")}, 
                    object::pair{std:("learning"), std:("New knowledge acquired")}, 
                    object::pair{std:("domain"), std:("general")}
                };
                auto experience = std::async([=]() { experienceService->recordExperience(experienceData); });
                expect(mockRuntime->useModel)->toHaveBeenCalledWith(std:("TEXT_EMBEDDING"), expect->objectContaining(object{
                    object::pair{std:("prompt"), expect->stringContaining(std:("Learning context"))}
                }));
                expect(experience->embedding)->toEqual(array<double>{ 0.1, 0.2, 0.3, 0.4, 0.5 });
            }
            );
            it(std:("should emit events when recording experiences"), [=]() mutable
            {
                auto experienceData = object{
                    object::pair{std:("type"), ExperienceType::DISCOVERY}, 
                    object::pair{std:("outcome"), OutcomeType::POSITIVE}, 
                    object::pair{std:("context"), std:("Discovery context")}, 
                    object::pair{std:("action"), std:("discover_something")}, 
                    object::pair{std:("result"), std:("New discovery made")}, 
                    object::pair{std:("learning"), std:("Discovered something interesting")}, 
                    object::pair{std:("domain"), std:("research")}
                };
                auto experience = std::async([=]() { experienceService->recordExperience(experienceData); });
                expect(mockRuntime->emitEvent)->toHaveBeenCalledWith(std:("EXPERIENCE_RECORDED"), expect->objectContaining(object{
                    object::pair{std:("experienceId"), experience->id}, 
                    object::pair{std:("eventType"), std:("created")}, 
                    object::pair{std:("timestamp"), experience->createdAt}
                }));
            }
            );
            it(std:("should handle missing optional fields with defaults"), [=]() mutable
            {
                auto experienceData = object{
                    object::pair{std:("context"), std:("Minimal context")}, 
                    object::pair{std:("action"), std:("minimal_action")}, 
                    object::pair{std:("result"), std:("Minimal result")}, 
                    object::pair{std:("learning"), std:("Minimal learning")}
                };
                auto experience = std::async([=]() { experienceService->recordExperience(experienceData); });
                expect(experience->type)->toBe(ExperienceType::LEARNING);
                expect(experience->outcome)->toBe(OutcomeType::NEUTRAL);
                expect(experience->confidence)->toBe(0.5);
                expect(experience->importance)->toBe(0.5);
                expect(experience->domain)->toBe(std:("general"));
                expect(experience->tags)->toEqual(array<any>());
            }
            );
        }
        );
        describe(std:("queryExperiences"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                std::async([=]() { experienceService->recordExperience(object{
                    object::pair{std:("type"), ExperienceType::SUCCESS}, 
                    object::pair{std:("outcome"), OutcomeType::POSITIVE}, 
                    object::pair{std:("context"), std:("Shell command execution")}, 
                    object::pair{std:("action"), std:("execute_command")}, 
                    object::pair{std:("result"), std:("Command executed successfully")}, 
                    object::pair{std:("learning"), std:("Shell commands work well")}, 
                    object::pair{std:("domain"), std:("shell")}, 
                    object::pair{std:("tags"), array<string>{ std:("shell"), std:("command") }}, 
                    object::pair{std:("confidence"), 0.9}, 
                    object::pair{std:("importance"), 0.8}
                }); });
                std::async([=]() { experienceService->recordExperience(object{
                    object::pair{std:("type"), ExperienceType::FAILURE}, 
                    object::pair{std:("outcome"), OutcomeType::NEGATIVE}, 
                    object::pair{std:("context"), std:("Code compilation")}, 
                    object::pair{std:("action"), std:("compile_code")}, 
                    object::pair{std:("result"), std:("Compilation failed")}, 
                    object::pair{std:("learning"), std:("Need to check syntax")}, 
                    object::pair{std:("domain"), std:("coding")}, 
                    object::pair{std:("tags"), array<string>{ std:("coding"), std:("compilation") }}, 
                    object::pair{std:("confidence"), 0.8}, 
                    object::pair{std:("importance"), 0.7}
                }); });
                std::async([=]() { experienceService->recordExperience(object{
                    object::pair{std:("type"), ExperienceType::DISCOVERY}, 
                    object::pair{std:("outcome"), OutcomeType::POSITIVE}, 
                    object::pair{std:("context"), std:("System exploration")}, 
                    object::pair{std:("action"), std:("explore_system")}, 
                    object::pair{std:("result"), std:("Found new tool")}, 
                    object::pair{std:("learning"), std:("System has useful tools")}, 
                    object::pair{std:("domain"), std:("system")}, 
                    object::pair{std:("tags"), array<string>{ std:("system"), std:("tools") }}, 
                    object::pair{std:("confidence"), 0.7}, 
                    object::pair{std:("importance"), 0.9}
                }); });
            }
            );
            it(std:("should query experiences by type"), [=]() mutable
            {
                auto experiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std:("type"), ExperienceType::SUCCESS}
                }); });
                expect(experiences)->toHaveLength(1);
                expect(const_(experiences)[0]->type)->toBe(ExperienceType::SUCCESS);
                expect(const_(experiences)[0]->domain)->toBe(std:("shell"));
            }
            );
            it(std:("should query experiences by outcome"), [=]() mutable
            {
                auto experiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std:("outcome"), OutcomeType::POSITIVE}
                }); });
                expect(experiences)->toHaveLength(2);
                expect(experiences->every([=](auto e) mutable
                {
                    return static_cast<long>(e->outcome) == OutcomeType::POSITIVE;
                }
                ))->toBe(true);
            }
            );
            it(std:("should query experiences by domain"), [=]() mutable
            {
                auto experiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std:("domain"), std:("coding")}
                }); });
                expect(experiences)->toHaveLength(1);
                expect(const_(experiences)[0]->domain)->toBe(std:("coding"));
                expect(const_(experiences)[0]->type)->toBe(ExperienceType::FAILURE);
            }
            );
            it(std:("should query experiences by tags"), [=]() mutable
            {
                auto experiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std:("tags"), array<string>{ std:("shell") }}
                }); });
                expect(experiences)->toHaveLength(1);
                expect(const_(experiences)[0]->tags)->toContain(std:("shell"));
            }
            );
            it(std:("should filter by minimum importance"), [=]() mutable
            {
                auto experiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std:("minImportance"), 0.85}
                }); });
                expect(experiences)->toHaveLength(1);
                expect(const_(experiences)[0]->importance)->toBeGreaterThanOrEqual(0.85);
                expect(const_(experiences)[0]->domain)->toBe(std:("system"));
            }
            );
            it(std:("should filter by minimum confidence"), [=]() mutable
            {
                auto experiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std:("minConfidence"), 0.85}
                }); });
                expect(experiences)->toHaveLength(1);
                expect(const_(experiences)[0]->confidence)->toBeGreaterThanOrEqual(0.85);
                expect(const_(experiences)[0]->domain)->toBe(std:("shell"));
            }
            );
            it(std:("should limit results"), [=]() mutable
            {
                auto experiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std:("limit"), 2}
                }); });
                expect(experiences)->toHaveLength(2);
            }
            );
            it(std:("should filter by time range"), [=]() mutable
            {
                shared now = Date->now();
                shared oneHourAgo = now - 60 * 60 * 1000;
                auto experiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std:("timeRange"), object{
                        object::pair{std:("start"), oneHourAgo}, 
                        object::pair{std:("end"), now}
                    }}
                }); });
                expect(experiences->get_length())->toBeGreaterThan(0);
                expect(experiences->every([=](auto e) mutable
                {
                    return AND((e->createdAt >= oneHourAgo), (e->createdAt <= now));
                }
                ))->toBe(true);
            }
            );
            it(std:("should update access counts when querying"), [=]() mutable
            {
                auto experiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std:("type"), ExperienceType::SUCCESS}
                }); });
                expect(const_(experiences)[0]->accessCount)->toBe(1);
                expect(const_(experiences)[0]->lastAccessedAt)->toBeDefined();
                auto experiencesAgain = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std:("type"), ExperienceType::SUCCESS}
                }); });
                expect(const_(experiencesAgain)[0]->accessCount)->toBe(2);
            }
            );
            it(std:("should combine multiple filters"), [=]() mutable
            {
                auto experiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std:("outcome"), OutcomeType::POSITIVE}, 
                    object::pair{std:("minConfidence"), 0.8}, 
                    object::pair{std:("domain"), std:("shell")}
                }); });
                expect(experiences)->toHaveLength(1);
                expect(const_(experiences)[0]->outcome)->toBe(OutcomeType::POSITIVE);
                expect(const_(experiences)[0]->confidence)->toBeGreaterThanOrEqual(0.8);
                expect(const_(experiences)[0]->domain)->toBe(std:("shell"));
            }
            );
        }
        );
        describe(std:("findSimilarExperiences"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                mockRuntime->useModel = vi->fn()->mockResolvedValueOnce(array<double>{ 0.1, 0.2, 0.3, 0.4, 0.5 })->mockResolvedValueOnce(array<double>{ 0.2, 0.3, 0.4, 0.5, 0.6 })->mockResolvedValueOnce(array<double>{ 0.9, 0.8, 0.7, 0.6, 0.5 })->mockResolvedValueOnce(array<double>{ 0.15, 0.25, 0.35, 0.45, 0.55 });
                std::async([=]() { experienceService->recordExperience(object{
                    object::pair{std:("type"), ExperienceType::SUCCESS}, 
                    object::pair{std:("outcome"), OutcomeType::POSITIVE}, 
                    object::pair{std:("context"), std:("Shell command execution")}, 
                    object::pair{std:("action"), std:("execute_command")}, 
                    object::pair{std:("result"), std:("Command executed successfully")}, 
                    object::pair{std:("learning"), std:("Shell commands work well")}, 
                    object::pair{std:("domain"), std:("shell")}
                }); });
                std::async([=]() { experienceService->recordExperience(object{
                    object::pair{std:("type"), ExperienceType::SUCCESS}, 
                    object::pair{std:("outcome"), OutcomeType::POSITIVE}, 
                    object::pair{std:("context"), std:("Terminal command execution")}, 
                    object::pair{std:("action"), std:("run_command")}, 
                    object::pair{std:("result"), std:("Command ran successfully")}, 
                    object::pair{std:("learning"), std:("Terminal commands are effective")}, 
                    object::pair{std:("domain"), std:("shell")}
                }); });
                std::async([=]() { experienceService->recordExperience(object{
                    object::pair{std:("type"), ExperienceType::FAILURE}, 
                    object::pair{std:("outcome"), OutcomeType::NEGATIVE}, 
                    object::pair{std:("context"), std:("Database query")}, 
                    object::pair{std:("action"), std:("query_database")}, 
                    object::pair{std:("result"), std:("Query failed")}, 
                    object::pair{std:("learning"), std:("Database connection issues")}, 
                    object::pair{std:("domain"), std:("database")}
                }); });
            }
            );
            it(std:("should find similar experiences based on semantic similarity"), [=]() mutable
            {
                auto similar = std::async([=]() { experienceService->findSimilarExperiences(std:("shell command execution"), 2); });
                expect(similar)->toHaveLength(2);
                expect(const_(similar)[0]->domain)->toBe(std:("shell"));
                expect(const_(similar)[0]->accessCount)->toBe(1);
            }
            );
            it(std:("should return empty array for empty query"), [=]() mutable
            {
                auto similar = std::async([=]() { experienceService->findSimilarExperiences(string_empty, 5); });
                expect(similar)->toHaveLength(0);
            }
            );
            it(std:("should handle embedding generation errors gracefully"), [=]() mutable
            {
                mockRuntime->useModel = vi->fn()->mockRejectedValue(std::make_shared<Error>(std:("Embedding failed")));
                auto similar = std::async([=]() { experienceService->findSimilarExperiences(std:("test query"), 5); });
                expect(similar)->toHaveLength(0);
            }
            );
        }
        );
        describe(std:("analyzeExperiences"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                for (auto i = 0; i < 5; i++)
                {
                    std::async([=]() { experienceService->recordExperience(object{
                        object::pair{std:("type"), ExperienceType::SUCCESS}, 
                        object::pair{std:("outcome"), OutcomeType::POSITIVE}, 
                        object::pair{std:("context"), std:("Shell command ") + i + string_empty}, 
                        object::pair{std:("action"), std:("execute_command")}, 
                        object::pair{std:("result"), std:("Command ") + i + std:(" executed successfully")}, 
                        object::pair{std:("learning"), std:("Shell command ") + i + std:(" works well")}, 
                        object::pair{std:("domain"), std:("shell")}, 
                        object::pair{std:("confidence"), 0.8 + i * 0.02}, 
                        object::pair{std:("importance"), 0.7}
                    }); });
                }
                std::async([=]() { experienceService->recordExperience(object{
                    object::pair{std:("type"), ExperienceType::FAILURE}, 
                    object::pair{std:("outcome"), OutcomeType::NEGATIVE}, 
                    object::pair{std:("context"), std:("Shell command failure")}, 
                    object::pair{std:("action"), std:("execute_command")}, 
                    object::pair{std:("result"), std:("Command failed")}, 
                    object::pair{std:("learning"), std:("Some shell commands fail")}, 
                    object::pair{std:("domain"), std:("shell")}, 
                    object::pair{std:("confidence"), 0.9}, 
                    object::pair{std:("importance"), 0.8}
                }); });
            }
            );
            it(std:("should analyze experiences for a domain"), [=]() mutable
            {
                auto analysis = std::async([=]() { experienceService->analyzeExperiences(std:("shell")); });
                expect(analysis->frequency)->toBe(6);
                expect(analysis->reliability)->toBeGreaterThan(0.5);
                expect(analysis->pattern)->toContain(std:("command"));
                expect(analysis->recommendations)->toBeDefined();
                expect(analysis->alternatives)->toBeDefined();
            }
            );
            it(std:("should analyze experiences for a specific type"), [=]() mutable
            {
                auto analysis = std::async([=]() { experienceService->analyzeExperiences(std:("shell"), ExperienceType::SUCCESS); });
                expect(analysis->frequency)->toBe(5);
                expect(analysis->reliability)->toBeGreaterThan(0.8);
            }
            );
            it(std:("should return empty analysis for no experiences"), [=]() mutable
            {
                auto analysis = std::async([=]() { experienceService->analyzeExperiences(std:("nonexistent")); });
                expect(analysis->frequency)->toBe(0);
                expect(analysis->reliability)->toBe(0);
                expect(analysis->pattern)->toContain(std:("No experiences found"));
            }
            );
            it(std:("should generate recommendations based on reliability"), [=]() mutable
            {
                auto analysis = std::async([=]() { experienceService->analyzeExperiences(std:("shell")); });
                expect(analysis->recommendations)->toContain(std:("Continue using successful approaches"));
            }
            );
        }
        );
        describe(std:("memory management"), [=]() mutable
        {
            it(std:("should prune old experiences when limit is exceeded"), [=]() mutable
            {
                (as<any>(experienceService))["maxExperiences"] = 3;
                for (auto i = 0; i < 5; i++)
                {
                    std::async([=]() { experienceService->recordExperience(object{
                        object::pair{std:("type"), ExperienceType::LEARNING}, 
                        object::pair{std:("outcome"), OutcomeType::NEUTRAL}, 
                        object::pair{std:("context"), std:("Context ") + i + string_empty}, 
                        object::pair{std:("action"), std:("action_") + i + string_empty}, 
                        object::pair{std:("result"), std:("Result ") + i + string_empty}, 
                        object::pair{std:("learning"), std:("Learning ") + i + string_empty}, 
                        object::pair{std:("domain"), std:("test")}, 
                        object::pair{std:("confidence"), 0.5}, 
                        object::pair{std:("importance"), (i < 2) ? 0.1 : 0.9}
                    }); });
                }
                auto allExperiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std:("limit"), 10}
                }); });
                expect(allExperiences->get_length())->toBeLessThanOrEqual(3);
                auto highImportanceRemaining = allExperiences->filter([=](auto e) mutable
                {
                    return e->importance > 0.5;
                }
                );
                expect(highImportanceRemaining->get_length())->toBeGreaterThan(0);
            }
            );
        }
        );
        describe(std:("error handling"), [=]() mutable
        {
            it(std:("should handle embedding generation errors gracefully"), [=]() mutable
            {
                mockRuntime->useModel = vi->fn()->mockRejectedValue(std::make_shared<Error>(std:("Model error")));
                auto experience = std::async([=]() { experienceService->recordExperience(object{
                    object::pair{std:("type"), ExperienceType::LEARNING}, 
                    object::pair{std:("outcome"), OutcomeType::NEUTRAL}, 
                    object::pair{std:("context"), std:("Test context")}, 
                    object::pair{std:("action"), std:("test_action")}, 
                    object::pair{std:("result"), std:("Test result")}, 
                    object::pair{std:("learning"), std:("Test learning")}, 
                    object::pair{std:("domain"), std:("test")}
                }); });
                expect(experience->id)->toBeDefined();
                expect(experience->embedding)->toBeUndefined();
            }
            );
            it(std:("should handle event emission errors gracefully"), [=]() mutable
            {
                mockRuntime->emitEvent = vi->fn()->mockRejectedValue(std::make_shared<Error>(std:("Event error")));
                auto experience = std::async([=]() { experienceService->recordExperience(object{
                    object::pair{std:("type"), ExperienceType::LEARNING}, 
                    object::pair{std:("outcome"), OutcomeType::NEUTRAL}, 
                    object::pair{std:("context"), std:("Test context")}, 
                    object::pair{std:("action"), std:("test_action")}, 
                    object::pair{std:("result"), std:("Test result")}, 
                    object::pair{std:("learning"), std:("Test learning")}, 
                    object::pair{std:("domain"), std:("test")}
                }); });
                expect(experience->id)->toBeDefined();
            }
            );
        }
        );
        describe(std:("cosine similarity calculation"), [=]() mutable
        {
            it(std:("should calculate cosine similarity correctly"), [=]() mutable
            {
                auto service = as<any>(experienceService);
                auto similarity1 = service["cosineSimilarity"](array<double>{ 1, 0, 0 }, array<double>{ 1, 0, 0 });
                expect(similarity1)->toBe(1);
                auto similarity2 = service["cosineSimilarity"](array<double>{ 1, 0, 0 }, array<double>{ 0, 1, 0 });
                expect(similarity2)->toBe(0);
                auto similarity3 = service["cosineSimilarity"](array<double>{ 1, 0, 0 }, array<any>{ -1, 0, 0 });
                expect(similarity3)->toBe(-1);
                auto similarity4 = service["cosineSimilarity"](array<double>{ 1, 0 }, array<double>{ 1, 0, 0 });
                expect(similarity4)->toBe(0);
                auto similarity5 = service["cosineSimilarity"](array<double>{ 0, 0, 0 }, array<double>{ 1, 0, 0 });
                expect(similarity5)->toBe(0);
            }
            );
        }
        );
    }
    );
}

MAIN
