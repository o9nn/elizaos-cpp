#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/project-starter/src/__tests__/routes.test.h"

void Main(void)
{
    describe(std::string("Plugin Routes"), [=]() mutable
    {
        it(std::string("should have routes defined"), [=]() mutable
        {
            expect(plugin->routes)->toBeDefined();
            if (plugin->routes) {
                expect(Array->isArray(plugin->routes))->toBe(true);
                expect(plugin->routes->length)->toBeGreaterThan(0);
            }
        }
        );
        it(std::string("should have a route for /helloworld"), [=]() mutable
        {
            if (plugin->routes) {
                auto helloWorldRoute = plugin->routes->find([=](auto route) mutable
                {
                    return route["path"] == std::string("/helloworld");
                }
                );
                expect(helloWorldRoute)->toBeDefined();
                if (helloWorldRoute) {
                    expect(helloWorldRoute->type)->toBe(std::string("GET"));
                    expect(type_of(helloWorldRoute->handler))->toBe(std::string("function"));
                }
            }
        }
        );
        it(std::string("should handle route requests correctly"), [=]() mutable
        {
            if (plugin->routes) {
                auto helloWorldRoute = plugin->routes->find([=](auto route) mutable
                {
                    return route["path"] == std::string("/helloworld");
                }
                );
                if (AND((helloWorldRoute), (helloWorldRoute->handler))) {
                    auto mockReq = object{};
                    auto mockRes = object{
                        object::pair{std::string("json"), mock()}
                    };
                    auto mockRuntime = as<any>(object{});
                    std::async([=]() { helloWorldRoute->handler(mockReq, mockRes, mockRuntime); });
                    expect(mockRes["json"])->toHaveBeenCalledTimes(1);
                    expect(mockRes["json"])->toHaveBeenCalledWith(object{
                        object::pair{std::string("message"), std::string("Hello World!")}
                    });
                }
            }
        }
        );
        it(std::string("should validate route structure"), [=]() mutable
        {
            if (plugin->routes) {
                plugin->routes->forEach([=](auto route) mutable
                {
                    expect(route)->toHaveProperty(std::string("path"));
                    expect(route)->toHaveProperty(std::string("type"));
                    expect(route)->toHaveProperty(std::string("handler"));
                    expect(type_of(route["path"]))->toBe(std::string("string"));
                    expect(route["path"]["startsWith"](std::string("/")))->toBe(true);
                    expect(array<string>{ std::string("GET"), std::string("POST"), std::string("PUT"), std::string("DELETE"), std::string("PATCH") })->toContain(route["type"]);
                    expect(type_of(route["handler"]))->toBe(std::string("function"));
                }
                );
            }
        }
        );
        it(std::string("should have unique route paths"), [=]() mutable
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
