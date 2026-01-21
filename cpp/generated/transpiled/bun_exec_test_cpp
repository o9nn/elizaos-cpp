#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/src/utils/__tests__/bun-exec.test.h"

object mockLogger = object{
    object::pair{std::string("debug"), mock([=]() mutable
    {
    }
    )}, 
    object::pair{std::string("info"), mock([=]() mutable
    {
    }
    )}, 
    object::pair{std::string("warn"), mock([=]() mutable
    {
    }
    )}, 
    object::pair{std::string("error"), mock([=]() mutable
    {
    }
    )}
};

void Main(void)
{
    mock->module(std::string("@elizaos/core"), [=]() mutable
    {
        return (object{
            object::pair{std::string("logger"), mockLogger}
        });
    }
    );
    describe(std::string("bun-exec"), [=]() mutable
    {
        shared<any> mockProc;
        shared<> originalSpawn;
        beforeEach([=]() mutable
        {
            originalSpawn = Bun->spawn;
            mockLogger["debug"]->mockClear();
            mockLogger["info"]->mockClear();
            mockLogger["warn"]->mockClear();
            mockLogger["error"]->mockClear();
            mockProc = object{
                object::pair{std::string("stdout"), std::make_shared<ReadableStream>(object{
                })}, 
                object::pair{std::string("stderr"), std::make_shared<ReadableStream>(object{
                })}, 
                object::pair{std::string("exited"), Promise->resolve(0)}, 
                object::pair{std::string("exitCode"), nullptr}, 
                object::pair{std::string("kill"), mock([=]() mutable
                {
                }
                )}, 
                object::pair{std::string("killed"), false}
            };
            Bun->spawn = mock([=]() mutable
            {
                return mockProc;
            }
            );
        }
        );
        afterEach([=]() mutable
        {
            Bun->spawn = originalSpawn;
        }
        );
        describe(std::string("bunExec"), [=]() mutable
        {
            it(std::string("should execute a command successfully"), [=]() mutable
            {
                mockProc["exited"]["then"]([=]() mutable
                {
                    mockProc["exitCode"] = 0;
                }
                );
                auto result = std::async([=]() { bunExec(std::string("echo"), array<string>{ std::string("hello") }); });
                expect(result)->toEqual(object{
                    object::pair{std::string("stdout"), std::string("test output")}, 
                    object::pair{std::string("stderr"), string_empty}, 
                    object::pair{std::string("exitCode"), 0}, 
                    object::pair{std::string("success"), true}
                });
                expect(Bun->spawn)->toHaveBeenCalledWith(array<string>{ std::string("echo"), std::string("hello") }, object{
                    object::pair{std::string("cwd"), undefined}, 
                    object::pair{std::string("env"), process->env}, 
                    object::pair{std::string("stdout"), std::string("pipe")}, 
                    object::pair{std::string("stderr"), std::string("pipe")}
                });
            }
            );
            it(std::string("should handle command failure with non-zero exit code"), [=]() mutable
            {
                mockProc = utils::assign(object{
                    , 
                    object::pair{std::string("exited"), Promise->resolve(1)->then([=](auto code) mutable
                    {
                        mockProc["exitCode"] = 1;
                        return code;
                    }
                    )}, 
                    object::pair{std::string("stderr"), std::make_shared<ReadableStream>(object{
                    })}
                }, mockProc);
                Bun->spawn = mock([=]() mutable
                {
                    return mockProc;
                }
                );
                auto result = std::async([=]() { bunExec(std::string("false")); });
                expect(result)->toEqual(object{
                    object::pair{std::string("stdout"), std::string("test output")}, 
                    object::pair{std::string("stderr"), std::string("error message")}, 
                    object::pair{std::string("exitCode"), 1}, 
                    object::pair{std::string("success"), false}
                });
            }
            );
            it(std::string("should read stdout and stderr concurrently"), [=]() mutable
            {
                shared stdoutReadStarted = false;
                shared stderrReadStarted = false;
                shared processExited = false;
                mockProc = object{
                    object::pair{std::string("exitCode"), nullptr}, 
                    object::pair{std::string("stdout"), std::make_shared<ReadableStream>(object{
                    })}, 
                    object::pair{std::string("stderr"), std::make_shared<ReadableStream>(object{
                    })}, 
                    object::pair{std::string("exited"), std::make_shared<Promise>([=](auto resolve) mutable
                    {
                        setTimeout([=]() mutable
                        {
                            processExited = true;
                            mockProc["exitCode"] = 0;
                            resolve(0);
                        }
                        , 100);
                    }
                    )}, 
                    object::pair{std::string("kill"), mock([=]() mutable
                    {
                    }
                    )}, 
                    object::pair{std::string("killed"), false}
                };
                Bun->spawn = mock([=]() mutable
                {
                    return mockProc;
                }
                );
                auto result = std::async([=]() { bunExec(std::string("test-concurrent")); });
                expect(stdoutReadStarted)->toBe(true);
                expect(stderrReadStarted)->toBe(true);
                expect(processExited)->toBe(true);
                expect(result->stdout)->toBe(std::string("stdout data"));
                expect(result->stderr)->toBe(std::string("stderr data"));
                expect(result->exitCode)->toBe(0);
            }
            );
            it(std::string("should handle custom options"), [=]() mutable
            {
                auto options = object{
                    object::pair{std::string("cwd"), std::string("/custom/path")}, 
                    object::pair{std::string("env"), object{
                        object::pair{std::string("CUSTOM_VAR"), std::string("value")}
                    }}, 
                    object::pair{std::string("stdout"), std::string("inherit")}, 
                    object::pair{std::string("stderr"), std::string("pipe")}
                };
                std::async([=]() { bunExec(std::string("ls"), array<string>{ std::string("-la") }, options); });
                expect(Bun->spawn)->toHaveBeenCalledWith(array<string>{ std::string("ls"), std::string("-la") }, object{
                    object::pair{std::string("cwd"), std::string("/custom/path")}, 
                    object::pair{std::string("env"), utils::assign(object{
                        , 
                        object::pair{std::string("CUSTOM_VAR"), std::string("value")}
                    }, process->env)}, 
                    object::pair{std::string("stdout"), std::string("inherit")}, 
                    object::pair{std::string("stderr"), std::string("pipe")}
                });
            }
            );
            it(std::string("should handle timeout for entire operation and cleanup process"), [=]() mutable
            {
                mockProc = object{
                    object::pair{std::string("exitCode"), nullptr}, 
                    object::pair{std::string("stdout"), std::make_shared<ReadableStream>(object{
                    })}, 
                    object::pair{std::string("stderr"), std::make_shared<ReadableStream>(object{
                    })}, 
                    object::pair{std::string("exited"), std::make_shared<Promise>([=]() mutable
                    {
                    }
                    )}, 
                    object::pair{std::string("kill"), mock([=]() mutable
                    {
                    }
                    )}, 
                    object::pair{std::string("killed"), false}
                };
                Bun->spawn = mock([=]() mutable
                {
                    return mockProc;
                }
                );
                try
                {
                    std::async([=]() { bunExec(std::string("sleep"), array<string>{ std::string("10") }, object{
                        object::pair{std::string("timeout"), 100}
                    }); });
                    expect(false)->toBe(true);
                }
                catch (const any& error)
                {
                    expect(error)->toBeInstanceOf(ProcessTimeoutError);
                    expect(mockProc["kill"])->toHaveBeenCalledTimes(1);
                }
            }
            );
            it(std::string("should handle abort signal"), [=]() mutable
            {
                mockProc = utils::assign(object{
                    , 
                    object::pair{std::string("exitCode"), nullptr}, 
                    object::pair{std::string("exited"), std::make_shared<Promise>([=](auto resolve) mutable
                    {
                        setTimeout([=]() mutable
                        {
                            mockProc["exitCode"] = 0;
                            resolve(0);
                        }
                        , 200);
                    }
                    )}
                }, mockProc);
                Bun->spawn = mock([=]() mutable
                {
                    return mockProc;
                }
                );
                shared controller = std::make_shared<AbortController>();
                auto execPromise = bunExec(std::string("sleep"), array<string>{ std::string("10") }, object{
                    object::pair{std::string("signal"), controller->signal}
                });
                setTimeout([=]() mutable
                {
                    return controller->abort();
                }
                , 50);
                std::async([=]() { execPromise; });
                expect(mockProc["kill"])->toHaveBeenCalled();
            }
            );
            it(std::string("should not attempt cleanup for already completed processes"), [=]() mutable
            {
                mockProc = utils::assign(object{
                    , 
                    object::pair{std::string("exitCode"), nullptr}, 
                    object::pair{std::string("exited"), Promise->resolve(0)->then([=](auto code) mutable
                    {
                        mockProc["exitCode"] = 0;
                        return code;
                    }
                    )}
                }, mockProc);
                Bun->spawn = mock([=]() mutable
                {
                    return mockProc;
                }
                );
                std::async([=]() { bunExec(std::string("echo"), array<string>{ std::string("test") }); });
                expect(mockProc["kill"])->not->toHaveBeenCalled();
            }
            );
            it(std::string("should attempt cleanup for processes that are still running after timeout"), [=]() mutable
            {
                shared killCallCount = 0;
                mockProc = object{
                    object::pair{std::string("exitCode"), nullptr}, 
                    object::pair{std::string("stdout"), std::make_shared<ReadableStream>(object{
                    })}, 
                    object::pair{std::string("stderr"), std::make_shared<ReadableStream>(object{
                    })}, 
                    object::pair{std::string("exited"), std::make_shared<Promise>([=]() mutable
                    {
                    }
                    )}, 
                    object::pair{std::string("kill"), mock([=]() mutable
                    {
                        killCallCount++;
                        if (killCallCount == 1) {
                        } else if (killCallCount == 2) {
                            mockProc["exitCode"] = 143;
                        }
                    }
                    )}, 
                    object::pair{std::string("killed"), false}
                };
                Bun->spawn = mock([=]() mutable
                {
                    return mockProc;
                }
                );
                try
                {
                    std::async([=]() { bunExec(std::string("stubborn-process"), array<any>(), object{
                        object::pair{std::string("timeout"), 50}
                    }); });
                }
                catch (const any& error)
                {
                    expect(error)->toBeInstanceOf(ProcessTimeoutError);
                }
                expect(mockProc["kill"])->toHaveBeenCalledTimes(2);
            }
            );
            it(std::string("should handle null stdout/stderr streams"), [=]() mutable
            {
                mockProc = utils::assign(object{
                    , 
                    object::pair{std::string("stdout"), nullptr}, 
                    object::pair{std::string("stderr"), nullptr}, 
                    object::pair{std::string("exited"), Promise->resolve(0)->then([=](auto code) mutable
                    {
                        mockProc["exitCode"] = 0;
                        return code;
                    }
                    )}
                }, mockProc);
                Bun->spawn = mock([=]() mutable
                {
                    return mockProc;
                }
                );
                auto result = std::async([=]() { bunExec(std::string("echo"), array<string>{ std::string("test") }); });
                expect(result)->toEqual(object{
                    object::pair{std::string("stdout"), string_empty}, 
                    object::pair{std::string("stderr"), string_empty}, 
                    object::pair{std::string("exitCode"), 0}, 
                    object::pair{std::string("success"), true}
                });
            }
            );
            it(std::string("should handle numeric stdout/stderr (file descriptors)"), [=]() mutable
            {
                mockProc = utils::assign(object{
                    , 
                    object::pair{std::string("stdout"), 1}, 
                    object::pair{std::string("stderr"), 2}, 
                    object::pair{std::string("exited"), Promise->resolve(0)->then([=](auto code) mutable
                    {
                        mockProc["exitCode"] = 0;
                        return code;
                    }
                    )}
                }, mockProc);
                Bun->spawn = mock([=]() mutable
                {
                    return mockProc;
                }
                );
                auto result = std::async([=]() { bunExec(std::string("echo"), array<string>{ std::string("test") }); });
                expect(result)->toEqual(object{
                    object::pair{std::string("stdout"), string_empty}, 
                    object::pair{std::string("stderr"), string_empty}, 
                    object::pair{std::string("exitCode"), 0}, 
                    object::pair{std::string("success"), true}
                });
            }
            );
            it(std::string("should handle stream reading errors gracefully"), [=]() mutable
            {
                mockProc = utils::assign(object{
                    , 
                    object::pair{std::string("exitCode"), nullptr}, 
                    object::pair{std::string("stdout"), std::make_shared<ReadableStream>(object{
                    })}, 
                    object::pair{std::string("stderr"), std::make_shared<ReadableStream>(object{
                    })}, 
                    object::pair{std::string("exited"), Promise->resolve(0)->then([=](auto code) mutable
                    {
                        mockProc["exitCode"] = 0;
                        return code;
                    }
                    )}
                }, mockProc);
                Bun->spawn = mock([=]() mutable
                {
                    return mockProc;
                }
                );
                auto result = std::async([=]() { bunExec(std::string("echo"), array<string>{ std::string("test") }); });
                expect(result->exitCode)->toBe(0);
                expect(result->success)->toBe(true);
                expect(result->stdout)->toBe(string_empty);
                expect(result->stderr)->toBe(std::string("stderr output"));
            }
            );
            it(std::string("should clean up process on error"), [=]() mutable
            {
                mockProc = utils::assign(object{
                    , 
                    object::pair{std::string("exitCode"), nullptr}, 
                    object::pair{std::string("exited"), Promise->reject(std::make_shared<Error>(std::string("Process error")))}
                }, mockProc);
                Bun->spawn = mock([=]() mutable
                {
                    return mockProc;
                }
                );
                try
                {
                    std::async([=]() { bunExec(std::string("bad-command")); });
                    expect(false)->toBe(true);
                }
                catch (const any& error)
                {
                    expect(error)->toBeInstanceOf(ProcessExecutionError);
                    expect(mockProc["kill"])->toHaveBeenCalled();
                }
            }
            );
            it(std::string("should handle cleanup errors gracefully"), [=]() mutable
            {
                mockProc = utils::assign(object{
                    , 
                    object::pair{std::string("exitCode"), nullptr}, 
                    object::pair{std::string("kill"), mock([=]() mutable
                    {
                        throw any(std::make_shared<Error>(std::string("Kill failed")));
                    }
                    )}, 
                    object::pair{std::string("exited"), Promise->reject(std::make_shared<Error>(std::string("Process error")))}
                }, mockProc);
                Bun->spawn = mock([=]() mutable
                {
                    return mockProc;
                }
                );
                try
                {
                    std::async([=]() { bunExec(std::string("bad-command")); });
                    expect(false)->toBe(true);
                }
                catch (const any& error)
                {
                    expect(error)->toBeInstanceOf(ProcessExecutionError);
                    expect((as<std::shared_ptr<ProcessExecutionError>>(error))->message)->toContain(std::string("Process error"));
                }
            }
            );
            it(std::string("should handle race condition where process exits during cleanup"), [=]() mutable
            {
                mockProc = object{
                    object::pair{std::string("exitCode"), nullptr}, 
                    object::pair{std::string("stdout"), std::make_shared<ReadableStream>(object{
                    })}, 
                    object::pair{std::string("stderr"), std::make_shared<ReadableStream>(object{
                    })}, 
                    object::pair{std::string("exited"), Promise->reject(std::make_shared<Error>(std::string("Process failed")))}, 
                    object::pair{std::string("kill"), mock([=]() mutable
                    {
                        mockProc["exitCode"] = 1;
                        throw any(std::make_shared<Error>(std::string("Process not found")));
                    }
                    )}, 
                    object::pair{std::string("killed"), false}
                };
                Bun->spawn = mock([=]() mutable
                {
                    return mockProc;
                }
                );
                try
                {
                    std::async([=]() { bunExec(std::string("race-condition-test")); });
                    expect(false)->toBe(true);
                }
                catch (const any& error)
                {
                    expect(error)->toBeInstanceOf(ProcessExecutionError);
                    expect(mockProc["kill"])->toHaveBeenCalledTimes(1);
                    expect(mockLogger["debug"])->toHaveBeenCalledWith(std::string("[bunExec] Process cleanup error (process may have already exited):"), expect->any(Error));
                }
            }
            );
        }
        );
        describe(std::string("bunExecSimple"), [=]() mutable
        {
            it(std::string("should return stdout on success"), [=]() mutable
            {
                mockProc["exited"]["then"]([=]() mutable
                {
                    mockProc["exitCode"] = 0;
                }
                );
                auto result = std::async([=]() { bunExecSimple(std::string("echo"), array<string>{ std::string("hello") }); });
                expect(result)->toEqual(object{
                    object::pair{std::string("stdout"), std::string("test output")}
                });
            }
            );
            it(std::string("should throw ProcessExecutionError on failure"), [=]() mutable
            {
                mockProc = utils::assign(object{
                    , 
                    object::pair{std::string("exitCode"), nullptr}, 
                    object::pair{std::string("exited"), Promise->resolve(1)->then([=](auto code) mutable
                    {
                        mockProc["exitCode"] = 1;
                        return code;
                    }
                    )}, 
                    object::pair{std::string("stderr"), std::make_shared<ReadableStream>(object{
                    })}
                }, mockProc);
                Bun->spawn = mock([=]() mutable
                {
                    return mockProc;
                }
                );
                try
                {
                    std::async([=]() { bunExecSimple(std::string("false")); });
                    expect(false)->toBe(true);
                }
                catch (const any& error)
                {
                    expect(error)->toBeInstanceOf(ProcessExecutionError);
                }
            }
            );
            it(std::string("should not throw error when stdio is ignore"), [=]() mutable
            {
                mockProc = utils::assign(object{
                    , 
                    object::pair{std::string("exitCode"), nullptr}, 
                    object::pair{std::string("exited"), Promise->resolve(1)->then([=](auto code) mutable
                    {
                        mockProc["exitCode"] = 1;
                        return code;
                    }
                    )}
                }, mockProc);
                Bun->spawn = mock([=]() mutable
                {
                    return mockProc;
                }
                );
                auto result = std::async([=]() { bunExecSimple(std::string("false"), array<any>(), object{
                    object::pair{std::string("stdio"), std::string("ignore")}
                }); });
                expect(result)->toEqual(object{
                    object::pair{std::string("stdout"), std::string("test output")}
                });
            }
            );
        }
        );
        describe(std::string("bunExecInherit"), [=]() mutable
        {
            it(std::string("should use inherit stdio"), [=]() mutable
            {
                std::async([=]() { bunExecInherit(std::string("echo"), array<string>{ std::string("hello") }); });
                expect(Bun->spawn)->toHaveBeenCalledWith(array<string>{ std::string("echo"), std::string("hello") }, object{
                    object::pair{std::string("cwd"), undefined}, 
                    object::pair{std::string("env"), process->env}, 
                    object::pair{std::string("stdout"), std::string("inherit")}, 
                    object::pair{std::string("stderr"), std::string("inherit")}
                });
            }
            );
            it(std::string("should override stdio option with inherit"), [=]() mutable
            {
                std::async([=]() { bunExecInherit(std::string("echo"), array<string>{ std::string("hello") }, object{
                    object::pair{std::string("stdout"), std::string("pipe")}
                }); });
                expect(Bun->spawn)->toHaveBeenCalledWith(array<string>{ std::string("echo"), std::string("hello") }, object{
                    object::pair{std::string("cwd"), undefined}, 
                    object::pair{std::string("env"), process->env}, 
                    object::pair{std::string("stdout"), std::string("inherit")}, 
                    object::pair{std::string("stderr"), std::string("inherit")}
                });
            }
            );
        }
        );
        describe(std::string("commandExists"), [=]() mutable
        {
            describe(std::string("on Unix systems"), [=]() mutable
            {
                beforeEach([=]() mutable
                {
                    Object->defineProperty(process, std::string("platform"), object{
                        object::pair{std::string("value"), std::string("linux")}, 
                        object::pair{std::string("configurable"), true}
                    });
                }
                );
                it(std::string("should return true when command exists"), [=]() mutable
                {
                    mockProc["exited"]["then"]([=]() mutable
                    {
                        mockProc["exitCode"] = 0;
                    }
                    );
                    auto exists = std::async([=]() { commandExists(std::string("node")); });
                    expect(exists)->toBe(true);
                    expect(Bun->spawn)->toHaveBeenCalledWith(array<string>{ std::string("which"), std::string("node") }, object{
                        object::pair{std::string("cwd"), undefined}, 
                        object::pair{std::string("env"), process->env}, 
                        object::pair{std::string("stdout"), std::string("ignore")}, 
                        object::pair{std::string("stderr"), std::string("ignore")}
                    });
                }
                );
                it(std::string("should return false when command does not exist"), [=]() mutable
                {
                    mockProc = utils::assign(object{
                        , 
                        object::pair{std::string("exitCode"), nullptr}, 
                        object::pair{std::string("exited"), Promise->resolve(1)->then([=](auto code) mutable
                        {
                            mockProc["exitCode"] = 1;
                            return code;
                        }
                        )}
                    }, mockProc);
                    Bun->spawn = mock([=]() mutable
                    {
                        return mockProc;
                    }
                    );
                    auto exists = std::async([=]() { commandExists(std::string("nonexistent")); });
                    expect(exists)->toBe(false);
                }
                );
                it(std::string("should return false on error"), [=]() mutable
                {
                    Bun->spawn = mock([=]() mutable
                    {
                        throw any(std::make_shared<Error>(std::string("Command failed")));
                    }
                    );
                    auto exists = std::async([=]() { commandExists(std::string("bad-command")); });
                    expect(exists)->toBe(false);
                }
                );
            }
            );
            describe(std::string("on Windows"), [=]() mutable
            {
                beforeEach([=]() mutable
                {
                    Object->defineProperty(process, std::string("platform"), object{
                        object::pair{std::string("value"), std::string("win32")}, 
                        object::pair{std::string("configurable"), true}
                    });
                }
                );
                it(std::string("should use where command on Windows"), [=]() mutable
                {
                    mockProc["exited"]["then"]([=]() mutable
                    {
                        mockProc["exitCode"] = 0;
                    }
                    );
                    auto exists = std::async([=]() { commandExists(std::string("node")); });
                    expect(exists)->toBe(true);
                    expect(Bun->spawn)->toHaveBeenCalledWith(array<string>{ std::string("where"), std::string("node") }, object{
                        object::pair{std::string("cwd"), undefined}, 
                        object::pair{std::string("env"), process->env}, 
                        object::pair{std::string("stdout"), std::string("ignore")}, 
                        object::pair{std::string("stderr"), std::string("ignore")}
                    });
                }
                );
            }
            );
        }
        );
        describe(std::string("argument escaping"), [=]() mutable
        {
            it(std::string("should handle arguments with spaces"), [=]() mutable
            {
                std::async([=]() { bunExec(std::string("echo"), array<string>{ std::string("hello world"), std::string("test arg") }); });
                expect(Bun->spawn)->toHaveBeenCalledWith(array<string>{ std::string("echo"), std::string("hello world"), std::string("test arg") }, expect->objectContaining(object{}));
            }
            );
            it(std::string("should handle arguments with quotes"), [=]() mutable
            {
                std::async([=]() { bunExec(std::string("echo"), array<string>{ std::string("hello "world""), std::string("test 'arg'") }); });
                expect(Bun->spawn)->toHaveBeenCalledWith(array<string>{ std::string("echo"), std::string("hello "world""), std::string("test 'arg'") }, expect->objectContaining(object{}));
            }
            );
            it(std::string("should handle arguments with special characters"), [=]() mutable
            {
                std::async([=]() { bunExec(std::string("echo"), array<string>{ std::string("hello; rm -rf /"), std::string("test$(whoami)"), std::string("test`id`") }); });
                expect(Bun->spawn)->toHaveBeenCalledWith(array<string>{ std::string("echo"), std::string("hello; rm -rf /"), std::string("test$(whoami)"), std::string("test`id`") }, expect->objectContaining(object{}));
            }
            );
            it(std::string("should handle empty arguments"), [=]() mutable
            {
                std::async([=]() { bunExec(std::string("echo"), array<string>{ string_empty, std::string("test"), string_empty }); });
                expect(Bun->spawn)->toHaveBeenCalledWith(array<string>{ std::string("echo"), string_empty, std::string("test"), string_empty }, expect->objectContaining(object{}));
            }
            );
        }
        );
        describe(std::string("error types"), [=]() mutable
        {
            it(std::string("should create ProcessExecutionError with correct properties"), [=]() mutable
            {
                auto error = std::make_shared<ProcessExecutionError>(std::string("Test error"), 1, std::string("stderr output"), std::string("test-command"));
                expect(error->name)->toBe(std::string("ProcessExecutionError"));
                expect(error->message)->toBe(std::string("Test error"));
                expect(error->exitCode)->toBe(1);
                expect(error->stderr)->toBe(std::string("stderr output"));
                expect(error->command)->toBe(std::string("test-command"));
            }
            );
            it(std::string("should create ProcessTimeoutError with correct properties"), [=]() mutable
            {
                auto error = std::make_shared<ProcessTimeoutError>(std::string("Timeout error"), std::string("test-command"), 5000);
                expect(error->name)->toBe(std::string("ProcessTimeoutError"));
                expect(error->message)->toBe(std::string("Timeout error"));
                expect(error->command)->toBe(std::string("test-command"));
                expect(error->timeout)->toBe(5000);
            }
            );
        }
        );
    }
    );
}

MAIN
