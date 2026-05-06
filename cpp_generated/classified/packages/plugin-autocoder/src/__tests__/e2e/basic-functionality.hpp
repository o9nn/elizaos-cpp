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
    string name = std::string("basic-functionality");

    string description = std::string("Basic functionality tests for autocoder plugin");

    array<object> tests = array<object>{ object{
        object::pair{std::string("name"), std::string("should have required services available")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Testing service availability..."));
            auto codeGenService = runtime->getService(std::string("code-generation"));
            auto projectPlanningService = runtime->getService(std::string("project-planning"));
            auto secretsManagerService = runtime->getService(std::string("secrets-manager"));
            auto projectStatusManager = runtime->getService(std::string("project-status-manager"));
            auto formsService = runtime->getService(std::string("forms"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std::string("Code generation service not available")));
            }
            if (!projectPlanningService) {
                throw any(std::make_shared<Error>(std::string("Project planning service not available")));
            }
            if (!secretsManagerService) {
                throw any(std::make_shared<Error>(std::string("Secrets manager service not available")));
            }
            if (!projectStatusManager) {
                throw any(std::make_shared<Error>(std::string("Project status manager service not available")));
            }
            if (!formsService) {
                throw any(std::make_shared<Error>(std::string("Forms service not available")));
            }
            console->log(std::string("✅ All required services are available"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("should have generate code action registered")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Testing action registration..."));
            auto generateCodeAction = runtime->actions->find([=](auto action) mutable
            {
                return action["name"] == std::string("GENERATE_CODE");
            }
            );
            if (!generateCodeAction) {
                throw any(std::make_shared<Error>(std::string("GENERATE_CODE action not found")));
            }
            console->log(std::string("✅ GENERATE_CODE action is registered"));
            auto testMessage = object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("generate a weather plugin")}, 
                    object::pair{std::string("type"), std::string("text")}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            auto isValid = std::async([=]() { generateCodeAction->validate(runtime, testMessage); });
            if (!isValid) {
                throw any(std::make_shared<Error>(std::string("GENERATE_CODE action validation failed")));
            }
            console->log(std::string("✅ GENERATE_CODE action validation passed"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("should have projects provider available")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Testing provider availability..."));
            auto projectsProvider = runtime->providers->find([=](auto provider) mutable
            {
                return provider["name"] == std::string("PROJECTS_CONTEXT");
            }
            );
            if (!projectsProvider) {
                throw any(std::make_shared<Error>(std::string("PROJECTS_CONTEXT provider not found")));
            }
            console->log(std::string("✅ PROJECTS_CONTEXT provider is registered"));
            auto testMessage = object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("test message")}, 
                    object::pair{std::string("type"), std::string("text")}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            auto testState = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}
            };
            auto result = std::async([=]() { projectsProvider->get(runtime, testMessage, testState); });
            if (!result) {
                throw any(std::make_shared<Error>(std::string("PROJECTS_CONTEXT provider returned null")));
            }
            console->log(std::string("✅ PROJECTS_CONTEXT provider functionality verified"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("should handle project status tracking")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Testing project status tracking..."));
            auto statusManager = as<any>(runtime->getService(std::string("project-status-manager")));
            if (!statusManager) {
                throw any(std::make_shared<Error>(std::string("Project status manager not available")));
            }
            auto projectId = statusManager["createProject"](std::string("test-project"), std::string("plugin"));
            if (!projectId) {
                throw any(std::make_shared<Error>(std::string("Failed to create project")));
            }
            console->log(std::string("✅ Created project: ") + projectId + string_empty);
            statusManager["updateStatus"](projectId, object{
                object::pair{std::string("status"), std::string("generating")}, 
                object::pair{std::string("progress"), 50}, 
                object::pair{std::string("message"), std::string("Generating code...")}
            });
            statusManager["updateStep"](projectId, std::string("Code Generation"), std::string("Creating plugin structure..."));
            statusManager["updateValidation"](projectId, std::string("lint"), true);
            statusManager["updateValidation"](projectId, std::string("typeCheck"), true);
            auto project = statusManager["getProject"](projectId);
            if (!project) {
                throw any(std::make_shared<Error>(std::string("Failed to retrieve project")));
            }
            console->log(std::string("✅ Project status tracking successful"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("should handle forms service functionality")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Testing forms service functionality..."));
            auto formsService = as<any>(runtime->getService(std::string("forms")));
            if (!formsService) {
                throw any(std::make_shared<Error>(std::string("Forms service not available")));
            }
            auto form = std::async([=]() { formsService["createForm"](object{
                object::pair{std::string("name"), std::string("test-form")}, 
                object::pair{std::string("description"), std::string("Test form")}, 
                object::pair{std::string("steps"), array<object>{ object{
                    object::pair{std::string("id"), std::string("step1")}, 
                    object::pair{std::string("name"), std::string("Step 1")}, 
                    object::pair{std::string("fields"), array<object>{ object{
                        object::pair{std::string("id"), std::string("projectName")}, 
                        object::pair{std::string("type"), std::string("text")}, 
                        object::pair{std::string("label"), std::string("Project Name")}, 
                        object::pair{std::string("description"), std::string("Enter project name")}
                    } }}
                } }}
            }); });
            if (!form) {
                throw any(std::make_shared<Error>(std::string("Failed to create form")));
            }
            console->log(std::string("✅ Created form: ") + form["id"] + string_empty);
            auto retrievedForm = std::async([=]() { formsService["getForm"](form["id"]); });
            if (!retrievedForm) {
                throw any(std::make_shared<Error>(std::string("Failed to retrieve form")));
            }
            console->log(std::string("✅ Form retrieval successful"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("should initialize without errors")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Testing plugin initialization..."));
            shared pluginName = std::string("@elizaos/plugin-autocoder");
            auto plugin = runtime->plugins->find([=](auto p) mutable
            {
                return p["name"] == pluginName;
            }
            );
            if (!plugin) {
                throw any(std::make_shared<Error>(std::string("Plugin ") + pluginName + std::string(" not found in runtime")));
            }
            console->log(std::string("✅ Plugin found in runtime"));
            auto services = array<string>{ std::string("code-generation"), std::string("project-planning"), std::string("secrets-manager"), std::string("project-status-manager") };
            for (auto& serviceName : services)
            {
                auto service = runtime->getService(serviceName);
                if (!service) {
                    throw any(std::make_shared<Error>(std::string("Service ") + serviceName + std::string(" not initialized")));
                }
            }
            console->log(std::string("✅ All plugin services initialized correctly"));
        }
        }
    } };
};

#endif
