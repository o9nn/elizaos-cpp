#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/core/src/__tests__/logger.test.h"

object mockEnv = object{
    object::pair{std:("LOG_LEVEL"), string_empty}, 
    object::pair{std:("DEFAULT_LOG_LEVEL"), string_empty}, 
    object::pair{std:("LOG_JSON_FORMAT"), string_empty}, 
    object::pair{std:("SENTRY_LOGGING"), std:("false")}, 
    object::pair{std:("LOG_DIAGNOSTIC"), string_empty}, 
    object::pair{std:("LOG_TIMESTAMPS"), string_empty}
};

void Main(void)
{
    mock->module(std:("adze"), [=]() mutable
    {
        return (object{
            object::pair{std:("adze"), object{
                object::pair{std:("withConfig"), mock([=]() mutable
                {
                    return (object{
                        object::pair{std:("info"), mock()}, 
                        object::pair{std:("error"), mock()}, 
                        object::pair{std:("warn"), mock()}, 
                        object::pair{std:("debug"), mock()}, 
                        object::pair{std:("verbose"), mock()}, 
                        object::pair{std:("trace"), mock()}, 
                        object::pair{std:("custom"), mock()}
                    });
                }
                )}, 
                object::pair{std:("use"), mock()}
            }}, 
            object::pair{std:("createShed"), mock([=]() mutable
            {
                return (object{
                    object::pair{std:("bundle"), mock([=]() mutable
                    {
                        return (object{});
                    }
                    )}, 
                    object::pair{std:("clear"), mock()}
                });
            }
            )}
        });
    }
    );
    describe(std:("Logger"), [=]() mutable
    {
        shared<std::shared_ptr<NodeJS::ProcessEnv>> originalEnv;
        beforeEach([=]() mutable
        {
            originalEnv = utils::assign(object{
            }, process->env);
            Object->keys(mockEnv)->forEach([=](auto key) mutable
            {
                process->env[key] = const_(mockEnv)[key];
            }
            );
            mock->restore();
        }
        );
        afterEach([=]() mutable
        {
            process->env = originalEnv;
        }
        );
        describe(std:("Logger Creation"), [=]() mutable
        {
            it(std:("should logger instance"), [=]() mutable
            {
                expect(logger)->toBeDefined();
                expect(type_of(logger["info"]))->toBe(std:("function"));
                expect(type_of(logger["error"]))->toBe(std:("function"));
                expect(logger["warn"])->toBeDefined();
                expect(logger["debug"])->toBeDefined();
                expect(type_of(logger["warn"]))->toBe(std:("function"));
                expect(type_of(logger["debug"]))->toBe(std:("function"));
            }
            );
            it(std:("should elizaLogger as alias for backward compatibility"), [=]() mutable
            {
                expect(elizaLogger)->toBeDefined();
                expect(elizaLogger)->toBe(logger);
            }
            );
            it(std:("should have custom log levels"), [=]() mutable
            {
                auto testLogger = createLogger(object{
                    object::pair{std:("agentName"), std:("test")}
                });
                expect(type_of(testLogger["fatal"]))->toBe(std:("function"));
                expect(type_of(testLogger["error"]))->toBe(std:("function"));
                expect(type_of(testLogger["warn"]))->toBe(std:("function"));
                expect(type_of(testLogger["info"]))->toBe(std:("function"));
                expect(type_of(testLogger["debug"]))->toBe(std:("function"));
                expect(type_of(testLogger["trace"]))->toBe(std:("function"));
                expect(type_of(testLogger["verbose"]))->toBe(std:("function"));
            }
            );
            it(std:("should have custom methods (log, progress, success)"), [=]() mutable
            {
                auto testLogger = createLogger();
                expect(type_of(testLogger["log"]))->toBe(std:("function"));
                expect(type_of(testLogger["progress"]))->toBe(std:("function"));
                expect(type_of(testLogger["success"]))->toBe(std:("function"));
            }
            );
        }
        );
        describe(std:("createLogger Function"), [=]() mutable
        {
            it(std:("should create logger with bindings"), [=]() mutable
            {
                auto bindings = object{
                    object::pair{std:("agentName"), std:("TestAgent")}, 
                    object::pair{std:("agentId"), std:("123")}
                };
                auto customLogger = createLogger(bindings);
                expect(customLogger)->toBeDefined();
                expect(type_of(customLogger["info"]))->toBe(std:("function"));
            }
            );
            it(std:("should create logger without bindings"), [=]() mutable
            {
                auto customLogger = createLogger();
                expect(customLogger)->toBeDefined();
                expect(type_of(customLogger["info"]))->toBe(std:("function"));
            }
            );
            it(std:("should handle boolean bindings parameter"), [=]() mutable
            {
                auto customLogger = createLogger(false);
                expect(customLogger)->toBeDefined();
                expect(type_of(customLogger["info"]))->toBe(std:("function"));
            }
            );
        }
        );
        describe(std:("Log Level Configuration"), [=]() mutable
        {
            it(std:("should use debug level when LOG_LEVEL is debug"), [=]() mutable
            {
                process->env->LOG_LEVEL = std:("debug");
                auto customLogger = createLogger();
                expect(customLogger["level"])->toBeDefined();
                expect(customLogger["level"])->toBe(std:("debug"));
            }
            );
            it(std:("should use DEFAULT_LOG_LEVEL when LOG_LEVEL is not debug"), [=]() mutable
            {
                process->env->LOG_LEVEL = string_empty;
                process->env->DEFAULT_LOG_LEVEL = std:("warn");
                auto customLogger = createLogger();
                expect(customLogger["level"])->toBeDefined();
                expect(customLogger["level"])->toBe(std:("warn"));
            }
            );
            it(std:("should default to info level when no log level is specified"), [=]() mutable
            {
                process->env->LOG_LEVEL = string_empty;
                process->env->DEFAULT_LOG_LEVEL = string_empty;
                auto customLogger = createLogger();
                expect(customLogger["level"])->toBeDefined();
                expect(customLogger["level"])->toBe(std:("info"));
            }
            );
        }
        );
        describe(std:("JSON Format Configuration"), [=]() mutable
        {
            it(std:("should use JSON format when LOG_JSON_FORMAT is true"), [=]() mutable
            {
                process->env->LOG_JSON_FORMAT = std:("true");
                auto customLogger = createLogger();
                expect(customLogger)->toBeDefined();
            }
            );
            it(std:("should use pretty format when LOG_JSON_FORMAT is false"), [=]() mutable
            {
                process->env->LOG_JSON_FORMAT = std:("false");
                auto customLogger = createLogger();
                expect(customLogger)->toBeDefined();
            }
            );
        }
        );
        describe(std:("Log Filtering"), [=]() mutable
        {
            it(std:("should filter service registration logs in non-debug mode"), [=]() mutable
            {
                process->env->LOG_LEVEL = std:("info");
                shared customLogger = createLogger();
                auto filteredMessages = array<string>{ std:("registered successfully"), std:("Registering"), std:("Success:"), std:("linked to"), std:("Started") };
                filteredMessages->forEach([=](auto msg) mutable
                {
                    expect([=]() mutable
                    {
                        return customLogger["info"](object{
                            object::pair{std:("agentName"), std:("test")}, 
                            object::pair{std:("agentId"), std:("123")}
                        }, msg);
                    }
                    )->not->toThrow();
                }
                );
            }
            );
            it(std:("should not filter service registration logs in debug mode"), [=]() mutable
            {
                process->env->LOG_LEVEL = std:("debug");
                auto customLogger = createLogger();
                expect(customLogger["level"])->toBeDefined();
                expect(customLogger["level"])->toBe(std:("debug"));
            }
            );
        }
        );
        describe(std:("Error Handling"), [=]() mutable
        {
            it(std:("should handle Error objects in log messages"), [=]() mutable
            {
                shared customLogger = createLogger();
                shared testError = std::make_shared<Error>(std:("Test error"));
                expect([=]() mutable
                {
                    return customLogger["error"](testError);
                }
                )->not->toThrow();
                expect([=]() mutable
                {
                    return customLogger["error"](std:("Message"), testError);
                }
                )->not->toThrow();
                expect([=]() mutable
                {
                    return customLogger["error"](object{
                        object::pair{std:("context"), std:("test")}
                    }, std:("Error occurred"), testError);
                }
                )->not->toThrow();
            }
            );
            it(std:("should format error messages properly"), [=]() mutable
            {
                shared customLogger = createLogger();
                shared testError = std::make_shared<Error>(std:("Test error"));
                testError->name = std:("TestError");
                expect([=]() mutable
                {
                    return customLogger["error"](testError);
                }
                )->not->toThrow();
            }
            );
        }
        );
        describe(std:("Clear Method"), [=]() mutable
        {
            it(std:("should have clear method"), [=]() mutable
            {
                shared customLogger = createLogger();
                expect(type_of(customLogger["clear"]))->toBe(std:("function"));
                expect([=]() mutable
                {
                    return customLogger["clear"]();
                }
                )->not->toThrow();
            }
            );
            it(std:("should clear logs when called"), [=]() mutable
            {
                shared destination = const_((as<any>(logger)))[Symbol->for(std:("pino-destination"))];
                expect(destination)->toBeDefined();
                expect(type_of(destination["clear"]))->toBe(std:("function"));
                expect([=]() mutable
                {
                    return destination["clear"]();
                }
                )->not->toThrow();
            }
            );
        }
        );
        describe(std:("Child Logger"), [=]() mutable
        {
            it(std:("should create child logger with additional bindings"), [=]() mutable
            {
                auto customLogger = createLogger(object{
                    object::pair{std:("parent"), std:("main")}
                });
                auto childLogger = customLogger["child"](object{
                    object::pair{std:("child"), std:("sub")}
                });
                expect(childLogger)->toBeDefined();
                expect(type_of(childLogger["info"]))->toBe(std:("function"));
            }
            );
        }
        );
        describe(std:("Log Input Formats"), [=]() mutable
        {
            it(std:("should handle various log input formats"), [=]() mutable
            {
                shared customLogger = createLogger();
                expect([=]() mutable
                {
                    return customLogger["info"](std:("Simple string"));
                }
                )->not->toThrow();
                expect([=]() mutable
                {
                    return customLogger["info"](object{
                        object::pair{std:("key"), std:("value")}
                    }, std:("With object"));
                }
                )->not->toThrow();
                expect([=]() mutable
                {
                    return customLogger["info"](std:("Multiple"), std:("string"), std:("parts"));
                }
                )->not->toThrow();
                expect([=]() mutable
                {
                    return customLogger["error"](std::make_shared<Error>(std:("Test")), std:("With error"));
                }
                )->not->toThrow();
            }
            );
            it(std:("should handle mixed arguments with errors"), [=]() mutable
            {
                shared customLogger = createLogger();
                shared error = std::make_shared<Error>(std:("Test error"));
                expect([=]() mutable
                {
                    return customLogger["error"](std:("Message"), error, object{
                        object::pair{std:("extra"), std:("data")}
                    });
                }
                )->not->toThrow();
            }
            );
        }
        );
        describe(std:("Legacy Pino Compatibility"), [=]() mutable
        {
            it(std:("should have legacy pino-compatible properties"), [=]() mutable
            {
                auto customLogger = createLogger();
                expect(customLogger["level"])->toBeDefined();
                expect(customLogger["levels"])->toBeDefined();
                expect(customLogger["levels"]["values"])->toBeDefined();
                expect(type_of(customLogger["isLevelEnabled"]))->toBe(std:("function"));
                expect(type_of(customLogger["flush"]))->toBe(std:("function"));
                expect(type_of(customLogger["flushSync"]))->toBe(std:("function"));
            }
            );
            it(std:("should have destination accessible via symbol"), [=]() mutable
            {
                auto customLogger = createLogger();
                auto destination = const_((as<any>(customLogger)))[Symbol->for(std:("pino-destination"))];
                expect(destination)->toBeDefined();
                expect(type_of(destination["recentLogs"]))->toBe(std:("function"));
                expect(type_of(destination["clear"]))->toBe(std:("function"));
            }
            );
        }
        );
        describe(std:("In-Memory Logger"), [=]() mutable
        {
            it(std:("should store recent logs"), [=]() mutable
            {
                auto destination = const_((as<any>(logger)))[Symbol->for(std:("pino-destination"))];
                expect(destination)->toBeDefined();
                auto logs = destination["recentLogs"]();
                expect(Array->isArray(logs))->toBe(true);
            }
            );
            it(std:("should add logs with proper format"), [=]() mutable
            {
                auto destination = const_((as<any>(logger)))[Symbol->for(std:("pino-destination"))];
                destination["addLog"](object{
                    object::pair{std:("time"), Date->now()}, 
                    object::pair{std:("level"), 30}, 
                    object::pair{std:("msg"), std:("Test message")}, 
                    object::pair{std:("agentName"), std:("TestAgent")}
                });
                auto logs = destination["recentLogs"]();
                auto lastLog = const_(logs)[logs["length"] - 1];
                expect(lastLog)->toBeDefined();
                expect(lastLog["msg"])->toBe(std:("Test message"));
                expect(lastLog["agentName"])->toBe(std:("TestAgent"));
            }
            );
        }
        );
        describe(std:("Log Level Methods"), [=]() mutable
        {
            it(std:("should support all log levels"), [=]() mutable
            {
                shared customLogger = createLogger();
                expect([=]() mutable
                {
                    return customLogger["fatal"](std:("Fatal message"));
                }
                )->not->toThrow();
                expect([=]() mutable
                {
                    return customLogger["error"](std:("Error message"));
                }
                )->not->toThrow();
                expect([=]() mutable
                {
                    return customLogger["warn"](std:("Warn message"));
                }
                )->not->toThrow();
                expect([=]() mutable
                {
                    return customLogger["info"](std:("Info message"));
                }
                )->not->toThrow();
                expect([=]() mutable
                {
                    return customLogger["debug"](std:("Debug message"));
                }
                )->not->toThrow();
                expect([=]() mutable
                {
                    return customLogger["trace"](std:("Trace message"));
                }
                )->not->toThrow();
                expect([=]() mutable
                {
                    return customLogger["verbose"](std:("Verbose message"));
                }
                )->not->toThrow();
                expect([=]() mutable
                {
                    return customLogger["log"](std:("Log message"));
                }
                )->not->toThrow();
                expect([=]() mutable
                {
                    return customLogger["progress"](std:("Progress message"));
                }
                )->not->toThrow();
                expect([=]() mutable
                {
                    return customLogger["success"](std:("Success message"));
                }
                )->not->toThrow();
            }
            );
        }
        );
        describe(std:("Timestamp Configuration"), [=]() mutable
        {
            it(std:("should respect LOG_TIMESTAMPS setting"), [=]() mutable
            {
                process->env->LOG_TIMESTAMPS = std:("false");
                auto customLogger = createLogger();
                expect(customLogger)->toBeDefined();
            }
            );
            it(std:("should default to showing timestamps"), [=]() mutable
            {
                process->env.Delete("LOG_TIMESTAMPS");
                auto customLogger = createLogger();
                expect(customLogger)->toBeDefined();
            }
            );
        }
        );
        describe(std:("Sentry Integration"), [=]() mutable
        {
            it(std:("should not capture exceptions when SENTRY_LOGGING is false"), [=]() mutable
            {
                process->env->SENTRY_LOGGING = std:("false");
                shared customLogger = createLogger();
                shared error = std::make_shared<Error>(std:("Test error"));
                expect([=]() mutable
                {
                    return customLogger["error"](error);
                }
                )->not->toThrow();
            }
            );
            it(std:("should handle Sentry integration when enabled"), [=]() mutable
            {
                process->env->SENTRY_LOGGING = string_empty;
                shared customLogger = createLogger();
                shared error = std::make_shared<Error>(std:("Sentry test"));
                expect([=]() mutable
                {
                    return customLogger["error"](error);
                }
                )->not->toThrow();
            }
            );
        }
        );
    }
    );
}

MAIN
