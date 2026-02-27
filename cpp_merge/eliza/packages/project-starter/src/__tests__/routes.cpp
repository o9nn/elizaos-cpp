#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/project-starter/src/__tests__/routes.test.h"

void Main(void)
{
    describe(std:("Plugin Routes"), [=]() mutable
    {
        it(std:("should have routes defined"), [=]() mutable
        {
            expect(plugin->routes)->toBeDefined();
            if (plugin->routes) {
                expect(Array->isArray(plugin->routes))->toBe(true);
                expect(plugin->routes->length)->toBeGreaterThan(0);
            }
        }
        );
        it(std:("should have a route for /helloworld"), [=]() mutable
        {
            if (plugin->routes) {
                auto helloWorldRoute = plugin->routes->find([=](auto route) mutable
                {
                    return route["path"] == std:("/helloworld");
                }
                );
                expect(helloWorldRoute)->toBeDefined();
                if (helloWorldRoute) {
                    expect(helloWorldRoute->type)->toBe(std:("GET"));
                    expect(type_of(helloWorldRoute->handler))->toBe(std:("function"));
                }
            }
        }
        );
        it(std:("should handle route requests correctly"), [=]() mutable
        {
            if (plugin->routes) {
                auto helloWorldRoute = plugin->routes->find([=](auto route) mutable
                {
                    return route["path"] == std:("/helloworld");
                }
                );
                if (AND((helloWorldRoute), (helloWorldRoute->handler))) {
                    auto mockReq = object{};
                    auto mockRes = object{
                        object::pair{std:("json"), mock()}
                    };
                    auto mockRuntime = as<any>(object{});
                    std::async([=]() { helloWorldRoute->handler(mockReq, mockRes, mockRuntime); });
                    expect(mockRes["json"])->toHaveBeenCalledTimes(1);
                    expect(mockRes["json"])->toHaveBeenCalledWith(object{
                        object::pair{std:("message"), std:("Hello World!")}
                    });
                }
            }
        }
        );
        it(std:("should validate route structure"), [=]() mutable
        {
            if (plugin->routes) {
                plugin->routes->forEach([=](auto route) mutable
                {
                    expect(route)->toHaveProperty(std:("path"));
                    expect(route)->toHaveProperty(std:("type"));
                    expect(route)->toHaveProperty(std:("handler"));
                    expect(type_of(route["path"]))->toBe(std:("string"));
                    expect(route["path"]["startsWith"](std:("/")))->toBe(true);
                    expect(array<string>{ std:("GET"), std:("POST"), std:("PUT"), std:("DELETE"), std:("PATCH") })->toContain(route["type"]);
                    expect(type_of(route["handler"]))->toBe(std:("function"));
                }
                );
            }
        }
        );
        it(std:("should have unique route paths"), [=]() mutable
        {
            if (plugin->routes) {
                auto paths = plugin->routes->map([=](auto route) mutable
                {
                    return route["path"];
                }
                );
                auto uniquePaths = std::make_shared<Set>(paths);
                expect(paths->length)->toBe(uniquePaths->size);
            }
        }
        );
    }
    );
}

MAIN
