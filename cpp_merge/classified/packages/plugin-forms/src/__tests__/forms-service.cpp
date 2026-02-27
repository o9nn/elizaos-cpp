#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-forms/src/__tests__/forms-service.test.h"

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
std::function<any()> createTypedMockRuntime = [=]() mutable
{
    auto testAgentId = asUUID(uuidv4());
    auto runtime = object{
        object::pair{std:("agentId"), testAgentId}, 
        object::pair{std:("providers"), array<any>()}, 
        object::pair{std:("actions"), array<any>()}, 
        object::pair{std:("evaluators"), array<any>()}, 
        object::pair{std:("plugins"), array<any>()}, 
        object::pair{std:("services"), std::make_shared<Map>()}, 
        object::pair{std:("clients"), array<any>()}, 
        object::pair{std:("messageManager"), nullptr}, 
        object::pair{std:("descriptionManager"), nullptr}, 
        object::pair{std:("documentsManager"), nullptr}, 
        object::pair{std:("knowledgeManager"), nullptr}, 
        object::pair{std:("ragKnowledgeManager"), nullptr}, 
        object::pair{std:("loreManager"), nullptr}, 
        object::pair{std:("serverUrl"), std:("http://localhost:7777")}, 
        object::pair{std:("databaseAdapter"), nullptr}, 
        object::pair{std:("token"), nullptr}, 
        object::pair{std:("modelProvider"), std:("openai")}, 
        object::pair{std:("imageModelProvider"), std:("openai")}, 
        object::pair{std:("imageVisionModelProvider"), std:("openai")}, 
        object::pair{std:("character"), object{
            object::pair{std:("name"), std:("Test Agent")}, 
            object::pair{std:("modelProvider"), std:("openai")}
        }}, 
        object::pair{std:("cacheManager"), object{
            object::pair{std:("get"), mock()}, 
            object::pair{std:("set"), mock()}, 
            object::pair{std:("delete"), mock()}
        }}, 
        object::pair{std:("fetch"), nullptr}, 
        object::pair{std:("initialize"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std:("registerMemoryManager"), mock()}, 
        object::pair{std:("getMemoryManager"), mock()}, 
        object::pair{std:("getService"), mock()}, 
        object::pair{std:("registerService"), mock()}, 
        object::pair{std:("getSetting"), mock()}, 
        object::pair{std:("getConversationLength"), mock()->mockReturnValue(0)}, 
        object::pair{std:("processActions"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std:("evaluate"), mock()->mockResolvedValue(nullptr)}, 
        object::pair{std:("ensureParticipantExists"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std:("ensureUserExists"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std:("registerAction"), mock()}, 
        object::pair{std:("ensureConnection"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std:("ensureParticipantInRoom"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std:("ensureRoomExists"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std:("composeState"), mock()->mockResolvedValue(object{})}, 
        object::pair{std:("updateRecentMessageState"), mock()->mockResolvedValue(object{})}, 
        object::pair{std:("useModel"), mock()->mockResolvedValue(std:("Mock response"))}
    };
    return as<std::shared_ptr<IAgentRuntime>>(as<any>(runtime));
};

void Main(void)
{
    describe(std:("FormsService"), [=]() mutable
    {
        shared<std::shared_ptr<FormsService>> service;
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        beforeEach([=]() mutable
        {
            mockRuntime = createTypedMockRuntime();
            service = std::make_shared<FormsService>(mockRuntime);
        }
        );
        describe(std:("Service initialization"), [=]() mutable
        {
            test(std:("should have correct service type"), [=]() mutable
            {
                expect(FormsService::serviceType)->toBeDefined();
            }
            );
            test(std:("should start properly"), [=]() mutable
            {
                auto startedService = std::async([=]() { FormsService::start(mockRuntime); });
                expect(startedService)->toBeInstanceOf(FormsService);
            }
            );
            test(std:("should register default contact template on creation"), [=]() mutable
            {
                auto templates = (as<object>(as<any>(service)))["templates"];
                expect(templates->has(std:("contact")))->toBe(true);
                auto contactTemplate = templates->get(std:("contact"));
                expect(contactTemplate)->toBeDefined();
                expect(contactTemplate->name)->toBe(std:("contact"));
                expect(contactTemplate->steps)->toHaveLength(1);
                expect(const_(contactTemplate->steps)[0]->fields)->toHaveLength(3);
            }
            );
        }
        );
        describe(std:("Form creation"), [=]() mutable
        {
            test(std:("should create a form from template"), [=]() mutable
            {
                auto form = std::async([=]() { service->createForm(std:("contact")); });
                expect(form)->toBeDefined();
                expect(form->id)->toBeDefined();
                expect(form->agentId)->toBe(mockRuntime->agentId);
                expect(form->status)->toBe(std:("active"));
                expect(form->name)->toBe(std:("contact"));
                expect(form->steps)->toHaveLength(1);
                expect(form->currentStepIndex)->toBe(0);
                expect(form->createdAt)->toBeDefined();
            }
            );
            test(std:("should create a custom form"), [=]() mutable
            {
                auto customForm = object{
                    object::pair{std:("name"), std:("custom-form")}, 
                    object::pair{std:("agentId"), mockRuntime->agentId}, 
                    object::pair{std:("steps"), array<object>{ object{
                        object::pair{std:("id"), std:("step1")}, 
                        object::pair{std:("name"), std:("Step 1")}, 
                        object::pair{std:("fields"), array<object>{ object{
                            object::pair{std:("id"), std:("field1")}, 
                            object::pair{std:("label"), std:("Field 1")}, 
                            object::pair{std:("type"), as<std::shared_ptr<const>>(std:("text"))}
                        } }}
                    } }}
                };
                auto form = std::async([=]() { service->createForm(customForm); });
                expect(form)->toBeDefined();
                expect(form->name)->toBe(std:("custom-form"));
                expect(form->steps)->toHaveLength(1);
                expect(const_(form->steps)[0]->fields)->toHaveLength(1);
            }
            );
            test(std:("should throw error for non-existent template"), [=]() mutable
            {
                std::async([=]() { expect(service->createForm(std:("non-existent")))->rejects->toThrow(std:("Template "non-existent" not found")); });
            }
            );
        }
        );
        describe(std:("Form updates"), [=]() mutable
        {
            shared<std::shared_ptr<Form>> testForm;
            beforeEach([=]() mutable
            {
                testForm = std::async([=]() { service->createForm(std:("contact")); });
            }
            );
            test(std:("should update form fields with extracted values"), [=]() mutable
            {
                auto message = createMockMemory(std:("My name is John Doe"));
                (as<ReturnType<mock>>(mockRuntime->useModel))["mockResolvedValueOnce"](std:("{"name": "John Doe"}"));
                auto result = std::async([=]() { service->updateForm(testForm->id, message); });
                expect(result->success)->toBe(true);
                expect(result->updatedFields)->toContain(std:("name"));
                auto updatedForm = std::async([=]() { service->getForm(testForm->id); });
                auto nameField = const_(updatedForm->steps)[0]->fields->find([=](auto f) mutable
                {
                    return f->id == std:("name");
                }
                );
                expect(nameField->value)->toBe(std:("John Doe"));
            }
            );
            test(std:("should progress to next step when all required fields are filled"), [=]() mutable
            {
                auto multiStepForm = std::async([=]() { service->createForm(object{
                    object::pair{std:("name"), std:("multi-step")}, 
                    object::pair{std:("agentId"), mockRuntime->agentId}, 
                    object::pair{std:("steps"), array<object>{ object{
                        object::pair{std:("id"), std:("step1")}, 
                        object::pair{std:("name"), std:("Step 1")}, 
                        object::pair{std:("fields"), array<object>{ object{
                            object::pair{std:("id"), std:("field1")}, 
                            object::pair{std:("label"), std:("Field 1")}, 
                            object::pair{std:("type"), as<std::shared_ptr<const>>(std:("text"))}
                        } }}
                    }, object{
                        object::pair{std:("id"), std:("step2")}, 
                        object::pair{std:("name"), std:("Step 2")}, 
                        object::pair{std:("fields"), array<object>{ object{
                            object::pair{std:("id"), std:("field2")}, 
                            object::pair{std:("label"), std:("Field 2")}, 
                            object::pair{std:("type"), as<std::shared_ptr<const>>(std:("text"))}
                        } }}
                    } }}
                }); });
                (as<ReturnType<mock>>(mockRuntime->useModel))["mockResolvedValueOnce"](std:("{"field1": "value1"}"));
                auto result = std::async([=]() { service->updateForm(multiStepForm->id, createMockMemory(std:("value1"))); });
                expect(result->success)->toBe(true);
                expect(result->stepCompleted)->toBe(true);
                auto updatedForm = std::async([=]() { service->getForm(multiStepForm->id); });
                expect(updatedForm->currentStepIndex)->toBe(1);
            }
            );
            test(std:("should mark form as completed when all steps are done"), [=]() mutable
            {
                (as<ReturnType<mock>>(mockRuntime->useModel))["mockResolvedValueOnce"](std:("{"name": "John Doe"}"))["mockResolvedValueOnce"](std:("{"email": "john@example.com"}"));
                std::async([=]() { service->updateForm(testForm->id, createMockMemory(std:("John Doe"))); });
                std::async([=]() { service->updateForm(testForm->id, createMockMemory(std:("john@example.com"))); });
                auto completedForm = std::async([=]() { service->getForm(testForm->id); });
                expect(completedForm->status)->toBe(std:("completed"));
            }
            );
            test(std:("should handle form not found"), [=]() mutable
            {
                auto result = std::async([=]() { service->updateForm(asUUID(uuidv4()), createMockMemory(std:("test"))); });
                expect(result->success)->toBe(false);
                expect(result->message)->toBe(std:("Form not found"));
            }
            );
            test(std:("should handle already completed forms"), [=]() mutable
            {
                auto forms = (as<object>(as<any>(service)))["forms"];
                auto formData = forms->get(testForm->id);
                if (formData) {
                    formData->status = std:("completed");
                }
                auto result = std::async([=]() { service->updateForm(testForm->id, createMockMemory(std:("test"))); });
                expect(result->success)->toBe(false);
                expect(result->message)->toBe(std:("Form is not active"));
            }
            );
        }
        );
        describe(std:("Form cancellation"), [=]() mutable
        {
            test(std:("should cancel an active form"), [=]() mutable
            {
                auto form = std::async([=]() { service->createForm(std:("contact")); });
                auto result = std::async([=]() { service->cancelForm(form->id); });
                expect(result)->toBe(true);
                auto cancelledForm = std::async([=]() { service->getForm(form->id); });
                expect(cancelledForm->status)->toBe(std:("cancelled"));
            }
            );
            test(std:("should return false for non-existent form"), [=]() mutable
            {
                auto result = std::async([=]() { service->cancelForm(asUUID(uuidv4())); });
                expect(result)->toBe(false);
            }
            );
        }
        );
        describe(std:("Form listing"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                std::async([=]() { service->createForm(std:("contact")); });
                auto form2 = std::async([=]() { service->createForm(std:("contact")); });
                std::async([=]() { service->cancelForm(form2->id); });
                auto form3 = std::async([=]() { service->createForm(std:("contact")); });
                auto forms = (as<object>(as<any>(service)))["forms"];
                auto form3Data = forms->get(form3->id);
                if (form3Data) {
                    form3Data->status = std:("completed");
                }
            }
            );
            test(std:("should list forms by status"), [=]() mutable
            {
                auto activeForms = std::async([=]() { service->listForms(std:("active")); });
                auto cancelledForms = std::async([=]() { service->listForms(std:("cancelled")); });
                auto completedForms = std::async([=]() { service->listForms(std:("completed")); });
                expect(activeForms)->toHaveLength(1);
                expect(cancelledForms)->toHaveLength(1);
                expect(completedForms)->toHaveLength(1);
            }
            );
            test(std:("should list all forms when no status specified"), [=]() mutable
            {
                auto allForms = std::async([=]() { service->listForms(); });
                expect(allForms)->toHaveLength(3);
            }
            );
        }
        );
        describe(std:("Template management"), [=]() mutable
        {
            test(std:("should register a new template"), [=]() mutable
            {
                auto template = object{
                    object::pair{std:("name"), std:("custom-template")}, 
                    object::pair{std:("steps"), array<object>{ object{
                        object::pair{std:("id"), std:("step1")}, 
                        object::pair{std:("name"), std:("Step 1")}, 
                        object::pair{std:("fields"), array<object>{ object{
                            object::pair{std:("id"), std:("field1")}, 
                            object::pair{std:("label"), std:("Field 1")}, 
                            object::pair{std:("type"), as<std::shared_ptr<const>>(std:("text"))}
                        } }}
                    } }}
                };
                service->registerTemplate(template);
                auto registeredTemplate = (as<object>(as<any>(service)))["templates"]->get(std:("custom-template"));
                expect(registeredTemplate)->toEqual(template);
            }
            );
        }
        );
        describe(std:("Cleanup"), [=]() mutable
        {
            test(std:("should remove old completed and cancelled forms"), [=]() mutable
            {
                auto form1 = std::async([=]() { service->createForm(std:("contact")); });
                auto form2 = std::async([=]() { service->createForm(std:("contact")); });
                auto oldTimestamp = Date->now() - 25 * 60 * 60 * 1000;
                auto forms = (as<object>(as<any>(service)))["forms"];
                auto form1Data = forms->get(form1->id);
                if (form1Data) {
                    form1Data->status = std:("completed");
                    form1Data->updatedAt = oldTimestamp;
                }
                auto form2Data = forms->get(form2->id);
                if (form2Data) {
                    form2Data->status = std:("cancelled");
                    form2Data->updatedAt = oldTimestamp;
                }
                auto form3 = std::async([=]() { service->createForm(std:("contact")); });
                auto form3Data = forms->get(form3->id);
                if (form3Data) {
                    form3Data->status = std:("completed");
                }
                auto cleanedCount = std::async([=]() { service->cleanup(); });
                expect(cleanedCount)->toBe(2);
                expect(std::async([=]() { service->getForm(form1->id); }))->toBeNull();
                expect(std::async([=]() { service->getForm(form2->id); }))->toBeNull();
                expect(std::async([=]() { service->getForm(form3->id); }))->toBeDefined();
            }
            );
        }
        );
        describe(std:("Secret field handling"), [=]() mutable
        {
            test(std:("should extract values for secret fields"), [=]() mutable
            {
                auto formWithSecret = std::async([=]() { service->createForm(object{
                    object::pair{std:("name"), std:("api-form")}, 
                    object::pair{std:("agentId"), mockRuntime->agentId}, 
                    object::pair{std:("steps"), array<object>{ object{
                        object::pair{std:("id"), std:("credentials")}, 
                        object::pair{std:("name"), std:("Credentials")}, 
                        object::pair{std:("fields"), array<object>{ object{
                            object::pair{std:("id"), std:("apiKey")}, 
                            object::pair{std:("label"), std:("API Key")}, 
                            object::pair{std:("type"), as<std::shared_ptr<const>>(std:("text"))}, 
                            object::pair{std:("secret"), true}
                        } }}
                    } }}
                }); });
                (as<ReturnType<mock>>(mockRuntime->useModel))["mockResolvedValueOnce"](std:("{"apiKey": "sk-12345"}"));
                auto result = std::async([=]() { service->updateForm(formWithSecret->id, createMockMemory(std:("My API key is sk-12345"))); });
                expect(result->success)->toBe(true);
                expect(mockRuntime->useModel)->toHaveBeenCalled();
                auto updatedForm = std::async([=]() { service->getForm(formWithSecret->id); });
                auto apiKeyField = const_(updatedForm->steps)[0]->fields->find([=](auto f) mutable
                {
                    return f->id == std:("apiKey");
                }
                );
                expect(apiKeyField->value)->toBeTruthy();
                expect(type_of(apiKeyField->value))->toBe(std:("string"));
                expect((as<string>(apiKeyField->value))->includes(std:(":")))->toBe(true);
                expect(apiKeyField->value)->not->toBe(std:("sk-12345"));
            }
            );
        }
        );
        describe(std:("Database persistence"), [=]() mutable
        {
            test(std:("should handle missing database tables gracefully"), [=]() mutable
            {
                auto runtime = as<std::shared_ptr<IAgentRuntime>>(as<any>(object{
                    object::pair{std:("agentId"), asUUID(uuidv4())}, 
                    object::pair{std:("character"), object{
                        object::pair{std:("name"), std:("TestAgent")}
                    }}, 
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
                        return Promise->resolve(std:("{}"));
                    }
                    )}
                }));
                (as<any>(runtime))["adapter"] = object{
                    object::pair{std:("fetch"), mock([=]() mutable
                    {
                        return Promise->resolve(object{
                            object::pair{std:("rows"), array<any>()}
                        });
                    }
                    )}, 
                    object::pair{std:("run"), mock([=]() mutable
                    {
                        return Promise->reject(std::make_shared<Error>(std:("relation "forms" does not exist")));
                    }
                    )}
                };
                auto formsService = std::async([=]() { FormsService::start(runtime); });
                expect(formsService)->toBeInstanceOf(FormsService);
                auto form = std::async([=]() { (as<std::shared_ptr<FormsService>>(formsService))->createForm(std:("contact")); });
                expect(form)->toBeTruthy();
                expect(form->name)->toBe(std:("contact"));
            }
            );
            test(std:("should persist forms when database is available"), [=]() mutable
            {
                auto runtime = as<std::shared_ptr<IAgentRuntime>>(as<any>(object{
                    object::pair{std:("agentId"), asUUID(uuidv4())}, 
                    object::pair{std:("character"), object{
                        object::pair{std:("name"), std:("TestAgent")}
                    }}, 
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
                        return Promise->resolve(std:("{}"));
                    }
                    )}
                }));
                shared mockRun = mock([=]() mutable
                {
                    return Promise->resolve();
                }
                );
                shared mockFetch = mock([=]() mutable
                {
                    return Promise->resolve(object{
                        object::pair{std:("rows"), array<object>{ object{
                            object::pair{std:("exists"), true}
                        } }}
                    });
                }
                );
                (as<any>(runtime))["getDatabase"] = mock([=]() mutable
                {
                    return Promise->resolve(object{
                        object::pair{std:("fetch"), mockFetch}, 
                        object::pair{std:("run"), mockRun}
                    });
                }
                );
                auto formsService = as<std::shared_ptr<FormsService>>((std::async([=]() { FormsService::start(runtime); })));
                std::async([=]() { formsService->createForm(std:("contact")); });
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    return setTimeout(resolve, 1100);
                }
                ); });
                expect((as<any>(runtime))["getDatabase"])->toHaveBeenCalled();
            }
            );
        }
        );
        describe(std:("Zod validation"), [=]() mutable
        {
            test(std:("should validate field values according to type"), [=]() mutable
            {
                auto service = as<std::shared_ptr<FormsService>>((std::async([=]() { FormsService::start(mockRuntime); })));
                auto form = std::async([=]() { service->createForm(object{
                    object::pair{std:("name"), std:("validation-test")}, 
                    object::pair{std:("agentId"), mockRuntime->agentId}, 
                    object::pair{std:("steps"), array<object>{ object{
                        object::pair{std:("id"), std:("step1")}, 
                        object::pair{std:("name"), std:("Validation Test")}, 
                        object::pair{std:("fields"), array<object>{ object{
                            object::pair{std:("id"), std:("email")}, 
                            object::pair{std:("label"), std:("Email")}, 
                            object::pair{std:("type"), as<std::shared_ptr<const>>(std:("email"))}
                        }, object{
                            object::pair{std:("id"), std:("age")}, 
                            object::pair{std:("label"), std:("Age")}, 
                            object::pair{std:("type"), as<std::shared_ptr<const>>(std:("number"))}
                        }, object{
                            object::pair{std:("id"), std:("website")}, 
                            object::pair{std:("label"), std:("Website")}, 
                            object::pair{std:("type"), as<std::shared_ptr<const>>(std:("url"))}
                        } }}
                    } }}
                }); });
                (as<ReturnType<mock>>(mockRuntime->useModel))["mockResolvedValueOnce"](std:("{"email": "not-an-email"}"));
                auto result1 = std::async([=]() { service->updateForm(form->id, createMockMemory(std:("My email is not-an-email"))); });
                expect(result1->success)->toBe(true);
                expect(mockRuntime->useModel)->toHaveBeenCalled();
                (as<ReturnType<mock>>(mockRuntime->useModel))["mockResolvedValueOnce"](std:("{"email": "test@example.com", "age": 25, "website": "https://example.com"}"));
                auto result2 = std::async([=]() { service->updateForm(form->id, createMockMemory(std:("Email test@example.com, age 25, website https://example.com"))); });
                expect(result2->errors)->toHaveLength(0);
                expect(result2->updatedFields)->toHaveLength(3);
            }
            );
            test(std:("should handle falsy values correctly"), [=]() mutable
            {
                auto service = as<std::shared_ptr<FormsService>>((std::async([=]() { FormsService::start(mockRuntime); })));
                auto form = std::async([=]() { service->createForm(object{
                    object::pair{std:("name"), std:("falsy-test")}, 
                    object::pair{std:("agentId"), mockRuntime->agentId}, 
                    object::pair{std:("steps"), array<object>{ object{
                        object::pair{std:("id"), std:("step1")}, 
                        object::pair{std:("name"), std:("Falsy Test")}, 
                        object::pair{std:("fields"), array<object>{ object{
                            object::pair{std:("id"), std:("enabled")}, 
                            object::pair{std:("label"), std:("Enabled")}, 
                            object::pair{std:("type"), as<std::shared_ptr<const>>(std:("checkbox"))}
                        }, object{
                            object::pair{std:("id"), std:("count")}, 
                            object::pair{std:("label"), std:("Count")}, 
                            object::pair{std:("type"), as<std::shared_ptr<const>>(std:("number"))}
                        }, object{
                            object::pair{std:("id"), std:("message")}, 
                            object::pair{std:("label"), std:("Message")}, 
                            object::pair{std:("type"), as<std::shared_ptr<const>>(std:("text"))}, 
                            object::pair{std:("optional"), true}
                        } }}
                    } }}
                }); });
                (as<ReturnType<mock>>(mockRuntime->useModel))["mockResolvedValueOnce"](std:("{"enabled": false, "count": 0, "message": ""}"));
                auto result = std::async([=]() { service->updateForm(form->id, createMockMemory(std:("Disabled, count 0, no message"))); });
                expect(result->success)->toBe(true);
                expect(result->updatedFields)->toHaveLength(3);
                auto updatedForm = std::async([=]() { service->getForm(form->id); });
                auto enabledField = const_(updatedForm->steps)[0]->fields->find([=](auto f) mutable
                {
                    return f->id == std:("enabled");
                }
                );
                auto countField = const_(updatedForm->steps)[0]->fields->find([=](auto f) mutable
                {
                    return f->id == std:("count");
                }
                );
                auto messageField = const_(updatedForm->steps)[0]->fields->find([=](auto f) mutable
                {
                    return f->id == std:("message");
                }
                );
                expect(enabledField->value)->toBe(false);
                expect(countField->value)->toBe(0);
                expect(messageField->value)->toBe(string_empty);
            }
            );
        }
        );
        describe(std:("Transaction safety"), [=]() mutable
        {
            test(std:("should rollback on database errors"), [=]() mutable
            {
                auto runtime = as<std::shared_ptr<IAgentRuntime>>(as<any>(object{
                    object::pair{std:("agentId"), asUUID(uuidv4())}, 
                    object::pair{std:("character"), object{
                        object::pair{std:("name"), std:("TestAgent")}
                    }}, 
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
                        return Promise->resolve(std:("{}"));
                    }
                    )}
                }));
                shared mockRun = mock([=](auto query) mutable
                {
                    if (query == std:("BEGIN")) {
                        return Promise->resolve();
                    }
                    if (query == std:("COMMIT")) {
                        return Promise->resolve();
                    }
                    if (query == std:("ROLLBACK")) {
                        return Promise->resolve();
                    }
                    if (query->includes(std:("INSERT INTO forms"))) {
                        return Promise->reject(std::make_shared<Error>(std:("Database error")));
                    }
                    return Promise->resolve();
                }
                );
                (as<any>(runtime))["getDatabase"] = mock([=]() mutable
                {
                    return Promise->resolve(object{
                        object::pair{std:("fetch"), mock([=]() mutable
                        {
                            return Promise->resolve(object{
                                object::pair{std:("rows"), array<object>{ object{
                                    object::pair{std:("exists"), true}
                                } }}
                            });
                        }
                        )}, 
                        object::pair{std:("run"), mockRun}
                    });
                }
                );
                auto formsService = as<std::shared_ptr<FormsService>>((std::async([=]() { FormsService::start(runtime); })));
                std::async([=]() { formsService->createForm(std:("contact")); });
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    return setTimeout(resolve, 1100);
                }
                ); });
                expect((as<any>(runtime))["getDatabase"])->toHaveBeenCalled();
                expect(formsService)->toBeInstanceOf(FormsService);
            }
            );
        }
        );
    }
    );
}

MAIN
