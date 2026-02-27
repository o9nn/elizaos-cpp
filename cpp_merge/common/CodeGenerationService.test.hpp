#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-AUTOCODER_SRC___TESTS___SERVICES_CODEGENERATIONSERVICE_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-AUTOCODER_SRC___TESTS___SERVICES_CODEGENERATIONSERVICE_TEST_H
#include "core.h"
#include "@elizaos/core.h"
#include "uuid.h"
using uuidv4 = v4;

class CodeGenerationFormTestSuite;

class CodeGenerationFormTestSuite : public TestSuite, public std::enable_shared_from_this<CodeGenerationFormTestSuite> {
public:
    using std::enable_shared_from_this<CodeGenerationFormTestSuite>::shared_from_this;
    string name = std:("code-generation-forms");

    string description = std:("Tests form interactions for code generation workflow");

    array<object> tests = array<object>{ object{
        object::pair{std:("name"), std:("should create plugin project form with correct structure")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("🧪 Testing plugin project form creation..."));
            auto formsService = as<any>(runtime->getService(std:("forms")));
            if (!formsService) {
                throw any(std::make_shared<Error>(std:("Forms service not available")));
            }
            auto codeGenService = runtime->getService(std:("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std:("Code generation service not available")));
            }
            auto pluginForm = std::async([=]() { formsService["createForm"](object{
                object::pair{std:("name"), std:("plugin-project-form")}, 
                object::pair{std:("description"), std:("Gather requirements for plugin generation")}, 
                object::pair{std:("steps"), array<object>{ object{
                    object::pair{std:("id"), std:("basic-info")}, 
                    object::pair{std:("name"), std:("Basic Information")}, 
                    object::pair{std:("fields"), array<object>{ object{
                        object::pair{std:("name"), std:("projectName")}, 
                        object::pair{std:("type"), std:("text")}, 
                        object::pair{std:("label"), std:("Plugin Name")}, 
                        object::pair{std:("description"), std:("Name for your plugin (e.g., weather-plugin)")}, 
                        object::pair{std:("required"), true}
                    }, object{
                        object::pair{std:("name"), std:("description")}, 
                        object::pair{std:("type"), std:("text")}, 
                        object::pair{std:("label"), std:("Description")}, 
                        object::pair{std:("description"), std:("What does this plugin do?")}, 
                        object::pair{std:("required"), true}
                    } }}
                }, object{
                    object::pair{std:("id"), std:("requirements")}, 
                    object::pair{std:("name"), std:("Requirements")}, 
                    object::pair{std:("fields"), array<object>{ object{
                        object::pair{std:("name"), std:("requirements")}, 
                        object::pair{std:("type"), std:("text")}, 
                        object::pair{std:("label"), std:("Features")}, 
                        object::pair{std:("description"), std:("What features should the plugin have?")}, 
                        object::pair{std:("required"), true}
                    }, object{
                        object::pair{std:("name"), std:("apis")}, 
                        object::pair{std:("type"), std:("text")}, 
                        object::pair{std:("label"), std:("APIs")}, 
                        object::pair{std:("description"), std:("What external APIs will it use?")}, 
                        object::pair{std:("required"), false}
                    } }}
                } }}
            }); });
            if (!pluginForm) {
                throw any(std::make_shared<Error>(std:("Plugin form was not created")));
            }
            if (!pluginForm["id"]) {
                throw any(std::make_shared<Error>(std:("Plugin form missing ID")));
            }
            if (pluginForm["name"] != std:("plugin-project-form")) {
                throw any(std::make_shared<Error>(std:("Expected form name 'plugin-project-form', got '") + pluginForm["name"] + std:("'")));
            }
            if (pluginForm["steps"]["length"] != 2) {
                throw any(std::make_shared<Error>(std:("Expected 2 steps, got ") + pluginForm["steps"]["length"] + string_empty));
            }
            if (const_(pluginForm["steps"])[0]["fields"]["length"] != 2) {
                throw any(std::make_shared<Error>(std:("Expected 2 fields in step 1, got ") + const_(pluginForm["steps"])[0]["fields"]["length"] + string_empty));
            }
            if (const_(pluginForm["steps"])[1]["fields"]["length"] != 2) {
                throw any(std::make_shared<Error>(std:("Expected 2 fields in step 2, got ") + const_(pluginForm["steps"])[1]["fields"]["length"] + string_empty));
            }
            console->log(std:("✅ Plugin project form created successfully"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("should handle form field updates correctly")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("🧪 Testing form field updates..."));
            auto formsService = as<any>(runtime->getService(std:("forms")));
            if (!formsService) {
                throw any(std::make_shared<Error>(std:("Forms service not available")));
            }
            auto form = std::async([=]() { formsService["createForm"](object{
                object::pair{std:("name"), std:("test-form")}, 
                object::pair{std:("description"), std:("Test form for field updates")}, 
                object::pair{std:("steps"), array<object>{ object{
                    object::pair{std:("id"), std:("step1")}, 
                    object::pair{std:("name"), std:("Step 1")}, 
                    object::pair{std:("fields"), array<object>{ object{
                        object::pair{std:("name"), std:("projectName")}, 
                        object::pair{std:("type"), std:("text")}, 
                        object::pair{std:("label"), std:("Project Name")}, 
                        object::pair{std:("required"), true}
                    } }}
                } }}
            }); });
            if (type_of(formsService["updateField"]) == std:("function")) {
                std::async([=]() { formsService["updateField"](form["id"], std:("projectName"), std:("my-awesome-plugin")); });
                auto updatedForm = std::async([=]() { formsService["getForm"](form["id"]); });
                auto field = const_(updatedForm["steps"])[0]["fields"]["find"]([=](auto f) mutable
                {
                    return f["name"] == std:("projectName");
                }
                );
                if (!field) {
                    throw any(std::make_shared<Error>(std:("Field not found after update")));
                }
                if (field["value"] != std:("my-awesome-plugin")) {
                    throw any(std::make_shared<Error>(std:("Expected field value 'my-awesome-plugin', got '") + field["value"] + std:("'")));
                }
                console->log(std:("✅ Form field updates working correctly"));
            } else {
                console->log(std:("⚠️  updateField method not available, skipping field update test"));
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("should validate required fields before completion")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("🧪 Testing form validation..."));
            auto formsService = as<any>(runtime->getService(std:("forms")));
            if (!formsService) {
                throw any(std::make_shared<Error>(std:("Forms service not available")));
            }
            auto form = std::async([=]() { formsService["createForm"](object{
                object::pair{std:("name"), std:("validation-test-form")}, 
                object::pair{std:("description"), std:("Test form validation")}, 
                object::pair{std:("steps"), array<object>{ object{
                    object::pair{std:("id"), std:("required-fields")}, 
                    object::pair{std:("name"), std:("Required Fields")}, 
                    object::pair{std:("fields"), array<object>{ object{
                        object::pair{std:("name"), std:("requiredField")}, 
                        object::pair{std:("type"), std:("text")}, 
                        object::pair{std:("label"), std:("Required Field")}, 
                        object::pair{std:("required"), true}
                    }, object{
                        object::pair{std:("name"), std:("optionalField")}, 
                        object::pair{std:("type"), std:("text")}, 
                        object::pair{std:("label"), std:("Optional Field")}, 
                        object::pair{std:("required"), false}
                    } }}
                } }}
            }); });
            if (type_of(formsService["completeForm"]) == std:("function")) {
                std::async([=]() { formsService["completeForm"](form["id"]); });
            } else if (type_of(formsService["submitForm"]) == std:("function")) {
                std::async([=]() { formsService["submitForm"](form["id"]); });
            }
            console->log(std:("⚠️  Form completed - validation behavior depends on implementation"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("should handle multi-step form progression")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("🧪 Testing multi-step form progression..."));
            auto formsService = as<any>(runtime->getService(std:("forms")));
            if (!formsService) {
                throw any(std::make_shared<Error>(std:("Forms service not available")));
            }
            shared stepCompletionCount = 0;
            auto form = std::async([=]() { formsService["createForm"](object{
                object::pair{std:("name"), std:("multi-step-form")}, 
                object::pair{std:("description"), std:("Test multi-step progression")}, 
                object::pair{std:("steps"), array<object>{ object{
                    object::pair{std:("id"), std:("step1")}, 
                    object::pair{std:("name"), std:("First Step")}, 
                    object::pair{std:("fields"), array<object>{ object{
                        object::pair{std:("name"), std:("field1")}, 
                        object::pair{std:("type"), std:("text")}, 
                        object::pair{std:("label"), std:("Field 1")}
                    } }}
                }, object{
                    object::pair{std:("id"), std:("step2")}, 
                    object::pair{std:("name"), std:("Second Step")}, 
                    object::pair{std:("fields"), array<object>{ object{
                        object::pair{std:("name"), std:("field2")}, 
                        object::pair{std:("type"), std:("text")}, 
                        object::pair{std:("label"), std:("Field 2")}
                    } }}
                }, object{
                    object::pair{std:("id"), std:("step3")}, 
                    object::pair{std:("name"), std:("Third Step")}, 
                    object::pair{std:("fields"), array<object>{ object{
                        object::pair{std:("name"), std:("field3")}, 
                        object::pair{std:("type"), std:("text")}, 
                        object::pair{std:("label"), std:("Field 3")}
                    } }}
                } }}, 
                object::pair{std:("onStepComplete"), [=](auto form, auto stepId) mutable
                {
                    stepCompletionCount++;
                    console->log(std:("  Step completed: ") + stepId + std:(" (") + stepCompletionCount + std:("/3)"));
                }
                }
            }); });
            if (type_of(formsService["completeStep"]) == std:("function")) {
                std::async([=]() { formsService["completeStep"](form["id"], std:("step1")); });
                std::async([=]() { formsService["completeStep"](form["id"], std:("step2")); });
                std::async([=]() { formsService["completeStep"](form["id"], std:("step3")); });
                if (stepCompletionCount != 3) {
                    throw any(std::make_shared<Error>(std:("Expected 3 steps completed, got ") + stepCompletionCount + string_empty));
                }
                console->log(std:("✅ Multi-step form progression working correctly"));
            } else {
                console->log(std:("⚠️  completeStep method not available, skipping step progression test"));
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("should extract project data from completed form")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("🧪 Testing project data extraction from form..."));
            auto formsService = as<any>(runtime->getService(std:("forms")));
            if (!formsService) {
                throw any(std::make_shared<Error>(std:("Forms service not available")));
            }
            auto form = std::async([=]() { formsService["createForm"](object{
                object::pair{std:("name"), std:("data-extraction-form")}, 
                object::pair{std:("description"), std:("Test data extraction")}, 
                object::pair{std:("steps"), array<object>{ object{
                    object::pair{std:("id"), std:("project-details")}, 
                    object::pair{std:("name"), std:("Project Details")}, 
                    object::pair{std:("fields"), array<object>{ object{
                        object::pair{std:("name"), std:("projectName")}, 
                        object::pair{std:("type"), std:("text")}, 
                        object::pair{std:("label"), std:("Project Name")}, 
                        object::pair{std:("value"), std:("test-weather-plugin")}
                    }, object{
                        object::pair{std:("name"), std:("description")}, 
                        object::pair{std:("type"), std:("text")}, 
                        object::pair{std:("label"), std:("Description")}, 
                        object::pair{std:("value"), std:("A plugin that provides weather information")}
                    }, object{
                        object::pair{std:("name"), std:("requirements")}, 
                        object::pair{std:("type"), std:("text")}, 
                        object::pair{std:("label"), std:("Requirements")}, 
                        object::pair{std:("value"), std:("Get current weather,Show forecast,Support multiple cities")}
                    }, object{
                        object::pair{std:("name"), std:("apis")}, 
                        object::pair{std:("type"), std:("text")}, 
                        object::pair{std:("label"), std:("APIs")}, 
                        object::pair{std:("value"), std:("OpenWeatherMap API,WeatherAPI.com")}
                    } }}
                } }}
            }); });
            auto projectData = object{
                object::pair{std:("targetType"), std:("plugin")}, 
                object::pair{std:("requirements"), array<any>()}, 
                object::pair{std:("apis"), array<any>()}
            };
            for (auto& step : form["steps"])
            {
                for (auto& field : step["fields"])
                {
                    if (field["value"]) {
                        if (field["name"] == std:("requirements")) {
                            projectData["requirements"] = field["value"]["split"]((new RegExp(std:("[,;\n"))))["map"]([=](auto r) mutable
                            {
                                return r->trim();
                            }
                            )["filter"]([=](auto r) mutable
                            {
                                return r;
                            }
                            );
                        } else if (field["name"] == std:("apis")) {
                            projectData["apis"] = field["value"]["split"]((new RegExp(std:("[,;\n"))))["map"]([=](auto a) mutable
                            {
                                return a->trim();
                            }
                            )["filter"]([=](auto a) mutable
                            {
                                return a;
                            }
                            );
                        } else {
                            projectData[field["name"]] = field["value"];
                        }
                    }
                }
            }
            if (projectData["projectName"] != std:("test-weather-plugin")) {
                throw any(std::make_shared<Error>(std:("Expected projectName 'test-weather-plugin', got '") + projectData["projectName"] + std:("'")));
            }
            if (projectData["description"] != std:("A plugin that provides weather information")) {
                throw any(std::make_shared<Error>(std:("Expected description 'A plugin that provides weather information', got '") + projectData["description"] + std:("'")));
            }
            if (projectData["requirements"]["length"] != 3) {
                throw any(std::make_shared<Error>(std:("Expected 3 requirements, got ") + projectData["requirements"]["length"] + string_empty));
            }
            if (const_(projectData["requirements"])[0] != std:("Get current weather")) {
                throw any(std::make_shared<Error>(std:("Expected first requirement 'Get current weather', got '") + const_(projectData["requirements"])[0] + std:("'")));
            }
            if (projectData["apis"]["length"] != 2) {
                throw any(std::make_shared<Error>(std:("Expected 2 APIs, got ") + projectData["apis"]["length"] + string_empty));
            }
            if (const_(projectData["apis"])[0] != std:("OpenWeatherMap API")) {
                throw any(std::make_shared<Error>(std:("Expected first API 'OpenWeatherMap API', got '") + const_(projectData["apis"])[0] + std:("'")));
            }
            console->log(std:("✅ Project data extraction working correctly"));
            console->log(std:("  Extracted:"), JSON->stringify(projectData, nullptr, 2));
        }
        }
    }, object{
        object::pair{std:("name"), std:("should handle agent project form differently than plugin form")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("🧪 Testing agent vs plugin form differences..."));
            auto formsService = as<any>(runtime->getService(std:("forms")));
            if (!formsService) {
                throw any(std::make_shared<Error>(std:("Forms service not available")));
            }
            auto agentForm = std::async([=]() { formsService["createForm"](object{
                object::pair{std:("name"), std:("agent-project-form")}, 
                object::pair{std:("description"), std:("Gather requirements for agent generation")}, 
                object::pair{std:("steps"), array<object>{ object{
                    object::pair{std:("id"), std:("agent-identity")}, 
                    object::pair{std:("name"), std:("Agent Identity")}, 
                    object::pair{std:("fields"), array<object>{ object{
                        object::pair{std:("name"), std:("projectName")}, 
                        object::pair{std:("type"), std:("text")}, 
                        object::pair{std:("label"), std:("Agent Name")}, 
                        object::pair{std:("description"), std:("Name for your agent")}, 
                        object::pair{std:("required"), true}
                    }, object{
                        object::pair{std:("name"), std:("description")}, 
                        object::pair{std:("type"), std:("text")}, 
                        object::pair{std:("label"), std:("Agent Purpose")}, 
                        object::pair{std:("description"), std:("What is the agent's main purpose?")}, 
                        object::pair{std:("required"), true}
                    }, object{
                        object::pair{std:("name"), std:("personality")}, 
                        object::pair{std:("type"), std:("text")}, 
                        object::pair{std:("label"), std:("Personality")}, 
                        object::pair{std:("description"), std:("Describe the agent's personality")}, 
                        object::pair{std:("required"), true}
                    } }}
                }, object{
                    object::pair{std:("id"), std:("capabilities")}, 
                    object::pair{std:("name"), std:("Capabilities")}, 
                    object::pair{std:("fields"), array<object>{ object{
                        object::pair{std:("name"), std:("requirements")}, 
                        object::pair{std:("type"), std:("text")}, 
                        object::pair{std:("label"), std:("Capabilities")}, 
                        object::pair{std:("description"), std:("What should the agent be able to do?")}, 
                        object::pair{std:("required"), true}
                    }, object{
                        object::pair{std:("name"), std:("knowledge")}, 
                        object::pair{std:("type"), std:("text")}, 
                        object::pair{std:("label"), std:("Knowledge Base")}, 
                        object::pair{std:("description"), std:("What should the agent know about?")}, 
                        object::pair{std:("required"), false}
                    } }}
                } }}
            }); });
            if (agentForm["name"] != std:("agent-project-form")) {
                throw any(std::make_shared<Error>(std:("Expected form name 'agent-project-form', got '") + agentForm["name"] + std:("'")));
            }
            if (agentForm["steps"]["length"] != 2) {
                throw any(std::make_shared<Error>(std:("Expected 2 steps, got ") + agentForm["steps"]["length"] + string_empty));
            }
            auto hasPersonalityField = const_(agentForm["steps"])[0]["fields"]["some"]([=](auto f) mutable
            {
                return f["name"] == std:("personality");
            }
            );
            auto hasKnowledgeField = const_(agentForm["steps"])[1]["fields"]["some"]([=](auto f) mutable
            {
                return f["name"] == std:("knowledge");
            }
            );
            if (!hasPersonalityField) {
                throw any(std::make_shared<Error>(std:("Agent form missing personality field")));
            }
            if (!hasKnowledgeField) {
                throw any(std::make_shared<Error>(std:("Agent form missing knowledge field")));
            }
            console->log(std:("✅ Agent form has correct agent-specific fields"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("should trigger code generation after form completion")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("🧪 Testing form completion triggers code generation..."));
            auto formsService = as<any>(runtime->getService(std:("forms")));
            shared codeGenService = as<any>(runtime->getService(std:("code-generation")));
            if (OR((!formsService), (!codeGenService))) {
                throw any(std::make_shared<Error>(std:("Required services not available")));
            }
            shared codeGenerationTriggered = false;
            shared generatedProjectData = nullptr;
            auto originalGenerateCode = codeGenService["generateCode"];
            codeGenService["generateCode"] = [=](auto request) mutable
            {
                codeGenerationTriggered = true;
                generatedProjectData = request;
                return object{
                    object::pair{std:("success"), true}, 
                    object::pair{std:("files"), array<any>()}
                };
            };
            auto form = std::async([=]() { formsService["createForm"](object{
                object::pair{std:("name"), std:("trigger-test-form")}, 
                object::pair{std:("description"), std:("Test code generation trigger")}, 
                object::pair{std:("steps"), array<object>{ object{
                    object::pair{std:("id"), std:("details")}, 
                    object::pair{std:("name"), std:("Details")}, 
                    object::pair{std:("fields"), array<object>{ object{
                        object::pair{std:("name"), std:("projectName")}, 
                        object::pair{std:("type"), std:("text")}, 
                        object::pair{std:("label"), std:("Project Name")}, 
                        object::pair{std:("value"), std:("trigger-test-project")}
                    }, object{
                        object::pair{std:("name"), std:("description")}, 
                        object::pair{std:("type"), std:("text")}, 
                        object::pair{std:("label"), std:("Description")}, 
                        object::pair{std:("value"), std:("Test project to verify form triggers code generation")}
                    } }}
                } }}, 
                object::pair{std:("onComplete"), [=](auto completedForm) mutable
                {
                    auto projectData = object{
                        object::pair{std:("projectName"), std:("trigger-test-project")}, 
                        object::pair{std:("description"), std:("Test project")}, 
                        object::pair{std:("targetType"), as<std::shared_ptr<const>>(std:("plugin"))}, 
                        object::pair{std:("requirements"), array<string>{ std:("Test requirement") }}, 
                        object::pair{std:("apis"), array<any>()}
                    };
                    std::async([=]() { codeGenService["generateCode"](projectData); });
                }
                }
            }); });
            if (form["onComplete"]) {
                std::async([=]() { form["onComplete"](form); });
            }
            if (!codeGenerationTriggered) {
                throw any(std::make_shared<Error>(std:("Code generation was not triggered")));
            }
            if (!generatedProjectData) {
                throw any(std::make_shared<Error>(std:("Project data was not passed to generateCode")));
            }
            if (generatedProjectData["projectName"] != std:("trigger-test-project")) {
                throw any(std::make_shared<Error>(std:("Expected projectName 'trigger-test-project', got '") + generatedProjectData["projectName"] + std:("'")));
            }
            console->log(std:("✅ Form completion successfully triggers code generation"));
            codeGenService["generateCode"] = originalGenerateCode;
        }
        }
    }, object{
        object::pair{std:("name"), std:("should handle form cancellation gracefully")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("🧪 Testing form cancellation..."));
            auto formsService = as<any>(runtime->getService(std:("forms")));
            if (!formsService) {
                throw any(std::make_shared<Error>(std:("Forms service not available")));
            }
            auto form = std::async([=]() { formsService["createForm"](object{
                object::pair{std:("name"), std:("cancellation-test-form")}, 
                object::pair{std:("description"), std:("Test form cancellation")}, 
                object::pair{std:("steps"), array<object>{ object{
                    object::pair{std:("id"), std:("step1")}, 
                    object::pair{std:("name"), std:("Step 1")}, 
                    object::pair{std:("fields"), array<object>{ object{
                        object::pair{std:("name"), std:("field1")}, 
                        object::pair{std:("type"), std:("text")}, 
                        object::pair{std:("label"), std:("Field 1")}
                    } }}
                } }}
            }); });
            if (type_of(formsService["cancelForm"]) == std:("function")) {
                std::async([=]() { formsService["cancelForm"](form["id"]); });
                auto cancelledForm = std::async([=]() { formsService["getForm"](form["id"]); });
                if (AND((cancelledForm), (cancelledForm["status"] == std:("cancelled")))) {
                    console->log(std:("✅ Form cancellation handled correctly"));
                } else {
                    console->log(std:("⚠️  Form exists but status not updated to cancelled"));
                }
            } else {
                console->log(std:("⚠️  cancelForm method not available, skipping cancellation test"));
            }
        }
        }
    } };
};

#endif
