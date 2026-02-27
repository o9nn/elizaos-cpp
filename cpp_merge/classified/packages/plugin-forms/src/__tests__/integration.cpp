#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-forms/src/__tests__/integration.test.h"

any createMockRuntime()
{
    shared services = std::make_shared<Map<string, any>>();
    auto mockRuntime = object{
        object::pair{std::string("agentId"), asUUID(uuidv4())}, 
        object::pair{std::string("character"), object{
            object::pair{std::string("name"), std::string("TestAgent")}, 
            object::pair{std::string("bio"), array<string>{ std::string("Test agent bio") }}, 
            object::pair{std::string("system"), std::string("You are a test agent")}, 
            object::pair{std::string("plugins"), array<string>{ std::string("forms") }}
        }}, 
        object::pair{std::string("actions"), array<any>{ createFormAction, updateFormAction, cancelFormAction }}, 
        object::pair{std::string("providers"), array<any>{ formsProvider }}, 
        object::pair{std::string("logger"), object{
            object::pair{std::string("info"), mock([=]() mutable
            {
            }
            )}, 
            object::pair{std::string("error"), mock([=]() mutable
            {
            }
            )}, 
            object::pair{std::string("warn"), mock([=]() mutable
            {
            }
            )}, 
            object::pair{std::string("debug"), mock([=]() mutable
            {
            }
            )}
        }}, 
        object::pair{std::string("useModel"), mock([=]() mutable
        {
            return Promise->resolve(std::string("{"name": "Test User"}"));
        }
        )}, 
        object::pair{std::string("getService"), [=](auto name) mutable
        {
            return services->get(name);
        }
        }, 
        object::pair{std::string("registerService"), [=](auto name, auto service) mutable
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
        object::pair{std::string("id"), asUUID(uuidv4())}, 
        object::pair{std::string("entityId"), asUUID(uuidv4())}, 
        object::pair{std::string("roomId"), asUUID(uuidv4())}, 
        object::pair{std::string("agentId"), asUUID(uuidv4())}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("text")}, 
            object::pair{std::string("source"), std::string("test")}
        }}, 
        object::pair{std::string("createdAt"), Date->now()}
    });
};
std::function<any()> createMockState = [=]() mutable
{
    return (object{
        object::pair{std::string("values"), object{}}, 
        object::pair{std::string("data"), object{}}, 
        object::pair{std::string("text"), string_empty}
    });
};

