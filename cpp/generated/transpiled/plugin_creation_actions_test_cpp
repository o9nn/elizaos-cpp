#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-dynamic/__tests__/plugin-creation-actions.test.h"

std::function<any(string)> createMockMemory = [=](auto text) mutable
{
    return as<std::shared_ptr<Memory>>(as<any>((object{
        object::pair{std::string("id"), as<any>(crypto->randomUUID())}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("text")}
        }}, 
        object::pair{std::string("userId"), as<any>(crypto->randomUUID())}, 
        object::pair{std::string("roomId"), as<any>(crypto->randomUUID())}, 
        object::pair{std::string("entityId"), std::string("entity-id")}, 
        object::pair{std::string("createdAt"), Date->now()}
    })));
};
std::function<any()> createMockRuntime = [=]() mutable
{
    auto service = as<std::shared_ptr<PluginCreationService>>(as<any>(object{
        object::pair{std::string("getAllJobs"), vi->fn()->mockReturnValue(array<any>())}, 
        object::pair{std::string("createPlugin"), vi->fn()->mockReturnValue(std::string("job-123"))}, 
        object::pair{std::string("getJobStatus"), vi->fn()}, 
        object::pair{std::string("cancelJob"), vi->fn()}
    }));
    return as<any>(object{
        object::pair{std::string("services"), object{
            object::pair{std::string("get"), vi->fn()->mockReturnValue(service)}
        }}, 
        object::pair{std::string("getSetting"), vi->fn()}
    });
};

