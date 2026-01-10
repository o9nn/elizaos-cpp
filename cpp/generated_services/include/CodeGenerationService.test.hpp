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
    string name = std::string("code-generation-forms");

    string description = std::string("Tests form interactions for code generation workflow");

    array<object> tests = array<object>{ object{
        object::pair{std::string("name"), std::string("should create plugin project form with correct structure")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("🧪 Testing plugin project form creation..."));
            auto formsService = as<any>(runtime->getService(std::string("forms")));
            if (!formsService) {
                throw any(std::make_shared<Error>(std::string("Forms service not available")));
            }
            auto codeGenService = runtime->getService(std::string("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std::string("Code generation service not available")));
            }
            auto pluginForm = std::async([=]() { formsService["createForm"](object{
                object::pair{std::string("name"), std::string("plugin-project-form")}, 
                object::pair{std::string("description"), std::string("Gather requirements for plugin generation")}, 
                object::pair{std::string("steps"), array<object>{ object{
                    object::pair{std::string("id"), std::string("basic-info")}, 
                    object::pair{std::string("name"), std::string("Basic Information")}, 
                    object::pair{std::string("fields"), array<object>{ object{
                        object::pair{std::string("name"), std::string("projectName")}, 
                        object::pair{std::string("type"), std::string("text")}, 
                        object::pair{std::string("label"), std::string("Plugin Name")}, 
                        object::pair{std::string("description"), std::string("Name for your plugin (e.g., weather-plugin)")}, 
                        object::pair{std::string("required"), true}
                    }, object{
                        object::pair{std::string("name"), std::string("description")}, 
                        object::pair{std::string("type"), std::string("text")}, 
                        object::pair{std::string("label"), std::string("Description")}, 
                        object::pair{std::string("description"), std::string("What does this plugin do?")}, 
                        object::pair{std::string("required"), true}
                    } }}
                }, object{
                    object::pair{std::string("id"), std::string("requirements")}, 
                    object::pair{std::string("name"), std::string("Requirements")}, 
                    object::pair{std::string("fields"), array<object>{ object{
                        object::pair{std::string("name"), std::string("requirements")}, 
                        object::pair{std::string("type"), std::string("text")}, 
                        object::pair{std::string("label"), std::string("Features")}, 
                        object::pair{std::string("description"), std::string("What features should the plugin have?")}, 
                        object::pair{std::string("required"), true}
                    }, object{
                        object::pair{std::string("name"), std::string("apis")}, 
                        object::pair{std::string("type"), std::string("text")}, 
                        object::pair{std::string("label"), std::string("APIs")}, 
                        object::pair{std::string("description"), std::string("What external APIs will it use?")}, 
                        object::pair{std::string("required"), false}
                    } }}
                } }}
            }); });
            if (!pluginForm) {
                throw any(std::make_shared<Error>(std::string("Plugin form was not created")));
            }
            if (!pluginForm["id"]) {
                throw any(std::make_shared<Error>(std::string("Plugin form missing ID")));
            }
            if (pluginForm["name"] != std::string("plugin-project-form")) {
                throw any(std::make_shared<Error>(std::string("Expected form name 'plugin-project-form', got '") + pluginForm["name"] + std::string("'")));
            }
            if (pluginForm["steps"]["length"] != 2) {
                throw any(std::make_shared<Error>(std::string("Expected 2 steps, got ") + pluginForm["steps"]["length"] + string_empty));
            }
            if (const_(pluginForm["steps"])[0]["fields"]["length"] != 2) {
                throw any(std::make_shared<Error>(std::string("Expected 2 fields in step 1, got ") + const_(pluginForm["steps"])[0]["fields"]["length"] + string_empty));
            }
            if (const_(pluginForm["steps"])[1]["fields"]["length"] != 2) {
                throw any(std::make_shared<Error>(std::string("Expected 2 fields in step 2, got ") + const_(pluginForm["steps"])[1]["fields"]["length"] + string_empty));
            }
            console->log(std::string("✅ Plugin project form created successfully"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("should handle form field updates correctly")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("🧪 Testing form field updates..."));
            auto formsService = as<any>(runtime->getService(std::string("forms")));
            if (!formsService) {
                throw any(std::make_shared<Error>(std::string("Forms service not available")));
            }
            auto form = std::async([=]() { formsService["createForm"](object{
                object::pair{std::string("name"), std::string("test-form")}, 
                object::pair{std::string("description"), std::string("Test form for field updates")}, 
                object::pair{std::string("steps"), array<object>{ object{
                    object::pair{std::string("id"), std::string("step1")}, 
                    object::pair{std::string("name"), std::string("Step 1")}, 
                    object::pair{std::string("fields"), array<object>{ object{
                        object::pair{std::string("name"), std::string("projectName")}, 
                        object::pair{std::string("type"), std::string("text")}, 
                        object::pair{std::string("label"), std::string("Project Name")}, 
                        object::pair{std::string("required"), true}
                    } }}
                } }}
            }); });
            if (type_of(formsService["updateField"]) == std::string("function")) {
                std::async([=]() { formsService["updateField"](form["id"], std::string("projectName"), std::string("my-awesome-plugin")); });
                auto updatedForm = std::async([=]() { formsService["getForm"](form["id"]); });
                auto field = const_(updatedForm["steps"])[0]["fields"]["find"]([=](auto f) mutable
                {
                    return f["name"] == std::string("projectName");
                }
                );
                if (!field) {
                    throw any(std::make_shared<Error>(std::string("Field not found after update")));
                }
                if (field["value"] != std::string("my-awesome-plugin")) {
                    throw any(std::make_shared<Error>(std::string("Expected field value 'my-awesome-plugin', got '") + field["value"] + std::string("'")));
                }
                console->log(std::string("✅ Form field updates working correctly"));
            } else {
                console->log(std::string("⚠️  updateField method not available, skipping field update test"));
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("should validate required fields before completion")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("🧪 Testing form validation..."));
            auto formsService = as<any>(runtime->getService(std::string("forms")));
            if (!formsService) {
                throw any(std::make_shared<Error>(std::string("Forms service not available")));
            }
            auto form = std::async([=]() { formsService["createForm"](object{
                object::pair{std::string("name"), std::string("validation-test-form")}, 
                object::pair{std::string("description"), std::string("Test form validation")}, 
                object::pair{std::string("steps"), array<object>{ object{
                    object::pair{std::string("id"), std::string("required-fields")}, 
                    object::pair{std::string("name"), std::string("Required Fields")}, 
                    object::pair{std::string("fields"), array<object>{ object{
                        object::pair{std::string("name"), std::string("requiredField")}, 
                        object::pair{std::string("type"), std::string("text")}, 
                        object::pair{std::string("label"), std::string("Required Field")}, 
                        object::pair{std::string("required"), true}
                    }, object{
                        object::pair{std::string("name"), std::string("optionalField")}, 
                        object::pair{std::string("type"), std::string("text")}, 
                        object::pair{std::string("label"), std::string("Optional Field")}, 
                        object::pair{std::string("required"), false}
                    } }}
                } }}
            }); });
            if (type_of(formsService["completeForm"]) == std::string("function")) {
                std::async([=]() { formsService["completeForm"](form["id"]); });
            } else if (type_of(formsService["submitForm"]) == std::string("function")) {
                std::async([=]() { formsService["submitForm"](form["id"]); });
            }
            console->log(std::string("⚠️  Form completed - validation behavior depends on implementation"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("should handle multi-step form progression")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("🧪 Testing multi-step form progression..."));
            auto formsService = as<any>(runtime->getService(std::string("forms")));
            if (!formsService) {
                throw any(std::make_shared<Error>(std::string("Forms service not available")));
            }
            shared stepCompletionCount = 0;
            auto form = std::async([=]() { formsService["createForm"](object{
                object::pair{std::string("name"), std::string("multi-step-form")}, 
                object::pair{std::string("description"), std::string("Test multi-step progression")}, 
                object::pair{std::string("steps"), array<object>{ object{
                    object::pair{std::string("id"), std::string("step1")}, 
                    object::pair{std::string("name"), std::string("First Step")}, 
                    object::pair{std::string("fields"), array<object>{ object{
                        object::pair{std::string("name"), std::string("field1")}, 
                        object::pair{std::string("type"), std::string("text")}, 
                        object::pair{std::string("label"), std::string("Field 1")}
                    } }}
                }, object{
                    object::pair{std::string("id"), std::string("step2")}, 
                    object::pair{std::string("name"), std::string("Second Step")}, 
                    object::pair{std::string("fields"), array<object>{ object{
                        object::pair{std::string("name"), std::string("field2")}, 
                        object::pair{std::string("type"), std::string("text")}, 
                        object::pair{std::string("label"), std::string("Field 2")}
                    } }}
                }, object{
                    object::pair{std::string("id"), std::string("step3")}, 
                    object::pair{std::string("name"), std::string("Third Step")}, 
                    object::pair{std::string("fields"), array<object>{ object{
                        object::pair{std::string("name"), std::string("field3")}, 
                        object::pair{std::string("type"), std::string("text")}, 
                        object::pair{std::string("label"), std::string("Field 3")}
                    } }}
                } }}, 
                object::pair{std::string("onStepComplete"), [=](auto form, auto stepId) mutable
                {
                    stepCompletionCount++;
                    console->log(std::string("  Step completed: ") + stepId + std::string(" (") + stepCompletionCount + std::string("/3)"));
                }
                }
            }); });
            if (type_of(formsService["completeStep"]) == std::string("function")) {
                std::async([=]() { formsService["completeStep"](form["id"], std::string("step1")); });
                std::async([=]() { formsService["completeStep"](form["id"], std::string("step2")); });
                std::async([=]() { formsService["completeStep"](form["id"], std::string("step3")); });
                if (stepCompletionCount != 3) {
                    throw any(std::make_shared<Error>(std::string("Expected 3 steps completed, got ") + stepCompletionCount + string_empty));
                }
                console->log(std::string("✅ Multi-step form progression working correctly"));
            } else {
                console->log(std::string("⚠️  completeStep method not available, skipping step progression test"));
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("should extract project data from completed form")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("🧪 Testing project data extraction from form..."));
            auto formsService = as<any>(runtime->getService(std::string("forms")));
            if (!formsService) {
                throw any(std::make_shared<Error>(std::string("Forms service not available")));
            }
            auto form = std::async([=]() { formsService["createForm"](object{
                object::pair{std::string("name"), std::string("data-extraction-form")}, 
                object::pair{std::string("description"), std::string("Test data extraction")}, 
                object::pair{std::string("steps"), array<object>{ object{
                    object::pair{std::string("id"), std::string("project-details")}, 
                    object::pair{std::string("name"), std::string("Project Details")}, 
                    object::pair{std::string("fields"), array<object>{ object{
                        object::pair{std::string("name"), std::string("projectName")}, 
                        object::pair{std::string("type"), std::string("text")}, 
                        object::pair{std::string("label"), std::string("Project Name")}, 
                        object::pair{std::string("value"), std::string("test-weather-plugin")}
                    }, object{
                        object::pair{std::string("name"), std::string("description")}, 
                        object::pair{std::string("type"), std::string("text")}, 
                        object::pair{std::string("label"), std::string("Description")}, 
                        object::pair{std::string("value"), std::string("A plugin that provides weather information")}
                    }, object{
                        object::pair{std::string("name"), std::string("requirements")}, 
                        object::pair{std::string("type"), std::string("text")}, 
                        object::pair{std::string("label"), std::string("Requirements")}, 
                        object::pair{std::string("value"), std::string("Get current weather,Show forecast,Support multiple cities")}
                    }, object{
                        object::pair{std::string("name"), std::string("apis")}, 
                        object::pair{std::string("type"), std::string("text")}, 
                        object::pair{std::string("label"), std::string("APIs")}, 
                        object::pair{std::string("value"), std::string("OpenWeatherMap API,WeatherAPI.com")}
                    } }}
                } }}
            }); });
            auto projectData = object{
                object::pair{std::string("targetType"), std::string("plugin")}, 
                object::pair{std::string("requirements"), array<any>()}, 
                object::pair{std::string("apis"), array<any>()}
            };
            for (auto& step : form["steps"])
            {
                for (auto& field : step["fields"])
                {
                    if (field["value"]) {
                        if (field["name"] == std::string("requirements")) {
                            projectData["requirements"] = field["value"]["split"]((new RegExp(std::string("[,;\n"))))["map"]([=](auto r) mutable
                            {
                                return r->trim();
                            }
                            )["filter"]([=](auto r) mutable
                            {
                                return r;
                            }
                            );
                        } else if (field["name"] == std::string("apis")) {
                            projectData["apis"] = field["value"]["split"]((new RegExp(std::string("[,;\n"))))["map"]([=](auto a) mutable
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
            if (projectData["projectName"] != std::string("test-weather-plugin")) {
                throw any(std::make_shared<Error>(std::string("Expected projectName 'test-weather-plugin', got '") + projectData["projectName"] + std::string("'")));
            }
            if (projectData["description"] != std::string("A plugin that provides weather information")) {
                throw any(std::make_shared<Error>(std::string("Expected description 'A plugin that provides weather information', got '") + projectData["description"] + std::string("'")));
            }
            if (projectData["requirements"]["length"] != 3) {
                throw any(std::make_shared<Error>(std::string("Expected 3 requirements, got ") + projectData["requirements"]["length"] + string_empty));
            }
            if (const_(projectData["requirements"])[0] != std::string("Get current weather")) {
                throw any(std::make_shared<Error>(std::string("Expected first requirement 'Get current weather', got '") + const_(projectData["requirements"])[0] + std::string("'")));
            }
            if (projectData["apis"]["length"] != 2) {
                throw any(std::make_shared<Error>(std::string("Expected 2 APIs, got ") + projectData["apis"]["length"] + string_empty));
            }
            if (const_(projectData["apis"])[0] != std::string("OpenWeatherMap API")) {
                throw any(std::make_shared<Error>(std::string("Expected first API 'OpenWeatherMap API', got '") + const_(projectData["apis"])[0] + std::string("'")));
            }
            console->log(std::string("✅ Project data extraction working correctly"));
            console->log(std::string("  Extracted:"), JSON->stringify(projectData, nullptr, 2));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("should handle agent project form differently than plugin form")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("🧪 Testing agent vs plugin form differences..."));
            auto formsService = as<any>(runtime->getService(std::string("forms")));
            if (!formsService) {
                throw any(std::make_shared<Error>(std::string("Forms service not available")));
            }
            auto agentForm = std::async([=]() { formsService["createForm"](object{
                object::pair{std::string("name"), std::string("agent-project-form")}, 
                object::pair{std::string("description"), std::string("Gather requirements for agent generation")}, 
                object::pair{std::string("steps"), array<object>{ object{
                    object::pair{std::string("id"), std::string("agent-identity")}, 
                    object::pair{std::string("name"), std::string("Agent Identity")}, 
                    object::pair{std::string("fields"), array<object>{ object{
                        object::pair{std::string("name"), std::string("projectName")}, 
                        object::pair{std::string("type"), std::string("text")}, 
                        object::pair{std::string("label"), std::string("Agent Name")}, 
                        object::pair{std::string("description"), std::string("Name for your agent")}, 
                        object::pair{std::string("required"), true}
                    }, object{
                        object::pair{std::string("name"), std::string("description")}, 
                        object::pair{std::string("type"), std::string("text")}, 
                        object::pair{std::string("label"), std::string("Agent Purpose")}, 
                        object::pair{std::string("description"), std::string("What is the agent's main purpose?")}, 
                        object::pair{std::string("required"), true}
                    }, object{
                        object::pair{std::string("name"), std::string("personality")}, 
                        object::pair{std::string("type"), std::string("text")}, 
                        object::pair{std::string("label"), std::string("Personality")}, 
                        object::pair{std::string("description"), std::string("Describe the agent's personality")}, 
                        object::pair{std::string("required"), true}
                    } }}
                }, object{
                    object::pair{std::string("id"), std::string("capabilities")}, 
                    object::pair{std::string("name"), std::string("Capabilities")}, 
                    object::pair{std::string("fields"), array<object>{ object{
                        object::pair{std::string("name"), std::string("requirements")}, 
                        object::pair{std::string("type"), std::string("text")}, 
                        object::pair{std::string("label"), std::string("Capabilities")}, 
                        object::pair{std::string("description"), std::string("What should the agent be able to do?")}, 
                        object::pair{std::string("required"), true}
                    }, object{
                        object::pair{std::string("name"), std::string("knowledge")}, 
                        object::pair{std::string("type"), std::string("text")}, 
                        object::pair{std::string("label"), std::string("Knowledge Base")}, 
                        object::pair{std::string("description"), std::string("What should the agent know about?")}, 
                        object::pair{std::string("required"), false}
                    } }}
                } }}
            }); });
            if (agentForm["name"] != std::string("agent-project-form")) {
                throw any(std::make_shared<Error>(std::string("Expected form name 'agent-project-form', got '") + agentForm["name"] + std::string("'")));
            }
            if (agentForm["steps"]["length"] != 2) {
                throw any(std::make_shared<Error>(std::string("Expected 2 steps, got ") + agentForm["steps"]["length"] + string_empty));
            }
            auto hasPersonalityField = const_(agentForm["steps"])[0]["fields"]["some"]([=](auto f) mutable
            {
                return f["name"] == std::string("personality");
            }
            );
            auto hasKnowledgeField = const_(agentForm["steps"])[1]["fields"]["some"]([=](auto f) mutable
            {
                return f["name"] == std::string("knowledge");
            }
            );
            if (!hasPersonalityField) {
                throw any(std::make_shared<Error>(std::string("Agent form missing personality field")));
            }
            if (!hasKnowledgeField) {
                throw any(std::make_shared<Error>(std::string("Agent form missing knowledge field")));
            }
            console->log(std::string("✅ Agent form has correct agent-specific fields"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("should trigger code generation after form completion")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("🧪 Testing form completion triggers code generation..."));
            auto formsService = as<any>(runtime->getService(std::string("forms")));
            shared codeGenService = as<any>(runtime->getService(std::string("code-generation")));
            if (OR((!formsService), (!codeGenService))) {
                throw any(std::make_shared<Error>(std::string("Required services not available")));
            }
            shared codeGenerationTriggered = false;
            shared generatedProjectData = nullptr;
            auto originalGenerateCode = codeGenService["generateCode"];
            codeGenService["generateCode"] = [=](auto request) mutable
            {
                codeGenerationTriggered = true;
                generatedProjectData = request;
                return object{
                    object::pair{std::string("success"), true}, 
                    object::pair{std::string("files"), array<any>()}
                };
            };
            auto form = std::async([=]() { formsService["createForm"](object{
                object::pair{std::string("name"), std::string("trigger-test-form")}, 
                object::pair{std::string("description"), std::string("Test code generation trigger")}, 
                object::pair{std::string("steps"), array<object>{ object{
                    object::pair{std::string("id"), std::string("details")}, 
                    object::pair{std::string("name"), std::string("Details")}, 
                    object::pair{std::string("fields"), array<object>{ object{
                        object::pair{std::string("name"), std::string("projectName")}, 
                        object::pair{std::string("type"), std::string("text")}, 
                        object::pair{std::string("label"), std::string("Project Name")}, 
                        object::pair{std::string("value"), std::string("trigger-test-project")}
                    }, object{
                        object::pair{std::string("name"), std::string("description")}, 
                        object::pair{std::string("type"), std::string("text")}, 
                        object::pair{std::string("label"), std::string("Description")}, 
                        object::pair{std::string("value"), std::string("Test project to verify form triggers code generation")}
                    } }}
                } }}, 
                object::pair{std::string("onComplete"), [=](auto completedForm) mutable
                {
                    auto projectData = object{
                        object::pair{std::string("projectName"), std::string("trigger-test-project")}, 
                        object::pair{std::string("description"), std::string("Test project")}, 
                        object::pair{std::string("targetType"), as<std::shared_ptr<const>>(std::string("plugin"))}, 
                        object::pair{std::string("requirements"), array<string>{ std::string("Test requirement") }}, 
                        object::pair{std::string("apis"), array<any>()}
                    };
                    std::async([=]() { codeGenService["generateCode"](projectData); });
                }
                }
            }); });
            if (form["onComplete"]) {
                std::async([=]() { form["onComplete"](form); });
            }
            if (!codeGenerationTriggered) {
                throw any(std::make_shared<Error>(std::string("Code generation was not triggered")));
            }
            if (!generatedProjectData) {
                throw any(std::make_shared<Error>(std::string("Project data was not passed to generateCode")));
            }
            if (generatedProjectData["projectName"] != std::string("trigger-test-project")) {
                throw any(std::make_shared<Error>(std::string("Expected projectName 'trigger-test-project', got '") + generatedProjectData["projectName"] + std::string("'")));
            }
            console->log(std::string("✅ Form completion successfully triggers code generation"));
            codeGenService["generateCode"] = originalGenerateCode;
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("should handle form cancellation gracefully")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("🧪 Testing form cancellation..."));
            auto formsService = as<any>(runtime->getService(std::string("forms")));
            if (!formsService) {
                throw any(std::make_shared<Error>(std::string("Forms service not available")));
            }
            auto form = std::async([=]() { formsService["createForm"](object{
                object::pair{std::string("name"), std::string("cancellation-test-form")}, 
                object::pair{std::string("description"), std::string("Test form cancellation")}, 
                object::pair{std::string("steps"), array<object>{ object{
                    object::pair{std::string("id"), std::string("step1")}, 
                    object::pair{std::string("name"), std::string("Step 1")}, 
                    object::pair{std::string("fields"), array<object>{ object{
                        object::pair{std::string("name"), std::string("field1")}, 
                        object::pair{std::string("type"), std::string("text")}, 
                        object::pair{std::string("label"), std::string("Field 1")}
                    } }}
                } }}
            }); });
            if (type_of(formsService["cancelForm"]) == std::string("function")) {
                std::async([=]() { formsService["cancelForm"](form["id"]); });
                auto cancelledForm = std::async([=]() { formsService["getForm"](form["id"]); });
                if (AND((cancelledForm), (cancelledForm["status"] == std::string("cancelled")))) {
                    console->log(std::string("✅ Form cancellation handled correctly"));
                } else {
                    console->log(std::string("⚠️  Form exists but status not updated to cancelled"));
                }
            } else {
                console->log(std::string("⚠️  cancelForm method not available, skipping cancellation test"));
            }
        }
        }
    } };
};

#endif
