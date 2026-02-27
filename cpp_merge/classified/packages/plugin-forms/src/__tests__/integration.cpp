#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-forms/src/__tests__/integration.test.h"

any createMockRuntime()
{
    shared services = std::make_shared<Map<string, any>>();
    auto mockRuntime = object{
        object::pair{std:("agentId"), asUUID(uuidv4())}, 
        object::pair{std:("character"), object{
            object::pair{std:("name"), std:("TestAgent")}, 
            object::pair{std:("bio"), array<string>{ std:("Test agent bio") }}, 
            object::pair{std:("system"), std:("You are a test agent")}, 
            object::pair{std:("plugins"), array<string>{ std:("forms") }}
        }}, 
        object::pair{std:("actions"), array<any>{ createFormAction, updateFormAction, cancelFormAction }}, 
        object::pair{std:("providers"), array<any>{ formsProvider }}, 
        object::pair{std:("logger"), object{
            object::pair{std:("info"), mock([=]() mutable
            {
            }
            )}, 
            object::pair{std:("error"), mock([=]() mutable
            {
            }
            )}, 
            object::pair{std:("warn"), mock([=]() mutable
            {
            }
            )}, 
            object::pair{std:("debug"), mock([=]() mutable
            {
            }
            )}
        }}, 
        object::pair{std:("useModel"), mock([=]() mutable
        {
            return Promise->resolve(std:("{"name": "Test User"}"));
        }
        )}, 
        object::pair{std:("getService"), [=](auto name) mutable
        {
            return services->get(name);
        }
        }, 
        object::pair{std:("registerService"), [=](auto name, auto service) mutable
        {
            return services->set(name, service);
        }
        }
    };
    return as<any>(as<any>(mockRuntime));
};


std::function<any(string)> createMockMemory = [=](auto text) mutable
{
    return (object{
        object::pair{std:("id"), asUUID(uuidv4())}, 
        object::pair{std:("entityId"), asUUID(uuidv4())}, 
        object::pair{std:("roomId"), asUUID(uuidv4())}, 
        object::pair{std:("agentId"), asUUID(uuidv4())}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("text")}, 
            object::pair{std:("source"), std:("test")}
        }}, 
        object::pair{std:("createdAt"), Date->now()}
    });
};
std::function<any()> createMockState = [=]() mutable
{
    return (object{
        object::pair{std:("values"), object{}}, 
        object::pair{std:("data"), object{}}, 
        object::pair{std:("text"), string_empty}
    });
};

