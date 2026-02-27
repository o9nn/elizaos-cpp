#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/server/src/__tests__/basic-functionality.test.h"

void Main(void)
{
    describe(std::string("Basic Server Functionality"), [=]() mutable
    {
        describe(std::string("Path utilities"), [=]() mutable
        {
            it(std::string("should handle path expansion logic"), [=]() mutable
            {
                auto testExpandTildePath = [=](auto filepath) mutable
                {
                    if (AND((filepath), (filepath->startsWith(std::string("~"))))) {
                        return path->join(process->cwd(), filepath->slice(1));
                    }
                    return filepath;
                };
                expect(testExpandTildePath(std::string("~/test")))->toBe(path->join(process->cwd(), std::string("test")));
                expect(testExpandTildePath(std::string("/absolute")))->toBe(std::string("/absolute"));
                expect(testExpandTildePath(std::string("relative")))->toBe(std::string("relative"));
                expect(testExpandTildePath(string_empty))->toBe(string_empty);
            }
            );
        }
        );
        describe(std::string("UUID validation logic"), [=]() mutable
        {
            it(std::string("should validate UUID format correctly"), [=]() mutable
            {
                auto validateUuidPattern = [=](auto id) mutable
                {
                    auto uuidRegex = (new RegExp(std::string("^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$")));
                    return uuidRegex->test(id);
                };
                expect(validateUuidPattern(std::string("123e4567-e89b-12d3-a456-426614174000")))->toBe(true);
                expect(validateUuidPattern(std::string("00000000-0000-0000-0000-000000000000")))->toBe(true);
                expect(validateUuidPattern(std::string("ffffffff-ffff-ffff-ffff-ffffffffffff")))->toBe(true);
                expect(validateUuidPattern(std::string("invalid-uuid")))->toBe(false);
                expect(validateUuidPattern(std::string("123e4567e89b12d3a456426614174000")))->toBe(false);
                expect(validateUuidPattern(std::string("123e4567-e89b-12d3-a456-42661417400")))->toBe(false);
                expect(validateUuidPattern(string_empty))->toBe(false);
            }
            );
        }
        );
        describe(std::string("Security pattern detection"), [=]() mutable
        {
            it(std::string("should detect suspicious patterns"), [=]() mutable
            {
                auto detectSuspiciousPatterns = [=](auto input) mutable
                {
                    auto suspiciousPatterns = array<string>{ std::string(".."), std::string("<"), std::string(">"), std::string("""), std::string("'"), std::string("\"), std::string("/") };
                    return suspiciousPatterns->some([=](auto pattern) mutable
                    {
                        return input->includes(pattern);
                    }
                    );
                };
                expect(detectSuspiciousPatterns(std::string("test../path")))->toBe(true);
                expect(detectSuspiciousPatterns(std::string("test<script>")))->toBe(true);
                expect(detectSuspiciousPatterns(std::string("test>alert")))->toBe(true);
                expect(detectSuspiciousPatterns(std::string("test"quote")))->toBe(true);
                expect(detectSuspiciousPatterns(std::string("test'quote")))->toBe(true);
                expect(detectSuspiciousPatterns(std::string("test\backslash")))->toBe(true);
                expect(detectSuspiciousPatterns(std::string("test/slash")))->toBe(true);
                expect(detectSuspiciousPatterns(std::string("123e4567-e89b-12d3-a456-426614174000")))->toBe(false);
                expect(detectSuspiciousPatterns(std::string("cleantext")))->toBe(false);
                expect(detectSuspiciousPatterns(std::string("clean-text-123")))->toBe(false);
            }
            );
            it(std::string("should detect path traversal attempts"), [=]() mutable
            {
                auto containsPathTraversal = [=](auto path) mutable
                {
                    return OR((path->includes(std::string("../"))), (path->includes(std::string("..\"))));
                };
                expect(containsPathTraversal(std::string("../../../etc/passwd")))->toBe(true);
                expect(containsPathTraversal(std::string("normal/path")))->toBe(false);
                expect(containsPathTraversal(std::string("..\windows\system32")))->toBe(true);
            }
            );
            it(std::string("should detect script injection patterns"), [=]() mutable
            {
                auto containsScriptInjection = [=](auto input) mutable
                {
                    auto scriptPatterns = array<string>{ std::string("<script"), std::string("javascript:"), std::string("onerror="), std::string("onload=") };
                    return scriptPatterns->some([=](auto pattern) mutable
                    {
                        return input->toLowerCase()->includes(pattern);
                    }
                    );
                };
                expect(containsScriptInjection(std::string("<script>alert(1)</script>")))->toBe(true);
                expect(containsScriptInjection(std::string("javascript:alert(1)")))->toBe(true);
                expect(containsScriptInjection(std::string("<img src=x onerror=alert(1)>")))->toBe(true);
                expect(containsScriptInjection(std::string("normal text")))->toBe(false);
            }
            );
        }
        );
        describe(std::string("Rate limiting logic"), [=]() mutable
        {
            it(std::string("should implement basic rate limiting concepts"), [=]() mutable
            {
                auto rateLimiter = std::make_shared<SimpleRateLimiter>(60000, 5);
                for (auto i = 0; i < 5; i++)
                {
                    expect(rateLimiter->isAllowed(std::string("client1")))->toBe(true);
                }
                expect(rateLimiter->isAllowed(std::string("client1")))->toBe(false);
                expect(rateLimiter->isAllowed(std::string("client2")))->toBe(true);
            }
            );
        }
        );
        describe(std::string("Middleware patterns"), [=]() mutable
        {
            it(std::string("should implement basic middleware concepts"), [=]() mutable
            {
                typedef std::function<void(any, any, std::function<void()>)> MiddlewareFunction;

                auto createValidationMiddleware = [=](auto paramName) mutable
                {
                    return [=](auto req, auto res, auto next) mutable
                    {
                        auto paramValue = const_(req["params"])[paramName];
                        if (!paramValue) {
                            res["error"] = std::string("Missing parameter");
                            return;
                        }
                        auto uuidRegex = (new RegExp(std::string("^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$")));
                        if (!uuidRegex->test(paramValue)) {
                            res["error"] = std::string("Invalid format");
                            return;
                        }
                        next();
                    };
                };
                auto middleware = createValidationMiddleware(std::string("id"));
                auto validReq = object{
                    object::pair{std::string("params"), object{
                        object::pair{std::string("id"), std::string("123e4567-e89b-12d3-a456-426614174000")}
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
                    object::pair{std::string("params"), object{
                        object::pair{std::string("id"), std::string("invalid-id")}
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
                expect(invalidRes["error"])->toBe(std::string("Invalid format"));
            }
            );
        }
        );
        describe(std::string("Server configuration patterns"), [=]() mutable
        {
            it(std::string("should handle server options correctly"), [=]() mutable
            {
                auto createServerConfig = [=](auto options = object{}) mutable
                {
                    return object{
                        object::pair{std::string("dataDir"), OR((options->dataDir), (std::string("./default-data")))}, 
                        object::pair{std::string("middlewares"), OR((options->middlewares), (array<any>()))}, 
                        object::pair{std::string("postgresUrl"), OR((options->postgresUrl), (nullptr))}, 
                        object::pair{std::string("hasCustomDataDir"), !!options->dataDir}, 
                        object::pair{std::string("hasCustomDb"), !!options->postgresUrl}
                    };
                };
                auto defaultConfig = createServerConfig();
                expect(defaultConfig["dataDir"])->toBe(std::string("./default-data"));
                expect(defaultConfig["middlewares"])->toEqual(array<any>());
                expect(defaultConfig["postgresUrl"])->toBeNull();
                expect(defaultConfig["hasCustomDataDir"])->toBe(false);
                expect(defaultConfig["hasCustomDb"])->toBe(false);
                auto customConfig = createServerConfig(object{
                    object::pair{std::string("dataDir"), std::string("/custom/data")}, 
                    object::pair{std::string("postgresUrl"), std::string("postgresql://localhost:5432/test")}, 
                    object::pair{std::string("middlewares"), array<string>{ std::string("middleware1"), std::string("middleware2") }}
                });
                expect(customConfig["dataDir"])->toBe(std::string("/custom/data"));
                expect(customConfig["postgresUrl"])->toBe(std::string("postgresql://localhost:5432/test"));
                expect(customConfig["middlewares"])->toEqual(array<string>{ std::string("middleware1"), std::string("middleware2") });
                expect(customConfig["hasCustomDataDir"])->toBe(true);
                expect(customConfig["hasCustomDb"])->toBe(true);
            }
            );
        }
        );
        describe(std::string("UI Enable/Disable Logic"), [=]() mutable
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
            it(std::string("should enable UI by default in development"), [=]() mutable
            {
                expect(testUIEnabled(std::string("development"), undefined))->toBe(true);
                expect(testUIEnabled(std::string("test"), undefined))->toBe(true);
                expect(testUIEnabled(undefined, undefined))->toBe(true);
            }
            );
            it(std::string("should disable UI by default in production"), [=]() mutable
            {
                expect(testUIEnabled(std::string("production"), undefined))->toBe(false);
            }
            );
            it(std::string("should allow explicit override with ELIZA_UI_ENABLE=true"), [=]() mutable
            {
                expect(testUIEnabled(std::string("production"), std::string("true")))->toBe(true);
                expect(testUIEnabled(std::string("development"), std::string("true")))->toBe(true);
                expect(testUIEnabled(std::string("production"), std::string("TRUE")))->toBe(true);
            }
            );
            it(std::string("should allow explicit override with ELIZA_UI_ENABLE=false"), [=]() mutable
            {
                expect(testUIEnabled(std::string("development"), std::string("false")))->toBe(false);
                expect(testUIEnabled(std::string("production"), std::string("false")))->toBe(false);
                expect(testUIEnabled(std::string("development"), std::string("FALSE")))->toBe(false);
            }
            );
            it(std::string("should treat empty strings as undefined"), [=]() mutable
            {
                expect(testUIEnabled(std::string("development"), string_empty))->toBe(true);
                expect(testUIEnabled(std::string("production"), string_empty))->toBe(false);
            }
            );
            it(std::string("should handle various boolean-like values using parseBooleanFromText"), [=]() mutable
            {
                expect(testUIEnabled(std::string("production"), std::string("1")))->toBe(true);
                expect(testUIEnabled(std::string("production"), std::string("yes")))->toBe(true);
                expect(testUIEnabled(std::string("production"), std::string("YES")))->toBe(true);
                expect(testUIEnabled(std::string("production"), std::string("on")))->toBe(true);
                expect(testUIEnabled(std::string("production"), std::string("enable")))->toBe(true);
                expect(testUIEnabled(std::string("development"), std::string("0")))->toBe(false);
                expect(testUIEnabled(std::string("development"), std::string("no")))->toBe(false);
                expect(testUIEnabled(std::string("development"), std::string("off")))->toBe(false);
                expect(testUIEnabled(std::string("development"), std::string("disable")))->toBe(false);
                expect(testUIEnabled(std::string("development"), std::string("invalid")))->toBe(false);
                expect(testUIEnabled(std::string("development"), std::string("maybe")))->toBe(false);
            }
            );
            it(std::string("should generate appropriate log messages"), [=]() mutable
            {
                auto getLogMessage = [=](auto uiEnabled, auto isProduction) mutable
                {
                    if (uiEnabled) {
                        return std::string("Web UI enabled");
                    } else {
                        return std::string("Web UI disabled for security (production mode)");
                    }
                };
                expect(getLogMessage(true, false))->toBe(std::string("Web UI enabled"));
                expect(getLogMessage(true, true))->toBe(std::string("Web UI enabled"));
                expect(getLogMessage(false, true))->toBe(std::string("Web UI disabled for security (production mode)"));
                expect(getLogMessage(false, false))->toBe(std::string("Web UI disabled for security (production mode)"));
            }
            );
            it(std::string("should provide correct startup URL messages"), [=]() mutable
            {
                auto getStartupMessage = [=](auto uiEnabled, auto port) mutable
                {
                    if (uiEnabled) {
                        return std::string("\x1b[32mStartup successful!\nGo to the dashboard at \x1b[1mhttp://localhost:") + port + std::string("\x1b[22m\x1b[0m");
                    } else {
                        return std::string("\x1b[32mStartup successful!\x1b[0m\n\x1b[33mWeb UI disabled.\x1b[0m \x1b[32mAPI endpoints available at:\x1b[0m\n  \x1b[1mhttp://localhost:") + port + std::string("/api/server/ping\x1b[22m\x1b[0m\n  \x1b[1mhttp://localhost:") + port + std::string("/api/agents\x1b[22m\x1b[0m\n  \x1b[1mhttp://localhost:") + port + std::string("/api/messaging\x1b[22m\x1b[0m");
                    }
                };
                auto uiEnabledMsg = getStartupMessage(true, 3000);
                auto uiDisabledMsg = getStartupMessage(false, 3000);
                expect(uiEnabledMsg)->toContain(std::string("dashboard at"));
                expect(uiEnabledMsg)->toContain(std::string("http://localhost:3000"));
                expect(uiDisabledMsg)->toContain(std::string("Web UI disabled."));
                expect(uiDisabledMsg)->toContain(std::string("API endpoints available at:"));
                expect(uiDisabledMsg)->toContain(std::string("/api/server/ping"));
            }
            );
        }
        );
    }
    );
}

MAIN
