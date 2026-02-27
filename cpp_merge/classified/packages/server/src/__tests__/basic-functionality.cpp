#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/server/src/__tests__/basic-functionality.test.h"

void Main(void)
{
    describe(std:("Basic Server Functionality"), [=]() mutable
    {
        describe(std:("Path utilities"), [=]() mutable
        {
            it(std:("should handle path expansion logic"), [=]() mutable
            {
                auto testExpandTildePath = [=](auto filepath) mutable
                {
                    if (AND((filepath), (filepath->startsWith(std:("~"))))) {
                        return path->join(process->cwd(), filepath->slice(1));
                    }
                    return filepath;
                };
                expect(testExpandTildePath(std:("~/test")))->toBe(path->join(process->cwd(), std:("test")));
                expect(testExpandTildePath(std:("/absolute")))->toBe(std:("/absolute"));
                expect(testExpandTildePath(std:("relative")))->toBe(std:("relative"));
                expect(testExpandTildePath(string_empty))->toBe(string_empty);
            }
            );
        }
        );
        describe(std:("UUID validation logic"), [=]() mutable
        {
            it(std:("should validate UUID format correctly"), [=]() mutable
            {
                auto validateUuidPattern = [=](auto id) mutable
                {
                    auto uuidRegex = (new RegExp(std:("^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$")));
                    return uuidRegex->test(id);
                };
                expect(validateUuidPattern(std:("123e4567-e89b-12d3-a456-426614174000")))->toBe(true);
                expect(validateUuidPattern(std:("00000000-0000-0000-0000-000000000000")))->toBe(true);
                expect(validateUuidPattern(std:("ffffffff-ffff-ffff-ffff-ffffffffffff")))->toBe(true);
                expect(validateUuidPattern(std:("invalid-uuid")))->toBe(false);
                expect(validateUuidPattern(std:("123e4567e89b12d3a456426614174000")))->toBe(false);
                expect(validateUuidPattern(std:("123e4567-e89b-12d3-a456-42661417400")))->toBe(false);
                expect(validateUuidPattern(string_empty))->toBe(false);
            }
            );
        }
        );
        describe(std:("Security pattern detection"), [=]() mutable
        {
            it(std:("should detect suspicious patterns"), [=]() mutable
            {
                auto detectSuspiciousPatterns = [=](auto input) mutable
                {
                    auto suspiciousPatterns = array<string>{ std:(".."), std:("<"), std:(">"), std:("""), std:("'"), std:("\"), std:("/") };
                    return suspiciousPatterns->some([=](auto pattern) mutable
                    {
                        return input->includes(pattern);
                    }
                    );
                };
                expect(detectSuspiciousPatterns(std:("test../path")))->toBe(true);
                expect(detectSuspiciousPatterns(std:("test<script>")))->toBe(true);
                expect(detectSuspiciousPatterns(std:("test>alert")))->toBe(true);
                expect(detectSuspiciousPatterns(std:("test"quote")))->toBe(true);
                expect(detectSuspiciousPatterns(std:("test'quote")))->toBe(true);
                expect(detectSuspiciousPatterns(std:("test\backslash")))->toBe(true);
                expect(detectSuspiciousPatterns(std:("test/slash")))->toBe(true);
                expect(detectSuspiciousPatterns(std:("123e4567-e89b-12d3-a456-426614174000")))->toBe(false);
                expect(detectSuspiciousPatterns(std:("cleantext")))->toBe(false);
                expect(detectSuspiciousPatterns(std:("clean-text-123")))->toBe(false);
            }
            );
            it(std:("should detect path traversal attempts"), [=]() mutable
            {
                auto containsPathTraversal = [=](auto path) mutable
                {
                    return OR((path->includes(std:("../"))), (path->includes(std:("..\"))));
                };
                expect(containsPathTraversal(std:("../../../etc/passwd")))->toBe(true);
                expect(containsPathTraversal(std:("normal/path")))->toBe(false);
                expect(containsPathTraversal(std:("..\windows\system32")))->toBe(true);
            }
            );
            it(std:("should detect script injection patterns"), [=]() mutable
            {
                auto containsScriptInjection = [=](auto input) mutable
                {
                    auto scriptPatterns = array<string>{ std:("<script"), std:("javascript:"), std:("onerror="), std:("onload=") };
                    return scriptPatterns->some([=](auto pattern) mutable
                    {
                        return input->toLowerCase()->includes(pattern);
                    }
                    );
                };
                expect(containsScriptInjection(std:("<script>alert(1)</script>")))->toBe(true);
                expect(containsScriptInjection(std:("javascript:alert(1)")))->toBe(true);
                expect(containsScriptInjection(std:("<img src=x onerror=alert(1)>")))->toBe(true);
                expect(containsScriptInjection(std:("normal text")))->toBe(false);
            }
            );
        }
        );
        describe(std:("Rate limiting logic"), [=]() mutable
        {
            it(std:("should implement basic rate limiting concepts"), [=]() mutable
            {
                auto rateLimiter = std::make_shared<SimpleRateLimiter>(60000, 5);
                for (auto i = 0; i < 5; i++)
                {
                    expect(rateLimiter->isAllowed(std:("client1")))->toBe(true);
                }
                expect(rateLimiter->isAllowed(std:("client1")))->toBe(false);
                expect(rateLimiter->isAllowed(std:("client2")))->toBe(true);
            }
            );
        }
        );
        describe(std:("Middleware patterns"), [=]() mutable
        {
            it(std:("should implement basic middleware concepts"), [=]() mutable
            {
                typedef std::function<void(any, any, std::function<void()>)> MiddlewareFunction;

                auto createValidationMiddleware = [=](auto paramName) mutable
                {
                    return [=](auto req, auto res, auto next) mutable
                    {
                        auto paramValue = const_(req["params"])[paramName];
                        if (!paramValue) {
                            res["error"] = std:("Missing parameter");
                            return;
                        }
                        auto uuidRegex = (new RegExp(std:("^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$")));
                        if (!uuidRegex->test(paramValue)) {
                            res["error"] = std:("Invalid format");
                            return;
                        }
                        next();
                    };
                };
                auto middleware = createValidationMiddleware(std:("id"));
                auto validReq = object{
                    object::pair{std:("params"), object{
                        object::pair{std:("id"), std:("123e4567-e89b-12d3-a456-426614174000")}
                    }}
                };
                auto validRes = object{};
                shared nextCalled = false;
                middleware(validReq, validRes, [=]() mutable
                {
                    nextCalled = true;
                }
                );
                expect(nextCalled)->toBe(true);
                expect(validRes["error"])->toBeUndefined();
                auto invalidReq = object{
                    object::pair{std:("params"), object{
                        object::pair{std:("id"), std:("invalid-id")}
                    }}
                };
                auto invalidRes = object{};
                nextCalled = false;
                middleware(invalidReq, invalidRes, [=]() mutable
                {
                    nextCalled = true;
                }
                );
                expect(nextCalled)->toBe(false);
                expect(invalidRes["error"])->toBe(std:("Invalid format"));
            }
            );
        }
        );
        describe(std:("Server configuration patterns"), [=]() mutable
        {
            it(std:("should handle server options correctly"), [=]() mutable
            {
                auto createServerConfig = [=](auto options = object{}) mutable
                {
                    return object{
                        object::pair{std:("dataDir"), OR((options->dataDir), (std:("./default-data")))}, 
                        object::pair{std:("middlewares"), OR((options->middlewares), (array<any>()))}, 
                        object::pair{std:("postgresUrl"), OR((options->postgresUrl), (nullptr))}, 
                        object::pair{std:("hasCustomDataDir"), !!options->dataDir}, 
                        object::pair{std:("hasCustomDb"), !!options->postgresUrl}
                    };
                };
                auto defaultConfig = createServerConfig();
                expect(defaultConfig["dataDir"])->toBe(std:("./default-data"));
                expect(defaultConfig["middlewares"])->toEqual(array<any>());
                expect(defaultConfig["postgresUrl"])->toBeNull();
                expect(defaultConfig["hasCustomDataDir"])->toBe(false);
                expect(defaultConfig["hasCustomDb"])->toBe(false);
                auto customConfig = createServerConfig(object{
                    object::pair{std:("dataDir"), std:("/custom/data")}, 
                    object::pair{std:("postgresUrl"), std:("postgresql://localhost:5432/test")}, 
                    object::pair{std:("middlewares"), array<string>{ std:("middleware1"), std:("middleware2") }}
                });
                expect(customConfig["dataDir"])->toBe(std:("/custom/data"));
                expect(customConfig["postgresUrl"])->toBe(std:("postgresql://localhost:5432/test"));
                expect(customConfig["middlewares"])->toEqual(array<string>{ std:("middleware1"), std:("middleware2") });
                expect(customConfig["hasCustomDataDir"])->toBe(true);
                expect(customConfig["hasCustomDb"])->toBe(true);
            }
            );
        }
        );
        describe(std:("UI Enable/Disable Logic"), [=]() mutable
        {
            shared testUIEnabled = [=](auto nodeEnv = undefined, auto elizaUIEnable = undefined) mutable
            {
                auto originalNodeEnv = process->env->NODE_ENV;
                auto originalUIEnable = process->env->ELIZA_UI_ENABLE;
                if (nodeEnv != undefined) {
                    process->env->NODE_ENV = nodeEnv;
                } else {
                    process->env.Delete("NODE_ENV");
                }
                if (elizaUIEnable != undefined) {
                    process->env->ELIZA_UI_ENABLE = elizaUIEnable;
                } else {
                    process->env.Delete("ELIZA_UI_ENABLE");
                }
                auto result = isWebUIEnabled();
                if (originalNodeEnv != undefined) {
                    process->env->NODE_ENV = originalNodeEnv;
                } else {
                    process->env.Delete("NODE_ENV");
                }
                if (originalUIEnable != undefined) {
                    process->env->ELIZA_UI_ENABLE = originalUIEnable;
                } else {
                    process->env.Delete("ELIZA_UI_ENABLE");
                }
                return result;
            };
            it(std:("should enable UI by default in development"), [=]() mutable
            {
                expect(testUIEnabled(std:("development"), undefined))->toBe(true);
                expect(testUIEnabled(std:("test"), undefined))->toBe(true);
                expect(testUIEnabled(undefined, undefined))->toBe(true);
            }
            );
            it(std:("should disable UI by default in production"), [=]() mutable
            {
                expect(testUIEnabled(std:("production"), undefined))->toBe(false);
            }
            );
            it(std:("should allow explicit override with ELIZA_UI_ENABLE=true"), [=]() mutable
            {
                expect(testUIEnabled(std:("production"), std:("true")))->toBe(true);
                expect(testUIEnabled(std:("development"), std:("true")))->toBe(true);
                expect(testUIEnabled(std:("production"), std:("TRUE")))->toBe(true);
            }
            );
            it(std:("should allow explicit override with ELIZA_UI_ENABLE=false"), [=]() mutable
            {
                expect(testUIEnabled(std:("development"), std:("false")))->toBe(false);
                expect(testUIEnabled(std:("production"), std:("false")))->toBe(false);
                expect(testUIEnabled(std:("development"), std:("FALSE")))->toBe(false);
            }
            );
            it(std:("should treat empty strings as undefined"), [=]() mutable
            {
                expect(testUIEnabled(std:("development"), string_empty))->toBe(true);
                expect(testUIEnabled(std:("production"), string_empty))->toBe(false);
            }
            );
            it(std:("should handle various boolean-like values using parseBooleanFromText"), [=]() mutable
            {
                expect(testUIEnabled(std:("production"), std:("1")))->toBe(true);
                expect(testUIEnabled(std:("production"), std:("yes")))->toBe(true);
                expect(testUIEnabled(std:("production"), std:("YES")))->toBe(true);
                expect(testUIEnabled(std:("production"), std:("on")))->toBe(true);
                expect(testUIEnabled(std:("production"), std:("enable")))->toBe(true);
                expect(testUIEnabled(std:("development"), std:("0")))->toBe(false);
                expect(testUIEnabled(std:("development"), std:("no")))->toBe(false);
                expect(testUIEnabled(std:("development"), std:("off")))->toBe(false);
                expect(testUIEnabled(std:("development"), std:("disable")))->toBe(false);
                expect(testUIEnabled(std:("development"), std:("invalid")))->toBe(false);
                expect(testUIEnabled(std:("development"), std:("maybe")))->toBe(false);
            }
            );
            it(std:("should generate appropriate log messages"), [=]() mutable
            {
                auto getLogMessage = [=](auto uiEnabled, auto isProduction) mutable
                {
                    if (uiEnabled) {
                        return std:("Web UI enabled");
                    } else {
                        return std:("Web UI disabled for security (production mode)");
                    }
                };
                expect(getLogMessage(true, false))->toBe(std:("Web UI enabled"));
                expect(getLogMessage(true, true))->toBe(std:("Web UI enabled"));
                expect(getLogMessage(false, true))->toBe(std:("Web UI disabled for security (production mode)"));
                expect(getLogMessage(false, false))->toBe(std:("Web UI disabled for security (production mode)"));
            }
            );
            it(std:("should provide correct startup URL messages"), [=]() mutable
            {
                auto getStartupMessage = [=](auto uiEnabled, auto port) mutable
                {
                    if (uiEnabled) {
                        return std:("\x1b[32mStartup successful!\nGo to the dashboard at \x1b[1mhttp://localhost:") + port + std:("\x1b[22m\x1b[0m");
                    } else {
                        return std:("\x1b[32mStartup successful!\x1b[0m\n\x1b[33mWeb UI disabled.\x1b[0m \x1b[32mAPI endpoints available at:\x1b[0m\n  \x1b[1mhttp://localhost:") + port + std:("/api/server/ping\x1b[22m\x1b[0m\n  \x1b[1mhttp://localhost:") + port + std:("/api/agents\x1b[22m\x1b[0m\n  \x1b[1mhttp://localhost:") + port + std:("/api/messaging\x1b[22m\x1b[0m");
                    }
                };
                auto uiEnabledMsg = getStartupMessage(true, 3000);
                auto uiDisabledMsg = getStartupMessage(false, 3000);
                expect(uiEnabledMsg)->toContain(std:("dashboard at"));
                expect(uiEnabledMsg)->toContain(std:("http://localhost:3000"));
                expect(uiDisabledMsg)->toContain(std:("Web UI disabled."));
                expect(uiDisabledMsg)->toContain(std:("API endpoints available at:"));
                expect(uiDisabledMsg)->toContain(std:("/api/server/ping"));
            }
            );
        }
        );
    }
    );
}

MAIN
