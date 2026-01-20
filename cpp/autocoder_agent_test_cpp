#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-autocoder/src/__tests__/autocoder-agent.test.h"

void Main(void)
{
    describe(std::string("Autocoder Agent Integration"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> runtime;
        beforeAll([=]() mutable
        {
            auto dotenv = require(std::string("dotenv"));
            auto path = require(std::string("path"));
            auto envPath = path["join"](process->cwd(), std::string(".."), std::string(".."), std::string(".env"));
            dotenv["config"](object{
                object::pair{std::string("path"), envPath}
            });
            runtime = std::make_shared<AgentRuntime>(object{
                object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("character"), object{
                    object::pair{std::string("plugins"), array<string>{ std::string("@elizaos/plugin-sql"), std::string("@elizaos/plugin-forms"), std::string("@elizaos/plugin-openai"), std::string("@elizaos/plugin-autocoder") }}, 
                    object::pair{std::string("name"), std::string("Autocoder Agent")}, 
                    object::pair{std::string("bio"), array<string>{ std::string("An AI assistant specialized in generating code autonomously") }}, 
                    object::pair{std::string("system"), std::string("You are an expert code generation assistant with access to the Autocoder plugin. Help users create plugins, agents, and other code projects.")}, 
                    object::pair{std::string("settings"), object{
                        object::pair{std::string("OPENAI_API_KEY"), process->env->OPENAI_API_KEY}, 
                        object::pair{std::string("ANTHROPIC_API_KEY"), process->env->ANTHROPIC_API_KEY}
                    }}
                }}
            });
            std::async([=]() { runtime->initialize(); });
        }
        );
        it(std::string("should have autocoder plugin loaded"), [=]() mutable
        {
            auto services = runtime->services;
            console->log(std::string("Available services:"), Array->from(services->values())->map([=](auto s) mutable
            {
                return s->constructor->name;
            }
            ));
            auto codeGenService = runtime->getService(std::string("code-generation"));
            expect(codeGenService)->toBeDefined();
            auto planningService = runtime->getService(std::string("project-planning"));
            expect(planningService)->toBeDefined();
            auto formsService = runtime->getService(std::string("forms"));
            expect(formsService)->toBeDefined();
        }
        );
        it(std::string("should have autocoder actions available"), [=]() mutable
        {
            auto actions = runtime->actions;
            console->log(std::string("Available actions:"), actions->map([=](auto a) mutable
            {
                return a["name"];
            }
            ));
            auto generateCodeAction = actions->find([=](auto a) mutable
            {
                return a["name"] == std::string("GENERATE_CODE");
            }
            );
            expect(generateCodeAction)->toBeDefined();
            auto createProjectAction = actions->find([=](auto a) mutable
            {
                return a["name"] == std::string("CREATE_PROJECT");
            }
            );
            expect(createProjectAction)->toBeDefined();
            auto setupEnvironmentAction = actions->find([=](auto a) mutable
            {
                return a["name"] == std::string("SETUP_ENVIRONMENT");
            }
            );
            expect(setupEnvironmentAction)->toBeDefined();
        }
        );
        it(std::string("should have autocoder providers available"), [=]() mutable
        {
            auto providers = runtime->providers;
            console->log(std::string("Available providers:"), providers->map([=](auto p) mutable
            {
                return p["name"];
            }
            ));
            auto projectsProvider = providers->find([=](auto p) mutable
            {
                return p["name"] == std::string("projects");
            }
            );
            expect(projectsProvider)->toBeDefined();
            auto currentProjectProvider = providers->find([=](auto p) mutable
            {
                return p["name"] == std::string("current-project");
            }
            );
            expect(currentProjectProvider)->toBeDefined();
        }
        );
        it(std::string("should validate GENERATE_CODE action for valid requests"), [=]() mutable
        {
            auto generateCodeAction = runtime->actions->find([=](auto a) mutable
            {
                return a["name"] == std::string("GENERATE_CODE");
            }
            );
            expect(generateCodeAction)->toBeDefined();
            auto validMessage = object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("test-room"))}, 
                object::pair{std::string("userId"), as<std::shared_ptr<UUID>>(std::string("test-user"))}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Create a new plugin that sends email notifications")}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            auto isValid = std::async([=]() { generateCodeAction->validate(runtime, validMessage); });
            expect(isValid)->toBe(true);
        }
        );
        it(std::string("should not validate GENERATE_CODE for non-code requests"), [=]() mutable
        {
            auto generateCodeAction = runtime->actions->find([=](auto a) mutable
            {
                return a["name"] == std::string("GENERATE_CODE");
            }
            );
            expect(generateCodeAction)->toBeDefined();
            auto invalidMessage = object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("test-room"))}, 
                object::pair{std::string("userId"), as<std::shared_ptr<UUID>>(std::string("test-user"))}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("What is the weather today?")}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            auto isValid = std::async([=]() { generateCodeAction->validate(runtime, invalidMessage); });
            expect(isValid)->toBe(false);
        }
        );
        it(std::string("should provide project context through providers"), [=]() mutable
        {
            auto projectsProvider = runtime->providers->find([=](auto p) mutable
            {
                return p["name"] == std::string("projects");
            }
            );
            expect(projectsProvider)->toBeDefined();
            auto context = std::async([=]() { projectsProvider->get(runtime, as<any>(object{}), as<any>(object{})); });
            console->log(std::string("Projects context:"), context);
            expect(context)->toHaveProperty(std::string("text"));
            expect(context->text)->toContain(std::string("[PROJECTS]"));
        }
        );
        it(std::string("should handle CREATE_PROJECT action"), [=]() mutable
        {
            auto createProjectAction = runtime->actions->find([=](auto a) mutable
            {
                return a["name"] == std::string("CREATE_PROJECT");
            }
            );
            expect(createProjectAction)->toBeDefined();
            auto createMessage = object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("test-room"))}, 
                object::pair{std::string("userId"), as<std::shared_ptr<UUID>>(std::string("test-user"))}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("I want to create a new plugin for weather data")}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            auto isValid = std::async([=]() { createProjectAction->validate(runtime, createMessage); });
            expect(isValid)->toBe(true);
        }
        );
        it(std::string("should have proper service initialization"), [=]() mutable
        {
            auto codeGenService = runtime->getService(std::string("code-generation"));
            expect(codeGenService)->toBeDefined();
            expect(type_of((as<any>(codeGenService))["generateCode"]))->toBe(std::string("function"));
            expect(type_of((as<any>(codeGenService))["start"]))->toBe(std::string("function"));
            expect(type_of((as<any>(codeGenService))["stop"]))->toBe(std::string("function"));
        }
        );
        it(std::string("should support form-based code generation"), [=]() mutable
        {
            auto formsService = runtime->getService(std::string("forms"));
            expect(formsService)->toBeDefined();
            auto createProjectAction = runtime->actions->find([=](auto a) mutable
            {
                return a["name"] == std::string("CREATE_PROJECT");
            }
            );
            auto message = object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("test-room"))}, 
                object::pair{std::string("userId"), as<std::shared_ptr<UUID>>(std::string("test-user"))}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Create a new plugin project")}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            auto isValid = std::async([=]() { createProjectAction->validate(runtime, message); });
            expect(isValid)->toBe(true);
        }
        );
    }
    );
}

MAIN
