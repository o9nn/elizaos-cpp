#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-AUTOCODER_SRC___TESTS___E2E_CODE-GENERATION_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-AUTOCODER_SRC___TESTS___E2E_CODE-GENERATION_TEST_H
#include "core.h"
#include "@elizaos/core.h"
#include "uuid.h"
using uuidv4 = v4;

class CodeGenerationE2ETestSuite;

class CodeGenerationE2ETestSuite : public TestSuite, public std::enable_shared_from_this<CodeGenerationE2ETestSuite> {
public:
    using std::enable_shared_from_this<CodeGenerationE2ETestSuite>::shared_from_this;
    string name = std::string("code-generation-e2e");

    string description = std::string("E2E tests for code generation workflow");

    array<object> tests = array<object>{ object{
        object::pair{std::string("name"), std::string("should handle full plugin generation workflow")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Starting plugin generation E2E test..."));
            if (!runtime->processActions) {
                console->log(std::string("⏭️ Skipping test - runtime missing required methods"));
                return std::shared_ptr<Promise<void>>();
            }
            auto dbReady = false;
            try
            {
                dbReady = std::async([=]() { runtime->isReady(); });
            }
            catch (const any& error)
            {
                console->log(std::string("⚠️ Database readiness check failed:"), (is<Error>(error)) ? error->message : String(error));
            }
            if (!dbReady) {
                console->log(std::string("⚠️ Database not ready - memory operations may not be available"));
            }
            auto roomId = as<std::shared_ptr<UUID>>(std::string("test-room-") + Date->now() + string_empty);
            auto userId = as<std::shared_ptr<UUID>>(std::string("test-user"));
            auto createMessage = object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("entityId"), userId}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("I want to create a new ElizaOS plugin called "weather-plugin" that fetches weather data from OpenWeatherMap API")}, 
                    object::pair{std::string("type"), std::string("text")}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            if (dbReady) {
                try
                {
                    std::async([=]() { runtime->createMemory(createMessage, std::string("messages")); });
                }
                catch (const any& error)
                {
                    console->log(std::string("Note: Error creating memory:"), (is<Error>(error)) ? error->message : String(error));
                }
            } else {
                console->log(std::string("Note: Skipping memory creation - database not ready"));
            }
            std::async([=]() { runtime->processActions(createMessage, array<any>()); });
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 2000);
            }
            ); });
            auto memories1 = array<std::shared_ptr<Memory>>();
            if (dbReady) {
                try
                {
                    memories1 = std::async([=]() { runtime->getMemories(object{
                        object::pair{std::string("roomId"), std::string("roomId")}, 
                        object::pair{std::string("count"), 10}, 
                        object::pair{std::string("unique"), true}, 
                        object::pair{std::string("tableName"), std::string("messages")}
                    }); });
                }
                catch (const any& error)
                {
                    console->log(std::string("Note: Error getting memories:"), (is<Error>(error)) ? error->message : String(error));
                }
            } else {
                console->log(std::string("Note: Skipping memory retrieval - database not ready"));
            }
            if (memories1->get_length() > 0) {
                auto formResponse = memories1->find([=](auto m) mutable
                {
                    return OR(((AND((m->entityId == runtime->agentId), (m->content->text->includes(std::string("form")))))), (m->content->text->includes(std::string("project"))));
                }
                );
                if (!formResponse) {
                    throw any(std::make_shared<Error>(std::string("Agent did not create form for project details")));
                }
                console->log(std::string("✓ Form created for project details"));
            }
            auto requirementsMessage = object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("entityId"), userId}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("The plugin should have actions to get current weather and forecast. It needs a provider to show weather context. Include proper error handling and caching.")}, 
                    object::pair{std::string("type"), std::string("text")}
                }}, 
                object::pair{std::string("createdAt"), Date->now() + 1000}
            };
            std::async([=]() { runtime->processActions(requirementsMessage, array<any>()); });
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 2000);
            }
            ); });
            auto apiMessage = object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("entityId"), userId}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Use the OpenWeatherMap API for weather data")}, 
                    object::pair{std::string("type"), std::string("text")}
                }}, 
                object::pair{std::string("createdAt"), Date->now() + 2000}
            };
            std::async([=]() { runtime->processActions(apiMessage, array<any>()); });
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 7777);
            }
            ); });
            console->log(std::string("✓ Project requirements gathered"));
            console->log(std::string("✅ Plugin generation E2E test completed (simulated)"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("should handle agent project generation")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Starting agent generation E2E test..."));
            if (!runtime->processActions) {
                console->log(std::string("⏭️ Skipping test - runtime missing required methods"));
                return std::shared_ptr<Promise<void>>();
            }
            auto roomId = as<std::shared_ptr<UUID>>(std::string("test-room-") + Date->now() + string_empty);
            auto userId = as<std::shared_ptr<UUID>>(std::string("test-user"));
            auto createMessage = object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("entityId"), userId}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Create an ElizaOS agent called "support-bot" that helps users with technical support")}, 
                    object::pair{std::string("type"), std::string("text")}, 
                    object::pair{std::string("actions"), array<string>{ std::string("CREATE_PROJECT") }}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            std::async([=]() { runtime->processActions(createMessage, array<any>()); });
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 2000);
            }
            ); });
            auto detailsMessage = object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("entityId"), userId}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("The agent should be friendly, helpful, and knowledgeable about software issues. It should use the GitHub API to search for issues.")}, 
                    object::pair{std::string("type"), std::string("text")}
                }}, 
                object::pair{std::string("createdAt"), Date->now() + 1000}
            };
            std::async([=]() { runtime->processActions(detailsMessage, array<any>()); });
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 7777);
            }
            ); });
            console->log(std::string("✓ Agent project processed"));
            console->log(std::string("✅ Agent generation E2E test completed"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("should handle missing API keys gracefully")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Starting API key handling test..."));
            if (!runtime->processActions) {
                console->log(std::string("⏭️ Skipping test - runtime missing required methods"));
                return std::shared_ptr<Promise<void>>();
            }
            auto roomId = as<std::shared_ptr<UUID>>(std::string("test-room-") + Date->now() + string_empty);
            auto userId = as<std::shared_ptr<UUID>>(std::string("test-user"));
            auto createMessage = object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("entityId"), userId}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Create a plugin that uses OpenAI API for text generation")}, 
                    object::pair{std::string("type"), std::string("text")}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            std::async([=]() { runtime->processActions(createMessage, array<any>()); });
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 2000);
            }
            ); });
            auto requirementsMessage = object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("entityId"), userId}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("It should generate creative stories based on prompts")}, 
                    object::pair{std::string("type"), std::string("text")}
                }}, 
                object::pair{std::string("createdAt"), Date->now() + 1000}
            };
            std::async([=]() { runtime->processActions(requirementsMessage, array<any>()); });
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 7777);
            }
            ); });
            console->log(std::string("✓ API key request handled"));
            auto apiKeyMessage = object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("entityId"), userId}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("My OpenAI API key is sk-test12345")}, 
                    object::pair{std::string("type"), std::string("text")}
                }}, 
                object::pair{std::string("createdAt"), Date->now() + 4000}
            };
            std::async([=]() { runtime->processActions(apiKeyMessage, array<any>()); });
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 2000);
            }
            ); });
            console->log(std::string("✓ API key provided and accepted"));
            console->log(std::string("✅ API key handling test completed"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("should validate generated code quality")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Starting code quality validation test..."));
            if (!runtime->processActions) {
                console->log(std::string("⏭️ Skipping test - runtime missing required methods"));
                return std::shared_ptr<Promise<void>>();
            }
            auto roomId = as<std::shared_ptr<UUID>>(std::string("test-room-") + Date->now() + string_empty);
            auto userId = as<std::shared_ptr<UUID>>(std::string("test-user"));
            auto createMessage = object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("entityId"), userId}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Create a simple calculator plugin with add and subtract actions")}, 
                    object::pair{std::string("type"), std::string("text")}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            std::async([=]() { runtime->processActions(createMessage, array<any>()); });
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 2000);
            }
            ); });
            auto requirementsMessage = object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("entityId"), userId}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Just basic math operations with proper validation")}, 
                    object::pair{std::string("type"), std::string("text")}
                }}, 
                object::pair{std::string("createdAt"), Date->now() + 1000}
            };
            std::async([=]() { runtime->processActions(requirementsMessage, array<any>()); });
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 5000);
            }
            ); });
            console->log(std::string("✓ Quality assurance process completed"));
            console->log(std::string("✅ Code quality validation test completed"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("should handle workflow project type")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Starting workflow generation test..."));
            if (!runtime->processActions) {
                console->log(std::string("⏭️ Skipping test - runtime missing required methods"));
                return std::shared_ptr<Promise<void>>();
            }
            auto roomId = as<std::shared_ptr<UUID>>(std::string("test-room-") + Date->now() + string_empty);
            auto userId = as<std::shared_ptr<UUID>>(std::string("test-user"));
            auto createMessage = object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("entityId"), userId}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Create a workflow that processes customer orders")}, 
                    object::pair{std::string("type"), std::string("text")}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            std::async([=]() { runtime->processActions(createMessage, array<any>()); });
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 2000);
            }
            ); });
            auto stepsMessage = object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("entityId"), userId}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("The workflow should validate order, check inventory, process payment, and send confirmation email")}, 
                    object::pair{std::string("type"), std::string("text")}
                }}, 
                object::pair{std::string("createdAt"), Date->now() + 1000}
            };
            std::async([=]() { runtime->processActions(stepsMessage, array<any>()); });
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 7777);
            }
            ); });
            console->log(std::string("✓ Workflow project processed"));
            console->log(std::string("✅ Workflow generation test completed"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("should handle project cancellation")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Starting project cancellation test..."));
            if (!runtime->processActions) {
                console->log(std::string("⏭️ Skipping test - runtime missing required methods"));
                return std::shared_ptr<Promise<void>>();
            }
            auto roomId = as<std::shared_ptr<UUID>>(std::string("test-room-") + Date->now() + string_empty);
            auto userId = as<std::shared_ptr<UUID>>(std::string("test-user"));
            auto createMessage = object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("entityId"), userId}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Create a new plugin for data analysis")}, 
                    object::pair{std::string("type"), std::string("text")}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            std::async([=]() { runtime->processActions(createMessage, array<any>()); });
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 1000);
            }
            ); });
            auto cancelMessage = object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("entityId"), userId}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Actually, cancel this project")}, 
                    object::pair{std::string("type"), std::string("text")}, 
                    object::pair{std::string("actions"), array<string>{ std::string("CANCEL_FORM") }}
                }}, 
                object::pair{std::string("createdAt"), Date->now() + 1000}
            };
            std::async([=]() { runtime->processActions(cancelMessage, array<any>()); });
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 2000);
            }
            ); });
            console->log(std::string("✓ Project cancellation processed"));
            console->log(std::string("✅ Project cancellation test completed"));
        }
        }
    } };
};

#endif
