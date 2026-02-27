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
    string name = std:("forms-plugin-e2e");

    string description = std:("E2E tests for the forms plugin");

    array<object> tests = array<object>{ object{
        object::pair{std:("name"), std:("Basic service functionality")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Test runtime agent ID:"), runtime->agentId);
            auto formsService = as<std::shared_ptr<FormsService>>(runtime->getService(std:("forms")));
            if (!formsService) {
                throw any(std::make_shared<Error>(std:("Forms service not found")));
            }
            console->log(std:("Forms service found"));
            console->log(std:("Service runtime agent ID:"), (as<any>(formsService))["runtime"]["agentId"]);
            auto form = std::async([=]() { formsService->createForm(std:("contact")); });
            console->log(std:("Created form:"), object{
                object::pair{std:("id"), form->id}, 
                object::pair{std:("agentId"), form->agentId}, 
                object::pair{std:("name"), form->name}, 
                object::pair{std:("status"), form->status}
            });
            auto allForms = std::async([=]() { formsService->listForms(); });
            console->log(std:("All forms:"), allForms->get_length());
            allForms->forEach([=](auto f) mutable
            {
                console->log(std:("Form in list:"), object{
                    object::pair{std:("id"), f->id}, 
                    object::pair{std:("agentId"), f->agentId}, 
                    object::pair{std:("name"), f->name}, 
                    object::pair{std:("status"), f->status}
                });
            }
            );
            auto activeForms = std::async([=]() { formsService->listForms(std:("active")); });
            console->log(std:("Active forms:"), activeForms->get_length());
            if (activeForms->get_length() == 0) {
                throw any(std::make_shared<Error>(std:("Form was created but not found in list")));
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("Forms plugin loads correctly")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Runtime services:"), Array->from(OR((runtime->services->keys()), (array<any>()))));
            console->log(std:("Runtime actions:"), OR((runtime->actions->map([=](auto a) mutable
            {
                return a["name"];
            }
            )), (array<any>())));
            console->log(std:("Runtime providers:"), OR((runtime->providers->map([=](auto p) mutable
            {
                return p["name"];
            }
            )), (array<any>())));
            auto formsService = runtime->getService(std:("forms"));
            if (!formsService) {
                throw any(std::make_shared<Error>(std:("Forms service not registered")));
            }
            console->log(std:("✓ Forms service registered successfully"));
            auto createFormAction = runtime->actions->find([=](auto a) mutable
            {
                return a["name"] == std:("CREATE_FORM");
            }
            );
            auto updateFormAction = runtime->actions->find([=](auto a) mutable
            {
                return a["name"] == std:("UPDATE_FORM");
            }
            );
            auto cancelFormAction = runtime->actions->find([=](auto a) mutable
            {
                return a["name"] == std:("CANCEL_FORM");
            }
            );
            if (!createFormAction) {
                throw any(std::make_shared<Error>(std:("CREATE_FORM action not found")));
            }
            if (!updateFormAction) {
                throw any(std::make_shared<Error>(std:("UPDATE_FORM action not found")));
            }
            if (!cancelFormAction) {
                throw any(std::make_shared<Error>(std:("CANCEL_FORM action not found")));
            }
            console->log(std:("✓ All forms actions registered"));
            auto formsProvider = runtime->providers->find([=](auto p) mutable
            {
                return p["name"] == std:("FORMS_CONTEXT");
            }
            );
            if (!formsProvider) {
                throw any(std::make_shared<Error>(std:("Forms provider not found")));
            }
            console->log(std:("✓ Forms provider registered"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("Create and complete a simple contact form")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            setupModelHandlers(runtime);
            auto roomId = asUUID(uuidv4());
            auto userId = asUUID(uuidv4());
            auto formsService = as<std::shared_ptr<FormsService>>(runtime->getService(std:("forms")));
            if (!formsService) {
                throw any(std::make_shared<Error>(std:("Forms service not found")));
            }
            console->log(std:("Creating form via service..."));
            auto form = std::async([=]() { formsService->createForm(std:("contact")); });
            console->log(std:("Form created:"), form->id, form->name, form->status);
            auto activeForms = std::async([=]() { formsService->listForms(std:("active")); });
            console->log(std:("Active forms:"), activeForms->get_length());
            if (activeForms->get_length() == 0) {
                throw any(std::make_shared<Error>(std:("No active forms found after creation")));
            }
            auto createFormAction = runtime->actions->find([=](auto a) mutable
            {
                return a["name"] == std:("CREATE_FORM");
            }
            );
            if (!createFormAction) {
                throw any(std::make_shared<Error>(std:("CREATE_FORM action not found")));
            }
            if (!form) {
                throw any(std::make_shared<Error>(std:("Created form not found")));
            }
            console->log(std:("✓ Form created successfully:"), form->id);
            auto updateFormAction = runtime->actions->find([=](auto a) mutable
            {
                return a["name"] == std:("UPDATE_FORM");
            }
            );
            if (!updateFormAction) {
                throw any(std::make_shared<Error>(std:("UPDATE_FORM action not found")));
            }
            auto state = object{
                object::pair{std:("values"), object{
                    object::pair{std:("activeFormId"), form->id}
                }}, 
                object::pair{std:("data"), object{
                    object::pair{std:("activeFormId"), form->id}
                }}, 
                object::pair{std:("text"), string_empty}
            };
            auto updateMessage1 = object{
                object::pair{std:("entityId"), userId}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("My name is John Doe")}, 
                    object::pair{std:("source"), std:("test")}
                }}, 
                object::pair{std:("createdAt"), Date->now() + 1000}
            };
            std::async([=]() { updateFormAction->handler(runtime, updateMessage1, state, object{}, [=](auto response) mutable
            {
                console->log(std:("Action response:"), response);
                return array<any>();
            }
            ); });
            auto updateMessage2 = object{
                object::pair{std:("entityId"), userId}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("My email is john@example.com")}, 
                    object::pair{std:("source"), std:("test")}
                }}, 
                object::pair{std:("createdAt"), Date->now() + 2000}
            };
            std::async([=]() { updateFormAction->handler(runtime, updateMessage2, state, object{}, [=](auto response) mutable
            {
                console->log(std:("Action response:"), response);
                return array<any>();
            }
            ); });
            auto updateMessage3 = object{
                object::pair{std:("entityId"), userId}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("I would like to learn more about your consulting services")}, 
                    object::pair{std:("source"), std:("test")}
                }}, 
                object::pair{std:("createdAt"), Date->now() + 7777}
            };
            std::async([=]() { updateFormAction->handler(runtime, updateMessage3, state, object{}, [=](auto response) mutable
            {
                console->log(std:("Action response:"), response);
                return array<any>();
            }
            ); });
            auto completedForms = std::async([=]() { formsService->listForms(std:("completed")); });
            if (completedForms->get_length() == 0) {
                throw any(std::make_shared<Error>(std:("Form was not marked as completed")));
            }
            auto completedForm = const_(completedForms)[0];
            console->log(std:("✓ Form completed successfully"));
            auto nameField = const_(completedForm->steps)[0]->fields->find([=](auto f) mutable
            {
                return f->id == std:("name");
            }
            );
            auto emailField = const_(completedForm->steps)[0]->fields->find([=](auto f) mutable
            {
                return f->id == std:("email");
            }
            );
            auto messageField = const_(completedForm->steps)[0]->fields->find([=](auto f) mutable
            {
                return f->id == std:("message");
            }
            );
            if (nameField->value != std:("John Doe")) {
                throw any(std::make_shared<Error>(std:("Name field not updated correctly")));
            }
            if (emailField->value != std:("john@example.com")) {
                throw any(std::make_shared<Error>(std:("Email field not updated correctly")));
            }
            if (AND((AND((messageField), (messageField->value))), (type_of(messageField->value) == std:("string")))) {
                if (!messageField->value->includes(std:("services"))) {
                    throw any(std::make_shared<Error>(std:("Message field not updated correctly")));
                }
            }
            console->log(std:("✓ All form fields updated correctly"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("Cancel an active form")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            setupModelHandlers(runtime);
            auto roomId = asUUID(uuidv4());
            auto userId = asUUID(uuidv4());
            auto createFormAction = runtime->actions->find([=](auto a) mutable
            {
                return a["name"] == std:("CREATE_FORM");
            }
            );
            auto cancelFormAction = runtime->actions->find([=](auto a) mutable
            {
                return a["name"] == std:("CANCEL_FORM");
            }
            );
            if (OR((!createFormAction), (!cancelFormAction))) {
                throw any(std::make_shared<Error>(std:("Required actions not found")));
            }
            auto createMessage = object{
                object::pair{std:("entityId"), userId}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Create a contact form")}, 
                    object::pair{std:("source"), std:("test")}
                }}, 
                object::pair{std:("createdAt"), Date->now()}
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
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
            std::async([=]() { createFormAction->handler(runtime, createMessage, state, object{}, callback); });
            if (!createdFormId) {
                throw any(std::make_shared<Error>(std:("Form was not created")));
            }
            console->log(std:("Created form to cancel:"), createdFormId);
            auto formsService = as<std::shared_ptr<FormsService>>(runtime->getService(std:("forms")));
            auto activeForms = std::async([=]() { formsService->listForms(std:("active")); });
            if (activeForms->get_length() == 0) {
                throw any(std::make_shared<Error>(std:("No active forms found")));
            }
            auto cancelMessage = object{
                object::pair{std:("entityId"), userId}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Cancel the form")}, 
                    object::pair{std:("source"), std:("test")}
                }}, 
                object::pair{std:("createdAt"), Date->now() + 1000}
            };
            std::async([=]() { cancelFormAction->handler(runtime, cancelMessage, object{
                object::pair{std:("values"), object{
                    object::pair{std:("activeFormId"), createdFormId}
                }}, 
                object::pair{std:("data"), object{
                    object::pair{std:("activeFormId"), createdFormId}
                }}, 
                object::pair{std:("text"), string_empty}
            }, object{
                object::pair{std:("formId"), createdFormId}
            }, callback); });
            auto cancelledForms = std::async([=]() { formsService->listForms(std:("cancelled")); });
            console->log(std:("Cancelled forms:"), cancelledForms->get_length());
            if (cancelledForms->get_length() == 0) {
                throw any(std::make_shared<Error>(std:("Form was not cancelled")));
            }
            auto activeFormsAfter = std::async([=]() { formsService->listForms(std:("active")); });
            console->log(std:("Active forms after cancel:"), activeFormsAfter->get_length());
            activeFormsAfter->forEach([=](auto f) mutable
            {
                console->log(std:("Still active:"), f->id, f->name);
            }
            );
            auto targetFormStillActive = activeFormsAfter->find([=](auto f) mutable
            {
                return f->id == createdFormId;
            }
            );
            if (targetFormStillActive) {
                throw any(std::make_shared<Error>(std:("Form still active after cancellation")));
            }
            console->log(std:("✓ Form cancelled successfully"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("Forms provider shows active forms")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            setupModelHandlers(runtime);
            auto roomId = asUUID(uuidv4());
            auto userId = asUUID(uuidv4());
            auto createFormAction = runtime->actions->find([=](auto a) mutable
            {
                return a["name"] == std:("CREATE_FORM");
            }
            );
            if (!createFormAction) {
                throw any(std::make_shared<Error>(std:("CREATE_FORM action not found")));
            }
            auto createMessage = object{
                object::pair{std:("entityId"), userId}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Create a new contact form")}, 
                    object::pair{std:("source"), std:("test")}
                }}, 
                object::pair{std:("createdAt"), Date->now()}
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
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
            std::async([=]() { createFormAction->handler(runtime, createMessage, state, object{}, callback); });
            if (!createdFormId) {
                throw any(std::make_shared<Error>(std:("Form was not created")));
            }
            auto formsProvider = runtime->providers->find([=](auto p) mutable
            {
                return p["name"] == std:("FORMS_CONTEXT");
            }
            );
            if (!formsProvider) {
                throw any(std::make_shared<Error>(std:("Forms provider not found")));
            }
            auto mockMessage = object{
                object::pair{std:("entityId"), userId}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("test")}, 
                    object::pair{std:("source"), std:("test")}
                }}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            auto mockState = object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
            auto providerResult = std::async([=]() { formsProvider->get(runtime, mockMessage, mockState); });
            console->log(std:("Provider result:"), object{
                object::pair{std:("hasText"), !!providerResult->text}, 
                object::pair{std:("textIncludes"), providerResult->text->includes(std:("Active Form:"))}, 
                object::pair{std:("data"), providerResult->data}
            });
            if (!providerResult->text->includes(std:("Active Form:"))) {
                console->log(std:("Provider text:"), providerResult->text);
                throw any(std::make_shared<Error>(std:("Provider did not return active forms information")));
            }
            if (OR((!providerResult->data->forms), (providerResult->data->forms->length == 0))) {
                throw any(std::make_shared<Error>(std:("Provider did not return active forms data")));
            }
            console->log(std:("✓ Forms provider returns active forms"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("Handle secret fields correctly")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            setupModelHandlers(runtime);
            auto formsService = as<std::shared_ptr<FormsService>>(runtime->getService(std:("forms")));
            auto customForm = std::async([=]() { formsService->createForm(object{
                object::pair{std:("name"), std:("api-setup")}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("steps"), array<object>{ object{
                    object::pair{std:("id"), std:("credentials")}, 
                    object::pair{std:("name"), std:("API Credentials")}, 
                    object::pair{std:("fields"), array<object>{ object{
                        object::pair{std:("id"), std:("apiKey")}, 
                        object::pair{std:("label"), std:("API Key")}, 
                        object::pair{std:("type"), std:("text")}, 
                        object::pair{std:("description"), std:("Your API key")}, 
                        object::pair{std:("optional"), false}, 
                        object::pair{std:("secret"), true}
                    }, object{
                        object::pair{std:("id"), std:("endpoint")}, 
                        object::pair{std:("label"), std:("API Endpoint")}, 
                        object::pair{std:("type"), std:("text")}, 
                        object::pair{std:("description"), std:("API endpoint URL")}, 
                        object::pair{std:("optional"), false}
                    } }}
                } }}
            }); });
            console->log(std:("✓ Created form with secret field"));
            auto allForms = std::async([=]() { formsService->listForms(); });
            console->log(std:("All forms after creation:"), allForms->map([=](auto f) mutable
            {
                return (object{
                    object::pair{std:("id"), f->id}, 
                    object::pair{std:("name"), f->name}, 
                    object::pair{std:("status"), f->status}
                });
            }
            ));
            auto apiForm = std::async([=]() { formsService->getForm(customForm->id); });
            console->log(std:("API form status:"), apiForm->status, std:("fields:"), const_(apiForm->steps)[0]->fields->map([=](auto f) mutable
            {
                return (object{
                    object::pair{std:("id"), f->id}, 
                    object::pair{std:("value"), f->value}
                });
            }
            ));
            auto roomId = asUUID(uuidv4());
            auto userId = asUUID(uuidv4());
            auto updateMessage = object{
                object::pair{std:("id"), asUUID(uuidv4())}, 
                object::pair{std:("entityId"), userId}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("My API key is sk-12345 and the endpoint is https://api.example.com")}, 
                    object::pair{std:("source"), std:("test")}
                }}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            auto updateResult = std::async([=]() { formsService->updateForm(customForm->id, updateMessage); });
            if (!updateResult->success) {
                throw any(std::make_shared<Error>(std:("Failed to update form with secret")));
            }
            console->log(std:("✓ Form updated with secret values"));
            auto updatedForm = std::async([=]() { formsService->getForm(customForm->id); });
            console->log(std:("Updated form status:"), updatedForm->status);
            console->log(std:("Updated form fields:"), const_(updatedForm->steps)[0]->fields->map([=](auto f) mutable
            {
                return (object{
                    object::pair{std:("id"), f->id}, 
                    object::pair{std:("value"), f->value}, 
                    object::pair{std:("hasValue"), f->value != undefined}
                });
            }
            ));
            auto completedForms = std::async([=]() { formsService->listForms(std:("completed")); });
            console->log(std:("Completed forms:"), completedForms->map([=](auto f) mutable
            {
                return (object{
                    object::pair{std:("id"), f->id}, 
                    object::pair{std:("name"), f->name}
                });
            }
            ));
            auto completedApiForm = completedForms->find([=](auto f) mutable
            {
                return f->name == std:("api-setup");
            }
            );
            if (!completedApiForm) {
                throw any(std::make_shared<Error>(std:("api-setup form not found in completed forms")));
            }
            auto secretField = const_(completedApiForm->steps)[0]->fields->find([=](auto f) mutable
            {
                return f["id"] == std:("apiKey");
            }
            );
            if (OR((!secretField), (!secretField->value))) {
                throw any(std::make_shared<Error>(std:("Secret field was not set")));
            }
            if (!secretField->secret) {
                throw any(std::make_shared<Error>(std:("API key field is not marked as secret")));
            }
            console->log(std:("✓ Secret fields are properly masked"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("Multi-step form progression")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            setupModelHandlers(runtime);
            auto formsService = as<std::shared_ptr<FormsService>>(runtime->getService(std:("forms")));
            shared step1Completed = false;
            shared step2Completed = false;
            shared formCompleted = false;
            auto multiStepForm = std::async([=]() { formsService->createForm(object{
                object::pair{std:("name"), std:("project-setup")}, 
                object::pair{std:("steps"), array<object>{ object{
                    object::pair{std:("id"), std:("basic-info")}, 
                    object::pair{std:("name"), std:("Basic Information")}, 
                    object::pair{std:("fields"), array<object>{ object{
                        object::pair{std:("id"), std:("projectName")}, 
                        object::pair{std:("label"), std:("Project Name")}, 
                        object::pair{std:("type"), std:("text")}, 
                        object::pair{std:("description"), std:("Name of your project")}, 
                        object::pair{std:("optional"), false}
                    }, object{
                        object::pair{std:("id"), std:("description")}, 
                        object::pair{std:("label"), std:("Description")}, 
                        object::pair{std:("type"), std:("textarea")}, 
                        object::pair{std:("description"), std:("Brief project description")}, 
                        object::pair{std:("optional"), true}
                    } }}, 
                    object::pair{std:("onComplete"), [=]() mutable
                    {
                        step1Completed = true;
                        console->log(std:("✓ Step 1 callback executed"));
                    }
                    }
                }, object{
                    object::pair{std:("id"), std:("tech-stack")}, 
                    object::pair{std:("name"), std:("Technology Stack")}, 
                    object::pair{std:("fields"), array<object>{ object{
                        object::pair{std:("id"), std:("language")}, 
                        object::pair{std:("label"), std:("Programming Language")}, 
                        object::pair{std:("type"), std:("choice")}, 
                        object::pair{std:("description"), std:("Primary programming language")}, 
                        object::pair{std:("optional"), false}, 
                        object::pair{std:("metadata"), object{
                            object::pair{std:("choices"), array<string>{ std:("TypeScript"), std:("JavaScript"), std:("Python") }}
                        }}
                    }, object{
                        object::pair{std:("id"), std:("framework")}, 
                        object::pair{std:("label"), std:("Framework")}, 
                        object::pair{std:("type"), std:("text")}, 
                        object::pair{std:("description"), std:("Framework to use")}, 
                        object::pair{std:("optional"), false}
                    } }}, 
                    object::pair{std:("onComplete"), [=]() mutable
                    {
                        step2Completed = true;
                        console->log(std:("✓ Step 2 callback executed"));
                    }
                    }
                } }}, 
                object::pair{std:("onComplete"), [=]() mutable
                {
                    formCompleted = true;
                    console->log(std:("✓ Form completion callback executed"));
                }
                }
            }); });
            std::async([=]() { formsService->updateForm(multiStepForm->id, as<std::shared_ptr<Memory>>(object{
                object::pair{std:("entityId"), asUUID(uuidv4())}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), asUUID(uuidv4())}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("The project name is MyAwesomeApp")}, 
                    object::pair{std:("source"), std:("test")}
                }}, 
                object::pair{std:("createdAt"), Date->now()}
            })); });
            if (!step1Completed) {
                throw any(std::make_shared<Error>(std:("Step 1 callback was not executed")));
            }
            auto formAfterStep1 = std::async([=]() { formsService->getForm(multiStepForm->id); });
            if (OR((!formAfterStep1), (formAfterStep1->currentStepIndex != 1))) {
                throw any(std::make_shared<Error>(std:("Form did not progress to step 2")));
            }
            std::async([=]() { formsService->updateForm(multiStepForm->id, as<std::shared_ptr<Memory>>(object{
                object::pair{std:("entityId"), asUUID(uuidv4())}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), asUUID(uuidv4())}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("I want to use TypeScript with Next.js framework")}, 
                    object::pair{std:("source"), std:("test")}
                }}, 
                object::pair{std:("createdAt"), Date->now()}
            })); });
            if (!step2Completed) {
                throw any(std::make_shared<Error>(std:("Step 2 callback was not executed")));
            }
            if (!formCompleted) {
                throw any(std::make_shared<Error>(std:("Form completion callback was not executed")));
            }
            auto completedForm = std::async([=]() { formsService->getForm(multiStepForm->id); });
            if (OR((!completedForm), (completedForm->status != std:("completed")))) {
                throw any(std::make_shared<Error>(std:("Form was not marked as completed")));
            }
            console->log(std:("✓ Multi-step form completed successfully"));
        }
        }
    } };
};

void setupModelHandlers(std::shared_ptr<IAgentRuntime> runtime);

#endif
