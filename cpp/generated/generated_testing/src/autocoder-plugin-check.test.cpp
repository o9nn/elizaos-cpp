#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-autocoder/src/__tests__/autocoder-plugin-check.test.h"

void Main(void)
{
    describe(std::string("Autocoder Plugin Configuration"), [=]() mutable
    {
        it(std::string("should have correct plugin structure"), [=]() mutable
        {
            expect(autocoderPlugin)->toBeDefined();
            expect(autocoderPlugin->name)->toBe(std::string("@elizaos/plugin-autocoder"));
            expect(autocoderPlugin->description)->toContain(std::string("code generation"));
        }
        );
        it(std::string("should export all required components"), [=]() mutable
        {
            expect(autocoderPlugin->actions)->toBeDefined();
            expect(autocoderPlugin->actions)->toBeInstanceOf(Array);
            auto actionNames = autocoderPlugin->actions->map([=](auto a) mutable
            {
                return a["name"];
            }
            );
            console->log(std::string("Exported actions:"), actionNames);
            expect(actionNames)->toContain(std::string("GENERATE_CODE"));
            expect(actionNames)->toContain(std::string("CREATE_PROJECT"));
            expect(actionNames->length)->toBe(2);
        }
        );
        it(std::string("should export providers"), [=]() mutable
        {
            expect(autocoderPlugin->providers)->toBeDefined();
            expect(autocoderPlugin->providers)->toBeInstanceOf(Array);
            auto providerNames = autocoderPlugin->providers->map([=](auto p) mutable
            {
                return p["name"];
            }
            );
            console->log(std::string("Exported providers:"), providerNames);
            expect(providerNames)->toContain(std::string("PROJECTS_CONTEXT"));
            expect(providerNames)->toContain(std::string("CURRENT_PROJECT_CONTEXT"));
        }
        );
        it(std::string("should export services"), [=]() mutable
        {
            expect(autocoderPlugin->services)->toBeDefined();
            expect(autocoderPlugin->services)->toBeInstanceOf(Array);
            auto serviceNames = autocoderPlugin->services->map([=](auto s) mutable
            {
                return s["serviceName"];
            }
            );
            console->log(std::string("Exported services:"), serviceNames);
            expect(serviceNames)->toContain(std::string("code-generation"));
            expect(serviceNames)->toContain(std::string("project-planning"));
            expect(serviceNames)->toContain(std::string("secrets-manager"));
        }
        );
        it(std::string("should have proper plugin dependencies"), [=]() mutable
        {
            expect(autocoderPlugin->dependencies)->toBeDefined();
            expect(autocoderPlugin->dependencies)->toBeInstanceOf(Array);
            console->log(std::string("Plugin dependencies:"), autocoderPlugin->dependencies);
            expect(autocoderPlugin->dependencies)->toContain(std::string("@elizaos/plugin-forms"));
            expect(autocoderPlugin->dependencies)->toContain(std::string("@elizaos/plugin-plugin-manager"));
        }
        );
        it(std::string("should validate action configurations"), [=]() mutable
        {
            auto generateCodeAction = autocoderPlugin->actions->find([=](auto a) mutable
            {
                return a["name"] == std::string("GENERATE_CODE");
            }
            );
            expect(generateCodeAction)->toBeDefined();
            expect(generateCodeAction->description)->toContain(std::string("code"));
            expect(generateCodeAction->validate)->toBeDefined();
            expect(generateCodeAction->handler)->toBeDefined();
            expect(generateCodeAction->similes)->toContain(std::string("BUILD_CODE"));
            expect(generateCodeAction->similes)->toContain(std::string("CREATE_CODE"));
            expect(generateCodeAction->similes)->toContain(std::string("WRITE_CODE"));
        }
        );
        it(std::string("should have proper provider configuration"), [=]() mutable
        {
            auto projectsProvider = autocoderPlugin->providers->find([=](auto p) mutable
            {
                return p["name"] == std::string("PROJECTS_CONTEXT");
            }
            );
            expect(projectsProvider)->toBeDefined();
            expect(projectsProvider->description)->toContain(std::string("project"));
            auto currentProjectProvider = autocoderPlugin->providers->find([=](auto p) mutable
            {
                return p["name"] == std::string("CURRENT_PROJECT_CONTEXT");
            }
            );
            expect(currentProjectProvider)->toBeDefined();
            expect(currentProjectProvider->description)->toContain(std::string("current project"));
        }
        );
        it(std::string("should have code generation service configured"), [=]() mutable
        {
            auto codeGenService = autocoderPlugin->services->find([=](auto s) mutable
            {
                return s["serviceName"] == std::string("code-generation");
            }
            );
            expect(codeGenService)->toBeDefined();
            expect(codeGenService->serviceType)->toBe(std::string("unknown"));
        }
        );
    }
    );
}

MAIN