void Main(void)
{
    describe(std::string("Forms Plugin Integration Tests"), [=]() mutable
    {
        shared<any> mockRuntime;
        shared<std::shared_ptr<FormsService>> formsService;
        beforeEach([=]() mutable
        {
            mockRuntime = createMockRuntime();
            formsService = as<std::shared_ptr<FormsService>>((std::async([=]() { FormsService::start(mockRuntime); })));
            (as<object>(as<any>(mockRuntime)))["registerService"](std::string("forms"), formsService);
        }
        );
        describe(std::string("Form creation through action"), [=]() mutable
        {
            test(std::string("should handle CREATE_FORM action"), [=]() mutable
            {
                auto message = createMockMemory(std::string("I need to create a contact form"));
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
                expect(responseText)->toContain(std::string("created"));
                expect(responseText->toLowerCase())->toContain(std::string("contact"));
                auto forms = std::async([=]() { formsService->listForms(std::string("active")); });
                expect(forms)->toHaveLength(1);
                expect(const_(forms)[0]->name)->toBe(std::string("contact"));
            }
            );
            test(std::string("should handle form creation with provider context"), [=]() mutable
            {
                auto form = std::async([=]() { formsService->createForm(std::string("contact")); });
                auto message = createMockMemory(std::string("test"));
                auto state = createMockState();
                auto providerResult = std::async([=]() { formsProvider->get(mockRuntime, message, state); });
                expect(providerResult->text)->toContain(std::string("[FORMS]"));
                expect(providerResult->text)->toContain(std::string("contact"));
                expect(providerResult->data->forms)->toHaveLength(1);
                expect(const_(providerResult->data->forms)[0]->id)->toBe(form->id);
            }
            );
        }
        );
        describe(std::string("Form updates through action"), [=]() mutable
        {
            shared<std::shared_ptr<Awaited<std::shared_ptr<ReturnType<any>>>>> testForm;
            beforeEach([=]() mutable
            {
                testForm = std::async([=]() { formsService->createForm(std::string("contact")); });
            }
            );
            test(std::string("should handle UPDATE_FORM action"), [=]() mutable
            {
                auto message = createMockMemory(std::string("My name is John Doe"));
                auto state = createMockState();
                shared responseReceived = false;
                shared responseText = string_empty;
                auto callback = [=](auto response) mutable
                {
                    responseReceived = true;
                    responseText = OR((response->text), (string_empty));
                    return array<any>();
                };
                mockRuntime["useModel"]["mockResolvedValueOnce"](std::string("{"name": "John Doe"}"));
                auto isValid = std::async([=]() { updateFormAction->validate(mockRuntime, message, state); });
                expect(isValid)->toBe(true);
                std::async([=]() { updateFormAction->handler(mockRuntime, message, state, object{}, callback); });
                expect(responseReceived)->toBe(true);
                expect(responseText)->toContain(std::string("Updated"));
                auto updatedForm = std::async([=]() { formsService->getForm(testForm->id); });
                auto nameField = const_(updatedForm->steps)[0]->fields->find([=](auto f) mutable
                {
                    return f->id == std::string("name");
                }
                );
                expect(nameField->value)->toBe(std::string("John Doe"));
            }
            );
            test(std::string("should complete form when all fields are filled"), [=]() mutable
            {
                auto messages = array<string>{ std::string("My name is John Doe"), std::string("My email is john@example.com") };
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
                    if (text->includes(std::string("name"))) {
                        mockRuntime["useModel"]["mockResolvedValueOnce"](std::string("{"name": "John Doe"}"));
                    } else if (text->includes(std::string("email"))) {
                        mockRuntime["useModel"]["mockResolvedValueOnce"](std::string("{"email": "john@example.com"}"));
                    }
                    std::async([=]() { updateFormAction->handler(mockRuntime, message, state, object{}, callback); });
                }
                auto completedForm = std::async([=]() { formsService->getForm(testForm->id); });
                expect(completedForm->status)->toBe(std::string("completed"));
                expect(completionMessage)->toContain(std::string("completed"));
            }
            );
        }
        );
        describe(std::string("Form cancellation through action"), [=]() mutable
        {
            shared<std::shared_ptr<Awaited<std::shared_ptr<ReturnType<any>>>>> testForm;
            beforeEach([=]() mutable
            {
                testForm = std::async([=]() { formsService->createForm(std::string("contact")); });
            }
            );
            test(std::string("should handle CANCEL_FORM action"), [=]() mutable
            {
                auto message = createMockMemory(std::string("Cancel the form"));
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
                expect(responseText)->toContain(std::string("cancelled"));
                auto cancelledForm = std::async([=]() { formsService->getForm(testForm->id); });
                expect(cancelledForm->status)->toBe(std::string("cancelled"));
            }
            );
        }
        );
        describe(std::string("Plugin initialization"), [=]() mutable
        {
            test(std::string("should register all components correctly"), [=]() mutable
            {
                expect(formsPlugin->services)->toContain(FormsService);
                expect(formsPlugin->actions)->toContain(createFormAction);
                expect(formsPlugin->actions)->toContain(updateFormAction);
                expect(formsPlugin->actions)->toContain(cancelFormAction);
                expect(formsPlugin->providers)->toContain(formsProvider);
            }
            );
            test(std::string("should have correct plugin metadata"), [=]() mutable
            {
                expect(formsPlugin->name)->toBe(std::string("@elizaos/plugin-forms"));
                expect(formsPlugin->description)->toBeTruthy();
            }
            );
        }
        );
        describe(std::string("Secret field handling in integration"), [=]() mutable
        {
            test(std::string("should mask secret fields in provider output"), [=]() mutable
            {
                auto form = std::async([=]() { formsService->createForm(object{
                    object::pair{std::string("name"), std::string("api-config")}, 
                    object::pair{std::string("agentId"), mockRuntime["agentId"]}, 
                    object::pair{std::string("steps"), array<object>{ object{
                        object::pair{std::string("id"), std::string("credentials")}, 
                        object::pair{std::string("name"), std::string("API Credentials")}, 
                        object::pair{std::string("fields"), array<object>{ object{
                            object::pair{std::string("id"), std::string("apiKey")}, 
                            object::pair{std::string("label"), std::string("API Key")}, 
                            object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("text"))}, 
                            object::pair{std::string("secret"), true}
                        }, object{
                            object::pair{std::string("id"), std::string("endpoint")}, 
                            object::pair{std::string("label"), std::string("API Endpoint")}, 
                            object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("text"))}, 
                            object::pair{std::string("description"), std::string("The API endpoint URL")}
                        } }}
                    } }}
                }); });
                mockRuntime["useModel"]["mockResolvedValueOnce"](std::string("{"apiKey": "sk-12345"}"));
                std::async([=]() { formsService->updateForm(form->id, createMockMemory(std::string("My API key is sk-12345"))); });
                auto providerResult = std::async([=]() { formsProvider->get(mockRuntime, createMockMemory(std::string("test")), createMockState()); });
                expect(providerResult->text)->toContain(std::string("[SECRET]"));
                expect(providerResult->text)->not->toContain(std::string("sk-12345"));
            }
            );
        }
        );
        describe(std::string("Error handling in actions"), [=]() mutable
        {
            test(std::string("should handle form service not available"), [=]() mutable
            {
                auto runtimeWithoutService = createMockRuntime();
                auto message = createMockMemory(std::string("Create a contact form"));
                auto state = createMockState();
                auto isValid = std::async([=]() { createFormAction->validate(runtimeWithoutService, message, state); });
                expect(isValid)->toBe(false);
            }
            );
            test(std::string("should handle LLM extraction errors gracefully"), [=]() mutable
            {
                auto message = createMockMemory(std::string("My name is John and email is john@example.com"));
                auto state = createMockState();
                shared responseText = string_empty;
                auto callback = [=](auto response) mutable
                {
                    responseText = OR((response->text), (string_empty));
                    return array<any>();
                };
                mockRuntime["useModel"]["mockResolvedValueOnce"](std::string("invalid json response"));
                std::async([=]() { updateFormAction->handler(mockRuntime, message, state, object{}, callback); });
                expect(responseText)->toContain(std::string("No active forms to update"));
            }
            );
        }
        );
        describe(std::string("Multi-step form workflow"), [=]() mutable
        {
            test(std::string("should complete multi-step form through actions"), [=]() mutable
            {
                auto multiStepForm = std::async([=]() { formsService->createForm(object{
                    object::pair{std::string("name"), std::string("registration")}, 
                    object::pair{std::string("agentId"), mockRuntime["agentId"]}, 
                    object::pair{std::string("steps"), array<object>{ object{
                        object::pair{std::string("id"), std::string("personal")}, 
                        object::pair{std::string("name"), std::string("Personal Info")}, 
                        object::pair{std::string("fields"), array<object>{ object{
                            object::pair{std::string("id"), std::string("name")}, 
                            object::pair{std::string("label"), std::string("Name")}, 
                            object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("text"))}
                        }, object{
                            object::pair{std::string("id"), std::string("age")}, 
                            object::pair{std::string("label"), std::string("Age")}, 
                            object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("number"))}
                        } }}
                    }, object{
                        object::pair{std::string("id"), std::string("contact")}, 
                        object::pair{std::string("name"), std::string("Contact Info")}, 
                        object::pair{std::string("fields"), array<object>{ object{
                            object::pair{std::string("id"), std::string("email")}, 
                            object::pair{std::string("label"), std::string("Email")}, 
                            object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("email"))}
                        }, object{
                            object::pair{std::string("id"), std::string("phone")}, 
                            object::pair{std::string("label"), std::string("Phone")}, 
                            object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("tel"))}, 
                            object::pair{std::string("optional"), true}
                        } }}
                    } }}
                }); });
                shared lastResponse = string_empty;
                auto callback = [=](auto response) mutable
                {
                    lastResponse = OR((response->text), (string_empty));
                    return array<any>();
                };
                mockRuntime["useModel"]["mockResolvedValueOnce"](std::string("{"name": "Alice", "age": 30}"));
                std::async([=]() { updateFormAction->handler(mockRuntime, createMockMemory(std::string("My name is Alice and I am 30 years old")), createMockState(), object{}, callback); });
                expect(lastResponse)->toContain(std::string("Personal Info"));
                expect(lastResponse)->toContain(std::string("completed"));
                expect(lastResponse)->toContain(std::string("Contact Info"));
                mockRuntime["useModel"]["mockResolvedValueOnce"](std::string("{"email": "alice@example.com"}"));
                std::async([=]() { updateFormAction->handler(mockRuntime, createMockMemory(std::string("My email is alice@example.com")), createMockState(), object{}, callback); });
                expect(lastResponse)->toContain(std::string("completed successfully"));
                auto completedForm = std::async([=]() { formsService->getForm(multiStepForm->id); });
                expect(completedForm->status)->toBe(std::string("completed"));
            }
            );
        }
        );
        describe(std::string("Form templates and customization"), [=]() mutable
        {
            test(std::string("should handle custom form templates"), [=]() mutable
            {
                formsService->registerTemplate(object{
                    object::pair{std::string("name"), std::string("survey")}, 
                    object::pair{std::string("description"), std::string("Customer satisfaction survey")}, 
                    object::pair{std::string("steps"), array<object>{ object{
                        object::pair{std::string("id"), std::string("rating")}, 
                        object::pair{std::string("name"), std::string("Rating")}, 
                        object::pair{std::string("fields"), array<object>{ object{
                            object::pair{std::string("id"), std::string("satisfaction")}, 
                            object::pair{std::string("label"), std::string("How satisfied are you?")}, 
                            object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("choice"))}, 
                            object::pair{std::string("metadata"), object{
                                object::pair{std::string("options"), array<string>{ std::string("Very satisfied"), std::string("Satisfied"), std::string("Neutral"), std::string("Dissatisfied") }}
                            }}
                        }, object{
                            object::pair{std::string("id"), std::string("comments")}, 
                            object::pair{std::string("label"), std::string("Additional comments")}, 
                            object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("textarea"))}, 
                            object::pair{std::string("optional"), true}
                        } }}
                    } }}
                });
                auto surveyForm = std::async([=]() { formsService->createForm(std::string("survey")); });
                expect(surveyForm->description)->toBe(std::string("Customer satisfaction survey"));
                expect(const_(surveyForm->steps)[0]->fields)->toHaveLength(2);
                mockRuntime["useModel"]["mockResolvedValueOnce"](std::string("{"satisfaction": "Very satisfied"}"));
                auto result = std::async([=]() { formsService->updateForm(surveyForm->id, createMockMemory(std::string("I am very satisfied"))); });
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
