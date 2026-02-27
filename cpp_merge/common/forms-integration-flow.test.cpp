#include "forms-integration-flow.test.h"

void Main(void)
{
    describe(std:("Forms Integration Flow"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> runtime;
        shared<std::shared_ptr<FormsService>> formsService;
        beforeAll([=]() mutable
        {
            auto hasApiKey = !!(OR((process->env->OPENAI_API_KEY), (process->env->ANTHROPIC_API_KEY)));
            if (!hasApiKey) {
                console->log(std:("⚠️  No API keys found - skipping forms integration tests"));
                return std::shared_ptr<Promise<void>>();
            }
            process->env->FORCE_BUNSQLITE = std:("true");
            process->env->DATABASE_PATH = std:("./.eliza/.test-forms-flow");
            process->env->ELIZA_TEST_MODE = std:("true");
            process->env->SECRET_SALT = OR((process->env->SECRET_SALT), (std:("test-salt-for-testing-only-not-secure")));
            runtime = std::make_shared<AgentRuntime>(object{
                object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("character"), object{
                    object::pair{std:("name"), std:("Test Agent")}, 
                    object::pair{std:("bio"), array<string>{ std:("Test agent for forms integration") }}, 
                    object::pair{std:("system"), std:("You are a helpful coding assistant")}, 
                    object::pair{std:("settings"), object{
                        object::pair{std:("OPENAI_API_KEY"), process->env->OPENAI_API_KEY}, 
                        object::pair{std:("ANTHROPIC_API_KEY"), process->env->ANTHROPIC_API_KEY}
                    }}, 
                    object::pair{std:("modelProvider"), (process->env->ANTHROPIC_API_KEY) ? std:("anthropic") : std:("openai")}
                }}
            });
            std::async([=]() { runtime->registerPlugin(sqlPlugin); });
            std::async([=]() { runtime->registerPlugin(inferencePlugin); });
            if (process->env->ANTHROPIC_API_KEY) {
                std::async([=]() { runtime->registerPlugin(anthropicPlugin); });
            } else if (process->env->OPENAI_API_KEY) {
                std::async([=]() { runtime->registerPlugin(openaiPlugin); });
            }
            std::async([=]() { runtime->registerPlugin(formsPlugin); });
            std::async([=]() { runtime->registerPlugin(autocoderPlugin); });
            std::async([=]() { runtime->initialize(); });
            formsService = as<std::shared_ptr<FormsService>>(runtime->getService(std:("forms")));
        }
        );
        it(std:("should handle CREATE_PROJECT action and create a form"), [=]() mutable
        {
            auto hasApiKey = !!(OR((process->env->OPENAI_API_KEY), (process->env->ANTHROPIC_API_KEY)));
            if (!hasApiKey) {
                console->log(std:("Skipping test - no API key available"));
                return std::shared_ptr<Promise<void>>();
            }
            auto roomId = as<std::shared_ptr<UUID>>(uuidv4());
            auto userId = as<std::shared_ptr<UUID>>(uuidv4());
            auto message = object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("entityId"), userId}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Create a new plugin called weather-monitor that checks weather data")}, 
                    object::pair{std:("type"), std:("text")}
                }}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            auto createProjectAction = runtime->actions->find([=](auto a) mutable
            {
                return a["name"] == std:("CREATE_PROJECT");
            }
            );
            expect(createProjectAction)->toBeDefined();
            auto isValid = std::async([=]() { createProjectAction->validate(runtime, message); });
            expect(isValid)->toBe(true);
            if (isValid) {
                auto callback = [=](auto response) mutable
                {
                    console->log(std:("Action response:"), response);
                    auto forms = std::async([=]() { formsService->getAllForms(); });
                    auto projectForm = forms->find([=](auto f) mutable
                    {
                        return f["name"] == std:("create-plugin");
                    }
                    );
                    expect(projectForm)->toBeDefined();
                    expect(projectForm->status)->toBe(std:("active"));
                    console->log(std:("✅ Form created successfully"));
                };
                std::async([=]() { createProjectAction->handler(runtime, message, undefined, undefined, callback); });
            }
        }
        );
        it(std:("should handle complete form flow for plugin generation"), [=]() mutable
        {
            auto hasApiKey = !!(OR((process->env->OPENAI_API_KEY), (process->env->ANTHROPIC_API_KEY)));
            if (!hasApiKey) {
                console->log(std:("Skipping test - no API key available"));
                return std::shared_ptr<Promise<void>>();
            }
            auto roomId = as<std::shared_ptr<UUID>>(uuidv4());
            auto userId = as<std::shared_ptr<UUID>>(uuidv4());
            auto form = std::async([=]() { formsService->createForm(object{
                object::pair{std:("name"), std:("test-plugin-form")}, 
                object::pair{std:("description"), std:("Test plugin creation form")}, 
                object::pair{std:("status"), std:("active")}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("entityId"), userId}, 
                object::pair{std:("stepOrder"), array<string>{ std:("basic"), std:("requirements"), std:("apis"), std:("confirm") }}, 
                object::pair{std:("currentStep"), std:("basic")}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("projectType"), std:("plugin")}
                }}, 
                object::pair{std:("formData"), object{}}, 
                object::pair{std:("createdAt"), Date->now()}
            }); });
            expect(form)->toBeDefined();
            console->log(std:("✅ Form created"));
            std::async([=]() { formsService->updateForm(form->id, object{
                object::pair{std:("formData"), object{
                    object::pair{std:("projectName"), std:("test-calculator-plugin")}, 
                    object::pair{std:("projectDescription"), std:("A plugin that performs calculations")}, 
                    object::pair{std:("primaryFunction"), std:("Perform basic math operations")}
                }}, 
                object::pair{std:("currentStep"), std:("requirements")}
            }); });
            console->log(std:("✅ Basic info filled"));
            std::async([=]() { formsService->updateForm(form->id, object{
                object::pair{std:("formData"), utils::assign(object{
                    , 
                    object::pair{std:("keyFeatures"), array<string>{ std:("Add numbers"), std:("Subtract numbers"), std:("Multiply numbers") }}, 
                    object::pair{std:("targetUsers"), std:("Anyone who needs calculations")}
                }, form->formData)}, 
                object::pair{std:("currentStep"), std:("apis")}
            }); });
            console->log(std:("✅ Requirements filled"));
            std::async([=]() { formsService->updateForm(form->id, object{
                object::pair{std:("formData"), utils::assign(object{
                    , 
                    object::pair{std:("requiredAPIs"), array<any>()}, 
                    object::pair{std:("externalServices"), array<any>()}
                }, form->formData)}, 
                object::pair{std:("currentStep"), std:("confirm")}
            }); });
            console->log(std:("✅ API info filled"));
            std::async([=]() { formsService->updateForm(form->id, object{
                object::pair{std:("status"), std:("completed")}, 
                object::pair{std:("completedAt"), Date->now()}
            }); });
            console->log(std:("✅ Form completed"));
            shared generateAction = runtime->actions->find([=](auto a) mutable
            {
                return a["name"] == std:("GENERATE_CODE");
            }
            );
            expect(generateAction)->toBeDefined();
            shared generateMessage = object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("entityId"), userId}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Generate the code for the calculator plugin")}, 
                    object::pair{std:("type"), std:("text")}, 
                    object::pair{std:("formId"), form->id}
                }}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            auto isValid = std::async([=]() { generateAction->validate(runtime, generateMessage); });
            expect(isValid)->toBe(true);
            if (isValid) {
                auto result = std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    auto callback = [=](auto response) mutable
                    {
                        console->log(std:("Generation response:"), response);
                        resolve(response);
                    };
                    generateAction->handler(runtime, generateMessage, undefined, undefined, callback);
                }
                ); });
                console->log(std:("✅ Code generation triggered"));
            }
        }
        );
        it(std:("should track project status throughout form flow"), [=]() mutable
        {
            auto hasApiKey = !!(OR((process->env->OPENAI_API_KEY), (process->env->ANTHROPIC_API_KEY)));
            if (!hasApiKey) {
                console->log(std:("Skipping test - no API key available"));
                return std::shared_ptr<Promise<void>>();
            }
            auto statusManager = as<any>(runtime->getService(std:("project-status-manager")));
            expect(statusManager)->toBeDefined();
            auto activeProjects = statusManager["getActiveProjects"]();
            console->log(std:("Active projects: ") + activeProjects["length"] + string_empty);
            auto history = statusManager["getHistory"]();
            console->log(std:("Project history: ") + history["length"] + std:(" entries"));
            auto summary = statusManager["getStatusSummary"]();
            console->log(std:("Status summary:"), object{
                object::pair{std:("active"), summary["active"]}, 
                object::pair{std:("completed"), summary["completed"]}, 
                object::pair{std:("failed"), summary["failed"]}, 
                object::pair{std:("totalGenerated"), summary["totalGenerated"]}, 
                object::pair{std:("averageDuration"), string_empty + (summary["averageDuration"] / 1000)->toFixed(1) + std:("s")}
            });
        }
        );
    }
    );
}

MAIN
