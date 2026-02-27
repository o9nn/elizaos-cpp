#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-autocoder/src/__tests__/autocoder-agent.test.h"

void Main(void)
{
    describe(std:("Autocoder Agent Integration"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> runtime;
        beforeAll([=]() mutable
        {
            auto dotenv = require(std:("dotenv"));
            auto path = require(std:("path"));
            auto envPath = path["join"](process->cwd(), std:(".."), std:(".."), std:(".env"));
            dotenv["config"](object{
                object::pair{std:("path"), envPath}
            });
            runtime = std::make_shared<AgentRuntime>(object{
                object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("character"), object{
                    object::pair{std:("plugins"), array<string>{ std:("@elizaos/plugin-sql"), std:("@elizaos/plugin-forms"), std:("@elizaos/plugin-openai"), std:("@elizaos/plugin-autocoder") }}, 
                    object::pair{std:("name"), std:("Autocoder Agent")}, 
                    object::pair{std:("bio"), array<string>{ std:("An AI assistant specialized in generating code autonomously") }}, 
                    object::pair{std:("system"), std:("You are an expert code generation assistant with access to the Autocoder plugin. Help users create plugins, agents, and other code projects.")}, 
                    object::pair{std:("settings"), object{
                        object::pair{std:("OPENAI_API_KEY"), process->env->OPENAI_API_KEY}, 
                        object::pair{std:("ANTHROPIC_API_KEY"), process->env->ANTHROPIC_API_KEY}
                    }}
                }}
            });
            std::async([=]() { runtime->initialize(); });
        }
        );
        it(std:("should have autocoder plugin loaded"), [=]() mutable
        {
            auto services = runtime->services;
            console->log(std:("Available services:"), Array->from(services->values())->map([=](auto s) mutable
            {
                return s->constructor->name;
            }
            ));
            auto codeGenService = runtime->getService(std:("code-generation"));
            expect(codeGenService)->toBeDefined();
            auto planningService = runtime->getService(std:("project-planning"));
            expect(planningService)->toBeDefined();
            auto formsService = runtime->getService(std:("forms"));
            expect(formsService)->toBeDefined();
        }
        );
        it(std:("should have autocoder actions available"), [=]() mutable
        {
            auto actions = runtime->actions;
            console->log(std:("Available actions:"), actions->map([=](auto a) mutable
            {
                return a["name"];
            }
            ));
            auto generateCodeAction = actions->find([=](auto a) mutable
            {
                return a["name"] == std:("GENERATE_CODE");
            }
            );
            expect(generateCodeAction)->toBeDefined();
            auto createProjectAction = actions->find([=](auto a) mutable
            {
                return a["name"] == std:("CREATE_PROJECT");
            }
            );
            expect(createProjectAction)->toBeDefined();
            auto setupEnvironmentAction = actions->find([=](auto a) mutable
            {
                return a["name"] == std:("SETUP_ENVIRONMENT");
            }
            );
            expect(setupEnvironmentAction)->toBeDefined();
        }
        );
        it(std:("should have autocoder providers available"), [=]() mutable
        {
            auto providers = runtime->providers;
            console->log(std:("Available providers:"), providers->map([=](auto p) mutable
            {
                return p["name"];
            }
            ));
            auto projectsProvider = providers->find([=](auto p) mutable
            {
                return p["name"] == std:("projects");
            }
            );
            expect(projectsProvider)->toBeDefined();
            auto currentProjectProvider = providers->find([=](auto p) mutable
            {
                return p["name"] == std:("current-project");
            }
            );
            expect(currentProjectProvider)->toBeDefined();
        }
        );
        it(std:("should validate GENERATE_CODE action for valid requests"), [=]() mutable
        {
            auto generateCodeAction = runtime->actions->find([=](auto a) mutable
            {
                return a["name"] == std:("GENERATE_CODE");
            }
            );
            expect(generateCodeAction)->toBeDefined();
            auto validMessage = object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("test-room"))}, 
                object::pair{std:("userId"), as<std::shared_ptr<UUID>>(std:("test-user"))}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Create a new plugin that sends email notifications")}
                }}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            auto isValid = std::async([=]() { generateCodeAction->validate(runtime, validMessage); });
            expect(isValid)->toBe(true);
        }
        );
        it(std:("should not validate GENERATE_CODE for non-code requests"), [=]() mutable
        {
            auto generateCodeAction = runtime->actions->find([=](auto a) mutable
            {
                return a["name"] == std:("GENERATE_CODE");
            }
            );
            expect(generateCodeAction)->toBeDefined();
            auto invalidMessage = object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("test-room"))}, 
                object::pair{std:("userId"), as<std::shared_ptr<UUID>>(std:("test-user"))}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("What is the weather today?")}
                }}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            auto isValid = std::async([=]() { generateCodeAction->validate(runtime, invalidMessage); });
            expect(isValid)->toBe(false);
        }
        );
        it(std:("should provide project context through providers"), [=]() mutable
        {
            auto projectsProvider = runtime->providers->find([=](auto p) mutable
            {
                return p["name"] == std:("projects");
            }
            );
            expect(projectsProvider)->toBeDefined();
            auto context = std::async([=]() { projectsProvider->get(runtime, as<any>(object{}), as<any>(object{})); });
            console->log(std:("Projects context:"), context);
            expect(context)->toHaveProperty(std:("text"));
            expect(context->text)->toContain(std:("[PROJECTS]"));
        }
        );
        it(std:("should handle CREATE_PROJECT action"), [=]() mutable
        {
            auto createProjectAction = runtime->actions->find([=](auto a) mutable
            {
                return a["name"] == std:("CREATE_PROJECT");
            }
            );
            expect(createProjectAction)->toBeDefined();
            auto createMessage = object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("test-room"))}, 
                object::pair{std:("userId"), as<std::shared_ptr<UUID>>(std:("test-user"))}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("I want to create a new plugin for weather data")}
                }}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            auto isValid = std::async([=]() { createProjectAction->validate(runtime, createMessage); });
            expect(isValid)->toBe(true);
        }
        );
        it(std:("should have proper service initialization"), [=]() mutable
        {
            auto codeGenService = runtime->getService(std:("code-generation"));
            expect(codeGenService)->toBeDefined();
            expect(type_of((as<any>(codeGenService))["generateCode"]))->toBe(std:("function"));
            expect(type_of((as<any>(codeGenService))["start"]))->toBe(std:("function"));
            expect(type_of((as<any>(codeGenService))["stop"]))->toBe(std:("function"));
        }
        );
        it(std:("should support form-based code generation"), [=]() mutable
        {
            auto formsService = runtime->getService(std:("forms"));
            expect(formsService)->toBeDefined();
            auto createProjectAction = runtime->actions->find([=](auto a) mutable
            {
                return a["name"] == std:("CREATE_PROJECT");
            }
            );
            auto message = object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("test-room"))}, 
                object::pair{std:("userId"), as<std::shared_ptr<UUID>>(std:("test-user"))}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Create a new plugin project")}
                }}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            auto isValid = std::async([=]() { createProjectAction->validate(runtime, message); });
            expect(isValid)->toBe(true);
        }
        );
    }
    );
}

MAIN
