#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-experience/tests/experienceService.test.h"

any mockRuntime = as<std::shared_ptr<IAgentRuntime>>(as<any>(object{
    object::pair{std::string("agentId"), as<std::shared_ptr<const>>(std::string("test-agent-123"))}, 
    object::pair{std::string("getService"), vi->fn()}, 
    object::pair{std::string("useModel"), vi->fn()}, 
    object::pair{std::string("emitEvent"), vi->fn()}
}));

void Main(void)
{
    describe(std::string("ExperienceService"), [=]() mutable
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
        describe(std::string("recordExperience"), [=]() mutable
        {
            it(std::string("should record a basic experience"), [=]() mutable
            {
                auto experienceData = object{
                    object::pair{std::string("type"), ExperienceType::SUCCESS}, 
                    object::pair{std::string("outcome"), OutcomeType::POSITIVE}, 
                    object::pair{std::string("context"), std::string("Testing context")}, 
                    object::pair{std::string("action"), std::string("test_action")}, 
                    object::pair{std::string("result"), std::string("Test successful")}, 
                    object::pair{std::string("learning"), std::string("Testing works well")}, 
                    object::pair{std::string("domain"), std::string("testing")}, 
                    object::pair{std::string("tags"), array<string>{ std::string("test"), std::string("success") }}, 
                    object::pair{std::string("confidence"), 0.8}, 
                    object::pair{std::string("importance"), 0.7}
                };
                auto experience = std::async([=]() { experienceService->recordExperience(experienceData); });
                expect(experience->id)->toBeDefined();
                expect(experience->type)->toBe(ExperienceType::SUCCESS);
                expect(experience->outcome)->toBe(OutcomeType::POSITIVE);
                expect(experience->learning)->toBe(std::string("Testing works well"));
                expect(experience->confidence)->toBe(0.8);
                expect(experience->importance)->toBe(0.7);
                expect(experience->domain)->toBe(std::string("testing"));
                expect(experience->tags)->toEqual(array<string>{ std::string("test"), std::string("success") });
                expect(experience->agentId)->toBe(std::string("test-agent-123"));
                expect(experience->createdAt)->toBeDefined();
                expect(experience->accessCount)->toBe(0);
            }
            );
            it(std::string("should generate embeddings for experiences"), [=]() mutable
            {
                auto experienceData = object{
                    object::pair{std::string("type"), ExperienceType::LEARNING}, 
                    object::pair{std::string("outcome"), OutcomeType::NEUTRAL}, 
                    object::pair{std::string("context"), std::string("Learning context")}, 
                    object::pair{std::string("action"), std::string("learn_something")}, 
                    object::pair{std::string("result"), std::string("Knowledge gained")}, 
                    object::pair{std::string("learning"), std::string("New knowledge acquired")}, 
                    object::pair{std::string("domain"), std::string("general")}
                };
                auto experience = std::async([=]() { experienceService->recordExperience(experienceData); });
                expect(mockRuntime->useModel)->toHaveBeenCalledWith(std::string("TEXT_EMBEDDING"), expect->objectContaining(object{
                    object::pair{std::string("prompt"), expect->stringContaining(std::string("Learning context"))}
                }));
                expect(experience->embedding)->toEqual(array<double>{ 0.1, 0.2, 0.3, 0.4, 0.5 });
            }
            );
            it(std::string("should emit events when recording experiences"), [=]() mutable
            {
                auto experienceData = object{
                    object::pair{std::string("type"), ExperienceType::DISCOVERY}, 
                    object::pair{std::string("outcome"), OutcomeType::POSITIVE}, 
                    object::pair{std::string("context"), std::string("Discovery context")}, 
                    object::pair{std::string("action"), std::string("discover_something")}, 
                    object::pair{std::string("result"), std::string("New discovery made")}, 
                    object::pair{std::string("learning"), std::string("Discovered something interesting")}, 
                    object::pair{std::string("domain"), std::string("research")}
                };
                auto experience = std::async([=]() { experienceService->recordExperience(experienceData); });
                expect(mockRuntime->emitEvent)->toHaveBeenCalledWith(std::string("EXPERIENCE_RECORDED"), expect->objectContaining(object{
                    object::pair{std::string("experienceId"), experience->id}, 
                    object::pair{std::string("eventType"), std::string("created")}, 
                    object::pair{std::string("timestamp"), experience->createdAt}
                }));
            }
            );
            it(std::string("should handle missing optional fields with defaults"), [=]() mutable
            {
                auto experienceData = object{
                    object::pair{std::string("context"), std::string("Minimal context")}, 
                    object::pair{std::string("action"), std::string("minimal_action")}, 
                    object::pair{std::string("result"), std::string("Minimal result")}, 
                    object::pair{std::string("learning"), std::string("Minimal learning")}
                };
                auto experience = std::async([=]() { experienceService->recordExperience(experienceData); });
                expect(experience->type)->toBe(ExperienceType::LEARNING);
                expect(experience->outcome)->toBe(OutcomeType::NEUTRAL);
                expect(experience->confidence)->toBe(0.5);
                expect(experience->importance)->toBe(0.5);
                expect(experience->domain)->toBe(std::string("general"));
                expect(experience->tags)->toEqual(array<any>());
            }
            );
        }
        );
        describe(std::string("queryExperiences"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                std::async([=]() { experienceService->recordExperience(object{
                    object::pair{std::string("type"), ExperienceType::SUCCESS}, 
                    object::pair{std::string("outcome"), OutcomeType::POSITIVE}, 
                    object::pair{std::string("context"), std::string("Shell command execution")}, 
                    object::pair{std::string("action"), std::string("execute_command")}, 
                    object::pair{std::string("result"), std::string("Command executed successfully")}, 
                    object::pair{std::string("learning"), std::string("Shell commands work well")}, 
                    object::pair{std::string("domain"), std::string("shell")}, 
                    object::pair{std::string("tags"), array<string>{ std::string("shell"), std::string("command") }}, 
                    object::pair{std::string("confidence"), 0.9}, 
                    object::pair{std::string("importance"), 0.8}
                }); });
                std::async([=]() { experienceService->recordExperience(object{
                    object::pair{std::string("type"), ExperienceType::FAILURE}, 
                    object::pair{std::string("outcome"), OutcomeType::NEGATIVE}, 
                    object::pair{std::string("context"), std::string("Code compilation")}, 
                    object::pair{std::string("action"), std::string("compile_code")}, 
                    object::pair{std::string("result"), std::string("Compilation failed")}, 
                    object::pair{std::string("learning"), std::string("Need to check syntax")}, 
                    object::pair{std::string("domain"), std::string("coding")}, 
                    object::pair{std::string("tags"), array<string>{ std::string("coding"), std::string("compilation") }}, 
                    object::pair{std::string("confidence"), 0.8}, 
                    object::pair{std::string("importance"), 0.7}
                }); });
                std::async([=]() { experienceService->recordExperience(object{
                    object::pair{std::string("type"), ExperienceType::DISCOVERY}, 
                    object::pair{std::string("outcome"), OutcomeType::POSITIVE}, 
                    object::pair{std::string("context"), std::string("System exploration")}, 
                    object::pair{std::string("action"), std::string("explore_system")}, 
                    object::pair{std::string("result"), std::string("Found new tool")}, 
                    object::pair{std::string("learning"), std::string("System has useful tools")}, 
                    object::pair{std::string("domain"), std::string("system")}, 
                    object::pair{std::string("tags"), array<string>{ std::string("system"), std::string("tools") }}, 
                    object::pair{std::string("confidence"), 0.7}, 
                    object::pair{std::string("importance"), 0.9}
                }); });
            }
            );
            it(std::string("should query experiences by type"), [=]() mutable
            {
                auto experiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std::string("type"), ExperienceType::SUCCESS}
                }); });
                expect(experiences)->toHaveLength(1);
                expect(const_(experiences)[0]->type)->toBe(ExperienceType::SUCCESS);
                expect(const_(experiences)[0]->domain)->toBe(std::string("shell"));
            }
            );
            it(std::string("should query experiences by outcome"), [=]() mutable
            {
                auto experiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std::string("outcome"), OutcomeType::POSITIVE}
                }); });
                expect(experiences)->toHaveLength(2);
                expect(experiences->every([=](auto e) mutable
                {
                    return static_cast<long>(e->outcome) == OutcomeType::POSITIVE;
                }
                ))->toBe(true);
            }
            );
            it(std::string("should query experiences by domain"), [=]() mutable
            {
                auto experiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std::string("domain"), std::string("coding")}
                }); });
                expect(experiences)->toHaveLength(1);
                expect(const_(experiences)[0]->domain)->toBe(std::string("coding"));
                expect(const_(experiences)[0]->type)->toBe(ExperienceType::FAILURE);
            }
            );
            it(std::string("should query experiences by tags"), [=]() mutable
            {
                auto experiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std::string("tags"), array<string>{ std::string("shell") }}
                }); });
                expect(experiences)->toHaveLength(1);
                expect(const_(experiences)[0]->tags)->toContain(std::string("shell"));
            }
            );
            it(std::string("should filter by minimum importance"), [=]() mutable
            {
                auto experiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std::string("minImportance"), 0.85}
                }); });
                expect(experiences)->toHaveLength(1);
                expect(const_(experiences)[0]->importance)->toBeGreaterThanOrEqual(0.85);
                expect(const_(experiences)[0]->domain)->toBe(std::string("system"));
            }
            );
            it(std::string("should filter by minimum confidence"), [=]() mutable
            {
                auto experiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std::string("minConfidence"), 0.85}
                }); });
                expect(experiences)->toHaveLength(1);
                expect(const_(experiences)[0]->confidence)->toBeGreaterThanOrEqual(0.85);
                expect(const_(experiences)[0]->domain)->toBe(std::string("shell"));
            }
            );
            it(std::string("should limit results"), [=]() mutable
            {
                auto experiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std::string("limit"), 2}
                }); });
                expect(experiences)->toHaveLength(2);
            }
            );
            it(std::string("should filter by time range"), [=]() mutable
            {
                shared now = Date->now();
                shared oneHourAgo = now - 60 * 60 * 1000;
                auto experiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std::string("timeRange"), object{
                        object::pair{std::string("start"), oneHourAgo}, 
                        object::pair{std::string("end"), now}
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
            it(std::string("should update access counts when querying"), [=]() mutable
            {
                auto experiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std::string("type"), ExperienceType::SUCCESS}
                }); });
                expect(const_(experiences)[0]->accessCount)->toBe(1);
                expect(const_(experiences)[0]->lastAccessedAt)->toBeDefined();
                auto experiencesAgain = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std::string("type"), ExperienceType::SUCCESS}
                }); });
                expect(const_(experiencesAgain)[0]->accessCount)->toBe(2);
            }
            );
            it(std::string("should combine multiple filters"), [=]() mutable
            {
                auto experiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std::string("outcome"), OutcomeType::POSITIVE}, 
                    object::pair{std::string("minConfidence"), 0.8}, 
                    object::pair{std::string("domain"), std::string("shell")}
                }); });
                expect(experiences)->toHaveLength(1);
                expect(const_(experiences)[0]->outcome)->toBe(OutcomeType::POSITIVE);
                expect(const_(experiences)[0]->confidence)->toBeGreaterThanOrEqual(0.8);
                expect(const_(experiences)[0]->domain)->toBe(std::string("shell"));
            }
            );
        }
        );
        describe(std::string("findSimilarExperiences"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                mockRuntime->useModel = vi->fn()->mockResolvedValueOnce(array<double>{ 0.1, 0.2, 0.3, 0.4, 0.5 })->mockResolvedValueOnce(array<double>{ 0.2, 0.3, 0.4, 0.5, 0.6 })->mockResolvedValueOnce(array<double>{ 0.9, 0.8, 0.7, 0.6, 0.5 })->mockResolvedValueOnce(array<double>{ 0.15, 0.25, 0.35, 0.45, 0.55 });
                std::async([=]() { experienceService->recordExperience(object{
                    object::pair{std::string("type"), ExperienceType::SUCCESS}, 
                    object::pair{std::string("outcome"), OutcomeType::POSITIVE}, 
                    object::pair{std::string("context"), std::string("Shell command execution")}, 
                    object::pair{std::string("action"), std::string("execute_command")}, 
                    object::pair{std::string("result"), std::string("Command executed successfully")}, 
                    object::pair{std::string("learning"), std::string("Shell commands work well")}, 
                    object::pair{std::string("domain"), std::string("shell")}
                }); });
                std::async([=]() { experienceService->recordExperience(object{
                    object::pair{std::string("type"), ExperienceType::SUCCESS}, 
                    object::pair{std::string("outcome"), OutcomeType::POSITIVE}, 
                    object::pair{std::string("context"), std::string("Terminal command execution")}, 
                    object::pair{std::string("action"), std::string("run_command")}, 
                    object::pair{std::string("result"), std::string("Command ran successfully")}, 
                    object::pair{std::string("learning"), std::string("Terminal commands are effective")}, 
                    object::pair{std::string("domain"), std::string("shell")}
                }); });
                std::async([=]() { experienceService->recordExperience(object{
                    object::pair{std::string("type"), ExperienceType::FAILURE}, 
                    object::pair{std::string("outcome"), OutcomeType::NEGATIVE}, 
                    object::pair{std::string("context"), std::string("Database query")}, 
                    object::pair{std::string("action"), std::string("query_database")}, 
                    object::pair{std::string("result"), std::string("Query failed")}, 
                    object::pair{std::string("learning"), std::string("Database connection issues")}, 
                    object::pair{std::string("domain"), std::string("database")}
                }); });
            }
            );
            it(std::string("should find similar experiences based on semantic similarity"), [=]() mutable
            {
                auto similar = std::async([=]() { experienceService->findSimilarExperiences(std::string("shell command execution"), 2); });
                expect(similar)->toHaveLength(2);
                expect(const_(similar)[0]->domain)->toBe(std::string("shell"));
                expect(const_(similar)[0]->accessCount)->toBe(1);
            }
            );
            it(std::string("should return empty array for empty query"), [=]() mutable
            {
                auto similar = std::async([=]() { experienceService->findSimilarExperiences(string_empty, 5); });
                expect(similar)->toHaveLength(0);
            }
            );
            it(std::string("should handle embedding generation errors gracefully"), [=]() mutable
            {
                mockRuntime->useModel = vi->fn()->mockRejectedValue(std::make_shared<Error>(std::string("Embedding failed")));
                auto similar = std::async([=]() { experienceService->findSimilarExperiences(std::string("test query"), 5); });
                expect(similar)->toHaveLength(0);
            }
            );
        }
        );
        describe(std::string("analyzeExperiences"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                for (auto i = 0; i < 5; i++)
                {
                    std::async([=]() { experienceService->recordExperience(object{
                        object::pair{std::string("type"), ExperienceType::SUCCESS}, 
                        object::pair{std::string("outcome"), OutcomeType::POSITIVE}, 
                        object::pair{std::string("context"), std::string("Shell command ") + i + string_empty}, 
                        object::pair{std::string("action"), std::string("execute_command")}, 
                        object::pair{std::string("result"), std::string("Command ") + i + std::string(" executed successfully")}, 
                        object::pair{std::string("learning"), std::string("Shell command ") + i + std::string(" works well")}, 
                        object::pair{std::string("domain"), std::string("shell")}, 
                        object::pair{std::string("confidence"), 0.8 + i * 0.02}, 
                        object::pair{std::string("importance"), 0.7}
                    }); });
                }
                std::async([=]() { experienceService->recordExperience(object{
                    object::pair{std::string("type"), ExperienceType::FAILURE}, 
                    object::pair{std::string("outcome"), OutcomeType::NEGATIVE}, 
                    object::pair{std::string("context"), std::string("Shell command failure")}, 
                    object::pair{std::string("action"), std::string("execute_command")}, 
                    object::pair{std::string("result"), std::string("Command failed")}, 
                    object::pair{std::string("learning"), std::string("Some shell commands fail")}, 
                    object::pair{std::string("domain"), std::string("shell")}, 
                    object::pair{std::string("confidence"), 0.9}, 
                    object::pair{std::string("importance"), 0.8}
                }); });
            }
            );
            it(std::string("should analyze experiences for a domain"), [=]() mutable
            {
                auto analysis = std::async([=]() { experienceService->analyzeExperiences(std::string("shell")); });
                expect(analysis->frequency)->toBe(6);
                expect(analysis->reliability)->toBeGreaterThan(0.5);
                expect(analysis->pattern)->toContain(std::string("command"));
                expect(analysis->recommendations)->toBeDefined();
                expect(analysis->alternatives)->toBeDefined();
            }
            );
            it(std::string("should analyze experiences for a specific type"), [=]() mutable
            {
                auto analysis = std::async([=]() { experienceService->analyzeExperiences(std::string("shell"), ExperienceType::SUCCESS); });
                expect(analysis->frequency)->toBe(5);
                expect(analysis->reliability)->toBeGreaterThan(0.8);
            }
            );
            it(std::string("should return empty analysis for no experiences"), [=]() mutable
            {
                auto analysis = std::async([=]() { experienceService->analyzeExperiences(std::string("nonexistent")); });
                expect(analysis->frequency)->toBe(0);
                expect(analysis->reliability)->toBe(0);
                expect(analysis->pattern)->toContain(std::string("No experiences found"));
            }
            );
            it(std::string("should generate recommendations based on reliability"), [=]() mutable
            {
                auto analysis = std::async([=]() { experienceService->analyzeExperiences(std::string("shell")); });
                expect(analysis->recommendations)->toContain(std::string("Continue using successful approaches"));
            }
            );
        }
        );
        describe(std::string("memory management"), [=]() mutable
        {
            it(std::string("should prune old experiences when limit is exceeded"), [=]() mutable
            {
                (as<any>(experienceService))["maxExperiences"] = 3;
                for (auto i = 0; i < 5; i++)
                {
                    std::async([=]() { experienceService->recordExperience(object{
                        object::pair{std::string("type"), ExperienceType::LEARNING}, 
                        object::pair{std::string("outcome"), OutcomeType::NEUTRAL}, 
                        object::pair{std::string("context"), std::string("Context ") + i + string_empty}, 
                        object::pair{std::string("action"), std::string("action_") + i + string_empty}, 
                        object::pair{std::string("result"), std::string("Result ") + i + string_empty}, 
                        object::pair{std::string("learning"), std::string("Learning ") + i + string_empty}, 
                        object::pair{std::string("domain"), std::string("test")}, 
                        object::pair{std::string("confidence"), 0.5}, 
                        object::pair{std::string("importance"), (i < 2) ? 0.1 : 0.9}
                    }); });
                }
                auto allExperiences = std::async([=]() { experienceService->queryExperiences(object{
                    object::pair{std::string("limit"), 10}
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
        describe(std::string("error handling"), [=]() mutable
        {
            it(std::string("should handle embedding generation errors gracefully"), [=]() mutable
            {
                mockRuntime->useModel = vi->fn()->mockRejectedValue(std::make_shared<Error>(std::string("Model error")));
                auto experience = std::async([=]() { experienceService->recordExperience(object{
                    object::pair{std::string("type"), ExperienceType::LEARNING}, 
                    object::pair{std::string("outcome"), OutcomeType::NEUTRAL}, 
                    object::pair{std::string("context"), std::string("Test context")}, 
                    object::pair{std::string("action"), std::string("test_action")}, 
                    object::pair{std::string("result"), std::string("Test result")}, 
                    object::pair{std::string("learning"), std::string("Test learning")}, 
                    object::pair{std::string("domain"), std::string("test")}
                }); });
                expect(experience->id)->toBeDefined();
                expect(experience->embedding)->toBeUndefined();
            }
            );
            it(std::string("should handle event emission errors gracefully"), [=]() mutable
            {
                mockRuntime->emitEvent = vi->fn()->mockRejectedValue(std::make_shared<Error>(std::string("Event error")));
                auto experience = std::async([=]() { experienceService->recordExperience(object{
                    object::pair{std::string("type"), ExperienceType::LEARNING}, 
                    object::pair{std::string("outcome"), OutcomeType::NEUTRAL}, 
                    object::pair{std::string("context"), std::string("Test context")}, 
                    object::pair{std::string("action"), std::string("test_action")}, 
                    object::pair{std::string("result"), std::string("Test result")}, 
                    object::pair{std::string("learning"), std::string("Test learning")}, 
                    object::pair{std::string("domain"), std::string("test")}
                }); });
                expect(experience->id)->toBeDefined();
            }
            );
        }
        );
        describe(std::string("cosine similarity calculation"), [=]() mutable
        {
            it(std::string("should calculate cosine similarity correctly"), [=]() mutable
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
