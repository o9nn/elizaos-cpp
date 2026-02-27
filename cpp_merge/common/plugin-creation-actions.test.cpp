#include "plugin-creation-actions.test.h"

std::function<any(string)> createMockMemory = [=](auto text) mutable
{
    return as<std::shared_ptr<Memory>>(as<any>((object{
        object::pair{std:("id"), as<any>(crypto->randomUUID())}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("text")}
        }}, 
        object::pair{std:("userId"), as<any>(crypto->randomUUID())}, 
        object::pair{std:("roomId"), as<any>(crypto->randomUUID())}, 
        object::pair{std:("entityId"), std:("entity-id")}, 
        object::pair{std:("createdAt"), Date->now()}
    })));
};
std::function<any()> createMockRuntime = [=]() mutable
{
    auto service = as<std::shared_ptr<PluginCreationService>>(as<any>(object{
        object::pair{std:("getAllJobs"), vi->fn()->mockReturnValue(array<any>())}, 
        object::pair{std:("createPlugin"), vi->fn()->mockReturnValue(std:("job-123"))}, 
        object::pair{std:("getJobStatus"), vi->fn()}, 
        object::pair{std:("cancelJob"), vi->fn()}
    }));
    return as<any>(object{
        object::pair{std:("services"), object{
            object::pair{std:("get"), vi->fn()->mockReturnValue(service)}
        }}, 
        object::pair{std:("getSetting"), vi->fn()}
    });
};

