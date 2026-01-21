#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-autocoder/src/__tests__/e2e/forms-integration-flow.test.h"

void Main(void)
{
    describe(std::string("Forms Integration Flow"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> runtime;
        shared<std::shared_ptr<FormsService>> formsService;
        beforeAll([=]() mutable
        {
            auto hasApiKey = !!(OR((process->env->OPENAI_API_KEY), (process->env->ANTHROPIC_API_KEY)));
            if (!hasApiKey) {
                console->log(std::string("⚠️  No API keys found - skipping forms integration tests"));
                return std::shared_ptr<Promise<void>>();
            }
            process->env->FORCE_BUNSQLITE = std::string("true");
            process->env->DATABASE_PATH = std::string("./.eliza/.test-forms-flow");
            process->env->ELIZA_TEST_MODE = std::string("true");
            process->env->SECRET_SALT = OR((process->env->SECRET_SALT), (std::string("test-salt-for-testing-only-not-secure")));
            runtime = std::make_shared<AgentRuntime>(object{
                object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("character"), object{
                    object::pair{std::string("name"), std::string("Test Agent")}, 
                    object::pair{std::string("bio"), array<string>{ std::string("Test agent for forms integration") }}, 
                    object::pair{std::string("system"), std::string("You are a helpful coding assistant")}, 
                    object::pair{std::string("settings"), object{
                        object::pair{std::string("OPENAI_API_KEY"), process->env->OPENAI_API_KEY}, 
                        object::pair{std::string("ANTHROPIC_API_KEY"), process->env->ANTHROPIC_API_KEY}
                    }}, 
                    object::pair{std::string("modelProvider"), (process->env->ANTHROPIC_API_KEY) ? std::string("anthropic") : std::string("openai")}
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
            formsService = as<std::shared_ptr<FormsService>>(runtime->getService(std::string("forms")));
        }
        );
        it(std::string("should handle CREATE_PROJECT action and create a form"), [=]() mutable
        {
            auto hasApiKey = !!(OR((process->env->OPENAI_API_KEY), (process->env->ANTHROPIC_API_KEY)));
            if (!hasApiKey) {
                console->log(std::string("Skipping test - no API key available"));
                return std::shared_ptr<Promise<void>>();
            }
            auto roomId = as<std::shared_ptr<UUID>>(uuidv4());
            auto userId = as<std::shared_ptr<UUID>>(uuidv4());
            auto message = object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("entityId"), userId}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Create a new plugin called weather-monitor that checks weather data")}, 
                    object::pair{std::string("type"), std::string("text")}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            auto createProjectAction = runtime->actions->find([=](auto a) mutable
            {
                return a["name"] == std::string("CREATE_PROJECT");
            }
            );
            expect(createProjectAction)->toBeDefined();
            auto isValid = std::async([=]() { createProjectAction->validate(runtime, message); });
            expect(isValid)->toBe(true);
            if (isValid) {
                auto callback = [=](auto response) mutable
                {
                    console->log(std::string("Action response:"), response);
                    auto forms = std::async([=]() { formsService->getAllForms(); });
                    auto projectForm = forms->find([=](auto f) mutable
                    {
                        return f["name"] == std::string("create-plugin");
                    }
                    );
                    expect(projectForm)->toBeDefined();
                    expect(projectForm->status)->toBe(std::string("active"));
                    console->log(std::string("✅ Form created successfully"));
                };
                std::async([=]() { createProjectAction->handler(runtime, message, undefined, undefined, callback); });
            }
        }
        );
        it(std::string("should handle complete form flow for plugin generation"), [=]() mutable
        {
            auto hasApiKey = !!(OR((process->env->OPENAI_API_KEY), (process->env->ANTHROPIC_API_KEY)));
            if (!hasApiKey) {
                console->log(std::string("Skipping test - no API key available"));
                return std::shared_ptr<Promise<void>>();
            }
            auto roomId = as<std::shared_ptr<UUID>>(uuidv4());
            auto userId = as<std::shared_ptr<UUID>>(uuidv4());
            auto form = std::async([=]() { formsService->createForm(object{
                object::pair{std::string("name"), std::string("test-plugin-form")}, 
                object::pair{std::string("description"), std::string("Test plugin creation form")}, 
                object::pair{std::string("status"), std::string("active")}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("entityId"), userId}, 
                object::pair{std::string("stepOrder"), array<string>{ std::string("basic"), std::string("requirements"), std::string("apis"), std::string("confirm") }}, 
                object::pair{std::string("currentStep"), std::string("basic")}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("projectType"), std::string("plugin")}
                }}, 
                object::pair{std::string("formData"), object{}}, 
                object::pair{std::string("createdAt"), Date->now()}
            }); });
            expect(form)->toBeDefined();
            console->log(std::string("✅ Form created"));
            std::async([=]() { formsService->updateForm(form->id, object{
                object::pair{std::string("formData"), object{
                    object::pair{std::string("projectName"), std::string("test-calculator-plugin")}, 
                    object::pair{std::string("projectDescription"), std::string("A plugin that performs calculations")}, 
                    object::pair{std::string("primaryFunction"), std::string("Perform basic math operations")}
                }}, 
                object::pair{std::string("currentStep"), std::string("requirements")}
            }); });
            console->log(std::string("✅ Basic info filled"));
            std::async([=]() { formsService->updateForm(form->id, object{
                object::pair{std::string("formData"), utils::assign(object{
                    , 
                    object::pair{std::string("keyFeatures"), array<string>{ std::string("Add numbers"), std::string("Subtract numbers"), std::string("Multiply numbers") }}, 
                    object::pair{std::string("targetUsers"), std::string("Anyone who needs calculations")}
                }, form->formData)}, 
                object::pair{std::string("currentStep"), std::string("apis")}
            }); });
            console->log(std::string("✅ Requirements filled"));
            std::async([=]() { formsService->updateForm(form->id, object{
                object::pair{std::string("formData"), utils::assign(object{
                    , 
                    object::pair{std::string("requiredAPIs"), array<any>()}, 
                    object::pair{std::string("externalServices"), array<any>()}
                }, form->formData)}, 
                object::pair{std::string("currentStep"), std::string("confirm")}
            }); });
            console->log(std::string("✅ API info filled"));
            std::async([=]() { formsService->updateForm(form->id, object{
                object::pair{std::string("status"), std::string("completed")}, 
                object::pair{std::string("completedAt"), Date->now()}
            }); });
            console->log(std::string("✅ Form completed"));
            shared generateAction = runtime->actions->find([=](auto a) mutable
            {
                return a["name"] == std::string("GENERATE_CODE");
            }
            );
            expect(generateAction)->toBeDefined();
            shared generateMessage = object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("entityId"), userId}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Generate the code for the calculator plugin")}, 
                    object::pair{std::string("type"), std::string("text")}, 
                    object::pair{std::string("formId"), form->id}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            auto isValid = std::async([=]() { generateAction->validate(runtime, generateMessage); });
            expect(isValid)->toBe(true);
            if (isValid) {
                auto result = std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    auto callback = [=](auto response) mutable
                    {
                        console->log(std::string("Generation response:"), response);
                        resolve(response);
                    };
                    generateAction->handler(runtime, generateMessage, undefined, undefined, callback);
                }
                ); });
                console->log(std::string("✅ Code generation triggered"));
            }
        }
        );
        it(std::string("should track project status throughout form flow"), [=]() mutable
        {
            auto hasApiKey = !!(OR((process->env->OPENAI_API_KEY), (process->env->ANTHROPIC_API_KEY)));
            if (!hasApiKey) {
                console->log(std::string("Skipping test - no API key available"));
                return std::shared_ptr<Promise<void>>();
            }
            auto statusManager = as<any>(runtime->getService(std::string("project-status-manager")));
            expect(statusManager)->toBeDefined();
            auto activeProjects = statusManager["getActiveProjects"]();
            console->log(std::string("Active projects: ") + activeProjects["length"] + string_empty);
            auto history = statusManager["getHistory"]();
            console->log(std::string("Project history: ") + history["length"] + std::string(" entries"));
            auto summary = statusManager["getStatusSummary"]();
            console->log(std::string("Status summary:"), object{
                object::pair{std::string("active"), summary["active"]}, 
                object::pair{std::string("completed"), summary["completed"]}, 
                object::pair{std::string("failed"), summary["failed"]}, 
                object::pair{std::string("totalGenerated"), summary["totalGenerated"]}, 
                object::pair{std::string("averageDuration"), string_empty + (summary["averageDuration"] / 1000)->toFixed(1) + std::string("s")}
            });
        }
        );
    }
    );
}

MAIN
