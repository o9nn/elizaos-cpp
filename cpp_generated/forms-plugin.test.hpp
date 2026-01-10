#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-FORMS_SRC___TESTS___E2E_FORMS-PLUGIN_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-FORMS_SRC___TESTS___E2E_FORMS-PLUGIN_TEST_H
#include "core.h"
#include "@elizaos/core.h"
#include "uuid.h"
using uuidv4 = v4;
#include "../../services/forms-service.h"
#include "../../types.h"

class FormsPluginTestSuite;

class FormsPluginTestSuite : public TestSuite, public std::enable_shared_from_this<FormsPluginTestSuite> {
public:
    using std::enable_shared_from_this<FormsPluginTestSuite>::shared_from_this;
    string name = std::string("forms-plugin-e2e");

    string description = std::string("E2E tests for the forms plugin");

    array<object> tests = array<object>{ object{
        object::pair{std::string("name"), std::string("Basic service functionality")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Test runtime agent ID:"), runtime->agentId);
            auto formsService = as<std::shared_ptr<FormsService>>(runtime->getService(std::string("forms")));
            if (!formsService) {
                throw any(std::make_shared<Error>(std::string("Forms service not found")));
            }
            console->log(std::string("Forms service found"));
            console->log(std::string("Service runtime agent ID:"), (as<any>(formsService))["runtime"]["agentId"]);
            auto form = std::async([=]() { formsService->createForm(std::string("contact")); });
            console->log(std::string("Created form:"), object{
                object::pair{std::string("id"), form->id}, 
                object::pair{std::string("agentId"), form->agentId}, 
                object::pair{std::string("name"), form->name}, 
                object::pair{std::string("status"), form->status}
            });
            auto allForms = std::async([=]() { formsService->listForms(); });
            console->log(std::string("All forms:"), allForms->get_length());
            allForms->forEach([=](auto f) mutable
            {
                console->log(std::string("Form in list:"), object{
                    object::pair{std::string("id"), f->id}, 
                    object::pair{std::string("agentId"), f->agentId}, 
                    object::pair{std::string("name"), f->name}, 
                    object::pair{std::string("status"), f->status}
                });
            }
            );
            auto activeForms = std::async([=]() { formsService->listForms(std::string("active")); });
            console->log(std::string("Active forms:"), activeForms->get_length());
            if (activeForms->get_length() == 0) {
                throw any(std::make_shared<Error>(std::string("Form was created but not found in list")));
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Forms plugin loads correctly")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Runtime services:"), Array->from(OR((runtime->services->keys()), (array<any>()))));
            console->log(std::string("Runtime actions:"), OR((runtime->actions->map([=](auto a) mutable
            {
                return a["name"];
            }
            )), (array<any>())));
            console->log(std::string("Runtime providers:"), OR((runtime->providers->map([=](auto p) mutable
            {
                return p["name"];
            }
            )), (array<any>())));
            auto formsService = runtime->getService(std::string("forms"));
            if (!formsService) {
                throw any(std::make_shared<Error>(std::string("Forms service not registered")));
            }
            console->log(std::string("✓ Forms service registered successfully"));
            auto createFormAction = runtime->actions->find([=](auto a) mutable
            {
                return a["name"] == std::string("CREATE_FORM");
            }
            );
            auto updateFormAction = runtime->actions->find([=](auto a) mutable
            {
                return a["name"] == std::string("UPDATE_FORM");
            }
            );
            auto cancelFormAction = runtime->actions->find([=](auto a) mutable
            {
                return a["name"] == std::string("CANCEL_FORM");
            }
            );
            if (!createFormAction) {
                throw any(std::make_shared<Error>(std::string("CREATE_FORM action not found")));
            }
            if (!updateFormAction) {
                throw any(std::make_shared<Error>(std::string("UPDATE_FORM action not found")));
            }
            if (!cancelFormAction) {
                throw any(std::make_shared<Error>(std::string("CANCEL_FORM action not found")));
            }
            console->log(std::string("✓ All forms actions registered"));
            auto formsProvider = runtime->providers->find([=](auto p) mutable
            {
                return p["name"] == std::string("FORMS_CONTEXT");
            }
            );
            if (!formsProvider) {
                throw any(std::make_shared<Error>(std::string("Forms provider not found")));
            }
            console->log(std::string("✓ Forms provider registered"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Create and complete a simple contact form")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            setupModelHandlers(runtime);
            auto roomId = asUUID(uuidv4());
            auto userId = asUUID(uuidv4());
            auto formsService = as<std::shared_ptr<FormsService>>(runtime->getService(std::string("forms")));
            if (!formsService) {
                throw any(std::make_shared<Error>(std::string("Forms service not found")));
            }
            console->log(std::string("Creating form via service..."));
            auto form = std::async([=]() { formsService->createForm(std::string("contact")); });
            console->log(std::string("Form created:"), form->id, form->name, form->status);
            auto activeForms = std::async([=]() { formsService->listForms(std::string("active")); });
            console->log(std::string("Active forms:"), activeForms->get_length());
            if (activeForms->get_length() == 0) {
                throw any(std::make_shared<Error>(std::string("No active forms found after creation")));
            }
            auto createFormAction = runtime->actions->find([=](auto a) mutable
            {
                return a["name"] == std::string("CREATE_FORM");
            }
            );
            if (!createFormAction) {
                throw any(std::make_shared<Error>(std::string("CREATE_FORM action not found")));
            }
            if (!form) {
                throw any(std::make_shared<Error>(std::string("Created form not found")));
            }
            console->log(std::string("✓ Form created successfully:"), form->id);
            auto updateFormAction = runtime->actions->find([=](auto a) mutable
            {
                return a["name"] == std::string("UPDATE_FORM");
            }
            );
            if (!updateFormAction) {
                throw any(std::make_shared<Error>(std::string("UPDATE_FORM action not found")));
            }
            auto state = object{
                object::pair{std::string("values"), object{
                    object::pair{std::string("activeFormId"), form->id}
                }}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("activeFormId"), form->id}
                }}, 
                object::pair{std::string("text"), string_empty}
            };
            auto updateMessage1 = object{
                object::pair{std::string("entityId"), userId}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("My name is John Doe")}, 
                    object::pair{std::string("source"), std::string("test")}
                }}, 
                object::pair{std::string("createdAt"), Date->now() + 1000}
            };
            std::async([=]() { updateFormAction->handler(runtime, updateMessage1, state, object{}, [=](auto response) mutable
            {
                console->log(std::string("Action response:"), response);
                return array<any>();
            }
            ); });
            auto updateMessage2 = object{
                object::pair{std::string("entityId"), userId}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("My email is john@example.com")}, 
                    object::pair{std::string("source"), std::string("test")}
                }}, 
                object::pair{std::string("createdAt"), Date->now() + 2000}
            };
            std::async([=]() { updateFormAction->handler(runtime, updateMessage2, state, object{}, [=](auto response) mutable
            {
                console->log(std::string("Action response:"), response);
                return array<any>();
            }
            ); });
            auto updateMessage3 = object{
                object::pair{std::string("entityId"), userId}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("I would like to learn more about your consulting services")}, 
                    object::pair{std::string("source"), std::string("test")}
                }}, 
                object::pair{std::string("createdAt"), Date->now() + 7777}
            };
            std::async([=]() { updateFormAction->handler(runtime, updateMessage3, state, object{}, [=](auto response) mutable
            {
                console->log(std::string("Action response:"), response);
                return array<any>();
            }
            ); });
            auto completedForms = std::async([=]() { formsService->listForms(std::string("completed")); });
            if (completedForms->get_length() == 0) {
                throw any(std::make_shared<Error>(std::string("Form was not marked as completed")));
            }
            auto completedForm = const_(completedForms)[0];
            console->log(std::string("✓ Form completed successfully"));
            auto nameField = const_(completedForm->steps)[0]->fields->find([=](auto f) mutable
            {
                return f->id == std::string("name");
            }
            );
            auto emailField = const_(completedForm->steps)[0]->fields->find([=](auto f) mutable
            {
                return f->id == std::string("email");
            }
            );
            auto messageField = const_(completedForm->steps)[0]->fields->find([=](auto f) mutable
            {
                return f->id == std::string("message");
            }
            );
            if (nameField->value != std::string("John Doe")) {
                throw any(std::make_shared<Error>(std::string("Name field not updated correctly")));
            }
            if (emailField->value != std::string("john@example.com")) {
                throw any(std::make_shared<Error>(std::string("Email field not updated correctly")));
            }
            if (AND((AND((messageField), (messageField->value))), (type_of(messageField->value) == std::string("string")))) {
                if (!messageField->value->includes(std::string("services"))) {
                    throw any(std::make_shared<Error>(std::string("Message field not updated correctly")));
                }
            }
            console->log(std::string("✓ All form fields updated correctly"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Cancel an active form")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            setupModelHandlers(runtime);
            auto roomId = asUUID(uuidv4());
            auto userId = asUUID(uuidv4());
            auto createFormAction = runtime->actions->find([=](auto a) mutable
            {
                return a["name"] == std::string("CREATE_FORM");
            }
            );
            auto cancelFormAction = runtime->actions->find([=](auto a) mutable
            {
                return a["name"] == std::string("CANCEL_FORM");
            }
            );
            if (OR((!createFormAction), (!cancelFormAction))) {
                throw any(std::make_shared<Error>(std::string("Required actions not found")));
            }
            auto createMessage = object{
                object::pair{std::string("entityId"), userId}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Create a contact form")}, 
                    object::pair{std::string("source"), std::string("test")}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            shared<any> createdFormId;
            auto callback = [=](auto response) mutable
            {
                if (response["data"]["formId"]) {
                    createdFormId = response["data"]["formId"];
                }
                return array<any>();
            };
            auto state = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}
            };
            std::async([=]() { createFormAction->handler(runtime, createMessage, state, object{}, callback); });
            if (!createdFormId) {
                throw any(std::make_shared<Error>(std::string("Form was not created")));
            }
            console->log(std::string("Created form to cancel:"), createdFormId);
            auto formsService = as<std::shared_ptr<FormsService>>(runtime->getService(std::string("forms")));
            auto activeForms = std::async([=]() { formsService->listForms(std::string("active")); });
            if (activeForms->get_length() == 0) {
                throw any(std::make_shared<Error>(std::string("No active forms found")));
            }
            auto cancelMessage = object{
                object::pair{std::string("entityId"), userId}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Cancel the form")}, 
                    object::pair{std::string("source"), std::string("test")}
                }}, 
                object::pair{std::string("createdAt"), Date->now() + 1000}
            };
            std::async([=]() { cancelFormAction->handler(runtime, cancelMessage, object{
                object::pair{std::string("values"), object{
                    object::pair{std::string("activeFormId"), createdFormId}
                }}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("activeFormId"), createdFormId}
                }}, 
                object::pair{std::string("text"), string_empty}
            }, object{
                object::pair{std::string("formId"), createdFormId}
            }, callback); });
            auto cancelledForms = std::async([=]() { formsService->listForms(std::string("cancelled")); });
            console->log(std::string("Cancelled forms:"), cancelledForms->get_length());
            if (cancelledForms->get_length() == 0) {
                throw any(std::make_shared<Error>(std::string("Form was not cancelled")));
            }
            auto activeFormsAfter = std::async([=]() { formsService->listForms(std::string("active")); });
            console->log(std::string("Active forms after cancel:"), activeFormsAfter->get_length());
            activeFormsAfter->forEach([=](auto f) mutable
            {
                console->log(std::string("Still active:"), f->id, f->name);
            }
            );
            auto targetFormStillActive = activeFormsAfter->find([=](auto f) mutable
            {
                return f->id == createdFormId;
            }
            );
            if (targetFormStillActive) {
                throw any(std::make_shared<Error>(std::string("Form still active after cancellation")));
            }
            console->log(std::string("✓ Form cancelled successfully"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Forms provider shows active forms")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            setupModelHandlers(runtime);
            auto roomId = asUUID(uuidv4());
            auto userId = asUUID(uuidv4());
            auto createFormAction = runtime->actions->find([=](auto a) mutable
            {
                return a["name"] == std::string("CREATE_FORM");
            }
            );
            if (!createFormAction) {
                throw any(std::make_shared<Error>(std::string("CREATE_FORM action not found")));
            }
            auto createMessage = object{
                object::pair{std::string("entityId"), userId}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Create a new contact form")}, 
                    object::pair{std::string("source"), std::string("test")}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            shared<any> createdFormId;
            auto callback = [=](auto response) mutable
            {
                if (response["data"]["formId"]) {
                    createdFormId = response["data"]["formId"];
                }
                return array<any>();
            };
            auto state = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}
            };
            std::async([=]() { createFormAction->handler(runtime, createMessage, state, object{}, callback); });
            if (!createdFormId) {
                throw any(std::make_shared<Error>(std::string("Form was not created")));
            }
            auto formsProvider = runtime->providers->find([=](auto p) mutable
            {
                return p["name"] == std::string("FORMS_CONTEXT");
            }
            );
            if (!formsProvider) {
                throw any(std::make_shared<Error>(std::string("Forms provider not found")));
            }
            auto mockMessage = object{
                object::pair{std::string("entityId"), userId}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("test")}, 
                    object::pair{std::string("source"), std::string("test")}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            auto mockState = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}
            };
            auto providerResult = std::async([=]() { formsProvider->get(runtime, mockMessage, mockState); });
            console->log(std::string("Provider result:"), object{
                object::pair{std::string("hasText"), !!providerResult->text}, 
                object::pair{std::string("textIncludes"), providerResult->text->includes(std::string("Active Form:"))}, 
                object::pair{std::string("data"), providerResult->data}
            });
            if (!providerResult->text->includes(std::string("Active Form:"))) {
                console->log(std::string("Provider text:"), providerResult->text);
                throw any(std::make_shared<Error>(std::string("Provider did not return active forms information")));
            }
            if (OR((!providerResult->data->forms), (providerResult->data->forms->length == 0))) {
                throw any(std::make_shared<Error>(std::string("Provider did not return active forms data")));
            }
            console->log(std::string("✓ Forms provider returns active forms"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Handle secret fields correctly")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            setupModelHandlers(runtime);
            auto formsService = as<std::shared_ptr<FormsService>>(runtime->getService(std::string("forms")));
            auto customForm = std::async([=]() { formsService->createForm(object{
                object::pair{std::string("name"), std::string("api-setup")}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("steps"), array<object>{ object{
                    object::pair{std::string("id"), std::string("credentials")}, 
                    object::pair{std::string("name"), std::string("API Credentials")}, 
                    object::pair{std::string("fields"), array<object>{ object{
                        object::pair{std::string("id"), std::string("apiKey")}, 
                        object::pair{std::string("label"), std::string("API Key")}, 
                        object::pair{std::string("type"), std::string("text")}, 
                        object::pair{std::string("description"), std::string("Your API key")}, 
                        object::pair{std::string("optional"), false}, 
                        object::pair{std::string("secret"), true}
                    }, object{
                        object::pair{std::string("id"), std::string("endpoint")}, 
                        object::pair{std::string("label"), std::string("API Endpoint")}, 
                        object::pair{std::string("type"), std::string("text")}, 
                        object::pair{std::string("description"), std::string("API endpoint URL")}, 
                        object::pair{std::string("optional"), false}
                    } }}
                } }}
            }); });
            console->log(std::string("✓ Created form with secret field"));
            auto allForms = std::async([=]() { formsService->listForms(); });
            console->log(std::string("All forms after creation:"), allForms->map([=](auto f) mutable
            {
                return (object{
                    object::pair{std::string("id"), f->id}, 
                    object::pair{std::string("name"), f->name}, 
                    object::pair{std::string("status"), f->status}
                });
            }
            ));
            auto apiForm = std::async([=]() { formsService->getForm(customForm->id); });
            console->log(std::string("API form status:"), apiForm->status, std::string("fields:"), const_(apiForm->steps)[0]->fields->map([=](auto f) mutable
            {
                return (object{
                    object::pair{std::string("id"), f->id}, 
                    object::pair{std::string("value"), f->value}
                });
            }
            ));
            auto roomId = asUUID(uuidv4());
            auto userId = asUUID(uuidv4());
            auto updateMessage = object{
                object::pair{std::string("id"), asUUID(uuidv4())}, 
                object::pair{std::string("entityId"), userId}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("My API key is sk-12345 and the endpoint is https://api.example.com")}, 
                    object::pair{std::string("source"), std::string("test")}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            auto updateResult = std::async([=]() { formsService->updateForm(customForm->id, updateMessage); });
            if (!updateResult->success) {
                throw any(std::make_shared<Error>(std::string("Failed to update form with secret")));
            }
            console->log(std::string("✓ Form updated with secret values"));
            auto updatedForm = std::async([=]() { formsService->getForm(customForm->id); });
            console->log(std::string("Updated form status:"), updatedForm->status);
            console->log(std::string("Updated form fields:"), const_(updatedForm->steps)[0]->fields->map([=](auto f) mutable
            {
                return (object{
                    object::pair{std::string("id"), f->id}, 
                    object::pair{std::string("value"), f->value}, 
                    object::pair{std::string("hasValue"), f->value != undefined}
                });
            }
            ));
            auto completedForms = std::async([=]() { formsService->listForms(std::string("completed")); });
            console->log(std::string("Completed forms:"), completedForms->map([=](auto f) mutable
            {
                return (object{
                    object::pair{std::string("id"), f->id}, 
                    object::pair{std::string("name"), f->name}
                });
            }
            ));
            auto completedApiForm = completedForms->find([=](auto f) mutable
            {
                return f->name == std::string("api-setup");
            }
            );
            if (!completedApiForm) {
                throw any(std::make_shared<Error>(std::string("api-setup form not found in completed forms")));
            }
            auto secretField = const_(completedApiForm->steps)[0]->fields->find([=](auto f) mutable
            {
                return f["id"] == std::string("apiKey");
            }
            );
            if (OR((!secretField), (!secretField->value))) {
                throw any(std::make_shared<Error>(std::string("Secret field was not set")));
            }
            if (!secretField->secret) {
                throw any(std::make_shared<Error>(std::string("API key field is not marked as secret")));
            }
            console->log(std::string("✓ Secret fields are properly masked"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Multi-step form progression")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            setupModelHandlers(runtime);
            auto formsService = as<std::shared_ptr<FormsService>>(runtime->getService(std::string("forms")));
            shared step1Completed = false;
            shared step2Completed = false;
            shared formCompleted = false;
            auto multiStepForm = std::async([=]() { formsService->createForm(object{
                object::pair{std::string("name"), std::string("project-setup")}, 
                object::pair{std::string("steps"), array<object>{ object{
                    object::pair{std::string("id"), std::string("basic-info")}, 
                    object::pair{std::string("name"), std::string("Basic Information")}, 
                    object::pair{std::string("fields"), array<object>{ object{
                        object::pair{std::string("id"), std::string("projectName")}, 
                        object::pair{std::string("label"), std::string("Project Name")}, 
                        object::pair{std::string("type"), std::string("text")}, 
                        object::pair{std::string("description"), std::string("Name of your project")}, 
                        object::pair{std::string("optional"), false}
                    }, object{
                        object::pair{std::string("id"), std::string("description")}, 
                        object::pair{std::string("label"), std::string("Description")}, 
                        object::pair{std::string("type"), std::string("textarea")}, 
                        object::pair{std::string("description"), std::string("Brief project description")}, 
                        object::pair{std::string("optional"), true}
                    } }}, 
                    object::pair{std::string("onComplete"), [=]() mutable
                    {
                        step1Completed = true;
                        console->log(std::string("✓ Step 1 callback executed"));
                    }
                    }
                }, object{
                    object::pair{std::string("id"), std::string("tech-stack")}, 
                    object::pair{std::string("name"), std::string("Technology Stack")}, 
                    object::pair{std::string("fields"), array<object>{ object{
                        object::pair{std::string("id"), std::string("language")}, 
                        object::pair{std::string("label"), std::string("Programming Language")}, 
                        object::pair{std::string("type"), std::string("choice")}, 
                        object::pair{std::string("description"), std::string("Primary programming language")}, 
                        object::pair{std::string("optional"), false}, 
                        object::pair{std::string("metadata"), object{
                            object::pair{std::string("choices"), array<string>{ std::string("TypeScript"), std::string("JavaScript"), std::string("Python") }}
                        }}
                    }, object{
                        object::pair{std::string("id"), std::string("framework")}, 
                        object::pair{std::string("label"), std::string("Framework")}, 
                        object::pair{std::string("type"), std::string("text")}, 
                        object::pair{std::string("description"), std::string("Framework to use")}, 
                        object::pair{std::string("optional"), false}
                    } }}, 
                    object::pair{std::string("onComplete"), [=]() mutable
                    {
                        step2Completed = true;
                        console->log(std::string("✓ Step 2 callback executed"));
                    }
                    }
                } }}, 
                object::pair{std::string("onComplete"), [=]() mutable
                {
                    formCompleted = true;
                    console->log(std::string("✓ Form completion callback executed"));
                }
                }
            }); });
            std::async([=]() { formsService->updateForm(multiStepForm->id, as<std::shared_ptr<Memory>>(object{
                object::pair{std::string("entityId"), asUUID(uuidv4())}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), asUUID(uuidv4())}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("The project name is MyAwesomeApp")}, 
                    object::pair{std::string("source"), std::string("test")}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            })); });
            if (!step1Completed) {
                throw any(std::make_shared<Error>(std::string("Step 1 callback was not executed")));
            }
            auto formAfterStep1 = std::async([=]() { formsService->getForm(multiStepForm->id); });
            if (OR((!formAfterStep1), (formAfterStep1->currentStepIndex != 1))) {
                throw any(std::make_shared<Error>(std::string("Form did not progress to step 2")));
            }
            std::async([=]() { formsService->updateForm(multiStepForm->id, as<std::shared_ptr<Memory>>(object{
                object::pair{std::string("entityId"), asUUID(uuidv4())}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), asUUID(uuidv4())}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("I want to use TypeScript with Next.js framework")}, 
                    object::pair{std::string("source"), std::string("test")}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            })); });
            if (!step2Completed) {
                throw any(std::make_shared<Error>(std::string("Step 2 callback was not executed")));
            }
            if (!formCompleted) {
                throw any(std::make_shared<Error>(std::string("Form completion callback was not executed")));
            }
            auto completedForm = std::async([=]() { formsService->getForm(multiStepForm->id); });
            if (OR((!completedForm), (completedForm->status != std::string("completed")))) {
                throw any(std::make_shared<Error>(std::string("Form was not marked as completed")));
            }
            console->log(std::string("✓ Multi-step form completed successfully"));
        }
        }
    } };
};

void setupModelHandlers(std::shared_ptr<IAgentRuntime> runtime);

#endif
