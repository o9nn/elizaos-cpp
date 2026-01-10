#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-forms/src/__tests__/forms-service.test.h"

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
std::function<any()> createTypedMockRuntime = [=]() mutable
{
    auto testAgentId = asUUID(uuidv4());
    auto runtime = object{
        object::pair{std::string("agentId"), testAgentId}, 
        object::pair{std::string("providers"), array<any>()}, 
        object::pair{std::string("actions"), array<any>()}, 
        object::pair{std::string("evaluators"), array<any>()}, 
        object::pair{std::string("plugins"), array<any>()}, 
        object::pair{std::string("services"), std::make_shared<Map>()}, 
        object::pair{std::string("clients"), array<any>()}, 
        object::pair{std::string("messageManager"), nullptr}, 
        object::pair{std::string("descriptionManager"), nullptr}, 
        object::pair{std::string("documentsManager"), nullptr}, 
        object::pair{std::string("knowledgeManager"), nullptr}, 
        object::pair{std::string("ragKnowledgeManager"), nullptr}, 
        object::pair{std::string("loreManager"), nullptr}, 
        object::pair{std::string("serverUrl"), std::string("http://localhost:7777")}, 
        object::pair{std::string("databaseAdapter"), nullptr}, 
        object::pair{std::string("token"), nullptr}, 
        object::pair{std::string("modelProvider"), std::string("openai")}, 
        object::pair{std::string("imageModelProvider"), std::string("openai")}, 
        object::pair{std::string("imageVisionModelProvider"), std::string("openai")}, 
        object::pair{std::string("character"), object{
            object::pair{std::string("name"), std::string("Test Agent")}, 
            object::pair{std::string("modelProvider"), std::string("openai")}
        }}, 
        object::pair{std::string("cacheManager"), object{
            object::pair{std::string("get"), mock()}, 
            object::pair{std::string("set"), mock()}, 
            object::pair{std::string("delete"), mock()}
        }}, 
        object::pair{std::string("fetch"), nullptr}, 
        object::pair{std::string("initialize"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std::string("registerMemoryManager"), mock()}, 
        object::pair{std::string("getMemoryManager"), mock()}, 
        object::pair{std::string("getService"), mock()}, 
        object::pair{std::string("registerService"), mock()}, 
        object::pair{std::string("getSetting"), mock()}, 
        object::pair{std::string("getConversationLength"), mock()->mockReturnValue(0)}, 
        object::pair{std::string("processActions"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std::string("evaluate"), mock()->mockResolvedValue(nullptr)}, 
        object::pair{std::string("ensureParticipantExists"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std::string("ensureUserExists"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std::string("registerAction"), mock()}, 
        object::pair{std::string("ensureConnection"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std::string("ensureParticipantInRoom"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std::string("ensureRoomExists"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std::string("composeState"), mock()->mockResolvedValue(object{})}, 
        object::pair{std::string("updateRecentMessageState"), mock()->mockResolvedValue(object{})}, 
        object::pair{std::string("useModel"), mock()->mockResolvedValue(std::string("Mock response"))}
    };
    return as<std::shared_ptr<IAgentRuntime>>(as<any>(runtime));
};

void Main(void)
{
    describe(std::string("FormsService"), [=]() mutable
    {
        shared<std::shared_ptr<FormsService>> service;
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        beforeEach([=]() mutable
        {
            mockRuntime = createTypedMockRuntime();
            service = std::make_shared<FormsService>(mockRuntime);
        }
        );
        describe(std::string("Service initialization"), [=]() mutable
        {
            test(std::string("should have correct service type"), [=]() mutable
            {
                expect(FormsService::serviceType)->toBeDefined();
            }
            );
            test(std::string("should start properly"), [=]() mutable
            {
                auto startedService = std::async([=]() { FormsService::start(mockRuntime); });
                expect(startedService)->toBeInstanceOf(FormsService);
            }
            );
            test(std::string("should register default contact template on creation"), [=]() mutable
            {
                auto templates = (as<object>(as<any>(service)))["templates"];
                expect(templates->has(std::string("contact")))->toBe(true);
                auto contactTemplate = templates->get(std::string("contact"));
                expect(contactTemplate)->toBeDefined();
                expect(contactTemplate->name)->toBe(std::string("contact"));
                expect(contactTemplate->steps)->toHaveLength(1);
                expect(const_(contactTemplate->steps)[0]->fields)->toHaveLength(3);
            }
            );
        }
        );
        describe(std::string("Form creation"), [=]() mutable
        {
            test(std::string("should create a form from template"), [=]() mutable
            {
                auto form = std::async([=]() { service->createForm(std::string("contact")); });
                expect(form)->toBeDefined();
                expect(form->id)->toBeDefined();
                expect(form->agentId)->toBe(mockRuntime->agentId);
                expect(form->status)->toBe(std::string("active"));
                expect(form->name)->toBe(std::string("contact"));
                expect(form->steps)->toHaveLength(1);
                expect(form->currentStepIndex)->toBe(0);
                expect(form->createdAt)->toBeDefined();
            }
            );
            test(std::string("should create a custom form"), [=]() mutable
            {
                auto customForm = object{
                    object::pair{std::string("name"), std::string("custom-form")}, 
                    object::pair{std::string("agentId"), mockRuntime->agentId}, 
                    object::pair{std::string("steps"), array<object>{ object{
                        object::pair{std::string("id"), std::string("step1")}, 
                        object::pair{std::string("name"), std::string("Step 1")}, 
                        object::pair{std::string("fields"), array<object>{ object{
                            object::pair{std::string("id"), std::string("field1")}, 
                            object::pair{std::string("label"), std::string("Field 1")}, 
                            object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("text"))}
                        } }}
                    } }}
                };
                auto form = std::async([=]() { service->createForm(customForm); });
                expect(form)->toBeDefined();
                expect(form->name)->toBe(std::string("custom-form"));
                expect(form->steps)->toHaveLength(1);
                expect(const_(form->steps)[0]->fields)->toHaveLength(1);
            }
            );
            test(std::string("should throw error for non-existent template"), [=]() mutable
            {
                std::async([=]() { expect(service->createForm(std::string("non-existent")))->rejects->toThrow(std::string("Template "non-existent" not found")); });
            }
            );
        }
        );
        describe(std::string("Form updates"), [=]() mutable
        {
            shared<std::shared_ptr<Form>> testForm;
            beforeEach([=]() mutable
            {
                testForm = std::async([=]() { service->createForm(std::string("contact")); });
            }
            );
            test(std::string("should update form fields with extracted values"), [=]() mutable
            {
                auto message = createMockMemory(std::string("My name is John Doe"));
                (as<ReturnType<mock>>(mockRuntime->useModel))["mockResolvedValueOnce"](std::string("{"name": "John Doe"}"));
                auto result = std::async([=]() { service->updateForm(testForm->id, message); });
                expect(result->success)->toBe(true);
                expect(result->updatedFields)->toContain(std::string("name"));
                auto updatedForm = std::async([=]() { service->getForm(testForm->id); });
                auto nameField = const_(updatedForm->steps)[0]->fields->find([=](auto f) mutable
                {
                    return f->id == std::string("name");
                }
                );
                expect(nameField->value)->toBe(std::string("John Doe"));
            }
            );
            test(std::string("should progress to next step when all required fields are filled"), [=]() mutable
            {
                auto multiStepForm = std::async([=]() { service->createForm(object{
                    object::pair{std::string("name"), std::string("multi-step")}, 
                    object::pair{std::string("agentId"), mockRuntime->agentId}, 
                    object::pair{std::string("steps"), array<object>{ object{
                        object::pair{std::string("id"), std::string("step1")}, 
                        object::pair{std::string("name"), std::string("Step 1")}, 
                        object::pair{std::string("fields"), array<object>{ object{
                            object::pair{std::string("id"), std::string("field1")}, 
                            object::pair{std::string("label"), std::string("Field 1")}, 
                            object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("text"))}
                        } }}
                    }, object{
                        object::pair{std::string("id"), std::string("step2")}, 
                        object::pair{std::string("name"), std::string("Step 2")}, 
                        object::pair{std::string("fields"), array<object>{ object{
                            object::pair{std::string("id"), std::string("field2")}, 
                            object::pair{std::string("label"), std::string("Field 2")}, 
                            object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("text"))}
                        } }}
                    } }}
                }); });
                (as<ReturnType<mock>>(mockRuntime->useModel))["mockResolvedValueOnce"](std::string("{"field1": "value1"}"));
                auto result = std::async([=]() { service->updateForm(multiStepForm->id, createMockMemory(std::string("value1"))); });
                expect(result->success)->toBe(true);
                expect(result->stepCompleted)->toBe(true);
                auto updatedForm = std::async([=]() { service->getForm(multiStepForm->id); });
                expect(updatedForm->currentStepIndex)->toBe(1);
            }
            );
            test(std::string("should mark form as completed when all steps are done"), [=]() mutable
            {
                (as<ReturnType<mock>>(mockRuntime->useModel))["mockResolvedValueOnce"](std::string("{"name": "John Doe"}"))["mockResolvedValueOnce"](std::string("{"email": "john@example.com"}"));
                std::async([=]() { service->updateForm(testForm->id, createMockMemory(std::string("John Doe"))); });
                std::async([=]() { service->updateForm(testForm->id, createMockMemory(std::string("john@example.com"))); });
                auto completedForm = std::async([=]() { service->getForm(testForm->id); });
                expect(completedForm->status)->toBe(std::string("completed"));
            }
            );
            test(std::string("should handle form not found"), [=]() mutable
            {
                auto result = std::async([=]() { service->updateForm(asUUID(uuidv4()), createMockMemory(std::string("test"))); });
                expect(result->success)->toBe(false);
                expect(result->message)->toBe(std::string("Form not found"));
            }
            );
            test(std::string("should handle already completed forms"), [=]() mutable
            {
                auto forms = (as<object>(as<any>(service)))["forms"];
                auto formData = forms->get(testForm->id);
                if (formData) {
                    formData->status = std::string("completed");
                }
                auto result = std::async([=]() { service->updateForm(testForm->id, createMockMemory(std::string("test"))); });
                expect(result->success)->toBe(false);
                expect(result->message)->toBe(std::string("Form is not active"));
            }
            );
        }
        );
        describe(std::string("Form cancellation"), [=]() mutable
        {
            test(std::string("should cancel an active form"), [=]() mutable
            {
                auto form = std::async([=]() { service->createForm(std::string("contact")); });
                auto result = std::async([=]() { service->cancelForm(form->id); });
                expect(result)->toBe(true);
                auto cancelledForm = std::async([=]() { service->getForm(form->id); });
                expect(cancelledForm->status)->toBe(std::string("cancelled"));
            }
            );
            test(std::string("should return false for non-existent form"), [=]() mutable
            {
                auto result = std::async([=]() { service->cancelForm(asUUID(uuidv4())); });
                expect(result)->toBe(false);
            }
            );
        }
        );
        describe(std::string("Form listing"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                std::async([=]() { service->createForm(std::string("contact")); });
                auto form2 = std::async([=]() { service->createForm(std::string("contact")); });
                std::async([=]() { service->cancelForm(form2->id); });
                auto form3 = std::async([=]() { service->createForm(std::string("contact")); });
                auto forms = (as<object>(as<any>(service)))["forms"];
                auto form3Data = forms->get(form3->id);
                if (form3Data) {
                    form3Data->status = std::string("completed");
                }
            }
            );
            test(std::string("should list forms by status"), [=]() mutable
            {
                auto activeForms = std::async([=]() { service->listForms(std::string("active")); });
                auto cancelledForms = std::async([=]() { service->listForms(std::string("cancelled")); });
                auto completedForms = std::async([=]() { service->listForms(std::string("completed")); });
                expect(activeForms)->toHaveLength(1);
                expect(cancelledForms)->toHaveLength(1);
                expect(completedForms)->toHaveLength(1);
            }
            );
            test(std::string("should list all forms when no status specified"), [=]() mutable
            {
                auto allForms = std::async([=]() { service->listForms(); });
                expect(allForms)->toHaveLength(3);
            }
            );
        }
        );
        describe(std::string("Template management"), [=]() mutable
        {
            test(std::string("should register a new template"), [=]() mutable
            {
                auto template = object{
                    object::pair{std::string("name"), std::string("custom-template")}, 
                    object::pair{std::string("steps"), array<object>{ object{
                        object::pair{std::string("id"), std::string("step1")}, 
                        object::pair{std::string("name"), std::string("Step 1")}, 
                        object::pair{std::string("fields"), array<object>{ object{
                            object::pair{std::string("id"), std::string("field1")}, 
                            object::pair{std::string("label"), std::string("Field 1")}, 
                            object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("text"))}
                        } }}
                    } }}
                };
                service->registerTemplate(template);
                auto registeredTemplate = (as<object>(as<any>(service)))["templates"]->get(std::string("custom-template"));
                expect(registeredTemplate)->toEqual(template);
            }
            );
        }
        );
        describe(std::string("Cleanup"), [=]() mutable
        {
            test(std::string("should remove old completed and cancelled forms"), [=]() mutable
            {
                auto form1 = std::async([=]() { service->createForm(std::string("contact")); });
                auto form2 = std::async([=]() { service->createForm(std::string("contact")); });
                auto oldTimestamp = Date->now() - 25 * 60 * 60 * 1000;
                auto forms = (as<object>(as<any>(service)))["forms"];
                auto form1Data = forms->get(form1->id);
                if (form1Data) {
                    form1Data->status = std::string("completed");
                    form1Data->updatedAt = oldTimestamp;
                }
                auto form2Data = forms->get(form2->id);
                if (form2Data) {
                    form2Data->status = std::string("cancelled");
                    form2Data->updatedAt = oldTimestamp;
                }
                auto form3 = std::async([=]() { service->createForm(std::string("contact")); });
                auto form3Data = forms->get(form3->id);
                if (form3Data) {
                    form3Data->status = std::string("completed");
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
        describe(std::string("Secret field handling"), [=]() mutable
        {
            test(std::string("should extract values for secret fields"), [=]() mutable
            {
                auto formWithSecret = std::async([=]() { service->createForm(object{
                    object::pair{std::string("name"), std::string("api-form")}, 
                    object::pair{std::string("agentId"), mockRuntime->agentId}, 
                    object::pair{std::string("steps"), array<object>{ object{
                        object::pair{std::string("id"), std::string("credentials")}, 
                        object::pair{std::string("name"), std::string("Credentials")}, 
                        object::pair{std::string("fields"), array<object>{ object{
                            object::pair{std::string("id"), std::string("apiKey")}, 
                            object::pair{std::string("label"), std::string("API Key")}, 
                            object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("text"))}, 
                            object::pair{std::string("secret"), true}
                        } }}
                    } }}
                }); });
                (as<ReturnType<mock>>(mockRuntime->useModel))["mockResolvedValueOnce"](std::string("{"apiKey": "sk-12345"}"));
                auto result = std::async([=]() { service->updateForm(formWithSecret->id, createMockMemory(std::string("My API key is sk-12345"))); });
                expect(result->success)->toBe(true);
                expect(mockRuntime->useModel)->toHaveBeenCalled();
                auto updatedForm = std::async([=]() { service->getForm(formWithSecret->id); });
                auto apiKeyField = const_(updatedForm->steps)[0]->fields->find([=](auto f) mutable
                {
                    return f->id == std::string("apiKey");
                }
                );
                expect(apiKeyField->value)->toBeTruthy();
                expect(type_of(apiKeyField->value))->toBe(std::string("string"));
                expect((as<string>(apiKeyField->value))->includes(std::string(":")))->toBe(true);
                expect(apiKeyField->value)->not->toBe(std::string("sk-12345"));
            }
            );
        }
        );
        describe(std::string("Database persistence"), [=]() mutable
        {
            test(std::string("should handle missing database tables gracefully"), [=]() mutable
            {
                auto runtime = as<std::shared_ptr<IAgentRuntime>>(as<any>(object{
                    object::pair{std::string("agentId"), asUUID(uuidv4())}, 
                    object::pair{std::string("character"), object{
                        object::pair{std::string("name"), std::string("TestAgent")}
                    }}, 
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
                        return Promise->resolve(std::string("{}"));
                    }
                    )}
                }));
                (as<any>(runtime))["adapter"] = object{
                    object::pair{std::string("fetch"), mock([=]() mutable
                    {
                        return Promise->resolve(object{
                            object::pair{std::string("rows"), array<any>()}
                        });
                    }
                    )}, 
                    object::pair{std::string("run"), mock([=]() mutable
                    {
                        return Promise->reject(std::make_shared<Error>(std::string("relation "forms" does not exist")));
                    }
                    )}
                };
                auto formsService = std::async([=]() { FormsService::start(runtime); });
                expect(formsService)->toBeInstanceOf(FormsService);
                auto form = std::async([=]() { (as<std::shared_ptr<FormsService>>(formsService))->createForm(std::string("contact")); });
                expect(form)->toBeTruthy();
                expect(form->name)->toBe(std::string("contact"));
            }
            );
            test(std::string("should persist forms when database is available"), [=]() mutable
            {
                auto runtime = as<std::shared_ptr<IAgentRuntime>>(as<any>(object{
                    object::pair{std::string("agentId"), asUUID(uuidv4())}, 
                    object::pair{std::string("character"), object{
                        object::pair{std::string("name"), std::string("TestAgent")}
                    }}, 
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
                        return Promise->resolve(std::string("{}"));
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
                        object::pair{std::string("rows"), array<object>{ object{
                            object::pair{std::string("exists"), true}
                        } }}
                    });
                }
                );
                (as<any>(runtime))["getDatabase"] = mock([=]() mutable
                {
                    return Promise->resolve(object{
                        object::pair{std::string("fetch"), mockFetch}, 
                        object::pair{std::string("run"), mockRun}
                    });
                }
                );
                auto formsService = as<std::shared_ptr<FormsService>>((std::async([=]() { FormsService::start(runtime); })));
                std::async([=]() { formsService->createForm(std::string("contact")); });
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
        describe(std::string("Zod validation"), [=]() mutable
        {
            test(std::string("should validate field values according to type"), [=]() mutable
            {
                auto service = as<std::shared_ptr<FormsService>>((std::async([=]() { FormsService::start(mockRuntime); })));
                auto form = std::async([=]() { service->createForm(object{
                    object::pair{std::string("name"), std::string("validation-test")}, 
                    object::pair{std::string("agentId"), mockRuntime->agentId}, 
                    object::pair{std::string("steps"), array<object>{ object{
                        object::pair{std::string("id"), std::string("step1")}, 
                        object::pair{std::string("name"), std::string("Validation Test")}, 
                        object::pair{std::string("fields"), array<object>{ object{
                            object::pair{std::string("id"), std::string("email")}, 
                            object::pair{std::string("label"), std::string("Email")}, 
                            object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("email"))}
                        }, object{
                            object::pair{std::string("id"), std::string("age")}, 
                            object::pair{std::string("label"), std::string("Age")}, 
                            object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("number"))}
                        }, object{
                            object::pair{std::string("id"), std::string("website")}, 
                            object::pair{std::string("label"), std::string("Website")}, 
                            object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("url"))}
                        } }}
                    } }}
                }); });
                (as<ReturnType<mock>>(mockRuntime->useModel))["mockResolvedValueOnce"](std::string("{"email": "not-an-email"}"));
                auto result1 = std::async([=]() { service->updateForm(form->id, createMockMemory(std::string("My email is not-an-email"))); });
                expect(result1->success)->toBe(true);
                expect(mockRuntime->useModel)->toHaveBeenCalled();
                (as<ReturnType<mock>>(mockRuntime->useModel))["mockResolvedValueOnce"](std::string("{"email": "test@example.com", "age": 25, "website": "https://example.com"}"));
                auto result2 = std::async([=]() { service->updateForm(form->id, createMockMemory(std::string("Email test@example.com, age 25, website https://example.com"))); });
                expect(result2->errors)->toHaveLength(0);
                expect(result2->updatedFields)->toHaveLength(3);
            }
            );
            test(std::string("should handle falsy values correctly"), [=]() mutable
            {
                auto service = as<std::shared_ptr<FormsService>>((std::async([=]() { FormsService::start(mockRuntime); })));
                auto form = std::async([=]() { service->createForm(object{
                    object::pair{std::string("name"), std::string("falsy-test")}, 
                    object::pair{std::string("agentId"), mockRuntime->agentId}, 
                    object::pair{std::string("steps"), array<object>{ object{
                        object::pair{std::string("id"), std::string("step1")}, 
                        object::pair{std::string("name"), std::string("Falsy Test")}, 
                        object::pair{std::string("fields"), array<object>{ object{
                            object::pair{std::string("id"), std::string("enabled")}, 
                            object::pair{std::string("label"), std::string("Enabled")}, 
                            object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("checkbox"))}
                        }, object{
                            object::pair{std::string("id"), std::string("count")}, 
                            object::pair{std::string("label"), std::string("Count")}, 
                            object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("number"))}
                        }, object{
                            object::pair{std::string("id"), std::string("message")}, 
                            object::pair{std::string("label"), std::string("Message")}, 
                            object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("text"))}, 
                            object::pair{std::string("optional"), true}
                        } }}
                    } }}
                }); });
                (as<ReturnType<mock>>(mockRuntime->useModel))["mockResolvedValueOnce"](std::string("{"enabled": false, "count": 0, "message": ""}"));
                auto result = std::async([=]() { service->updateForm(form->id, createMockMemory(std::string("Disabled, count 0, no message"))); });
                expect(result->success)->toBe(true);
                expect(result->updatedFields)->toHaveLength(3);
                auto updatedForm = std::async([=]() { service->getForm(form->id); });
                auto enabledField = const_(updatedForm->steps)[0]->fields->find([=](auto f) mutable
                {
                    return f->id == std::string("enabled");
                }
                );
                auto countField = const_(updatedForm->steps)[0]->fields->find([=](auto f) mutable
                {
                    return f->id == std::string("count");
                }
                );
                auto messageField = const_(updatedForm->steps)[0]->fields->find([=](auto f) mutable
                {
                    return f->id == std::string("message");
                }
                );
                expect(enabledField->value)->toBe(false);
                expect(countField->value)->toBe(0);
                expect(messageField->value)->toBe(string_empty);
            }
            );
        }
        );
        describe(std::string("Transaction safety"), [=]() mutable
        {
            test(std::string("should rollback on database errors"), [=]() mutable
            {
                auto runtime = as<std::shared_ptr<IAgentRuntime>>(as<any>(object{
                    object::pair{std::string("agentId"), asUUID(uuidv4())}, 
                    object::pair{std::string("character"), object{
                        object::pair{std::string("name"), std::string("TestAgent")}
                    }}, 
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
                        return Promise->resolve(std::string("{}"));
                    }
                    )}
                }));
                shared mockRun = mock([=](auto query) mutable
                {
                    if (query == std::string("BEGIN")) {
                        return Promise->resolve();
                    }
                    if (query == std::string("COMMIT")) {
                        return Promise->resolve();
                    }
                    if (query == std::string("ROLLBACK")) {
                        return Promise->resolve();
                    }
                    if (query->includes(std::string("INSERT INTO forms"))) {
                        return Promise->reject(std::make_shared<Error>(std::string("Database error")));
                    }
                    return Promise->resolve();
                }
                );
                (as<any>(runtime))["getDatabase"] = mock([=]() mutable
                {
                    return Promise->resolve(object{
                        object::pair{std::string("fetch"), mock([=]() mutable
                        {
                            return Promise->resolve(object{
                                object::pair{std::string("rows"), array<object>{ object{
                                    object::pair{std::string("exists"), true}
                                } }}
                            });
                        }
                        )}, 
                        object::pair{std::string("run"), mockRun}
                    });
                }
                );
                auto formsService = as<std::shared_ptr<FormsService>>((std::async([=]() { FormsService::start(runtime); })));
                std::async([=]() { formsService->createForm(std::string("contact")); });
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
