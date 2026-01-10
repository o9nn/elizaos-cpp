#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/tests/test-agent.test.h"

void Main(void)
{
    describe(std::string("Agent Tests"), [=]() mutable
    {
        shared<std::shared_ptr<SWEEnv>> dummyEnv;
        shared<std::shared_ptr<DefaultAgentConfig>> identityAgentConfig;
        shared<std::shared_ptr<DefaultAgentConfig>> thoughtActionAgentConfig;
        shared<std::shared_ptr<DefaultAgentConfig>> functionCallingAgentConfig;
        shared<std::shared_ptr<ToolConfig>> defaultToolConfig;
        beforeEach([=]() mutable
        {
            defaultToolConfig = object{
                object::pair{std::string("commands"), array<any>()}, 
                object::pair{std::string("parseFunction"), undefined}, 
                object::pair{std::string("executionTimeout"), 500}, 
                object::pair{std::string("maxConsecutiveExecutionTimeouts"), 5}, 
                object::pair{std::string("totalExecutionTimeout"), 5000}, 
                object::pair{std::string("submitCommand"), std::string("submit")}, 
                object::pair{std::string("useFunctionCalling"), false}, 
                object::pair{std::string("formatErrorTemplate"), std::string("Invalid format")}, 
                object::pair{std::string("envVariables"), object{}}
            };
            auto mockDeployment = object{
                object::pair{std::string("runtime"), object{
                    object::pair{std::string("createSession"), jest->fn([=]() mutable
                    {
                        return Promise->resolve();
                    }
                    )}, 
                    object::pair{std::string("runInSession"), jest->fn([=]() mutable
                    {
                        return Promise->resolve(object{
                            object::pair{std::string("output"), string_empty}, 
                            object::pair{std::string("exitCode"), 0}
                        });
                    }
                    )}, 
                    object::pair{std::string("execute"), jest->fn([=]() mutable
                    {
                        return Promise->resolve(object{
                            object::pair{std::string("exitCode"), 0}, 
                            object::pair{std::string("stdout"), string_empty}, 
                            object::pair{std::string("stderr"), string_empty}
                        });
                    }
                    )}, 
                    object::pair{std::string("readFile"), jest->fn([=]() mutable
                    {
                        return Promise->resolve(object{
                            object::pair{std::string("content"), string_empty}
                        });
                    }
                    )}, 
                    object::pair{std::string("writeFile"), jest->fn([=]() mutable
                    {
                        return Promise->resolve();
                    }
                    )}, 
                    object::pair{std::string("upload"), jest->fn([=]() mutable
                    {
                        return Promise->resolve();
                    }
                    )}
                }}, 
                object::pair{std::string("start"), jest->fn([=]() mutable
                {
                    return Promise->resolve();
                }
                )}, 
                object::pair{std::string("stop"), jest->fn([=]() mutable
                {
                    return Promise->resolve();
                }
                )}
            };
            dummyEnv = std::make_shared<SWEEnv>(object{
                object::pair{std::string("deployment"), as<any>(mockDeployment)}, 
                object::pair{std::string("repo"), undefined}, 
                object::pair{std::string("postStartupCommands"), array<any>()}
            });
            jest->spyOn(dummyEnv, std::string("readFile"))->mockImplementation([=](auto path) mutable
            {
                if (OR((path->includes(std::string("model.patch"))), (path->includes(std::string("test.patch"))))) {
                    return Promise->resolve(string_empty);
                }
                return Promise->resolve(std::string("test"));
            }
            );
            jest->spyOn(dummyEnv, std::string("writeFile"))->mockImplementation([=]() mutable
            {
                return Promise->resolve();
            }
            );
            jest->spyOn(dummyEnv, std::string("communicate"))->mockImplementation([=]() mutable
            {
                return Promise->resolve(string_empty);
            }
            );
            identityAgentConfig = object{
                object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("default"))}, 
                object::pair{std::string("name"), std::string("test-identity-agent")}, 
                object::pair{std::string("model"), object{
                    object::pair{std::string("name"), std::string("instant_empty_submit")}
                }}, 
                object::pair{std::string("templates"), as<any>(DEFAULT_TEMPLATE_CONFIG)}, 
                object::pair{std::string("tools"), utils::assign(object{
                    , 
                    object::pair{std::string("parseFunction"), std::string("identity")}
                }, defaultToolConfig)}, 
                object::pair{std::string("historyProcessors"), array<any>()}, 
                object::pair{std::string("maxRequeries"), 3}
            };
            thoughtActionAgentConfig = object{
                object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("default"))}, 
                object::pair{std::string("name"), std::string("test-thought-action-agent")}, 
                object::pair{std::string("model"), object{
                    object::pair{std::string("name"), std::string("instant_empty_submit")}
                }}, 
                object::pair{std::string("templates"), as<any>(DEFAULT_TEMPLATE_CONFIG)}, 
                object::pair{std::string("tools"), utils::assign(object{
                    , 
                    object::pair{std::string("parseFunction"), std::string("thought_action")}
                }, defaultToolConfig)}, 
                object::pair{std::string("historyProcessors"), array<any>()}, 
                object::pair{std::string("maxRequeries"), 3}
            };
            functionCallingAgentConfig = object{
                object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("default"))}, 
                object::pair{std::string("name"), std::string("test-function-calling-agent")}, 
                object::pair{std::string("model"), object{
                    object::pair{std::string("name"), std::string("instant_empty_submit")}
                }}, 
                object::pair{std::string("templates"), as<any>(DEFAULT_TEMPLATE_CONFIG)}, 
                object::pair{std::string("tools"), utils::assign(object{
                    , 
                    object::pair{std::string("parseFunction"), std::string("function_calling")}, 
                    object::pair{std::string("useFunctionCalling"), true}
                }, defaultToolConfig)}, 
                object::pair{std::string("historyProcessors"), array<any>()}, 
                object::pair{std::string("maxRequeries"), 3}
            };
        }
        );
        describe(std::string("Exit conditions"), [=]() mutable
        {
            it(std::string("should exit on cost limit"), [=]() mutable
            {
                auto agent = DefaultAgent::fromConfig(identityAgentConfig);
                agent->model = std::make_shared<PredeterminedTestModel>(array<string>{ std::string("raise_cost") }, defaultToolConfig);
                jest->spyOn(agent, std::string("attemptAutosubmissionAfterError"))->mockImplementation([=](auto step) mutable
                {
                    return step;
                }
                );
                auto result = std::async([=]() { agent->run(dummyEnv, std::make_shared<EmptyProblemStatement>(), std::string("/tmp/test")); });
                expect(result->info->exitStatus)->toBe(std::string("exit_cost"));
            }
            );
            it(std::string("should exit on context limit"), [=]() mutable
            {
                auto agent = DefaultAgent::fromConfig(identityAgentConfig);
                agent->model = std::make_shared<PredeterminedTestModel>(array<string>{ std::string("raise_context") }, defaultToolConfig);
                jest->spyOn(agent, std::string("attemptAutosubmissionAfterError"))->mockImplementation([=](auto step) mutable
                {
                    return step;
                }
                );
                auto result = std::async([=]() { agent->run(dummyEnv, std::make_shared<EmptyProblemStatement>(), std::string("/tmp/test")); });
                expect(result->info->exitStatus)->toBe(std::string("exit_context"));
            }
            );
            it(std::string("should exit on model error"), [=]() mutable
            {
                auto agent = DefaultAgent::fromConfig(identityAgentConfig);
                agent->model = std::make_shared<PredeterminedTestModel>(array<string>{ std::string("raise_runtime") }, defaultToolConfig);
                jest->spyOn(agent, std::string("attemptAutosubmissionAfterError"))->mockImplementation([=](auto step) mutable
                {
                    return step;
                }
                );
                auto result = std::async([=]() { agent->run(dummyEnv, std::make_shared<EmptyProblemStatement>(), std::string("/tmp/test")); });
                expect(result->info->exitStatus)->toBe(std::string("exit_environment_error"));
            }
            );
            it(std::string("should exit on format error"), [=]() mutable
            {
                auto agent = DefaultAgent::fromConfig(thoughtActionAgentConfig);
                agent->model = std::make_shared<PredeterminedTestModel>(array<string>{ std::string("a"), std::string("b"), std::string("c"), std::string("d") }, defaultToolConfig);
                jest->spyOn(agent, std::string("attemptAutosubmissionAfterError"))->mockImplementation([=](auto step) mutable
                {
                    return step;
                }
                );
                auto result = std::async([=]() { agent->run(dummyEnv, std::make_shared<EmptyProblemStatement>(), std::string("/tmp/test")); });
                expect(result->info->exitStatus)->toBe(std::string("exit_format"));
            }
            );
            it(std::string("should exit on blocklist"), [=]() mutable
            {
                auto configWithBlocklist = utils::assign(object{
                    , 
                    object::pair{std::string("tools"), utils::assign(object{
                        , 
                        object::pair{std::string("parseFunction"), std::string("identity")}, 
                        object::pair{std::string("filter"), object{
                            object::pair{std::string("blocklist"), array<string>{ std::string("vim"), std::string("python"), std::string("su"), std::string("nano") }}, 
                            object::pair{std::string("blocklistStandalone"), array<any>()}, 
                            object::pair{std::string("blocklistErrorTemplate"), std::string("Command {{action}} is not allowed")}
                        }}
                    }, defaultToolConfig)}
                }, identityAgentConfig);
                auto agent = DefaultAgent::fromConfig(configWithBlocklist);
                agent->model = std::make_shared<PredeterminedTestModel>(array<string>{ std::string("vim"), std::string("python"), std::string("su"), std::string("nano") }, defaultToolConfig);
                jest->spyOn(agent, std::string("attemptAutosubmissionAfterError"))->mockImplementation([=](auto step) mutable
                {
                    return step;
                }
                );
                auto result = std::async([=]() { agent->run(dummyEnv, std::make_shared<EmptyProblemStatement>(), std::string("/tmp/test")); });
                expect(result->info->exitStatus)->toBe(std::string("exit_format"));
            }
            );
        }
        );
        describe(std::string("Step execution"), [=]() mutable
        {
            it(std::string("should run step by step checking history"), [=]() mutable
            {
                auto agent = DefaultAgent::fromConfig(identityAgentConfig);
                agent->model = std::make_shared<PredeterminedTestModel>(array<string>{ std::string("asdf"), std::string("```\
ls\
```"), std::string("```\
echo "asdf"\
```"), std::string("raise_cost") }, defaultToolConfig);
                jest->spyOn(agent, std::string("attemptAutosubmissionAfterError"))->mockImplementation([=](auto step) mutable
                {
                    return step;
                }
                );
                std::async([=]() { agent->setup(dummyEnv, std::make_shared<TextProblemStatement>(object{
                    object::pair{std::string("text"), std::string("asdf123")}
                })); });
                shared mockIndex = 0;
                shared mockOutputs = array<object>{ object{
                    object::pair{std::string("output"), std::string("file_a file_b")}, 
                    object::pair{std::string("exitCode"), 0}
                }, object{
                    object::pair{std::string("output"), string_empty}, 
                    object::pair{std::string("exitCode"), 0}
                }, object{
                    object::pair{std::string("output"), std::string("asdf")}, 
                    object::pair{std::string("exitCode"), 0}
                }, object{
                    object::pair{std::string("output"), string_empty}, 
                    object::pair{std::string("exitCode"), 0}
                } };
                jest->spyOn(dummyEnv->deployment->runtime, std::string("runInSession"))->mockImplementation([=]() mutable
                {
                    return const_(mockOutputs)[mockIndex++];
                }
                );
                expect(agent->history->get_length())->toBeGreaterThan(0);
                expect(agent->trajectory->get_length())->toBe(0);
                std::async([=]() { agent->step(); });
                expect(agent->trajectory->get_length())->toBeGreaterThanOrEqual(1);
                std::async([=]() { agent->step(); });
                expect(agent->trajectory->get_length())->toBeGreaterThanOrEqual(2);
                std::async([=]() { agent->step(); });
                expect(agent->trajectory->get_length())->toBeGreaterThanOrEqual(3);
                auto finalStep = std::async([=]() { agent->step(); });
                expect(OR((finalStep->exitStatus), (agent->info->exitStatus)))->toBe(std::string("exit_cost"));
            }
            );
        }
        );
        describe(std::string("Submission"), [=]() mutable
        {
            it(std::string("should handle successful submission"), [=]() mutable
            {
                auto agent = DefaultAgent::fromConfig(identityAgentConfig);
                agent->model = std::make_shared<PredeterminedTestModel>(array<string>{ std::string("```\
submit\
```") }, defaultToolConfig);
                std::async([=]() { agent->setup(dummyEnv, std::make_shared<EmptyProblemStatement>()); });
                jest->spyOn(dummyEnv, std::string("communicate"))->mockImplementation([=](auto command) mutable
                {
                    if (command->trim() == std::string("submit")) {
                        return std::string("<<SWE_AGENT_SUBMISSION>>");
                    }
                    return string_empty;
                }
                );
                jest->spyOn(dummyEnv, std::string("readFile"))->mockImplementation([=](auto path) mutable
                {
                    if (path->includes(std::string("model.patch"))) {
                        return std::string("test");
                    }
                    return string_empty;
                }
                );
                auto result = std::async([=]() { agent->step(); });
                expect(OR((result->exitStatus), (agent->info->exitStatus)))->toBe(std::string("submitted"));
                expect(OR((result->submission), (agent->info->submission)))->toBe(std::string("test"));
            }
            );
            it(std::string("should handle human exit command"), [=]() mutable
            {
                auto agent = DefaultAgent::fromConfig(identityAgentConfig);
                agent->model = std::make_shared<PredeterminedTestModel>(array<string>{ std::string("```\
exit\
```") }, defaultToolConfig);
                std::async([=]() { agent->setup(dummyEnv, std::make_shared<EmptyProblemStatement>()); });
                auto result = std::async([=]() { agent->step(); });
                expect(result->done)->toBe(true);
                expect(result->exitStatus)->toBe(std::string("exit_command"));
                expect(result->action->trim())->toBe(std::string("exit"));
            }
            );
        }
        );
        describe(std::string("Function calling"), [=]() mutable
        {
            it(std::string("should handle function calling format"), [=]() mutable
            {
                auto agent = DefaultAgent::fromConfig(functionCallingAgentConfig);
                auto validResponse = object{
                    object::pair{std::string("message"), std::string("I'll list the contents of the directory")}, 
                    object::pair{std::string("toolCalls"), array<object>{ object{
                        object::pair{std::string("type"), std::string("function")}, 
                        object::pair{std::string("function"), object{
                            object::pair{std::string("name"), std::string("bash")}, 
                            object::pair{std::string("arguments"), std::string("{"command": "ls"}")}
                        }}, 
                        object::pair{std::string("id"), std::string("abc123")}
                    } }}
                };
                agent->model = std::make_shared<PredeterminedTestModel>(array<std::shared_ptr<ModelOutput>>{ validResponse }, defaultToolConfig);
                std::async([=]() { agent->setup(dummyEnv, std::make_shared<EmptyProblemStatement>()); });
                jest->spyOn(dummyEnv, std::string("communicate"))->mockImplementation([=](auto command) mutable
                {
                    if (command->trim() == std::string("ls")) {
                        return std::string("file1 file2");
                    }
                    return string_empty;
                }
                );
                auto result = std::async([=]() { agent->step(); });
                expect(result->done)->toBe(false);
                expect(result->action->trim())->toBe(std::string("ls"));
                expect(OR((result->observation), (string_empty)))->toContain(std::string("file1 file2"));
            }
            );
        }
        );
        describe(std::string("Auto-submission"), [=]() mutable
        {
            it(std::string("should auto-submit when patch is found"), [=]() mutable
            {
                auto agent = DefaultAgent::fromConfig(identityAgentConfig);
                agent->model = std::make_shared<PredeterminedTestModel>(array<string>{ std::string("raise_cost") }, defaultToolConfig);
                std::async([=]() { agent->setup(dummyEnv, std::make_shared<EmptyProblemStatement>()); });
                std::bind(&SWEEnv::isAlive, dummyEnv) = as<any>(jest->fn()->mockReturnValue(true));
                jest->spyOn(dummyEnv, std::string("executeCommand"))->mockImplementation([=]() mutable
                {
                }
                );
                jest->spyOn(dummyEnv, std::string("writeFile"))->mockImplementation([=](auto path, auto _content) mutable
                {
                    if (path == std::string("/root/model.patch")) {
                    }
                }
                );
                jest->spyOn(dummyEnv, std::string("readFile"))->mockImplementation([=](auto path) mutable
                {
                    if (path == std::string("/root/model.patch")) {
                        return std::string("mysubmission");
                    }
                    return string_empty;
                }
                );
                jest->spyOn(dummyEnv->deployment->runtime, std::string("runInSession"))->mockImplementationOnce([=]() mutable
                {
                    return (object{
                        object::pair{std::string("output"), string_empty}, 
                        object::pair{std::string("exitCode"), 0}
                    });
                }
                )->mockImplementationOnce([=]() mutable
                {
                    return (object{
                        object::pair{std::string("output"), std::string("<<SWE_AGENT_SUBMISSION>>\
mysubmission\
<<SWE_AGENT_SUBMISSION>>")}, 
                        object::pair{std::string("exitCode"), 0}
                    });
                }
                );
                auto result = std::async([=]() { agent->step(); });
                expect(agent->info)->toBeDefined();
                expect(agent->info->exitStatus)->toBe(std::string("submitted (exit_cost)"));
                expect(agent->info->submission)->toBe(std::string("mysubmission"));
                expect(result->done)->toBe(true);
                expect(result->submission)->toBe(std::string("mysubmission"));
                expect(result->exitStatus)->toBe(std::string("submitted (exit_cost)"));
            }
            );
        }
        );
        describe(std::string("Template handling"), [=]() mutable
        {
            it(std::string("should use no output template when appropriate"), [=]() mutable
            {
                auto agent = DefaultAgent::fromConfig(identityAgentConfig);
                agent->templates->nextStepNoOutputTemplate = std::string("no output template");
                std::async([=]() { agent->setup(dummyEnv, std::make_shared<EmptyProblemStatement>()); });
                agent->model = std::make_shared<PredeterminedTestModel>(array<string>{ std::string("```\
ls\
```"), std::string("```\
test\
```") }, defaultToolConfig);
                jest->spyOn(dummyEnv->deployment->runtime, std::string("runInSession"))->mockImplementation([=]() mutable
                {
                    return (object{
                        object::pair{std::string("output"), string_empty}, 
                        object::pair{std::string("exitCode"), 0}
                    });
                }
                );
                std::async([=]() { agent->step(); });
                std::async([=]() { agent->step(); });
                auto lastMessage = (*const_(agent->history))[agent->history->get_length() - 1];
                expect(lastMessage->content)->toContain(std::string("no output template"));
            }
            );
        }
        );
        describe(std::string("Early exit conditions"), [=]() mutable
        {
            it(std::string("should handle early environment error"), [=]() mutable
            {
                auto agent = DefaultAgent::fromConfig(identityAgentConfig);
                agent->model = std::make_shared<PredeterminedTestModel>(array<string>{ std::string("raise") }, defaultToolConfig);
                agent->_catchErrors = true;
                jest->spyOn(dummyEnv, std::string("communicate"))->mockImplementation([=](auto command) mutable
                {
                    if (command->trim() == std::string("raise")) {
                        throw any(std::make_shared<Error>(std::string("SwerexException")));
                    }
                    return string_empty;
                }
                );
                auto result = std::async([=]() { agent->run(dummyEnv, std::make_shared<EmptyProblemStatement>(), std::string("/tmp/test")); });
                expect(result->info->exitStatus)->toBe(std::string("exit_environment_error"));
            }
            );
        }
        );
        describe(std::string("Action parsing"), [=]() mutable
        {
            it(std::string("should handle various action formats"), [=]() mutable
            {
                auto agent = DefaultAgent::fromConfig(identityAgentConfig);
                auto testActions = array<string>{ std::string("simple_command"), std::string("command with args"), std::string("command "with quotes""), std::string("command | pipe"), std::string("multi\
line\
command") };
                for (auto& action : testActions)
                {
                    agent->model = std::make_shared<PredeterminedTestModel>(array<string>{ action }, defaultToolConfig);
                    std::async([=]() { agent->setup(dummyEnv, std::make_shared<EmptyProblemStatement>()); });
                    auto result = std::async([=]() { agent->step(); });
                    expect(result->action)->toBe(action);
                }
            }
            );
        }
        );
        describe(std::string("State management"), [=]() mutable
        {
            it(std::string("should track agent state correctly"), [=]() mutable
            {
                auto agent = DefaultAgent::fromConfig(identityAgentConfig);
                expect(agent->info)->toEqual(object{});
                expect(agent->trajectory)->toEqual(array<any>());
                std::async([=]() { agent->setup(dummyEnv, std::make_shared<EmptyProblemStatement>()); });
                expect(agent->history->get_length())->toBeGreaterThan(0);
                agent->model = std::make_shared<PredeterminedTestModel>(array<string>{ std::string("test_action") }, defaultToolConfig);
                std::async([=]() { agent->step(); });
                expect(agent->trajectory->get_length())->toBe(1);
                expect((*const_(agent->trajectory))[0])->toHaveProperty(std::string("action"), std::string("test_action"));
            }
            );
        }
        );
        describe(std::string("Cost tracking"), [=]() mutable
        {
            it(std::string("should track costs correctly"), [=]() mutable
            {
                auto agent = DefaultAgent::fromConfig(identityAgentConfig);
                agent->model = std::make_shared<PredeterminedTestModel>(array<string>{ std::string("action1"), std::string("action2") }, defaultToolConfig);
                agent->model->stats = as<any>(object{
                    object::pair{std::string("instanceCost"), 0}, 
                    object::pair{std::string("totalCost"), 0}, 
                    object::pair{std::string("tokensSent"), 0}, 
                    object::pair{std::string("tokensReceived"), 0}, 
                    object::pair{std::string("apiCalls"), 0}
                });
                std::async([=]() { agent->setup(dummyEnv, std::make_shared<EmptyProblemStatement>()); });
                agent->model->stats->instanceCost = 0.01;
                std::async([=]() { agent->step(); });
                agent->model->stats->instanceCost = 0.02;
                std::async([=]() { agent->step(); });
                expect(agent->info->modelStats)->toBeDefined();
                expect(agent->info->modelStats->instanceCost)->toBe(0.02);
            }
            );
        }
        );
        describe(std::string("Retry logic"), [=]() mutable
        {
            it(std::string("should retry on format errors up to max retries"), [=]() mutable
            {
                auto agent = DefaultAgent::fromConfig(thoughtActionAgentConfig);
                agent->maxRequeries = 3;
                auto responses = array<string>{ std::string("invalid1"), std::string("invalid2"), std::string("invalid3"), std::string("invalid4") };
                agent->model = std::make_shared<PredeterminedTestModel>(responses, defaultToolConfig);
                std::async([=]() { agent->setup(dummyEnv, std::make_shared<EmptyProblemStatement>()); });
                std::async([=]() { agent->step(); });
                expect(agent->info->exitStatus)->toBe(std::string("exit_format"));
                expect(agent->trajectory->get_length())->toBeGreaterThanOrEqual(agent->maxRequeries);
            }
            );
        }
        );
    }
    );
}

MAIN
