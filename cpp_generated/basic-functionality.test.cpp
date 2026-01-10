#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/server/src/__tests__/basic-functionality.test.h"

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
    }
    );
}

MAIN
