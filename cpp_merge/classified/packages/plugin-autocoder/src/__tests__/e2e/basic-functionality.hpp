#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-AUTOCODER_SRC___TESTS___E2E_BASIC-FUNCTIONALITY_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-AUTOCODER_SRC___TESTS___E2E_BASIC-FUNCTIONALITY_TEST_H
#include "core.h"
#include "@elizaos/core.h"
#include "uuid.h"
using uuidv4 = v4;

class BasicFunctionalityTestSuite;

class BasicFunctionalityTestSuite : public TestSuite, public std::enable_shared_from_this<BasicFunctionalityTestSuite> {
public:
    using std::enable_shared_from_this<BasicFunctionalityTestSuite>::shared_from_this;
    string name = std:("basic-functionality");

    string description = std:("Basic functionality tests for autocoder plugin");

    array<object> tests = array<object>{ object{
        object::pair{std:("name"), std:("should have required services available")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Testing service availability..."));
            auto codeGenService = runtime->getService(std:("code-generation"));
            auto projectPlanningService = runtime->getService(std:("project-planning"));
            auto secretsManagerService = runtime->getService(std:("secrets-manager"));
            auto projectStatusManager = runtime->getService(std:("project-status-manager"));
            auto formsService = runtime->getService(std:("forms"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std:("Code generation service not available")));
            }
            if (!projectPlanningService) {
                throw any(std::make_shared<Error>(std:("Project planning service not available")));
            }
            if (!secretsManagerService) {
                throw any(std::make_shared<Error>(std:("Secrets manager service not available")));
            }
            if (!projectStatusManager) {
                throw any(std::make_shared<Error>(std:("Project status manager service not available")));
            }
            if (!formsService) {
                throw any(std::make_shared<Error>(std:("Forms service not available")));
            }
            console->log(std:("✅ All required services are available"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("should have generate code action registered")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Testing action registration..."));
            auto generateCodeAction = runtime->actions->find([=](auto action) mutable
            {
                return action["name"] == std:("GENERATE_CODE");
            }
            );
            if (!generateCodeAction) {
                throw any(std::make_shared<Error>(std:("GENERATE_CODE action not found")));
            }
            console->log(std:("✅ GENERATE_CODE action is registered"));
            auto testMessage = object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("generate a weather plugin")}, 
                    object::pair{std:("type"), std:("text")}
                }}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            auto isValid = std::async([=]() { generateCodeAction->validate(runtime, testMessage); });
            if (!isValid) {
                throw any(std::make_shared<Error>(std:("GENERATE_CODE action validation failed")));
            }
            console->log(std:("✅ GENERATE_CODE action validation passed"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("should have projects provider available")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Testing provider availability..."));
            auto projectsProvider = runtime->providers->find([=](auto provider) mutable
            {
                return provider["name"] == std:("PROJECTS_CONTEXT");
            }
            );
            if (!projectsProvider) {
                throw any(std::make_shared<Error>(std:("PROJECTS_CONTEXT provider not found")));
            }
            console->log(std:("✅ PROJECTS_CONTEXT provider is registered"));
            auto testMessage = object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("test message")}, 
                    object::pair{std:("type"), std:("text")}
                }}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            auto testState = object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
            auto result = std::async([=]() { projectsProvider->get(runtime, testMessage, testState); });
            if (!result) {
                throw any(std::make_shared<Error>(std:("PROJECTS_CONTEXT provider returned null")));
            }
            console->log(std:("✅ PROJECTS_CONTEXT provider functionality verified"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("should handle project status tracking")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Testing project status tracking..."));
            auto statusManager = as<any>(runtime->getService(std:("project-status-manager")));
            if (!statusManager) {
                throw any(std::make_shared<Error>(std:("Project status manager not available")));
            }
            auto projectId = statusManager["createProject"](std:("test-project"), std:("plugin"));
            if (!projectId) {
                throw any(std::make_shared<Error>(std:("Failed to create project")));
            }
            console->log(std:("✅ Created project: ") + projectId + string_empty);
            statusManager["updateStatus"](projectId, object{
                object::pair{std:("status"), std:("generating")}, 
                object::pair{std:("progress"), 50}, 
                object::pair{std:("message"), std:("Generating code...")}
            });
            statusManager["updateStep"](projectId, std:("Code Generation"), std:("Creating plugin structure..."));
            statusManager["updateValidation"](projectId, std:("lint"), true);
            statusManager["updateValidation"](projectId, std:("typeCheck"), true);
            auto project = statusManager["getProject"](projectId);
            if (!project) {
                throw any(std::make_shared<Error>(std:("Failed to retrieve project")));
            }
            console->log(std:("✅ Project status tracking successful"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("should handle forms service functionality")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Testing forms service functionality..."));
            auto formsService = as<any>(runtime->getService(std:("forms")));
            if (!formsService) {
                throw any(std::make_shared<Error>(std:("Forms service not available")));
            }
            auto form = std::async([=]() { formsService["createForm"](object{
                object::pair{std:("name"), std:("test-form")}, 
                object::pair{std:("description"), std:("Test form")}, 
                object::pair{std:("steps"), array<object>{ object{
                    object::pair{std:("id"), std:("step1")}, 
                    object::pair{std:("name"), std:("Step 1")}, 
                    object::pair{std:("fields"), array<object>{ object{
                        object::pair{std:("id"), std:("projectName")}, 
                        object::pair{std:("type"), std:("text")}, 
                        object::pair{std:("label"), std:("Project Name")}, 
                        object::pair{std:("description"), std:("Enter project name")}
                    } }}
                } }}
            }); });
            if (!form) {
                throw any(std::make_shared<Error>(std:("Failed to create form")));
            }
            console->log(std:("✅ Created form: ") + form["id"] + string_empty);
            auto retrievedForm = std::async([=]() { formsService["getForm"](form["id"]); });
            if (!retrievedForm) {
                throw any(std::make_shared<Error>(std:("Failed to retrieve form")));
            }
            console->log(std:("✅ Form retrieval successful"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("should initialize without errors")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Testing plugin initialization..."));
            shared pluginName = std:("@elizaos/plugin-autocoder");
            auto plugin = runtime->plugins->find([=](auto p) mutable
            {
                return p["name"] == pluginName;
            }
            );
            if (!plugin) {
                throw any(std::make_shared<Error>(std:("Plugin ") + pluginName + std:(" not found in runtime")));
            }
            console->log(std:("✅ Plugin found in runtime"));
            auto services = array<string>{ std:("code-generation"), std:("project-planning"), std:("secrets-manager"), std:("project-status-manager") };
            for (auto& serviceName : services)
            {
                auto service = runtime->getService(serviceName);
                if (!service) {
                    throw any(std::make_shared<Error>(std:("Service ") + serviceName + std:(" not initialized")));
                }
            }
            console->log(std:("✅ All plugin services initialized correctly"));
        }
        }
    } };
};

#endif
