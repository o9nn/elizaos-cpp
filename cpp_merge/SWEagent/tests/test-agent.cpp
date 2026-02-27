#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/tests/test-agent.test.h"

void Main(void)
{
    describe(std:("Agent Tests"), [=]() mutable
    {
        shared<std::shared_ptr<SWEEnv>> dummyEnv;
        shared<std::shared_ptr<DefaultAgentConfig>> identityAgentConfig;
        shared<std::shared_ptr<DefaultAgentConfig>> thoughtActionAgentConfig;
        shared<std::shared_ptr<DefaultAgentConfig>> functionCallingAgentConfig;
        shared<std::shared_ptr<ToolConfig>> defaultToolConfig;
        beforeEach([=]() mutable
        {
            defaultToolConfig = object{
                object::pair{std:("commands"), array<any>()}, 
                object::pair{std:("parseFunction"), undefined}, 
                object::pair{std:("executionTimeout"), 500}, 
                object::pair{std:("maxConsecutiveExecutionTimeouts"), 5}, 
                object::pair{std:("totalExecutionTimeout"), 5000}, 
                object::pair{std:("submitCommand"), std:("submit")}, 
                object::pair{std:("useFunctionCalling"), false}, 
                object::pair{std:("formatErrorTemplate"), std:("Invalid format")}, 
                object::pair{std:("envVariables"), object{}}
            };
            auto mockDeployment = object{
                object::pair{std:("runtime"), object{
                    object::pair{std:("createSession"), jest->fn([=]() mutable
                    {
                        return Promise->resolve();
                    }
                    )}, 
                    object::pair{std:("runInSession"), jest->fn([=]() mutable
                    {
                        return Promise->resolve(object{
                            object::pair{std:("output"), string_empty}, 
                            object::pair{std:("exitCode"), 0}
                        });
                    }
                    )}, 
                    object::pair{std:("execute"), jest->fn([=]() mutable
                    {
                        return Promise->resolve(object{
                            object::pair{std:("exitCode"), 0}, 
                            object::pair{std:("stdout"), string_empty}, 
                            object::pair{std:("stderr"), string_empty}
                        });
                    }
                    )}, 
                    object::pair{std:("readFile"), jest->fn([=]() mutable
                    {
                        return Promise->resolve(object{
                            object::pair{std:("content"), string_empty}
                        });
                    }
                    )}, 
                    object::pair{std:("writeFile"), jest->fn([=]() mutable
                    {
                        return Promise->resolve();
                    }
                    )}, 
                    object::pair{std:("upload"), jest->fn([=]() mutable
                    {
                        return Promise->resolve();
                    }
                    )}
                }}, 
                object::pair{std:("start"), jest->fn([=]() mutable
                {
                    return Promise->resolve();
                }
                )}, 
                object::pair{std:("stop"), jest->fn([=]() mutable
                {
                    return Promise->resolve();
                }
                )}
            };
            dummyEnv = std::make_shared<SWEEnv>(object{
                object::pair{std:("deployment"), as<any>(mockDeployment)}, 
                object::pair{std:("repo"), undefined}, 
                object::pair{std:("postStartupCommands"), array<any>()}
            });
            jest->spyOn(dummyEnv, std:("readFile"))->mockImplementation([=](auto path) mutable
            {
                if (OR((path->includes(std:("model.patch"))), (path->includes(std:("test.patch"))))) {
                    return Promise->resolve(string_empty);
                }
                return Promise->resolve(std:("test"));
            }
            );
            jest->spyOn(dummyEnv, std:("writeFile"))->mockImplementation([=]() mutable
            {
                return Promise->resolve();
            }
            );
            jest->spyOn(dummyEnv, std:("communicate"))->mockImplementation([=]() mutable
            {
                return Promise->resolve(string_empty);
            }
            );
            identityAgentConfig = object{
                object::pair{std:("type"), as<std::shared_ptr<const>>(std:("default"))}, 
                object::pair{std:("name"), std:("test-identity-agent")}, 
                object::pair{std:("model"), object{
                    object::pair{std:("name"), std:("instant_empty_submit")}
                }}, 
                object::pair{std:("templates"), as<any>(DEFAULT_TEMPLATE_CONFIG)}, 
                object::pair{std:("tools"), utils::assign(object{
                    , 
                    object::pair{std:("parseFunction"), std:("identity")}
                }, defaultToolConfig)}, 
                object::pair{std:("historyProcessors"), array<any>()}, 
                object::pair{std:("maxRequeries"), 3}
            };
            thoughtActionAgentConfig = object{
                object::pair{std:("type"), as<std::shared_ptr<const>>(std:("default"))}, 
                object::pair{std:("name"), std:("test-thought-action-agent")}, 
                object::pair{std:("model"), object{
                    object::pair{std:("name"), std:("instant_empty_submit")}
                }}, 
                object::pair{std:("templates"), as<any>(DEFAULT_TEMPLATE_CONFIG)}, 
                object::pair{std:("tools"), utils::assign(object{
                    , 
                    object::pair{std:("parseFunction"), std:("thought_action")}
                }, defaultToolConfig)}, 
                object::pair{std:("historyProcessors"), array<any>()}, 
                object::pair{std:("maxRequeries"), 3}
            };
            functionCallingAgentConfig = object{
                object::pair{std:("type"), as<std::shared_ptr<const>>(std:("default"))}, 
                object::pair{std:("name"), std:("test-function-calling-agent")}, 
                object::pair{std:("model"), object{
                    object::pair{std:("name"), std:("instant_empty_submit")}
                }}, 
                object::pair{std:("templates"), as<any>(DEFAULT_TEMPLATE_CONFIG)}, 
                object::pair{std:("tools"), utils::assign(object{
                    , 
                    object::pair{std:("parseFunction"), std:("function_calling")}, 
                    object::pair{std:("useFunctionCalling"), true}
                }, defaultToolConfig)}, 
                object::pair{std:("historyProcessors"), array<any>()}, 
                object::pair{std:("maxRequeries"), 3}
            };
        }
        );
        describe(std:("Exit conditions"), [=]() mutable
        {
            it(std:("should exit on cost limit"), [=]() mutable
            {
                auto agent = DefaultAgent::fromConfig(identityAgentConfig);
                agent->model = std::make_shared<PredeterminedTestModel>(array<string>{ std:("raise_cost") }, defaultToolConfig);
                jest->spyOn(agent, std:("attemptAutosubmissionAfterError"))->mockImplementation([=](auto step) mutable
                {
                    return step;
                }
                );
                auto result = std::async([=]() { agent->run(dummyEnv, std::make_shared<EmptyProblemStatement>(), std:("/tmp/test")); });
                expect(result->info->exitStatus)->toBe(std:("exit_cost"));
            }
            );
            it(std:("should exit on context limit"), [=]() mutable
            {
                auto agent = DefaultAgent::fromConfig(identityAgentConfig);
                agent->model = std::make_shared<PredeterminedTestModel>(array<string>{ std:("raise_context") }, defaultToolConfig);
                jest->spyOn(agent, std:("attemptAutosubmissionAfterError"))->mockImplementation([=](auto step) mutable
                {
                    return step;
                }
                );
                auto result = std::async([=]() { agent->run(dummyEnv, std::make_shared<EmptyProblemStatement>(), std:("/tmp/test")); });
                expect(result->info->exitStatus)->toBe(std:("exit_context"));
            }
            );
            it(std:("should exit on model error"), [=]() mutable
            {
                auto agent = DefaultAgent::fromConfig(identityAgentConfig);
                agent->model = std::make_shared<PredeterminedTestModel>(array<string>{ std:("raise_runtime") }, defaultToolConfig);
                jest->spyOn(agent, std:("attemptAutosubmissionAfterError"))->mockImplementation([=](auto step) mutable
                {
                    return step;
                }
                );
                auto result = std::async([=]() { agent->run(dummyEnv, std::make_shared<EmptyProblemStatement>(), std:("/tmp/test")); });
                expect(result->info->exitStatus)->toBe(std:("exit_environment_error"));
            }
            );
            it(std:("should exit on format error"), [=]() mutable
            {
                auto agent = DefaultAgent::fromConfig(thoughtActionAgentConfig);
                agent->model = std::make_shared<PredeterminedTestModel>(array<string>{ std:("a"), std:("b"), std:("c"), std:("d") }, defaultToolConfig);
                jest->spyOn(agent, std:("attemptAutosubmissionAfterError"))->mockImplementation([=](auto step) mutable
                {
                    return step;
                }
                );
                auto result = std::async([=]() { agent->run(dummyEnv, std::make_shared<EmptyProblemStatement>(), std:("/tmp/test")); });
                expect(result->info->exitStatus)->toBe(std:("exit_format"));
            }
            );
            it(std:("should exit on blocklist"), [=]() mutable
            {
                auto configWithBlocklist = utils::assign(object{
                    , 
                    object::pair{std:("tools"), utils::assign(object{
                        , 
                        object::pair{std:("parseFunction"), std:("identity")}, 
                        object::pair{std:("filter"), object{
                            object::pair{std:("blocklist"), array<string>{ std:("vim"), std:("python"), std:("su"), std:("nano") }}, 
                            object::pair{std:("blocklistStandalone"), array<any>()}, 
                            object::pair{std:("blocklistErrorTemplate"), std:("Command {{action}} is not allowed")}
                        }}
                    }, defaultToolConfig)}
                }, identityAgentConfig);
                auto agent = DefaultAgent::fromConfig(configWithBlocklist);
                agent->model = std::make_shared<PredeterminedTestModel>(array<string>{ std:("vim"), std:("python"), std:("su"), std:("nano") }, defaultToolConfig);
                jest->spyOn(agent, std:("attemptAutosubmissionAfterError"))->mockImplementation([=](auto step) mutable
                {
                    return step;
                }
                );
                auto result = std::async([=]() { agent->run(dummyEnv, std::make_shared<EmptyProblemStatement>(), std:("/tmp/test")); });
                expect(result->info->exitStatus)->toBe(std:("exit_format"));
            }
            );
        }
        );
        describe(std:("Step execution"), [=]() mutable
        {
            it(std:("should run step by step checking history"), [=]() mutable
            {
                auto agent = DefaultAgent::fromConfig(identityAgentConfig);
                agent->model = std::make_shared<PredeterminedTestModel>(array<string>{ std:("asdf"), std:(""""\
ls\
""""), std:(""""\
echo "asdf"\
""""), std:("raise_cost") }, defaultToolConfig);
                jest->spyOn(agent, std:("attemptAutosubmissionAfterError"))->mockImplementation([=](auto step) mutable
                {
                    return step;
                }
                );
                std::async([=]() { agent->setup(dummyEnv, std::make_shared<TextProblemStatement>(object{
                    object::pair{std:("text"), std:("asdf123")}
                })); });
                shared mockIndex = 0;
                shared mockOutputs = array<object>{ object{
                    object::pair{std:("output"), std:("file_a file_b")}, 
                    object::pair{std:("exitCode"), 0}
                }, object{
                    object::pair{std:("output"), string_empty}, 
                    object::pair{std:("exitCode"), 0}
                }, object{
                    object::pair{std:("output"), std:("asdf")}, 
                    object::pair{std:("exitCode"), 0}
                }, object{
                    object::pair{std:("output"), string_empty}, 
                    object::pair{std:("exitCode"), 0}
                } };
                jest->spyOn(dummyEnv->deployment->runtime, std:("runInSession"))->mockImplementation([=]() mutable
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
                expect(OR((finalStep->exitStatus), (agent->info->exitStatus)))->toBe(std:("exit_cost"));
            }
            );
        }
        );
        describe(std:("Submission"), [=]() mutable
        {
            it(std:("should handle successful submission"), [=]() mutable
            {
                auto agent = DefaultAgent::fromConfig(identityAgentConfig);
                agent->model = std::make_shared<PredeterminedTestModel>(array<string>{ std:(""""\
submit\
"""") }, defaultToolConfig);
                std::async([=]() { agent->setup(dummyEnv, std::make_shared<EmptyProblemStatement>()); });
                jest->spyOn(dummyEnv, std:("communicate"))->mockImplementation([=](auto command) mutable
                {
                    if (command->trim() == std:("submit")) {
                        return std:("<<SWE_AGENT_SUBMISSION>>");
                    }
                    return string_empty;
                }
                );
                jest->spyOn(dummyEnv, std:("readFile"))->mockImplementation([=](auto path) mutable
                {
                    if (path->includes(std:("model.patch"))) {
                        return std:("test");
                    }
                    return string_empty;
                }
                );
                auto result = std::async([=]() { agent->step(); });
                expect(OR((result->exitStatus), (agent->info->exitStatus)))->toBe(std:("submitted"));
                expect(OR((result->submission), (agent->info->submission)))->toBe(std:("test"));
            }
            );
            it(std:("should handle human exit command"), [=]() mutable
            {
                auto agent = DefaultAgent::fromConfig(identityAgentConfig);
                agent->model = std::make_shared<PredeterminedTestModel>(array<string>{ std:(""""\
exit\
"""") }, defaultToolConfig);
                std::async([=]() { agent->setup(dummyEnv, std::make_shared<EmptyProblemStatement>()); });
                auto result = std::async([=]() { agent->step(); });
                expect(result->done)->toBe(true);
                expect(result->exitStatus)->toBe(std:("exit_command"));
                expect(result->action->trim())->toBe(std:("exit"));
            }
            );
        }
        );
        describe(std:("Function calling"), [=]() mutable
        {
            it(std:("should handle function calling format"), [=]() mutable
            {
                auto agent = DefaultAgent::fromConfig(functionCallingAgentConfig);
                auto validResponse = object{
                    object::pair{std:("message"), std:("I'll list the contents of the directory")}, 
                    object::pair{std:("toolCalls"), array<object>{ object{
                        object::pair{std:("type"), std:("function")}, 
                        object::pair{std:("function"), object{
                            object::pair{std:("name"), std:("bash")}, 
                            object::pair{std:("arguments"), std:("{"command": "ls"}")}
                        }}, 
                        object::pair{std:("id"), std:("abc123")}
                    } }}
                };
                agent->model = std::make_shared<PredeterminedTestModel>(array<std::shared_ptr<ModelOutput>>{ validResponse }, defaultToolConfig);
                std::async([=]() { agent->setup(dummyEnv, std::make_shared<EmptyProblemStatement>()); });
                jest->spyOn(dummyEnv, std:("communicate"))->mockImplementation([=](auto command) mutable
                {
                    if (command->trim() == std:("ls")) {
                        return std:("file1 file2");
                    }
                    return string_empty;
                }
                );
                auto result = std::async([=]() { agent->step(); });
                expect(result->done)->toBe(false);
                expect(result->action->trim())->toBe(std:("ls"));
                expect(OR((result->observation), (string_empty)))->toContain(std:("file1 file2"));
            }
            );
        }
        );
        describe(std:("Auto-submission"), [=]() mutable
        {
            it(std:("should auto-submit when patch is found"), [=]() mutable
            {
                auto agent = DefaultAgent::fromConfig(identityAgentConfig);
                agent->model = std::make_shared<PredeterminedTestModel>(array<string>{ std:("raise_cost") }, defaultToolConfig);
                std::async([=]() { agent->setup(dummyEnv, std::make_shared<EmptyProblemStatement>()); });
                std::bind(&SWEEnv::isAlive, dummyEnv) = as<any>(jest->fn()->mockReturnValue(true));
                jest->spyOn(dummyEnv, std:("executeCommand"))->mockImplementation([=]() mutable
                {
                }
                );
                jest->spyOn(dummyEnv, std:("writeFile"))->mockImplementation([=](auto path, auto _content) mutable
                {
                    if (path == std:("/root/model.patch")) {
                    }
                }
                );
                jest->spyOn(dummyEnv, std:("readFile"))->mockImplementation([=](auto path) mutable
                {
                    if (path == std:("/root/model.patch")) {
                        return std:("mysubmission");
                    }
                    return string_empty;
                }
                );
                jest->spyOn(dummyEnv->deployment->runtime, std:("runInSession"))->mockImplementationOnce([=]() mutable
                {
                    return (object{
                        object::pair{std:("output"), string_empty}, 
                        object::pair{std:("exitCode"), 0}
                    });
                }
                )->mockImplementationOnce([=]() mutable
                {
                    return (object{
                        object::pair{std:("output"), std:("<<SWE_AGENT_SUBMISSION>>\
mysubmission\
<<SWE_AGENT_SUBMISSION>>")}, 
                        object::pair{std:("exitCode"), 0}
                    });
                }
                );
                auto result = std::async([=]() { agent->step(); });
                expect(agent->info)->toBeDefined();
                expect(agent->info->exitStatus)->toBe(std:("submitted (exit_cost)"));
                expect(agent->info->submission)->toBe(std:("mysubmission"));
                expect(result->done)->toBe(true);
                expect(result->submission)->toBe(std:("mysubmission"));
                expect(result->exitStatus)->toBe(std:("submitted (exit_cost)"));
            }
            );
        }
        );
        describe(std:("Template handling"), [=]() mutable
        {
            it(std:("should use no output template when appropriate"), [=]() mutable
            {
                auto agent = DefaultAgent::fromConfig(identityAgentConfig);
                agent->templates->nextStepNoOutputTemplate = std:("no output template");
                std::async([=]() { agent->setup(dummyEnv, std::make_shared<EmptyProblemStatement>()); });
                agent->model = std::make_shared<PredeterminedTestModel>(array<string>{ std:(""""\
ls\
""""), std:(""""\
test\
"""") }, defaultToolConfig);
                jest->spyOn(dummyEnv->deployment->runtime, std:("runInSession"))->mockImplementation([=]() mutable
                {
                    return (object{
                        object::pair{std:("output"), string_empty}, 
                        object::pair{std:("exitCode"), 0}
                    });
                }
                );
                std::async([=]() { agent->step(); });
                std::async([=]() { agent->step(); });
                auto lastMessage = (*const_(agent->history))[agent->history->get_length() - 1];
                expect(lastMessage->content)->toContain(std:("no output template"));
            }
            );
        }
        );
        describe(std:("Early exit conditions"), [=]() mutable
        {
            it(std:("should handle early environment error"), [=]() mutable
            {
                auto agent = DefaultAgent::fromConfig(identityAgentConfig);
                agent->model = std::make_shared<PredeterminedTestModel>(array<string>{ std:("raise") }, defaultToolConfig);
                agent->_catchErrors = true;
                jest->spyOn(dummyEnv, std:("communicate"))->mockImplementation([=](auto command) mutable
                {
                    if (command->trim() == std:("raise")) {
                        throw any(std::make_shared<Error>(std:("SwerexException")));
                    }
                    return string_empty;
                }
                );
                auto result = std::async([=]() { agent->run(dummyEnv, std::make_shared<EmptyProblemStatement>(), std:("/tmp/test")); });
                expect(result->info->exitStatus)->toBe(std:("exit_environment_error"));
            }
            );
        }
        );
        describe(std:("Action parsing"), [=]() mutable
        {
            it(std:("should handle various action formats"), [=]() mutable
            {
                auto agent = DefaultAgent::fromConfig(identityAgentConfig);
                auto testActions = array<string>{ std:("simple_command"), std:("command with args"), std:("command "with quotes""), std:("command | pipe"), std:("multi\
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
        describe(std:("State management"), [=]() mutable
        {
            it(std:("should track agent state correctly"), [=]() mutable
            {
                auto agent = DefaultAgent::fromConfig(identityAgentConfig);
                expect(agent->info)->toEqual(object{});
                expect(agent->trajectory)->toEqual(array<any>());
                std::async([=]() { agent->setup(dummyEnv, std::make_shared<EmptyProblemStatement>()); });
                expect(agent->history->get_length())->toBeGreaterThan(0);
                agent->model = std::make_shared<PredeterminedTestModel>(array<string>{ std:("test_action") }, defaultToolConfig);
                std::async([=]() { agent->step(); });
                expect(agent->trajectory->get_length())->toBe(1);
                expect((*const_(agent->trajectory))[0])->toHaveProperty(std:("action"), std:("test_action"));
            }
            );
        }
        );
        describe(std:("Cost tracking"), [=]() mutable
        {
            it(std:("should track costs correctly"), [=]() mutable
            {
                auto agent = DefaultAgent::fromConfig(identityAgentConfig);
                agent->model = std::make_shared<PredeterminedTestModel>(array<string>{ std:("action1"), std:("action2") }, defaultToolConfig);
                agent->model->stats = as<any>(object{
                    object::pair{std:("instanceCost"), 0}, 
                    object::pair{std:("totalCost"), 0}, 
                    object::pair{std:("tokensSent"), 0}, 
                    object::pair{std:("tokensReceived"), 0}, 
                    object::pair{std:("apiCalls"), 0}
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
        describe(std:("Retry logic"), [=]() mutable
        {
            it(std:("should retry on format errors up to max retries"), [=]() mutable
            {
                auto agent = DefaultAgent::fromConfig(thoughtActionAgentConfig);
                agent->maxRequeries = 3;
                auto responses = array<string>{ std:("invalid1"), std:("invalid2"), std:("invalid3"), std:("invalid4") };
                agent->model = std::make_shared<PredeterminedTestModel>(responses, defaultToolConfig);
                std::async([=]() { agent->setup(dummyEnv, std::make_shared<EmptyProblemStatement>()); });
                std::async([=]() { agent->step(); });
                expect(agent->info->exitStatus)->toBe(std:("exit_format"));
                expect(agent->trajectory->get_length())->toBeGreaterThanOrEqual(agent->maxRequeries);
            }
            );
        }
        );
    }
    );
}

MAIN
