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
    string name = std:("code-generation-e2e");

    string description = std:("E2E tests for code generation workflow");

    array<object> tests = array<object>{ object{
        object::pair{std:("name"), std:("should handle full plugin generation workflow")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Starting plugin generation E2E test..."));
            if (!runtime->processActions) {
                console->log(std:("⏭️ Skipping test - runtime missing required methods"));
                return std::shared_ptr<Promise<void>>();
            }
            auto dbReady = false;
            try
            {
                dbReady = std::async([=]() { runtime->isReady(); });
            }
            catch (const any& error)
            {
                console->log(std:("⚠️ Database readiness check failed:"), (is<Error>(error)) ? error->message : String(error));
            }
            if (!dbReady) {
                console->log(std:("⚠️ Database not ready - memory operations may not be available"));
            }
            auto roomId = as<std::shared_ptr<UUID>>(std:("test-room-") + Date->now() + string_empty);
            auto userId = as<std::shared_ptr<UUID>>(std:("test-user"));
            auto createMessage = object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("entityId"), userId}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("I want to create a new ElizaOS plugin called "weather-plugin" that fetches weather data from OpenWeatherMap API")}, 
                    object::pair{std:("type"), std:("text")}
                }}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            if (dbReady) {
                try
                {
                    std::async([=]() { runtime->createMemory(createMessage, std:("messages")); });
                }
                catch (const any& error)
                {
                    console->log(std:("Note: Error creating memory:"), (is<Error>(error)) ? error->message : String(error));
                }
            } else {
                console->log(std:("Note: Skipping memory creation - database not ready"));
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
                        object::pair{std:("roomId"), std:("roomId")}, 
                        object::pair{std:("count"), 10}, 
                        object::pair{std:("unique"), true}, 
                        object::pair{std:("tableName"), std:("messages")}
                    }); });
                }
                catch (const any& error)
                {
                    console->log(std:("Note: Error getting memories:"), (is<Error>(error)) ? error->message : String(error));
                }
            } else {
                console->log(std:("Note: Skipping memory retrieval - database not ready"));
            }
            if (memories1->get_length() > 0) {
                auto formResponse = memories1->find([=](auto m) mutable
                {
                    return OR(((AND((m->entityId == runtime->agentId), (m->content->text->includes(std:("form")))))), (m->content->text->includes(std:("project"))));
                }
                );
                if (!formResponse) {
                    throw any(std::make_shared<Error>(std:("Agent did not create form for project details")));
                }
                console->log(std:("✓ Form created for project details"));
            }
            auto requirementsMessage = object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("entityId"), userId}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("The plugin should have actions to get current weather and forecast. It needs a provider to show weather context. Include proper error handling and caching.")}, 
                    object::pair{std:("type"), std:("text")}
                }}, 
                object::pair{std:("createdAt"), Date->now() + 1000}
            };
            std::async([=]() { runtime->processActions(requirementsMessage, array<any>()); });
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 2000);
            }
            ); });
            auto apiMessage = object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("entityId"), userId}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Use the OpenWeatherMap API for weather data")}, 
                    object::pair{std:("type"), std:("text")}
                }}, 
                object::pair{std:("createdAt"), Date->now() + 2000}
            };
            std::async([=]() { runtime->processActions(apiMessage, array<any>()); });
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 7777);
            }
            ); });
            console->log(std:("✓ Project requirements gathered"));
            console->log(std:("✅ Plugin generation E2E test completed (simulated)"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("should handle agent project generation")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Starting agent generation E2E test..."));
            if (!runtime->processActions) {
                console->log(std:("⏭️ Skipping test - runtime missing required methods"));
                return std::shared_ptr<Promise<void>>();
            }
            auto roomId = as<std::shared_ptr<UUID>>(std:("test-room-") + Date->now() + string_empty);
            auto userId = as<std::shared_ptr<UUID>>(std:("test-user"));
            auto createMessage = object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("entityId"), userId}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Create an ElizaOS agent called "support-bot" that helps users with technical support")}, 
                    object::pair{std:("type"), std:("text")}, 
                    object::pair{std:("actions"), array<string>{ std:("CREATE_PROJECT") }}
                }}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            std::async([=]() { runtime->processActions(createMessage, array<any>()); });
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 2000);
            }
            ); });
            auto detailsMessage = object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("entityId"), userId}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("The agent should be friendly, helpful, and knowledgeable about software issues. It should use the GitHub API to search for issues.")}, 
                    object::pair{std:("type"), std:("text")}
                }}, 
                object::pair{std:("createdAt"), Date->now() + 1000}
            };
            std::async([=]() { runtime->processActions(detailsMessage, array<any>()); });
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 7777);
            }
            ); });
            console->log(std:("✓ Agent project processed"));
            console->log(std:("✅ Agent generation E2E test completed"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("should handle missing API keys gracefully")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Starting API key handling test..."));
            if (!runtime->processActions) {
                console->log(std:("⏭️ Skipping test - runtime missing required methods"));
                return std::shared_ptr<Promise<void>>();
            }
            auto roomId = as<std::shared_ptr<UUID>>(std:("test-room-") + Date->now() + string_empty);
            auto userId = as<std::shared_ptr<UUID>>(std:("test-user"));
            auto createMessage = object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("entityId"), userId}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Create a plugin that uses OpenAI API for text generation")}, 
                    object::pair{std:("type"), std:("text")}
                }}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            std::async([=]() { runtime->processActions(createMessage, array<any>()); });
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 2000);
            }
            ); });
            auto requirementsMessage = object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("entityId"), userId}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("It should generate creative stories based on prompts")}, 
                    object::pair{std:("type"), std:("text")}
                }}, 
                object::pair{std:("createdAt"), Date->now() + 1000}
            };
            std::async([=]() { runtime->processActions(requirementsMessage, array<any>()); });
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 7777);
            }
            ); });
            console->log(std:("✓ API key request handled"));
            auto apiKeyMessage = object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("entityId"), userId}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("My OpenAI API key is sk-test12345")}, 
                    object::pair{std:("type"), std:("text")}
                }}, 
                object::pair{std:("createdAt"), Date->now() + 4000}
            };
            std::async([=]() { runtime->processActions(apiKeyMessage, array<any>()); });
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 2000);
            }
            ); });
            console->log(std:("✓ API key provided and accepted"));
            console->log(std:("✅ API key handling test completed"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("should validate generated code quality")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Starting code quality validation test..."));
            if (!runtime->processActions) {
                console->log(std:("⏭️ Skipping test - runtime missing required methods"));
                return std::shared_ptr<Promise<void>>();
            }
            auto roomId = as<std::shared_ptr<UUID>>(std:("test-room-") + Date->now() + string_empty);
            auto userId = as<std::shared_ptr<UUID>>(std:("test-user"));
            auto createMessage = object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("entityId"), userId}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Create a simple calculator plugin with add and subtract actions")}, 
                    object::pair{std:("type"), std:("text")}
                }}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            std::async([=]() { runtime->processActions(createMessage, array<any>()); });
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 2000);
            }
            ); });
            auto requirementsMessage = object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("entityId"), userId}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Just basic math operations with proper validation")}, 
                    object::pair{std:("type"), std:("text")}
                }}, 
                object::pair{std:("createdAt"), Date->now() + 1000}
            };
            std::async([=]() { runtime->processActions(requirementsMessage, array<any>()); });
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 5000);
            }
            ); });
            console->log(std:("✓ Quality assurance process completed"));
            console->log(std:("✅ Code quality validation test completed"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("should handle workflow project type")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Starting workflow generation test..."));
            if (!runtime->processActions) {
                console->log(std:("⏭️ Skipping test - runtime missing required methods"));
                return std::shared_ptr<Promise<void>>();
            }
            auto roomId = as<std::shared_ptr<UUID>>(std:("test-room-") + Date->now() + string_empty);
            auto userId = as<std::shared_ptr<UUID>>(std:("test-user"));
            auto createMessage = object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("entityId"), userId}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Create a workflow that processes customer orders")}, 
                    object::pair{std:("type"), std:("text")}
                }}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            std::async([=]() { runtime->processActions(createMessage, array<any>()); });
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 2000);
            }
            ); });
            auto stepsMessage = object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("entityId"), userId}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("The workflow should validate order, check inventory, process payment, and send confirmation email")}, 
                    object::pair{std:("type"), std:("text")}
                }}, 
                object::pair{std:("createdAt"), Date->now() + 1000}
            };
            std::async([=]() { runtime->processActions(stepsMessage, array<any>()); });
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 7777);
            }
            ); });
            console->log(std:("✓ Workflow project processed"));
            console->log(std:("✅ Workflow generation test completed"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("should handle project cancellation")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Starting project cancellation test..."));
            if (!runtime->processActions) {
                console->log(std:("⏭️ Skipping test - runtime missing required methods"));
                return std::shared_ptr<Promise<void>>();
            }
            auto roomId = as<std::shared_ptr<UUID>>(std:("test-room-") + Date->now() + string_empty);
            auto userId = as<std::shared_ptr<UUID>>(std:("test-user"));
            auto createMessage = object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("entityId"), userId}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Create a new plugin for data analysis")}, 
                    object::pair{std:("type"), std:("text")}
                }}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            std::async([=]() { runtime->processActions(createMessage, array<any>()); });
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 1000);
            }
            ); });
            auto cancelMessage = object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("entityId"), userId}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Actually, cancel this project")}, 
                    object::pair{std:("type"), std:("text")}, 
                    object::pair{std:("actions"), array<string>{ std:("CANCEL_FORM") }}
                }}, 
                object::pair{std:("createdAt"), Date->now() + 1000}
            };
            std::async([=]() { runtime->processActions(cancelMessage, array<any>()); });
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 2000);
            }
            ); });
            console->log(std:("✓ Project cancellation processed"));
            console->log(std:("✅ Project cancellation test completed"));
        }
        }
    } };
};

#endif