void Main(void)
{
    describe(std:("Plugin Creation Actions"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> runtime;
        shared<std::shared_ptr<State>> state;
        beforeEach([=]() mutable
        {
            runtime = createMockRuntime();
            state = object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
            vi->clearAllMocks();
        }
        );
        describe(std:("createPluginAction"), [=]() mutable
        {
            shared validSpec = JSON->stringify(object{
                object::pair{std:("name"), std:("@test/plugin")}, 
                object::pair{std:("description"), std:("Test plugin for testing")}, 
                object::pair{std:("version"), std:("1.0.0")}, 
                object::pair{std:("actions"), array<object>{ object{
                    object::pair{std:("name"), std:("testAction")}, 
                    object::pair{std:("description"), std:("Test")}
                } }}
            });
            it(std:("should validate when no active jobs and valid JSON"), [=]() mutable
            {
                auto message = createMockMemory(validSpec);
                auto result = std::async([=]() { createPluginAction->validate(runtime, message, state); });
                expect(result)->toBe(true);
            }
            );
            it(std:("should not validate when active job exists"), [=]() mutable
            {
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std:("plugin_creation")));
                (as<any>(std::bind(&PluginCreationService::getAllJobs, service)))["mockReturnValue"](array<object>{ object{
                    object::pair{std:("status"), std:("running")}
                } });
                auto message = createMockMemory(validSpec);
                auto result = std::async([=]() { createPluginAction->validate(runtime, message, state); });
                expect(result)->toBe(false);
            }
            );
            it(std:("should not validate with invalid JSON"), [=]() mutable
            {
                auto message = createMockMemory(std:("not json"));
                auto result = std::async([=]() { createPluginAction->validate(runtime, message, state); });
                expect(result)->toBe(false);
            }
            );
            it(std:("should not validate when service unavailable"), [=]() mutable
            {
                (as<any>(runtime->services->get))["mockReturnValue"](nullptr);
                auto message = createMockMemory(validSpec);
                auto result = std::async([=]() { createPluginAction->validate(runtime, message, state); });
                expect(result)->toBe(false);
            }
            );
            it(std:("should handle plugin creation with valid spec"), [=]() mutable
            {
                (as<any>(runtime->getSetting))["mockReturnValue"](std:("test-api-key"));
                auto message = createMockMemory(validSpec);
                auto result = std::async([=]() { createPluginAction->handler(runtime, message, state); });
                expect(result)->toContain(std:("Plugin creation job started successfully!"));
                expect(result)->toContain(std:("Job ID: job-123"));
                expect(result)->toContain(std:("@test/plugin"));
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std:("plugin_creation")));
                expect(std::bind(&PluginCreationService::createPlugin, service, std::placeholders::_1, std::placeholders::_2))->toHaveBeenCalledWith(expect->objectContaining(object{
                    object::pair{std:("name"), std:("@test/plugin")}, 
                    object::pair{std:("description"), std:("Test plugin for testing")}
                }), std:("test-api-key"));
            }
            );
            it(std:("should validate plugin specification"), [=]() mutable
            {
                auto invalidSpecs = array<object>{ object{
                    object::pair{std:("name"), std:("invalid name")}, 
                    object::pair{std:("description"), std:("test")}
                }, object{
                    object::pair{std:("name"), std:("@test/plugin")}, 
                    object::pair{std:("description"), std:("short")}
                }, object{
                    object::pair{std:("name"), std:("@test/plugin")}, 
                    object::pair{std:("description"), std:("Valid description")}, 
                    object::pair{std:("version"), std:("invalid")}
                }, object{
                    object::pair{std:("name"), std:("../../../etc/passwd")}, 
                    object::pair{std:("description"), std:("Path traversal attempt")}
                } };
                for (auto& spec : invalidSpecs)
                {
                    auto message = createMockMemory(JSON->stringify(spec));
                    auto result = std::async([=]() { createPluginAction->handler(runtime, message, state); });
                    expect(result)->toContain(std:("Invalid plugin specification"));
                }
            }
            );
            it(std:("should handle missing API key"), [=]() mutable
            {
                (as<any>(runtime->getSetting))["mockReturnValue"](nullptr);
                auto message = createMockMemory(validSpec);
                auto result = std::async([=]() { createPluginAction->handler(runtime, message, state); });
                expect(result)->toContain(std:("ANTHROPIC_API_KEY is not configured"));
            }
            );
            it(std:("should handle service unavailable"), [=]() mutable
            {
                (as<any>(runtime->services->get))["mockReturnValue"](nullptr);
                auto message = createMockMemory(validSpec);
                auto result = std::async([=]() { createPluginAction->handler(runtime, message, state); });
                expect(result)->toContain(std:("Plugin creation service not available"));
            }
            );
            it(std:("should handle invalid JSON"), [=]() mutable
            {
                auto message = createMockMemory(std:("{ invalid json }"));
                auto result = std::async([=]() { createPluginAction->handler(runtime, message, state); });
                expect(result)->toContain(std:("Failed to parse specification"));
            }
            );
            it(std:("should handle service errors"), [=]() mutable
            {
                (as<any>(runtime->getSetting))["mockReturnValue"](std:("test-api-key"));
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std:("plugin_creation")));
                (as<any>(std::bind(&PluginCreationService::createPlugin, service, std::placeholders::_1, std::placeholders::_2)))["mockRejectedValue"](std::make_shared<Error>(std:("Service error")));
                auto message = createMockMemory(validSpec);
                auto result = std::async([=]() { createPluginAction->handler(runtime, message, state); });
                expect(result)->toContain(std:("Failed to create plugin: Service error"));
            }
            );
        }
        );
        describe(std:("checkPluginCreationStatusAction"), [=]() mutable
        {
            it(std:("should validate when jobs exist"), [=]() mutable
            {
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std:("plugin_creation")));
                (as<any>(std::bind(&PluginCreationService::getAllJobs, service)))["mockReturnValue"](array<object>{ object{
                    object::pair{std:("id"), std:("job-123")}
                } });
                auto message = createMockMemory(std:("check status"));
                auto result = std::async([=]() { checkPluginCreationStatusAction->validate(runtime, message, state); });
                expect(result)->toBe(true);
            }
            );
            it(std:("should not validate when no jobs"), [=]() mutable
            {
                auto message = createMockMemory(std:("check status"));
                auto result = std::async([=]() { checkPluginCreationStatusAction->validate(runtime, message, state); });
                expect(result)->toBe(false);
            }
            );
            it(std:("should show detailed job status"), [=]() mutable
            {
                auto mockJob = object{
                    object::pair{std:("id"), std:("job-123")}, 
                    object::pair{std:("specification"), object{
                        object::pair{std:("name"), std:("@test/plugin")}
                    }}, 
                    object::pair{std:("status"), std:("running")}, 
                    object::pair{std:("currentPhase"), std:("building")}, 
                    object::pair{std:("progress"), 60}, 
                    object::pair{std:("startedAt"), std::make_shared<Date>()}, 
                    object::pair{std:("logs"), array<string>{ std:("[2024-01-01T10:00:00Z] Starting job"), std:("[2024-01-01T10:01:00Z] Building plugin") }}
                };
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std:("plugin_creation")));
                (as<any>(std::bind(&PluginCreationService::getAllJobs, service)))["mockReturnValue"](array<object>{ mockJob });
                (as<any>(std::bind(&PluginCreationService::getJobStatus, service, std::placeholders::_1)))["mockReturnValue"](mockJob);
                auto message = createMockMemory(std:("check status"));
                auto result = std::async([=]() { checkPluginCreationStatusAction->handler(runtime, message, state); });
                expect(result)->toContain(std:("Plugin Creation Status"));
                expect(result)->toContain(std:("Job ID: job-123"));
                expect(result)->toContain(std:("Status: RUNNING"));
                expect(result)->toContain(std:("Phase: building"));
                expect(result)->toContain(std:("Progress: 60%"));
                expect(result)->toContain(std:("Recent Activity:"));
            }
            );
            it(std:("should handle specific job ID in message"), [=]() mutable
            {
                auto jobId = std:("12345678-1234-1234-1234-123456789012");
                auto mockJob = object{
                    object::pair{std:("id"), jobId}, 
                    object::pair{std:("specification"), object{
                        object::pair{std:("name"), std:("@test/plugin")}
                    }}, 
                    object::pair{std:("status"), std:("completed")}, 
                    object::pair{std:("currentPhase"), std:("done")}, 
                    object::pair{std:("progress"), 100}, 
                    object::pair{std:("startedAt"), std::make_shared<Date>()}, 
                    object::pair{std:("completedAt"), std::make_shared<Date>()}, 
                    object::pair{std:("outputPath"), std:("/path/to/plugin")}, 
                    object::pair{std:("logs"), array<any>()}
                };
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std:("plugin_creation")));
                (as<any>(std::bind(&PluginCreationService::getAllJobs, service)))["mockReturnValue"](array<object>{ mockJob });
                (as<any>(std::bind(&PluginCreationService::getJobStatus, service, std::placeholders::_1)))["mockReturnValue"](mockJob);
                auto message = createMockMemory(std:("Check status for ") + jobId + string_empty);
                auto result = std::async([=]() { checkPluginCreationStatusAction->handler(runtime, message, state); });
                expect(std::bind(&PluginCreationService::getJobStatus, service, std::placeholders::_1))->toHaveBeenCalledWith(jobId);
                expect(result)->toContain(std:("Plugin created successfully!"));
                expect(result)->toContain(std:("Location: /path/to/plugin"));
            }
            );
            it(std:("should show failed job details"), [=]() mutable
            {
                auto mockJob = object{
                    object::pair{std:("id"), std:("job-123")}, 
                    object::pair{std:("specification"), object{
                        object::pair{std:("name"), std:("@test/plugin")}
                    }}, 
                    object::pair{std:("status"), std:("failed")}, 
                    object::pair{std:("currentPhase"), std:("testing")}, 
                    object::pair{std:("progress"), 80}, 
                    object::pair{std:("startedAt"), std::make_shared<Date>()}, 
                    object::pair{std:("completedAt"), std::make_shared<Date>()}, 
                    object::pair{std:("error"), std:("Tests failed: 3 failing tests")}, 
                    object::pair{std:("logs"), array<any>()}
                };
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std:("plugin_creation")));
                (as<any>(std::bind(&PluginCreationService::getAllJobs, service)))["mockReturnValue"](array<object>{ mockJob });
                auto message = createMockMemory(std:("status"));
                auto result = std::async([=]() { checkPluginCreationStatusAction->handler(runtime, message, state); });
                expect(result)->toContain(std:("Plugin creation failed"));
                expect(result)->toContain(std:("Tests failed: 3 failing tests"));
            }
            );
            it(std:("should handle no jobs found"), [=]() mutable
            {
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std:("plugin_creation")));
                (as<any>(std::bind(&PluginCreationService::getAllJobs, service)))["mockReturnValue"](array<any>());
                auto message = createMockMemory(std:("check status"));
                auto result = std::async([=]() { checkPluginCreationStatusAction->handler(runtime, message, state); });
                expect(result)->toBe(std:("No plugin creation jobs found."));
            }
            );
            it(std:("should handle job not found by ID"), [=]() mutable
            {
                auto jobId = std:("12345678-1234-1234-1234-123456789012");
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std:("plugin_creation")));
                (as<any>(std::bind(&PluginCreationService::getAllJobs, service)))["mockReturnValue"](array<object>{ object{
                    object::pair{std:("id"), std:("other-job")}
                } });
                (as<any>(std::bind(&PluginCreationService::getJobStatus, service, std::placeholders::_1)))["mockReturnValue"](nullptr);
                auto message = createMockMemory(std:("Check ") + jobId + string_empty);
                auto result = std::async([=]() { checkPluginCreationStatusAction->handler(runtime, message, state); });
                expect(result)->toContain(std:("Job with ID ") + jobId + std:(" not found"));
            }
            );
        }
        );
        describe(std:("cancelPluginCreationAction"), [=]() mutable
        {
            it(std:("should validate when active job exists"), [=]() mutable
            {
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std:("plugin_creation")));
                (as<any>(std::bind(&PluginCreationService::getAllJobs, service)))["mockReturnValue"](array<object>{ object{
                    object::pair{std:("id"), std:("job-123")}, 
                    object::pair{std:("status"), std:("running")}
                } });
                auto message = createMockMemory(std:("cancel"));
                auto result = std::async([=]() { cancelPluginCreationAction->validate(runtime, message, state); });
                expect(result)->toBe(true);
            }
            );
            it(std:("should not validate when no active job"), [=]() mutable
            {
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std:("plugin_creation")));
                (as<any>(std::bind(&PluginCreationService::getAllJobs, service)))["mockReturnValue"](array<object>{ object{
                    object::pair{std:("id"), std:("job-123")}, 
                    object::pair{std:("status"), std:("completed")}
                } });
                auto message = createMockMemory(std:("cancel"));
                auto result = std::async([=]() { cancelPluginCreationAction->validate(runtime, message, state); });
                expect(result)->toBe(false);
            }
            );
            it(std:("should cancel active job with details"), [=]() mutable
            {
                auto mockJob = object{
                    object::pair{std:("id"), std:("job-123")}, 
                    object::pair{std:("status"), std:("running")}, 
                    object::pair{std:("specification"), object{
                        object::pair{std:("name"), std:("@test/plugin")}
                    }}
                };
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std:("plugin_creation")));
                (as<any>(std::bind(&PluginCreationService::getAllJobs, service)))["mockReturnValue"](array<object>{ mockJob });
                auto message = createMockMemory(std:("cancel"));
                auto result = std::async([=]() { cancelPluginCreationAction->handler(runtime, message, state); });
                expect(result)->toContain(std:("Plugin creation job has been cancelled"));
                expect(result)->toContain(std:("Job ID: job-123"));
                expect(result)->toContain(std:("@test/plugin"));
                expect(std::bind(&PluginCreationService::cancelJob, service, std::placeholders::_1))->toHaveBeenCalledWith(std:("job-123"));
            }
            );
            it(std:("should handle no active job to cancel"), [=]() mutable
            {
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std:("plugin_creation")));
                (as<any>(std::bind(&PluginCreationService::getAllJobs, service)))["mockReturnValue"](array<any>());
                auto message = createMockMemory(std:("cancel"));
                auto result = std::async([=]() { cancelPluginCreationAction->handler(runtime, message, state); });
                expect(result)->toBe(std:("No active plugin creation job to cancel."));
            }
            );
        }
        );
        describe(std:("createPluginFromDescriptionAction"), [=]() mutable
        {
            it(std:("should validate with long description"), [=]() mutable
            {
                auto message = createMockMemory(std:("I need a plugin that manages user preferences with storage and retrieval"));
                auto result = std::async([=]() { createPluginFromDescriptionAction->validate(runtime, message, state); });
                expect(result)->toBe(true);
            }
            );
            it(std:("should not validate with short description"), [=]() mutable
            {
                auto message = createMockMemory(std:("plugin"));
                auto result = std::async([=]() { createPluginFromDescriptionAction->validate(runtime, message, state); });
                expect(result)->toBe(false);
            }
            );
            it(std:("should not validate when active job exists"), [=]() mutable
            {
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std:("plugin_creation")));
                (as<any>(std::bind(&PluginCreationService::getAllJobs, service)))["mockReturnValue"](array<object>{ object{
                    object::pair{std:("status"), std:("running")}
                } });
                auto message = createMockMemory(std:("I need a plugin that manages todo lists"));
                auto result = std::async([=]() { createPluginFromDescriptionAction->validate(runtime, message, state); });
                expect(result)->toBe(false);
            }
            );
            it(std:("should create plugin from todo description"), [=]() mutable
            {
                (as<any>(runtime->getSetting))["mockReturnValue"](std:("test-api-key"));
                auto message = createMockMemory(std:("I need a plugin that manages todo lists with add, remove, and list functionality"));
                auto result = std::async([=]() { createPluginFromDescriptionAction->handler(runtime, message, state); });
                expect(result)->toContain(std:("I'm creating a plugin based on your description!"));
                expect(result)->toContain(std:("Plugin: @elizaos/plugin-todo"));
                expect(result)->toContain(std:("Job ID: job-123"));
                expect(result)->toContain(std:("actions"));
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std:("plugin_creation")));
                expect(std::bind(&PluginCreationService::createPlugin, service, std::placeholders::_1, std::placeholders::_2))->toHaveBeenCalledWith(expect->objectContaining(object{
                    object::pair{std:("name"), std:("@elizaos/plugin-todo")}, 
                    object::pair{std:("actions"), expect->arrayContaining(array<any>{ expect->objectContaining(object{
                        object::pair{std:("name"), expect->stringContaining(std:("Todo"))}
                    }) })}
                }), std:("test-api-key"));
            }
            );
            it(std:("should create weather plugin from description"), [=]() mutable
            {
                (as<any>(runtime->getSetting))["mockReturnValue"](std:("test-api-key"));
                auto message = createMockMemory(std:("Create a weather information plugin that can fetch current weather and forecasts"));
                auto result = std::async([=]() { createPluginFromDescriptionAction->handler(runtime, message, state); });
                expect(result)->toContain(std:("@elizaos/plugin-weather"));
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std:("plugin_creation")));
                auto callArgs = const_(const_((as<any>(std::bind(&PluginCreationService::createPlugin, service, std::placeholders::_1, std::placeholders::_2)))["mock"]["calls"])[0])[0];
                expect(callArgs["name"])->toBe(std:("@elizaos/plugin-weather"));
                expect(callArgs["actions"])->toBeDefined();
                expect(callArgs["actions"]["length"])->toBeGreaterThan(0);
            }
            );
            it(std:("should create database plugin from description"), [=]() mutable
            {
                (as<any>(runtime->getSetting))["mockReturnValue"](std:("test-api-key"));
                auto message = createMockMemory(std:("Build a database plugin for SQL queries and data management"));
                auto result = std::async([=]() { createPluginFromDescriptionAction->handler(runtime, message, state); });
                expect(result)->toContain(std:("@elizaos/plugin-database"));
            }
            );
            it(std:("should detect multiple component types"), [=]() mutable
            {
                (as<any>(runtime->getSetting))["mockReturnValue"](std:("test-api-key"));
                auto message = createMockMemory(std:("I need a plugin that provides user data, has a background service to monitor changes, ") + std:("and can evaluate user activity patterns"));
                auto result = std::async([=]() { createPluginFromDescriptionAction->handler(runtime, message, state); });
                expect(result)->toContain(std:("providers"));
                expect(result)->toContain(std:("services"));
                expect(result)->toContain(std:("evaluators"));
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std:("plugin_creation")));
                auto callArgs = const_(const_((as<any>(std::bind(&PluginCreationService::createPlugin, service, std::placeholders::_1, std::placeholders::_2)))["mock"]["calls"])[0])[0];
                expect(callArgs["providers"]["length"])->toBeGreaterThan(0);
                expect(callArgs["services"]["length"])->toBeGreaterThan(0);
                expect(callArgs["evaluators"]["length"])->toBeGreaterThan(0);
            }
            );
            it(std:("should handle missing API key"), [=]() mutable
            {
                (as<any>(runtime->getSetting))["mockReturnValue"](nullptr);
                auto message = createMockMemory(std:("I need a todo plugin"));
                auto result = std::async([=]() { createPluginFromDescriptionAction->handler(runtime, message, state); });
                expect(result)->toContain(std:("ANTHROPIC_API_KEY is not configured"));
            }
            );
            it(std:("should create custom plugin for unrecognized type"), [=]() mutable
            {
                (as<any>(runtime->getSetting))["mockReturnValue"](std:("test-api-key"));
                auto message = createMockMemory(std:("I need a blockchain integration plugin for smart contracts"));
                auto result = std::async([=]() { createPluginFromDescriptionAction->handler(runtime, message, state); });
                expect(result)->toContain(std:("@elizaos/plugin-blockchain"));
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std:("plugin_creation")));
                auto callArgs = const_(const_((as<any>(std::bind(&PluginCreationService::createPlugin, service, std::placeholders::_1, std::placeholders::_2)))["mock"]["calls"])[0])[0];
                expect(callArgs["name"])->toContain(std:("blockchain"));
                expect(callArgs["actions"]["length"])->toBeGreaterThan(0);
            }
            );
            it(std:("should ensure at least one component exists"), [=]() mutable
            {
                (as<any>(runtime->getSetting))["mockReturnValue"](std:("test-api-key"));
                auto message = createMockMemory(std:("I need a simple utility plugin"));
                auto result = std::async([=]() { createPluginFromDescriptionAction->handler(runtime, message, state); });
                auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std:("plugin_creation")));
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