void Main(void)
{
    describe(std::string("Plugin Creation Actions"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> runtime;
        shared<std::shared_ptr<State>> state;
        beforeEach([=]() mutable
        {
            runtime = createMockRuntime();
            state = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}
            };
            vi->clearAllMocks();
        }
        );
        describe(std::string("createPluginAction"), [=]() mutable
        {
            shared validSpec = JSON->stringify(object{
                object::pair{std::string("name"), std::string("@test/plugin")}, 
                object::pair{std::string("description"), std::string("Test plugin for testing")}, 
                object::pair{std::string("version"), std::string("1.0.0")}, 
                object::pair{std::string("actions"), array<object>{ object{
                    object::pair{std::string("name"), std::string("testAction")}, 
                    object::pair{std::string("description"), std::string("Test")}
                } }}
            });
            it(std::string("should validate when no active jobs and valid JSON"), [=]() mutable
            {
                auto message = createMockMemory(validSpec);
                auto result = std::async([=]() { createPluginAction->validate(runtime, message, state); });
                expect(result)->toBe(true);
            }
            );
            it(std::string("should not validate when active job exists"), [=]() mutable
            {
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std::string("plugin_creation")));
                (as<any>(std::bind(&PluginCreationService::getAllJobs, service)))["mockReturnValue"](array<object>{ object{
                    object::pair{std::string("status"), std::string("running")}
                } });
                auto message = createMockMemory(validSpec);
                auto result = std::async([=]() { createPluginAction->validate(runtime, message, state); });
                expect(result)->toBe(false);
            }
            );
            it(std::string("should not validate with invalid JSON"), [=]() mutable
            {
                auto message = createMockMemory(std::string("not json"));
                auto result = std::async([=]() { createPluginAction->validate(runtime, message, state); });
                expect(result)->toBe(false);
            }
            );
            it(std::string("should not validate when service unavailable"), [=]() mutable
            {
                (as<any>(runtime->services->get))["mockReturnValue"](nullptr);
                auto message = createMockMemory(validSpec);
                auto result = std::async([=]() { createPluginAction->validate(runtime, message, state); });
                expect(result)->toBe(false);
            }
            );
            it(std::string("should handle plugin creation with valid spec"), [=]() mutable
            {
                (as<any>(runtime->getSetting))["mockReturnValue"](std::string("test-api-key"));
                auto message = createMockMemory(validSpec);
                auto result = std::async([=]() { createPluginAction->handler(runtime, message, state); });
                expect(result)->toContain(std::string("Plugin creation job started successfully!"));
                expect(result)->toContain(std::string("Job ID: job-123"));
                expect(result)->toContain(std::string("@test/plugin"));
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std::string("plugin_creation")));
                expect(std::bind(&PluginCreationService::createPlugin, service, std::placeholders::_1, std::placeholders::_2))->toHaveBeenCalledWith(expect->objectContaining(object{
                    object::pair{std::string("name"), std::string("@test/plugin")}, 
                    object::pair{std::string("description"), std::string("Test plugin for testing")}
                }), std::string("test-api-key"));
            }
            );
            it(std::string("should validate plugin specification"), [=]() mutable
            {
                auto invalidSpecs = array<object>{ object{
                    object::pair{std::string("name"), std::string("invalid name")}, 
                    object::pair{std::string("description"), std::string("test")}
                }, object{
                    object::pair{std::string("name"), std::string("@test/plugin")}, 
                    object::pair{std::string("description"), std::string("short")}
                }, object{
                    object::pair{std::string("name"), std::string("@test/plugin")}, 
                    object::pair{std::string("description"), std::string("Valid description")}, 
                    object::pair{std::string("version"), std::string("invalid")}
                }, object{
                    object::pair{std::string("name"), std::string("../../../etc/passwd")}, 
                    object::pair{std::string("description"), std::string("Path traversal attempt")}
                } };
                for (auto& spec : invalidSpecs)
                {
                    auto message = createMockMemory(JSON->stringify(spec));
                    auto result = std::async([=]() { createPluginAction->handler(runtime, message, state); });
                    expect(result)->toContain(std::string("Invalid plugin specification"));
                }
            }
            );
            it(std::string("should handle missing API key"), [=]() mutable
            {
                (as<any>(runtime->getSetting))["mockReturnValue"](nullptr);
                auto message = createMockMemory(validSpec);
                auto result = std::async([=]() { createPluginAction->handler(runtime, message, state); });
                expect(result)->toContain(std::string("ANTHROPIC_API_KEY is not configured"));
            }
            );
            it(std::string("should handle service unavailable"), [=]() mutable
            {
                (as<any>(runtime->services->get))["mockReturnValue"](nullptr);
                auto message = createMockMemory(validSpec);
                auto result = std::async([=]() { createPluginAction->handler(runtime, message, state); });
                expect(result)->toContain(std::string("Plugin creation service not available"));
            }
            );
            it(std::string("should handle invalid JSON"), [=]() mutable
            {
                auto message = createMockMemory(std::string("{ invalid json }"));
                auto result = std::async([=]() { createPluginAction->handler(runtime, message, state); });
                expect(result)->toContain(std::string("Failed to parse specification"));
            }
            );
            it(std::string("should handle service errors"), [=]() mutable
            {
                (as<any>(runtime->getSetting))["mockReturnValue"](std::string("test-api-key"));
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std::string("plugin_creation")));
                (as<any>(std::bind(&PluginCreationService::createPlugin, service, std::placeholders::_1, std::placeholders::_2)))["mockRejectedValue"](std::make_shared<Error>(std::string("Service error")));
                auto message = createMockMemory(validSpec);
                auto result = std::async([=]() { createPluginAction->handler(runtime, message, state); });
                expect(result)->toContain(std::string("Failed to create plugin: Service error"));
            }
            );
        }
        );
        describe(std::string("checkPluginCreationStatusAction"), [=]() mutable
        {
            it(std::string("should validate when jobs exist"), [=]() mutable
            {
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std::string("plugin_creation")));
                (as<any>(std::bind(&PluginCreationService::getAllJobs, service)))["mockReturnValue"](array<object>{ object{
                    object::pair{std::string("id"), std::string("job-123")}
                } });
                auto message = createMockMemory(std::string("check status"));
                auto result = std::async([=]() { checkPluginCreationStatusAction->validate(runtime, message, state); });
                expect(result)->toBe(true);
            }
            );
            it(std::string("should not validate when no jobs"), [=]() mutable
            {
                auto message = createMockMemory(std::string("check status"));
                auto result = std::async([=]() { checkPluginCreationStatusAction->validate(runtime, message, state); });
                expect(result)->toBe(false);
            }
            );
            it(std::string("should show detailed job status"), [=]() mutable
            {
                auto mockJob = object{
                    object::pair{std::string("id"), std::string("job-123")}, 
                    object::pair{std::string("specification"), object{
                        object::pair{std::string("name"), std::string("@test/plugin")}
                    }}, 
                    object::pair{std::string("status"), std::string("running")}, 
                    object::pair{std::string("currentPhase"), std::string("building")}, 
                    object::pair{std::string("progress"), 60}, 
                    object::pair{std::string("startedAt"), std::make_shared<Date>()}, 
                    object::pair{std::string("logs"), array<string>{ std::string("[2024-01-01T10:00:00Z] Starting job"), std::string("[2024-01-01T10:01:00Z] Building plugin") }}
                };
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std::string("plugin_creation")));
                (as<any>(std::bind(&PluginCreationService::getAllJobs, service)))["mockReturnValue"](array<object>{ mockJob });
                (as<any>(std::bind(&PluginCreationService::getJobStatus, service, std::placeholders::_1)))["mockReturnValue"](mockJob);
                auto message = createMockMemory(std::string("check status"));
                auto result = std::async([=]() { checkPluginCreationStatusAction->handler(runtime, message, state); });
                expect(result)->toContain(std::string("Plugin Creation Status"));
                expect(result)->toContain(std::string("Job ID: job-123"));
                expect(result)->toContain(std::string("Status: RUNNING"));
                expect(result)->toContain(std::string("Phase: building"));
                expect(result)->toContain(std::string("Progress: 60%"));
                expect(result)->toContain(std::string("Recent Activity:"));
            }
            );
            it(std::string("should handle specific job ID in message"), [=]() mutable
            {
                auto jobId = std::string("12345678-1234-1234-1234-123456789012");
                auto mockJob = object{
                    object::pair{std::string("id"), jobId}, 
                    object::pair{std::string("specification"), object{
                        object::pair{std::string("name"), std::string("@test/plugin")}
                    }}, 
                    object::pair{std::string("status"), std::string("completed")}, 
                    object::pair{std::string("currentPhase"), std::string("done")}, 
                    object::pair{std::string("progress"), 100}, 
                    object::pair{std::string("startedAt"), std::make_shared<Date>()}, 
                    object::pair{std::string("completedAt"), std::make_shared<Date>()}, 
                    object::pair{std::string("outputPath"), std::string("/path/to/plugin")}, 
                    object::pair{std::string("logs"), array<any>()}
                };
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std::string("plugin_creation")));
                (as<any>(std::bind(&PluginCreationService::getAllJobs, service)))["mockReturnValue"](array<object>{ mockJob });
                (as<any>(std::bind(&PluginCreationService::getJobStatus, service, std::placeholders::_1)))["mockReturnValue"](mockJob);
                auto message = createMockMemory(std::string("Check status for ") + jobId + string_empty);
                auto result = std::async([=]() { checkPluginCreationStatusAction->handler(runtime, message, state); });
                expect(std::bind(&PluginCreationService::getJobStatus, service, std::placeholders::_1))->toHaveBeenCalledWith(jobId);
                expect(result)->toContain(std::string("Plugin created successfully!"));
                expect(result)->toContain(std::string("Location: /path/to/plugin"));
            }
            );
            it(std::string("should show failed job details"), [=]() mutable
            {
                auto mockJob = object{
                    object::pair{std::string("id"), std::string("job-123")}, 
                    object::pair{std::string("specification"), object{
                        object::pair{std::string("name"), std::string("@test/plugin")}
                    }}, 
                    object::pair{std::string("status"), std::string("failed")}, 
                    object::pair{std::string("currentPhase"), std::string("testing")}, 
                    object::pair{std::string("progress"), 80}, 
                    object::pair{std::string("startedAt"), std::make_shared<Date>()}, 
                    object::pair{std::string("completedAt"), std::make_shared<Date>()}, 
                    object::pair{std::string("error"), std::string("Tests failed: 3 failing tests")}, 
                    object::pair{std::string("logs"), array<any>()}
                };
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std::string("plugin_creation")));
                (as<any>(std::bind(&PluginCreationService::getAllJobs, service)))["mockReturnValue"](array<object>{ mockJob });
                auto message = createMockMemory(std::string("status"));
                auto result = std::async([=]() { checkPluginCreationStatusAction->handler(runtime, message, state); });
                expect(result)->toContain(std::string("Plugin creation failed"));
                expect(result)->toContain(std::string("Tests failed: 3 failing tests"));
            }
            );
            it(std::string("should handle no jobs found"), [=]() mutable
            {
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std::string("plugin_creation")));
                (as<any>(std::bind(&PluginCreationService::getAllJobs, service)))["mockReturnValue"](array<any>());
                auto message = createMockMemory(std::string("check status"));
                auto result = std::async([=]() { checkPluginCreationStatusAction->handler(runtime, message, state); });
                expect(result)->toBe(std::string("No plugin creation jobs found."));
            }
            );
            it(std::string("should handle job not found by ID"), [=]() mutable
            {
                auto jobId = std::string("12345678-1234-1234-1234-123456789012");
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std::string("plugin_creation")));
                (as<any>(std::bind(&PluginCreationService::getAllJobs, service)))["mockReturnValue"](array<object>{ object{
                    object::pair{std::string("id"), std::string("other-job")}
                } });
                (as<any>(std::bind(&PluginCreationService::getJobStatus, service, std::placeholders::_1)))["mockReturnValue"](nullptr);
                auto message = createMockMemory(std::string("Check ") + jobId + string_empty);
                auto result = std::async([=]() { checkPluginCreationStatusAction->handler(runtime, message, state); });
                expect(result)->toContain(std::string("Job with ID ") + jobId + std::string(" not found"));
            }
            );
        }
        );
        describe(std::string("cancelPluginCreationAction"), [=]() mutable
        {
            it(std::string("should validate when active job exists"), [=]() mutable
            {
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std::string("plugin_creation")));
                (as<any>(std::bind(&PluginCreationService::getAllJobs, service)))["mockReturnValue"](array<object>{ object{
                    object::pair{std::string("id"), std::string("job-123")}, 
                    object::pair{std::string("status"), std::string("running")}
                } });
                auto message = createMockMemory(std::string("cancel"));
                auto result = std::async([=]() { cancelPluginCreationAction->validate(runtime, message, state); });
                expect(result)->toBe(true);
            }
            );
            it(std::string("should not validate when no active job"), [=]() mutable
            {
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std::string("plugin_creation")));
                (as<any>(std::bind(&PluginCreationService::getAllJobs, service)))["mockReturnValue"](array<object>{ object{
                    object::pair{std::string("id"), std::string("job-123")}, 
                    object::pair{std::string("status"), std::string("completed")}
                } });
                auto message = createMockMemory(std::string("cancel"));
                auto result = std::async([=]() { cancelPluginCreationAction->validate(runtime, message, state); });
                expect(result)->toBe(false);
            }
            );
            it(std::string("should cancel active job with details"), [=]() mutable
            {
                auto mockJob = object{
                    object::pair{std::string("id"), std::string("job-123")}, 
                    object::pair{std::string("status"), std::string("running")}, 
                    object::pair{std::string("specification"), object{
                        object::pair{std::string("name"), std::string("@test/plugin")}
                    }}
                };
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std::string("plugin_creation")));
                (as<any>(std::bind(&PluginCreationService::getAllJobs, service)))["mockReturnValue"](array<object>{ mockJob });
                auto message = createMockMemory(std::string("cancel"));
                auto result = std::async([=]() { cancelPluginCreationAction->handler(runtime, message, state); });
                expect(result)->toContain(std::string("Plugin creation job has been cancelled"));
                expect(result)->toContain(std::string("Job ID: job-123"));
                expect(result)->toContain(std::string("@test/plugin"));
                expect(std::bind(&PluginCreationService::cancelJob, service, std::placeholders::_1))->toHaveBeenCalledWith(std::string("job-123"));
            }
            );
            it(std::string("should handle no active job to cancel"), [=]() mutable
            {
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std::string("plugin_creation")));
                (as<any>(std::bind(&PluginCreationService::getAllJobs, service)))["mockReturnValue"](array<any>());
                auto message = createMockMemory(std::string("cancel"));
                auto result = std::async([=]() { cancelPluginCreationAction->handler(runtime, message, state); });
                expect(result)->toBe(std::string("No active plugin creation job to cancel."));
            }
            );
        }
        );
        describe(std::string("createPluginFromDescriptionAction"), [=]() mutable
        {
            it(std::string("should validate with long description"), [=]() mutable
            {
                auto message = createMockMemory(std::string("I need a plugin that manages user preferences with storage and retrieval"));
                auto result = std::async([=]() { createPluginFromDescriptionAction->validate(runtime, message, state); });
                expect(result)->toBe(true);
            }
            );
            it(std::string("should not validate with short description"), [=]() mutable
            {
                auto message = createMockMemory(std::string("plugin"));
                auto result = std::async([=]() { createPluginFromDescriptionAction->validate(runtime, message, state); });
                expect(result)->toBe(false);
            }
            );
            it(std::string("should not validate when active job exists"), [=]() mutable
            {
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std::string("plugin_creation")));
                (as<any>(std::bind(&PluginCreationService::getAllJobs, service)))["mockReturnValue"](array<object>{ object{
                    object::pair{std::string("status"), std::string("running")}
                } });
                auto message = createMockMemory(std::string("I need a plugin that manages todo lists"));
                auto result = std::async([=]() { createPluginFromDescriptionAction->validate(runtime, message, state); });
                expect(result)->toBe(false);
            }
            );
            it(std::string("should create plugin from todo description"), [=]() mutable
            {
                (as<any>(runtime->getSetting))["mockReturnValue"](std::string("test-api-key"));
                auto message = createMockMemory(std::string("I need a plugin that manages todo lists with add, remove, and list functionality"));
                auto result = std::async([=]() { createPluginFromDescriptionAction->handler(runtime, message, state); });
                expect(result)->toContain(std::string("I'm creating a plugin based on your description!"));
                expect(result)->toContain(std::string("Plugin: @elizaos/plugin-todo"));
                expect(result)->toContain(std::string("Job ID: job-123"));
                expect(result)->toContain(std::string("actions"));
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std::string("plugin_creation")));
                expect(std::bind(&PluginCreationService::createPlugin, service, std::placeholders::_1, std::placeholders::_2))->toHaveBeenCalledWith(expect->objectContaining(object{
                    object::pair{std::string("name"), std::string("@elizaos/plugin-todo")}, 
                    object::pair{std::string("actions"), expect->arrayContaining(array<any>{ expect->objectContaining(object{
                        object::pair{std::string("name"), expect->stringContaining(std::string("Todo"))}
                    }) })}
                }), std::string("test-api-key"));
            }
            );
            it(std::string("should create weather plugin from description"), [=]() mutable
            {
                (as<any>(runtime->getSetting))["mockReturnValue"](std::string("test-api-key"));
                auto message = createMockMemory(std::string("Create a weather information plugin that can fetch current weather and forecasts"));
                auto result = std::async([=]() { createPluginFromDescriptionAction->handler(runtime, message, state); });
                expect(result)->toContain(std::string("@elizaos/plugin-weather"));
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std::string("plugin_creation")));
                auto callArgs = const_(const_((as<any>(std::bind(&PluginCreationService::createPlugin, service, std::placeholders::_1, std::placeholders::_2)))["mock"]["calls"])[0])[0];
                expect(callArgs["name"])->toBe(std::string("@elizaos/plugin-weather"));
                expect(callArgs["actions"])->toBeDefined();
                expect(callArgs["actions"]["length"])->toBeGreaterThan(0);
            }
            );
            it(std::string("should create database plugin from description"), [=]() mutable
            {
                (as<any>(runtime->getSetting))["mockReturnValue"](std::string("test-api-key"));
                auto message = createMockMemory(std::string("Build a database plugin for SQL queries and data management"));
                auto result = std::async([=]() { createPluginFromDescriptionAction->handler(runtime, message, state); });
                expect(result)->toContain(std::string("@elizaos/plugin-database"));
            }
            );
            it(std::string("should detect multiple component types"), [=]() mutable
            {
                (as<any>(runtime->getSetting))["mockReturnValue"](std::string("test-api-key"));
                auto message = createMockMemory(std::string("I need a plugin that provides user data, has a background service to monitor changes, ") + std::string("and can evaluate user activity patterns"));
                auto result = std::async([=]() { createPluginFromDescriptionAction->handler(runtime, message, state); });
                expect(result)->toContain(std::string("providers"));
                expect(result)->toContain(std::string("services"));
                expect(result)->toContain(std::string("evaluators"));
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std::string("plugin_creation")));
                auto callArgs = const_(const_((as<any>(std::bind(&PluginCreationService::createPlugin, service, std::placeholders::_1, std::placeholders::_2)))["mock"]["calls"])[0])[0];
                expect(callArgs["providers"]["length"])->toBeGreaterThan(0);
                expect(callArgs["services"]["length"])->toBeGreaterThan(0);
                expect(callArgs["evaluators"]["length"])->toBeGreaterThan(0);
            }
            );
            it(std::string("should handle missing API key"), [=]() mutable
            {
                (as<any>(runtime->getSetting))["mockReturnValue"](nullptr);
                auto message = createMockMemory(std::string("I need a todo plugin"));
                auto result = std::async([=]() { createPluginFromDescriptionAction->handler(runtime, message, state); });
                expect(result)->toContain(std::string("ANTHROPIC_API_KEY is not configured"));
            }
            );
            it(std::string("should create custom plugin for unrecognized type"), [=]() mutable
            {
                (as<any>(runtime->getSetting))["mockReturnValue"](std::string("test-api-key"));
                auto message = createMockMemory(std::string("I need a blockchain integration plugin for smart contracts"));
                auto result = std::async([=]() { createPluginFromDescriptionAction->handler(runtime, message, state); });
                expect(result)->toContain(std::string("@elizaos/plugin-blockchain"));
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std::string("plugin_creation")));
                auto callArgs = const_(const_((as<any>(std::bind(&PluginCreationService::createPlugin, service, std::placeholders::_1, std::placeholders::_2)))["mock"]["calls"])[0])[0];
                expect(callArgs["name"])->toContain(std::string("blockchain"));
                expect(callArgs["actions"]["length"])->toBeGreaterThan(0);
            }
            );
            it(std::string("should ensure at least one component exists"), [=]() mutable
            {
                (as<any>(runtime->getSetting))["mockReturnValue"](std::string("test-api-key"));
                auto message = createMockMemory(std::string("I need a simple utility plugin"));
                auto result = std::async([=]() { createPluginFromDescriptionAction->handler(runtime, message, state); });
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std::string("plugin_creation")));
                auto callArgs = const_(const_((as<any>(std::bind(&PluginCreationService::createPlugin, service, std::placeholders::_1, std::placeholders::_2)))["mock"]["calls"])[0])[0];
                auto hasComponents = OR((OR((OR((callArgs["actions"]["length"] > 0), (callArgs["providers"]["length"] > 0))), (callArgs["services"]["length"] > 0))), (callArgs["evaluators"]["length"] > 0));
                expect(hasComponents)->toBe(true);
            }
            );
        }
        );
    }
    );
}

MAIN
