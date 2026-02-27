#include "bun-exec.test.h"

object mockLogger = object{
    object::pair{std:("debug"), mock([=]() mutable
    {
    }
    )}, 
    object::pair{std:("info"), mock([=]() mutable
    {
    }
    )}, 
    object::pair{std:("warn"), mock([=]() mutable
    {
    }
    )}, 
    object::pair{std:("error"), mock([=]() mutable
    {
    }
    )}
};

void Main(void)
{
    mock->module(std:("@elizaos/core"), [=]() mutable
    {
        return (object{
            object::pair{std:("logger"), mockLogger}
        });
    }
    );
    describe(std:("bun-exec"), [=]() mutable
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
                object::pair{std:("stdout"), std::make_shared<ReadableStream>(object{
                })}, 
                object::pair{std:("stderr"), std::make_shared<ReadableStream>(object{
                })}, 
                object::pair{std:("exited"), Promise->resolve(0)}, 
                object::pair{std:("exitCode"), nullptr}, 
                object::pair{std:("kill"), mock([=]() mutable
                {
                }
                )}, 
                object::pair{std:("killed"), false}
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
        describe(std:("bunExec"), [=]() mutable
        {
            it(std:("should execute a command successfully"), [=]() mutable
            {
                mockProc["exited"]["then"]([=]() mutable
                {
                    mockProc["exitCode"] = 0;
                }
                );
                auto result = std::async([=]() { bunExec(std:("echo"), array<string>{ std:("hello") }); });
                expect(result)->toEqual(object{
                    object::pair{std:("stdout"), std:("test output")}, 
                    object::pair{std:("stderr"), string_empty}, 
                    object::pair{std:("exitCode"), 0}, 
                    object::pair{std:("success"), true}
                });
                expect(Bun->spawn)->toHaveBeenCalledWith(array<string>{ std:("echo"), std:("hello") }, object{
                    object::pair{std:("cwd"), undefined}, 
                    object::pair{std:("env"), process->env}, 
                    object::pair{std:("stdout"), std:("pipe")}, 
                    object::pair{std:("stderr"), std:("pipe")}
                });
            }
            );
            it(std:("should handle command failure with non-zero exit code"), [=]() mutable
            {
                mockProc = utils::assign(object{
                    , 
                    object::pair{std:("exited"), Promise->resolve(1)->then([=](auto code) mutable
                    {
                        mockProc["exitCode"] = 1;
                        return code;
                    }
                    )}, 
                    object::pair{std:("stderr"), std::make_shared<ReadableStream>(object{
                    })}
                }, mockProc);
                Bun->spawn = mock([=]() mutable
                {
                    return mockProc;
                }
                );
                auto result = std::async([=]() { bunExec(std:("false")); });
                expect(result)->toEqual(object{
                    object::pair{std:("stdout"), std:("test output")}, 
                    object::pair{std:("stderr"), std:("error message")}, 
                    object::pair{std:("exitCode"), 1}, 
                    object::pair{std:("success"), false}
                });
            }
            );
            it(std:("should read stdout and stderr concurrently"), [=]() mutable
            {
                shared stdoutReadStarted = false;
                shared stderrReadStarted = false;
                shared processExited = false;
                mockProc = object{
                    object::pair{std:("exitCode"), nullptr}, 
                    object::pair{std:("stdout"), std::make_shared<ReadableStream>(object{
                    })}, 
                    object::pair{std:("stderr"), std::make_shared<ReadableStream>(object{
                    })}, 
                    object::pair{std:("exited"), std::make_shared<Promise>([=](auto resolve) mutable
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
                    object::pair{std:("kill"), mock([=]() mutable
                    {
                    }
                    )}, 
                    object::pair{std:("killed"), false}
                };
                Bun->spawn = mock([=]() mutable
                {
                    return mockProc;
                }
                );
                auto result = std::async([=]() { bunExec(std:("test-concurrent")); });
                expect(stdoutReadStarted)->toBe(true);
                expect(stderrReadStarted)->toBe(true);
                expect(processExited)->toBe(true);
                expect(result->stdout)->toBe(std:("stdout data"));
                expect(result->stderr)->toBe(std:("stderr data"));
                expect(result->exitCode)->toBe(0);
            }
            );
            it(std:("should handle custom options"), [=]() mutable
            {
                auto options = object{
                    object::pair{std:("cwd"), std:("/custom/path")}, 
                    object::pair{std:("env"), object{
                        object::pair{std:("CUSTOM_VAR"), std:("value")}
                    }}, 
                    object::pair{std:("stdout"), std:("inherit")}, 
                    object::pair{std:("stderr"), std:("pipe")}
                };
                std::async([=]() { bunExec(std:("ls"), array<string>{ std:("-la") }, options); });
                expect(Bun->spawn)->toHaveBeenCalledWith(array<string>{ std:("ls"), std:("-la") }, object{
                    object::pair{std:("cwd"), std:("/custom/path")}, 
                    object::pair{std:("env"), utils::assign(object{
                        , 
                        object::pair{std:("CUSTOM_VAR"), std:("value")}
                    }, process->env)}, 
                    object::pair{std:("stdout"), std:("inherit")}, 
                    object::pair{std:("stderr"), std:("pipe")}
                });
            }
            );
            it(std:("should handle timeout for entire operation and cleanup process"), [=]() mutable
            {
                mockProc = object{
                    object::pair{std:("exitCode"), nullptr}, 
                    object::pair{std:("stdout"), std::make_shared<ReadableStream>(object{
                    })}, 
                    object::pair{std:("stderr"), std::make_shared<ReadableStream>(object{
                    })}, 
                    object::pair{std:("exited"), std::make_shared<Promise>([=]() mutable
                    {
                    }
                    )}, 
                    object::pair{std:("kill"), mock([=]() mutable
                    {
                    }
                    )}, 
                    object::pair{std:("killed"), false}
                };
                Bun->spawn = mock([=]() mutable
                {
                    return mockProc;
                }
                );
                try
                {
                    std::async([=]() { bunExec(std:("sleep"), array<string>{ std:("10") }, object{
                        object::pair{std:("timeout"), 100}
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
            it(std:("should handle abort signal"), [=]() mutable
            {
                mockProc = utils::assign(object{
                    , 
                    object::pair{std:("exitCode"), nullptr}, 
                    object::pair{std:("exited"), std::make_shared<Promise>([=](auto resolve) mutable
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
                auto execPromise = bunExec(std:("sleep"), array<string>{ std:("10") }, object{
                    object::pair{std:("signal"), controller->signal}
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
            it(std:("should not attempt cleanup for already completed processes"), [=]() mutable
            {
                mockProc = utils::assign(object{
                    , 
                    object::pair{std:("exitCode"), nullptr}, 
                    object::pair{std:("exited"), Promise->resolve(0)->then([=](auto code) mutable
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
                std::async([=]() { bunExec(std:("echo"), array<string>{ std:("test") }); });
                expect(mockProc["kill"])->not->toHaveBeenCalled();
            }
            );
            it(std:("should attempt cleanup for processes that are still running after timeout"), [=]() mutable
            {
                shared killCallCount = 0;
                mockProc = object{
                    object::pair{std:("exitCode"), nullptr}, 
                    object::pair{std:("stdout"), std::make_shared<ReadableStream>(object{
                    })}, 
                    object::pair{std:("stderr"), std::make_shared<ReadableStream>(object{
                    })}, 
                    object::pair{std:("exited"), std::make_shared<Promise>([=]() mutable
                    {
                    }
                    )}, 
                    object::pair{std:("kill"), mock([=]() mutable
                    {
                        killCallCount++;
                        if (killCallCount == 1) {
                        } else if (killCallCount == 2) {
                            mockProc["exitCode"] = 143;
                        }
                    }
                    )}, 
                    object::pair{std:("killed"), false}
                };
                Bun->spawn = mock([=]() mutable
                {
                    return mockProc;
                }
                );
                try
                {
                    std::async([=]() { bunExec(std:("stubborn-process"), array<any>(), object{
                        object::pair{std:("timeout"), 50}
                    }); });
                }
                catch (const any& error)
                {
                    expect(error)->toBeInstanceOf(ProcessTimeoutError);
                }
                expect(mockProc["kill"])->toHaveBeenCalledTimes(2);
            }
            );
            it(std:("should handle null stdout/stderr streams"), [=]() mutable
            {
                mockProc = utils::assign(object{
                    , 
                    object::pair{std:("stdout"), nullptr}, 
                    object::pair{std:("stderr"), nullptr}, 
                    object::pair{std:("exited"), Promise->resolve(0)->then([=](auto code) mutable
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
                auto result = std::async([=]() { bunExec(std:("echo"), array<string>{ std:("test") }); });
                expect(result)->toEqual(object{
                    object::pair{std:("stdout"), string_empty}, 
                    object::pair{std:("stderr"), string_empty}, 
                    object::pair{std:("exitCode"), 0}, 
                    object::pair{std:("success"), true}
                });
            }
            );
            it(std:("should handle numeric stdout/stderr (file descriptors)"), [=]() mutable
            {
                mockProc = utils::assign(object{
                    , 
                    object::pair{std:("stdout"), 1}, 
                    object::pair{std:("stderr"), 2}, 
                    object::pair{std:("exited"), Promise->resolve(0)->then([=](auto code) mutable
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
                auto result = std::async([=]() { bunExec(std:("echo"), array<string>{ std:("test") }); });
                expect(result)->toEqual(object{
                    object::pair{std:("stdout"), string_empty}, 
                    object::pair{std:("stderr"), string_empty}, 
                    object::pair{std:("exitCode"), 0}, 
                    object::pair{std:("success"), true}
                });
            }
            );
            it(std:("should handle stream reading errors gracefully"), [=]() mutable
            {
                mockProc = utils::assign(object{
                    , 
                    object::pair{std:("exitCode"), nullptr}, 
                    object::pair{std:("stdout"), std::make_shared<ReadableStream>(object{
                    })}, 
                    object::pair{std:("stderr"), std::make_shared<ReadableStream>(object{
                    })}, 
                    object::pair{std:("exited"), Promise->resolve(0)->then([=](auto code) mutable
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
                auto result = std::async([=]() { bunExec(std:("echo"), array<string>{ std:("test") }); });
                expect(result->exitCode)->toBe(0);
                expect(result->success)->toBe(true);
                expect(result->stdout)->toBe(string_empty);
                expect(result->stderr)->toBe(std:("stderr output"));
            }
            );
            it(std:("should clean up process on error"), [=]() mutable
            {
                mockProc = utils::assign(object{
                    , 
                    object::pair{std:("exitCode"), nullptr}, 
                    object::pair{std:("exited"), Promise->reject(std::make_shared<Error>(std:("Process error")))}
                }, mockProc);
                Bun->spawn = mock([=]() mutable
                {
                    return mockProc;
                }
                );
                try
                {
                    std::async([=]() { bunExec(std:("bad-command")); });
                    expect(false)->toBe(true);
                }
                catch (const any& error)
                {
                    expect(error)->toBeInstanceOf(ProcessExecutionError);
                    expect(mockProc["kill"])->toHaveBeenCalled();
                }
            }
            );
            it(std:("should handle cleanup errors gracefully"), [=]() mutable
            {
                mockProc = utils::assign(object{
                    , 
                    object::pair{std:("exitCode"), nullptr}, 
                    object::pair{std:("kill"), mock([=]() mutable
                    {
                        throw any(std::make_shared<Error>(std:("Kill failed")));
                    }
                    )}, 
                    object::pair{std:("exited"), Promise->reject(std::make_shared<Error>(std:("Process error")))}
                }, mockProc);
                Bun->spawn = mock([=]() mutable
                {
                    return mockProc;
                }
                );
                try
                {
                    std::async([=]() { bunExec(std:("bad-command")); });
                    expect(false)->toBe(true);
                }
                catch (const any& error)
                {
                    expect(error)->toBeInstanceOf(ProcessExecutionError);
                    expect((as<std::shared_ptr<ProcessExecutionError>>(error))->message)->toContain(std:("Process error"));
                }
            }
            );
            it(std:("should handle race condition where process exits during cleanup"), [=]() mutable
            {
                mockProc = object{
                    object::pair{std:("exitCode"), nullptr}, 
                    object::pair{std:("stdout"), std::make_shared<ReadableStream>(object{
                    })}, 
                    object::pair{std:("stderr"), std::make_shared<ReadableStream>(object{
                    })}, 
                    object::pair{std:("exited"), Promise->reject(std::make_shared<Error>(std:("Process failed")))}, 
                    object::pair{std:("kill"), mock([=]() mutable
                    {
                        mockProc["exitCode"] = 1;
                        throw any(std::make_shared<Error>(std:("Process not found")));
                    }
                    )}, 
                    object::pair{std:("killed"), false}
                };
                Bun->spawn = mock([=]() mutable
                {
                    return mockProc;
                }
                );
                try
                {
                    std::async([=]() { bunExec(std:("race-condition-test")); });
                    expect(false)->toBe(true);
                }
                catch (const any& error)
                {
                    expect(error)->toBeInstanceOf(ProcessExecutionError);
                    expect(mockProc["kill"])->toHaveBeenCalledTimes(1);
                    expect(mockLogger["debug"])->toHaveBeenCalledWith(std:("[bunExec] Process cleanup error (process may have already exited):"), expect->any(Error));
                }
            }
            );
        }
        );
        describe(std:("bunExecSimple"), [=]() mutable
        {
            it(std:("should return stdout on success"), [=]() mutable
            {
                mockProc["exited"]["then"]([=]() mutable
                {
                    mockProc["exitCode"] = 0;
                }
                );
                auto result = std::async([=]() { bunExecSimple(std:("echo"), array<string>{ std:("hello") }); });
                expect(result)->toEqual(object{
                    object::pair{std:("stdout"), std:("test output")}
                });
            }
            );
            it(std:("should throw ProcessExecutionError on failure"), [=]() mutable
            {
                mockProc = utils::assign(object{
                    , 
                    object::pair{std:("exitCode"), nullptr}, 
                    object::pair{std:("exited"), Promise->resolve(1)->then([=](auto code) mutable
                    {
                        mockProc["exitCode"] = 1;
                        return code;
                    }
                    )}, 
                    object::pair{std:("stderr"), std::make_shared<ReadableStream>(object{
                    })}
                }, mockProc);
                Bun->spawn = mock([=]() mutable
                {
                    return mockProc;
                }
                );
                try
                {
                    std::async([=]() { bunExecSimple(std:("false")); });
                    expect(false)->toBe(true);
                }
                catch (const any& error)
                {
                    expect(error)->toBeInstanceOf(ProcessExecutionError);
                }
            }
            );
            it(std:("should not throw error when stdio is ignore"), [=]() mutable
            {
                mockProc = utils::assign(object{
                    , 
                    object::pair{std:("exitCode"), nullptr}, 
                    object::pair{std:("exited"), Promise->resolve(1)->then([=](auto code) mutable
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
                auto result = std::async([=]() { bunExecSimple(std:("false"), array<any>(), object{
                    object::pair{std:("stdio"), std:("ignore")}
                }); });
                expect(result)->toEqual(object{
                    object::pair{std:("stdout"), std:("test output")}
                });
            }
            );
        }
        );
        describe(std:("bunExecInherit"), [=]() mutable
        {
            it(std:("should use inherit stdio"), [=]() mutable
            {
                std::async([=]() { bunExecInherit(std:("echo"), array<string>{ std:("hello") }); });
                expect(Bun->spawn)->toHaveBeenCalledWith(array<string>{ std:("echo"), std:("hello") }, object{
                    object::pair{std:("cwd"), undefined}, 
                    object::pair{std:("env"), process->env}, 
                    object::pair{std:("stdout"), std:("inherit")}, 
                    object::pair{std:("stderr"), std:("inherit")}
                });
            }
            );
            it(std:("should override stdio option with inherit"), [=]() mutable
            {
                std::async([=]() { bunExecInherit(std:("echo"), array<string>{ std:("hello") }, object{
                    object::pair{std:("stdout"), std:("pipe")}
                }); });
                expect(Bun->spawn)->toHaveBeenCalledWith(array<string>{ std:("echo"), std:("hello") }, object{
                    object::pair{std:("cwd"), undefined}, 
                    object::pair{std:("env"), process->env}, 
                    object::pair{std:("stdout"), std:("inherit")}, 
                    object::pair{std:("stderr"), std:("inherit")}
                });
            }
            );
        }
        );
        describe(std:("commandExists"), [=]() mutable
        {
            describe(std:("on Unix systems"), [=]() mutable
            {
                beforeEach([=]() mutable
                {
                    Object->defineProperty(process, std:("platform"), object{
                        object::pair{std:("value"), std:("linux")}, 
                        object::pair{std:("configurable"), true}
                    });
                }
                );
                it(std:("should return true when command exists"), [=]() mutable
                {
                    mockProc["exited"]["then"]([=]() mutable
                    {
                        mockProc["exitCode"] = 0;
                    }
                    );
                    auto exists = std::async([=]() { commandExists(std:("node")); });
                    expect(exists)->toBe(true);
                    expect(Bun->spawn)->toHaveBeenCalledWith(array<string>{ std:("which"), std:("node") }, object{
                        object::pair{std:("cwd"), undefined}, 
                        object::pair{std:("env"), process->env}, 
                        object::pair{std:("stdout"), std:("ignore")}, 
                        object::pair{std:("stderr"), std:("ignore")}
                    });
                }
                );
                it(std:("should return false when command does not exist"), [=]() mutable
                {
                    mockProc = utils::assign(object{
                        , 
                        object::pair{std:("exitCode"), nullptr}, 
                        object::pair{std:("exited"), Promise->resolve(1)->then([=](auto code) mutable
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
                    auto exists = std::async([=]() { commandExists(std:("nonexistent")); });
                    expect(exists)->toBe(false);
                }
                );
                it(std:("should return false on error"), [=]() mutable
                {
                    Bun->spawn = mock([=]() mutable
                    {
                        throw any(std::make_shared<Error>(std:("Command failed")));
                    }
                    );
                    auto exists = std::async([=]() { commandExists(std:("bad-command")); });
                    expect(exists)->toBe(false);
                }
                );
            }
            );
            describe(std:("on Windows"), [=]() mutable
            {
                beforeEach([=]() mutable
                {
                    Object->defineProperty(process, std:("platform"), object{
                        object::pair{std:("value"), std:("win32")}, 
                        object::pair{std:("configurable"), true}
                    });
                }
                );
                it(std:("should use where command on Windows"), [=]() mutable
                {
                    mockProc["exited"]["then"]([=]() mutable
                    {
                        mockProc["exitCode"] = 0;
                    }
                    );
                    auto exists = std::async([=]() { commandExists(std:("node")); });
                    expect(exists)->toBe(true);
                    expect(Bun->spawn)->toHaveBeenCalledWith(array<string>{ std:("where"), std:("node") }, object{
                        object::pair{std:("cwd"), undefined}, 
                        object::pair{std:("env"), process->env}, 
                        object::pair{std:("stdout"), std:("ignore")}, 
                        object::pair{std:("stderr"), std:("ignore")}
                    });
                }
                );
            }
            );
        }
        );
        describe(std:("argument escaping"), [=]() mutable
        {
            it(std:("should handle arguments with spaces"), [=]() mutable
            {
                std::async([=]() { bunExec(std:("echo"), array<string>{ std:("hello world"), std:("test arg") }); });
                expect(Bun->spawn)->toHaveBeenCalledWith(array<string>{ std:("echo"), std:("hello world"), std:("test arg") }, expect->objectContaining(object{}));
            }
            );
            it(std:("should handle arguments with quotes"), [=]() mutable
            {
                std::async([=]() { bunExec(std:("echo"), array<string>{ std:("hello "world""), std:("test 'arg'") }); });
                expect(Bun->spawn)->toHaveBeenCalledWith(array<string>{ std:("echo"), std:("hello "world""), std:("test 'arg'") }, expect->objectContaining(object{}));
            }
            );
            it(std:("should handle arguments with special characters"), [=]() mutable
            {
                std::async([=]() { bunExec(std:("echo"), array<string>{ std:("hello; rm -rf /"), std:("test$(whoami)"), std:("test"id"") }); });
                expect(Bun->spawn)->toHaveBeenCalledWith(array<string>{ std:("echo"), std:("hello; rm -rf /"), std:("test$(whoami)"), std:("test"id"") }, expect->objectContaining(object{}));
            }
            );
            it(std:("should handle empty arguments"), [=]() mutable
            {
                std::async([=]() { bunExec(std:("echo"), array<string>{ string_empty, std:("test"), string_empty }); });
                expect(Bun->spawn)->toHaveBeenCalledWith(array<string>{ std:("echo"), string_empty, std:("test"), string_empty }, expect->objectContaining(object{}));
            }
            );
        }
        );
        describe(std:("error types"), [=]() mutable
        {
            it(std:("should create ProcessExecutionError with correct properties"), [=]() mutable
            {
                auto error = std::make_shared<ProcessExecutionError>(std:("Test error"), 1, std:("stderr output"), std:("test-command"));
                expect(error->name)->toBe(std:("ProcessExecutionError"));
                expect(error->message)->toBe(std:("Test error"));
                expect(error->exitCode)->toBe(1);
                expect(error->stderr)->toBe(std:("stderr output"));
                expect(error->command)->toBe(std:("test-command"));
            }
            );
            it(std:("should create ProcessTimeoutError with correct properties"), [=]() mutable
            {
                auto error = std::make_shared<ProcessTimeoutError>(std:("Timeout error"), std:("test-command"), 5000);
                expect(error->name)->toBe(std:("ProcessTimeoutError"));
                expect(error->message)->toBe(std:("Timeout error"));
                expect(error->command)->toBe(std:("test-command"));
                expect(error->timeout)->toBe(5000);
            }
            );
        }
        );
    }
    );
}

MAIN