void Main(void)
{
    describe(std:("Forms Plugin Integration Tests"), [=]() mutable
    {
        shared<any> mockRuntime;
        shared<std::shared_ptr<FormsService>> formsService;
        beforeEach([=]() mutable
        {
            mockRuntime = createMockRuntime();
            formsService = as<std::shared_ptr<FormsService>>((std::async([=]() { FormsService::start(mockRuntime); })));
            (as<object>(as<any>(mockRuntime)))["registerService"](std:("forms"), formsService);
        }
        );
        describe(std:("Form creation through action"), [=]() mutable
        {
            test(std:("should handle CREATE_FORM action"), [=]() mutable
            {
                auto message = createMockMemory(std:("I need to create a contact form"));
                auto state = createMockState();
                shared responseReceived = false;
                shared responseText = string_empty;
                auto callback = [=](auto response) mutable
                {
                    responseReceived = true;
                    responseText = OR((response->text), (string_empty));
                    return array<any>();
                };
                auto isValid = std::async([=]() { createFormAction->validate(mockRuntime, message, state); });
                expect(isValid)->toBe(true);
                std::async([=]() { createFormAction->handler(mockRuntime, message, state, object{}, callback); });
                expect(responseReceived)->toBe(true);
                expect(responseText)->toContain(std:("created"));
                expect(responseText->toLowerCase())->toContain(std:("contact"));
                auto forms = std::async([=]() { formsService->listForms(std:("active")); });
                expect(forms)->toHaveLength(1);
                expect(const_(forms)[0]->name)->toBe(std:("contact"));
            }
            );
            test(std:("should handle form creation with provider context"), [=]() mutable
            {
                auto form = std::async([=]() { formsService->createForm(std:("contact")); });
                auto message = createMockMemory(std:("test"));
                auto state = createMockState();
                auto providerResult = std::async([=]() { formsProvider->get(mockRuntime, message, state); });
                expect(providerResult->text)->toContain(std:("[FORMS]"));
                expect(providerResult->text)->toContain(std:("contact"));
                expect(providerResult->data->forms)->toHaveLength(1);
                expect(const_(providerResult->data->forms)[0]->id)->toBe(form->id);
            }
            );
        }
        );
        describe(std:("Form updates through action"), [=]() mutable
        {
            shared<std::shared_ptr<Awaited<std::shared_ptr<ReturnType<any>>>>> testForm;
            beforeEach([=]() mutable
            {
                testForm = std::async([=]() { formsService->createForm(std:("contact")); });
            }
            );
            test(std:("should handle UPDATE_FORM action"), [=]() mutable
            {
                auto message = createMockMemory(std:("My name is John Doe"));
                auto state = createMockState();
                shared responseReceived = false;
                shared responseText = string_empty;
                auto callback = [=](auto response) mutable
                {
                    responseReceived = true;
                    responseText = OR((response->text), (string_empty));
                    return array<any>();
                };
                mockRuntime["useModel"]["mockResolvedValueOnce"](std:("{"name": "John Doe"}"));
                auto isValid = std::async([=]() { updateFormAction->validate(mockRuntime, message, state); });
                expect(isValid)->toBe(true);
                std::async([=]() { updateFormAction->handler(mockRuntime, message, state, object{}, callback); });
                expect(responseReceived)->toBe(true);
                expect(responseText)->toContain(std:("Updated"));
                auto updatedForm = std::async([=]() { formsService->getForm(testForm->id); });
                auto nameField = const_(updatedForm->steps)[0]->fields->find([=](auto f) mutable
                {
                    return f->id == std:("name");
                }
                );
                expect(nameField->value)->toBe(std:("John Doe"));
            }
            );
            test(std:("should complete form when all fields are filled"), [=]() mutable
            {
                auto messages = array<string>{ std:("My name is John Doe"), std:("My email is john@example.com") };
                shared completionMessage = string_empty;
                for (auto& text : messages)
                {
                    auto message = createMockMemory(text);
                    auto state = createMockState();
                    auto callback = [=](auto response) mutable
                    {
                        completionMessage = OR((response->text), (string_empty));
                        return array<any>();
                    };
                    if (text->includes(std:("name"))) {
                        mockRuntime["useModel"]["mockResolvedValueOnce"](std:("{"name": "John Doe"}"));
                    } else if (text->includes(std:("email"))) {
                        mockRuntime["useModel"]["mockResolvedValueOnce"](std:("{"email": "john@example.com"}"));
                    }
                    std::async([=]() { updateFormAction->handler(mockRuntime, message, state, object{}, callback); });
                }
                auto completedForm = std::async([=]() { formsService->getForm(testForm->id); });
                expect(completedForm->status)->toBe(std:("completed"));
                expect(completionMessage)->toContain(std:("completed"));
            }
            );
        }
        );
        describe(std:("Form cancellation through action"), [=]() mutable
        {
            shared<std::shared_ptr<Awaited<std::shared_ptr<ReturnType<any>>>>> testForm;
            beforeEach([=]() mutable
            {
                testForm = std::async([=]() { formsService->createForm(std:("contact")); });
            }
            );
            test(std:("should handle CANCEL_FORM action"), [=]() mutable
            {
                auto message = createMockMemory(std:("Cancel the form"));
                auto state = createMockState();
                shared responseReceived = false;
                shared responseText = string_empty;
                auto callback = [=](auto response) mutable
                {
                    responseReceived = true;
                    responseText = OR((response->text), (string_empty));
                    return array<any>();
                };
                auto isValid = std::async([=]() { cancelFormAction->validate(mockRuntime, message, state); });
                expect(isValid)->toBe(true);
                std::async([=]() { cancelFormAction->handler(mockRuntime, message, state, object{}, callback); });
                expect(responseReceived)->toBe(true);
                expect(responseText)->toContain(std:("cancelled"));
                auto cancelledForm = std::async([=]() { formsService->getForm(testForm->id); });
                expect(cancelledForm->status)->toBe(std:("cancelled"));
            }
            );
        }
        );
        describe(std:("Plugin initialization"), [=]() mutable
        {
            test(std:("should register all components correctly"), [=]() mutable
            {
                expect(formsPlugin->services)->toContain(FormsService);
                expect(formsPlugin->actions)->toContain(createFormAction);
                expect(formsPlugin->actions)->toContain(updateFormAction);
                expect(formsPlugin->actions)->toContain(cancelFormAction);
                expect(formsPlugin->providers)->toContain(formsProvider);
            }
            );
            test(std:("should have correct plugin metadata"), [=]() mutable
            {
                expect(formsPlugin->name)->toBe(std:("@elizaos/plugin-forms"));
                expect(formsPlugin->description)->toBeTruthy();
            }
            );
        }
        );
        describe(std:("Secret field handling in integration"), [=]() mutable
        {
            test(std:("should mask secret fields in provider output"), [=]() mutable
            {
                auto form = std::async([=]() { formsService->createForm(object{
                    object::pair{std:("name"), std:("api-config")}, 
                    object::pair{std:("agentId"), mockRuntime["agentId"]}, 
                    object::pair{std:("steps"), array<object>{ object{
                        object::pair{std:("id"), std:("credentials")}, 
                        object::pair{std:("name"), std:("API Credentials")}, 
                        object::pair{std:("fields"), array<object>{ object{
                            object::pair{std:("id"), std:("apiKey")}, 
                            object::pair{std:("label"), std:("API Key")}, 
                            object::pair{std:("type"), as<std::shared_ptr<const>>(std:("text"))}, 
                            object::pair{std:("secret"), true}
                        }, object{
                            object::pair{std:("id"), std:("endpoint")}, 
                            object::pair{std:("label"), std:("API Endpoint")}, 
                            object::pair{std:("type"), as<std::shared_ptr<const>>(std:("text"))}, 
                            object::pair{std:("description"), std:("The API endpoint URL")}
                        } }}
                    } }}
                }); });
                mockRuntime["useModel"]["mockResolvedValueOnce"](std:("{"apiKey": "sk-12345"}"));
                std::async([=]() { formsService->updateForm(form->id, createMockMemory(std:("My API key is sk-12345"))); });
                auto providerResult = std::async([=]() { formsProvider->get(mockRuntime, createMockMemory(std:("test")), createMockState()); });
                expect(providerResult->text)->toContain(std:("[SECRET]"));
                expect(providerResult->text)->not->toContain(std:("sk-12345"));
            }
            );
        }
        );
        describe(std:("Error handling in actions"), [=]() mutable
        {
            test(std:("should handle form service not available"), [=]() mutable
            {
                auto runtimeWithoutService = createMockRuntime();
                auto message = createMockMemory(std:("Create a contact form"));
                auto state = createMockState();
                auto isValid = std::async([=]() { createFormAction->validate(runtimeWithoutService, message, state); });
                expect(isValid)->toBe(false);
            }
            );
            test(std:("should handle LLM extraction errors gracefully"), [=]() mutable
            {
                auto message = createMockMemory(std:("My name is John and email is john@example.com"));
                auto state = createMockState();
                shared responseText = string_empty;
                auto callback = [=](auto response) mutable
                {
                    responseText = OR((response->text), (string_empty));
                    return array<any>();
                };
                mockRuntime["useModel"]["mockResolvedValueOnce"](std:("invalid json response"));
                std::async([=]() { updateFormAction->handler(mockRuntime, message, state, object{}, callback); });
                expect(responseText)->toContain(std:("No active forms to update"));
            }
            );
        }
        );
        describe(std:("Multi-step form workflow"), [=]() mutable
        {
            test(std:("should complete multi-step form through actions"), [=]() mutable
            {
                auto multiStepForm = std::async([=]() { formsService->createForm(object{
                    object::pair{std:("name"), std:("registration")}, 
                    object::pair{std:("agentId"), mockRuntime["agentId"]}, 
                    object::pair{std:("steps"), array<object>{ object{
                        object::pair{std:("id"), std:("personal")}, 
                        object::pair{std:("name"), std:("Personal Info")}, 
                        object::pair{std:("fields"), array<object>{ object{
                            object::pair{std:("id"), std:("name")}, 
                            object::pair{std:("label"), std:("Name")}, 
                            object::pair{std:("type"), as<std::shared_ptr<const>>(std:("text"))}
                        }, object{
                            object::pair{std:("id"), std:("age")}, 
                            object::pair{std:("label"), std:("Age")}, 
                            object::pair{std:("type"), as<std::shared_ptr<const>>(std:("number"))}
                        } }}
                    }, object{
                        object::pair{std:("id"), std:("contact")}, 
                        object::pair{std:("name"), std:("Contact Info")}, 
                        object::pair{std:("fields"), array<object>{ object{
                            object::pair{std:("id"), std:("email")}, 
                            object::pair{std:("label"), std:("Email")}, 
                            object::pair{std:("type"), as<std::shared_ptr<const>>(std:("email"))}
                        }, object{
                            object::pair{std:("id"), std:("phone")}, 
                            object::pair{std:("label"), std:("Phone")}, 
                            object::pair{std:("type"), as<std::shared_ptr<const>>(std:("tel"))}, 
                            object::pair{std:("optional"), true}
                        } }}
                    } }}
                }); });
                shared lastResponse = string_empty;
                auto callback = [=](auto response) mutable
                {
                    lastResponse = OR((response->text), (string_empty));
                    return array<any>();
                };
                mockRuntime["useModel"]["mockResolvedValueOnce"](std:("{"name": "Alice", "age": 30}"));
                std::async([=]() { updateFormAction->handler(mockRuntime, createMockMemory(std:("My name is Alice and I am 30 years old")), createMockState(), object{}, callback); });
                expect(lastResponse)->toContain(std:("Personal Info"));
                expect(lastResponse)->toContain(std:("completed"));
                expect(lastResponse)->toContain(std:("Contact Info"));
                mockRuntime["useModel"]["mockResolvedValueOnce"](std:("{"email": "alice@example.com"}"));
                std::async([=]() { updateFormAction->handler(mockRuntime, createMockMemory(std:("My email is alice@example.com")), createMockState(), object{}, callback); });
                expect(lastResponse)->toContain(std:("completed successfully"));
                auto completedForm = std::async([=]() { formsService->getForm(multiStepForm->id); });
                expect(completedForm->status)->toBe(std:("completed"));
            }
            );
        }
        );
        describe(std:("Form templates and customization"), [=]() mutable
        {
            test(std:("should handle custom form templates"), [=]() mutable
            {
                formsService->registerTemplate(object{
                    object::pair{std:("name"), std:("survey")}, 
                    object::pair{std:("description"), std:("Customer satisfaction survey")}, 
                    object::pair{std:("steps"), array<object>{ object{
                        object::pair{std:("id"), std:("rating")}, 
                        object::pair{std:("name"), std:("Rating")}, 
                        object::pair{std:("fields"), array<object>{ object{
                            object::pair{std:("id"), std:("satisfaction")}, 
                            object::pair{std:("label"), std:("How satisfied are you?")}, 
                            object::pair{std:("type"), as<std::shared_ptr<const>>(std:("choice"))}, 
                            object::pair{std:("metadata"), object{
                                object::pair{std:("options"), array<string>{ std:("Very satisfied"), std:("Satisfied"), std:("Neutral"), std:("Dissatisfied") }}
                            }}
                        }, object{
                            object::pair{std:("id"), std:("comments")}, 
                            object::pair{std:("label"), std:("Additional comments")}, 
                            object::pair{std:("type"), as<std::shared_ptr<const>>(std:("textarea"))}, 
                            object::pair{std:("optional"), true}
                        } }}
                    } }}
                });
                auto surveyForm = std::async([=]() { formsService->createForm(std:("survey")); });
                expect(surveyForm->description)->toBe(std:("Customer satisfaction survey"));
                expect(const_(surveyForm->steps)[0]->fields)->toHaveLength(2);
                mockRuntime["useModel"]["mockResolvedValueOnce"](std:("{"satisfaction": "Very satisfied"}"));
                auto result = std::async([=]() { formsService->updateForm(surveyForm->id, createMockMemory(std:("I am very satisfied"))); });
                expect(result->success)->toBe(true);
                expect(result->formCompleted)->toBe(true);
            }
            );
        }
        );
    }
    );
}

MAIN
