#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/core/src/__tests__/logger.test.h"

object mockEnv = object{
    object::pair{std::string("LOG_LEVEL"), string_empty}, 
    object::pair{std::string("DEFAULT_LOG_LEVEL"), string_empty}, 
    object::pair{std::string("LOG_JSON_FORMAT"), string_empty}, 
    object::pair{std::string("SENTRY_LOGGING"), std::string("false")}, 
    object::pair{std::string("LOG_DIAGNOSTIC"), string_empty}, 
    object::pair{std::string("LOG_TIMESTAMPS"), string_empty}
};

void Main(void)
{
    mock->module(std::string("adze"), [=]() mutable
    {
        return (object{
            object::pair{std::string("adze"), object{
                object::pair{std::string("withConfig"), mock([=]() mutable
                {
                    return (object{
                        object::pair{std::string("info"), mock()}, 
                        object::pair{std::string("error"), mock()}, 
                        object::pair{std::string("warn"), mock()}, 
                        object::pair{std::string("debug"), mock()}, 
                        object::pair{std::string("verbose"), mock()}, 
                        object::pair{std::string("trace"), mock()}, 
                        object::pair{std::string("custom"), mock()}
                    });
                }
                )}, 
                object::pair{std::string("use"), mock()}
            }}, 
            object::pair{std::string("createShed"), mock([=]() mutable
            {
                return (object{
                    object::pair{std::string("bundle"), mock([=]() mutable
                    {
                        return (object{});
                    }
                    )}, 
                    object::pair{std::string("clear"), mock()}
                });
            }
            )}
        });
    }
    );
    describe(std::string("Logger"), [=]() mutable
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
        describe(std::string("Logger Creation"), [=]() mutable
        {
            it(std::string("should export logger instance"), [=]() mutable
            {
                expect(logger)->toBeDefined();
                expect(type_of(logger["info"]))->toBe(std::string("function"));
                expect(type_of(logger["error"]))->toBe(std::string("function"));
                expect(logger["warn"])->toBeDefined();
                expect(logger["debug"])->toBeDefined();
                expect(type_of(logger["warn"]))->toBe(std::string("function"));
                expect(type_of(logger["debug"]))->toBe(std::string("function"));
            }
            );
            it(std::string("should export elizaLogger as alias for backward compatibility"), [=]() mutable
            {
                expect(elizaLogger)->toBeDefined();
                expect(elizaLogger)->toBe(logger);
            }
            );
            it(std::string("should have custom log levels"), [=]() mutable
            {
                auto testLogger = createLogger(object{
                    object::pair{std::string("agentName"), std::string("test")}
                });
                expect(type_of(testLogger["fatal"]))->toBe(std::string("function"));
                expect(type_of(testLogger["error"]))->toBe(std::string("function"));
                expect(type_of(testLogger["warn"]))->toBe(std::string("function"));
                expect(type_of(testLogger["info"]))->toBe(std::string("function"));
                expect(type_of(testLogger["debug"]))->toBe(std::string("function"));
                expect(type_of(testLogger["trace"]))->toBe(std::string("function"));
                expect(type_of(testLogger["verbose"]))->toBe(std::string("function"));
            }
            );
            it(std::string("should have custom methods (log, progress, success)"), [=]() mutable
            {
                auto testLogger = createLogger();
                expect(type_of(testLogger["log"]))->toBe(std::string("function"));
                expect(type_of(testLogger["progress"]))->toBe(std::string("function"));
                expect(type_of(testLogger["success"]))->toBe(std::string("function"));
            }
            );
        }
        );
        describe(std::string("createLogger Function"), [=]() mutable
        {
            it(std::string("should create logger with bindings"), [=]() mutable
            {
                auto bindings = object{
                    object::pair{std::string("agentName"), std::string("TestAgent")}, 
                    object::pair{std::string("agentId"), std::string("123")}
                };
                auto customLogger = createLogger(bindings);
                expect(customLogger)->toBeDefined();
                expect(type_of(customLogger["info"]))->toBe(std::string("function"));
            }
            );
            it(std::string("should create logger without bindings"), [=]() mutable
            {
                auto customLogger = createLogger();
                expect(customLogger)->toBeDefined();
                expect(type_of(customLogger["info"]))->toBe(std::string("function"));
            }
            );
            it(std::string("should handle boolean bindings parameter"), [=]() mutable
            {
                auto customLogger = createLogger(false);
                expect(customLogger)->toBeDefined();
                expect(type_of(customLogger["info"]))->toBe(std::string("function"));
            }
            );
        }
        );
        describe(std::string("Log Level Configuration"), [=]() mutable
        {
            it(std::string("should use debug level when LOG_LEVEL is debug"), [=]() mutable
            {
                process->env->LOG_LEVEL = std::string("debug");
                auto customLogger = createLogger();
                expect(customLogger["level"])->toBeDefined();
                expect(customLogger["level"])->toBe(std::string("debug"));
            }
            );
            it(std::string("should use DEFAULT_LOG_LEVEL when LOG_LEVEL is not debug"), [=]() mutable
            {
                process->env->LOG_LEVEL = string_empty;
                process->env->DEFAULT_LOG_LEVEL = std::string("warn");
                auto customLogger = createLogger();
                expect(customLogger["level"])->toBeDefined();
                expect(customLogger["level"])->toBe(std::string("warn"));
            }
            );
            it(std::string("should default to info level when no log level is specified"), [=]() mutable
            {
                process->env->LOG_LEVEL = string_empty;
                process->env->DEFAULT_LOG_LEVEL = string_empty;
                auto customLogger = createLogger();
                expect(customLogger["level"])->toBeDefined();
                expect(customLogger["level"])->toBe(std::string("info"));
            }
            );
        }
        );
        describe(std::string("JSON Format Configuration"), [=]() mutable
        {
            it(std::string("should use JSON format when LOG_JSON_FORMAT is true"), [=]() mutable
            {
                process->env->LOG_JSON_FORMAT = std::string("true");
                auto customLogger = createLogger();
                expect(customLogger)->toBeDefined();
            }
            );
            it(std::string("should use pretty format when LOG_JSON_FORMAT is false"), [=]() mutable
            {
                process->env->LOG_JSON_FORMAT = std::string("false");
                auto customLogger = createLogger();
                expect(customLogger)->toBeDefined();
            }
            );
        }
        );
        describe(std::string("Log Filtering"), [=]() mutable
        {
            it(std::string("should filter service registration logs in non-debug mode"), [=]() mutable
            {
                process->env->LOG_LEVEL = std::string("info");
                shared customLogger = createLogger();
                auto filteredMessages = array<string>{ std::string("registered successfully"), std::string("Registering"), std::string("Success:"), std::string("linked to"), std::string("Started") };
                filteredMessages->forEach([=](auto msg) mutable
                {
                    expect([=]() mutable
                    {
                        return customLogger["info"](object{
                            object::pair{std::string("agentName"), std::string("test")}, 
                            object::pair{std::string("agentId"), std::string("123")}
                        }, msg);
                    }
                    )->not->toThrow();
                }
                );
            }
            );
            it(std::string("should not filter service registration logs in debug mode"), [=]() mutable
            {
                process->env->LOG_LEVEL = std::string("debug");
                auto customLogger = createLogger();
                expect(customLogger["level"])->toBeDefined();
                expect(customLogger["level"])->toBe(std::string("debug"));
            }
            );
        }
        );
        describe(std::string("Error Handling"), [=]() mutable
        {
            it(std::string("should handle Error objects in log messages"), [=]() mutable
            {
                shared customLogger = createLogger();
                shared testError = std::make_shared<Error>(std::string("Test error"));
                expect([=]() mutable
                {
                    return customLogger["error"](testError);
                }
                )->not->toThrow();
                expect([=]() mutable
                {
                    return customLogger["error"](std::string("Message"), testError);
                }
                )->not->toThrow();
                expect([=]() mutable
                {
                    return customLogger["error"](object{
                        object::pair{std::string("context"), std::string("test")}
                    }, std::string("Error occurred"), testError);
                }
                )->not->toThrow();
            }
            );
            it(std::string("should format error messages properly"), [=]() mutable
            {
                shared customLogger = createLogger();
                shared testError = std::make_shared<Error>(std::string("Test error"));
                testError->name = std::string("TestError");
                expect([=]() mutable
                {
                    return customLogger["error"](testError);
                }
                )->not->toThrow();
            }
            );
        }
        );
        describe(std::string("Clear Method"), [=]() mutable
        {
            it(std::string("should have clear method"), [=]() mutable
            {
                shared customLogger = createLogger();
                expect(type_of(customLogger["clear"]))->toBe(std::string("function"));
                expect([=]() mutable
                {
                    return customLogger["clear"]();
                }
                )->not->toThrow();
            }
            );
            it(std::string("should clear logs when called"), [=]() mutable
            {
                shared destination = const_((as<any>(logger)))[Symbol->for(std::string("pino-destination"))];
                expect(destination)->toBeDefined();
                expect(type_of(destination["clear"]))->toBe(std::string("function"));
                expect([=]() mutable
                {
                    return destination["clear"]();
                }
                )->not->toThrow();
            }
            );
        }
        );
        describe(std::string("Child Logger"), [=]() mutable
        {
            it(std::string("should create child logger with additional bindings"), [=]() mutable
            {
                auto customLogger = createLogger(object{
                    object::pair{std::string("parent"), std::string("main")}
                });
                auto childLogger = customLogger["child"](object{
                    object::pair{std::string("child"), std::string("sub")}
                });
                expect(childLogger)->toBeDefined();
                expect(type_of(childLogger["info"]))->toBe(std::string("function"));
            }
            );
        }
        );
        describe(std::string("Log Input Formats"), [=]() mutable
        {
            it(std::string("should handle various log input formats"), [=]() mutable
            {
                shared customLogger = createLogger();
                expect([=]() mutable
                {
                    return customLogger["info"](std::string("Simple string"));
                }
                )->not->toThrow();
                expect([=]() mutable
                {
                    return customLogger["info"](object{
                        object::pair{std::string("key"), std::string("value")}
                    }, std::string("With object"));
                }
                )->not->toThrow();
                expect([=]() mutable
                {
                    return customLogger["info"](std::string("Multiple"), std::string("string"), std::string("parts"));
                }
                )->not->toThrow();
                expect([=]() mutable
                {
                    return customLogger["error"](std::make_shared<Error>(std::string("Test")), std::string("With error"));
                }
                )->not->toThrow();
            }
            );
            it(std::string("should handle mixed arguments with errors"), [=]() mutable
            {
                shared customLogger = createLogger();
                shared error = std::make_shared<Error>(std::string("Test error"));
                expect([=]() mutable
                {
                    return customLogger["error"](std::string("Message"), error, object{
                        object::pair{std::string("extra"), std::string("data")}
                    });
                }
                )->not->toThrow();
            }
            );
        }
        );
        describe(std::string("Legacy Pino Compatibility"), [=]() mutable
        {
            it(std::string("should have legacy pino-compatible properties"), [=]() mutable
            {
                auto customLogger = createLogger();
                expect(customLogger["level"])->toBeDefined();
                expect(customLogger["levels"])->toBeDefined();
                expect(customLogger["levels"]["values"])->toBeDefined();
                expect(type_of(customLogger["isLevelEnabled"]))->toBe(std::string("function"));
                expect(type_of(customLogger["flush"]))->toBe(std::string("function"));
                expect(type_of(customLogger["flushSync"]))->toBe(std::string("function"));
            }
            );
            it(std::string("should have destination accessible via symbol"), [=]() mutable
            {
                auto customLogger = createLogger();
                auto destination = const_((as<any>(customLogger)))[Symbol->for(std::string("pino-destination"))];
                expect(destination)->toBeDefined();
                expect(type_of(destination["recentLogs"]))->toBe(std::string("function"));
                expect(type_of(destination["clear"]))->toBe(std::string("function"));
            }
            );
        }
        );
        describe(std::string("In-Memory Logger"), [=]() mutable
        {
            it(std::string("should store recent logs"), [=]() mutable
            {
                auto destination = const_((as<any>(logger)))[Symbol->for(std::string("pino-destination"))];
                expect(destination)->toBeDefined();
                auto logs = destination["recentLogs"]();
                expect(Array->isArray(logs))->toBe(true);
            }
            );
            it(std::string("should add logs with proper format"), [=]() mutable
            {
                auto destination = const_((as<any>(logger)))[Symbol->for(std::string("pino-destination"))];
                destination["addLog"](object{
                    object::pair{std::string("time"), Date->now()}, 
                    object::pair{std::string("level"), 30}, 
                    object::pair{std::string("msg"), std::string("Test message")}, 
                    object::pair{std::string("agentName"), std::string("TestAgent")}
                });
                auto logs = destination["recentLogs"]();
                auto lastLog = const_(logs)[logs["length"] - 1];
                expect(lastLog)->toBeDefined();
                expect(lastLog["msg"])->toBe(std::string("Test message"));
                expect(lastLog["agentName"])->toBe(std::string("TestAgent"));
            }
            );
        }
        );
        describe(std::string("Log Level Methods"), [=]() mutable
        {
            it(std::string("should support all log levels"), [=]() mutable
            {
                shared customLogger = createLogger();
                expect([=]() mutable
                {
                    return customLogger["fatal"](std::string("Fatal message"));
                }
                )->not->toThrow();
                expect([=]() mutable
                {
                    return customLogger["error"](std::string("Error message"));
                }
                )->not->toThrow();
                expect([=]() mutable
                {
                    return customLogger["warn"](std::string("Warn message"));
                }
                )->not->toThrow();
                expect([=]() mutable
                {
                    return customLogger["info"](std::string("Info message"));
                }
                )->not->toThrow();
                expect([=]() mutable
                {
                    return customLogger["debug"](std::string("Debug message"));
                }
                )->not->toThrow();
                expect([=]() mutable
                {
                    return customLogger["trace"](std::string("Trace message"));
                }
                )->not->toThrow();
                expect([=]() mutable
                {
                    return customLogger["verbose"](std::string("Verbose message"));
                }
                )->not->toThrow();
                expect([=]() mutable
                {
                    return customLogger["log"](std::string("Log message"));
                }
                )->not->toThrow();
                expect([=]() mutable
                {
                    return customLogger["progress"](std::string("Progress message"));
                }
                )->not->toThrow();
                expect([=]() mutable
                {
                    return customLogger["success"](std::string("Success message"));
                }
                )->not->toThrow();
            }
            );
        }
        );
        describe(std::string("Timestamp Configuration"), [=]() mutable
        {
            it(std::string("should respect LOG_TIMESTAMPS setting"), [=]() mutable
            {
                process->env->LOG_TIMESTAMPS = std::string("false");
                auto customLogger = createLogger();
                expect(customLogger)->toBeDefined();
            }
            );
            it(std::string("should default to showing timestamps"), [=]() mutable
            {
                process->env.Delete("LOG_TIMESTAMPS");
                auto customLogger = createLogger();
                expect(customLogger)->toBeDefined();
            }
            );
        }
        );
        describe(std::string("Sentry Integration"), [=]() mutable
        {
            it(std::string("should not capture exceptions when SENTRY_LOGGING is false"), [=]() mutable
            {
                process->env->SENTRY_LOGGING = std::string("false");
                shared customLogger = createLogger();
                shared error = std::make_shared<Error>(std::string("Test error"));
                expect([=]() mutable
                {
                    return customLogger["error"](error);
                }
                )->not->toThrow();
            }
            );
            it(std::string("should handle Sentry integration when enabled"), [=]() mutable
            {
                process->env->SENTRY_LOGGING = string_empty;
                shared customLogger = createLogger();
                shared error = std::make_shared<Error>(std::string("Sentry test"));
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
