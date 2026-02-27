#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-autocoder/src/__tests__/autocoder-plugin-check.test.h"

void Main(void)
{
    describe(std:("Autocoder Plugin Configuration"), [=]() mutable
    {
        it(std:("should have correct plugin structure"), [=]() mutable
        {
            expect(autocoderPlugin)->toBeDefined();
            expect(autocoderPlugin->name)->toBe(std:("@elizaos/plugin-autocoder"));
            expect(autocoderPlugin->description)->toContain(std:("code generation"));
        }
        );
        it(std:("should all required components"), [=]() mutable
        {
            expect(autocoderPlugin->actions)->toBeDefined();
            expect(autocoderPlugin->actions)->toBeInstanceOf(Array);
            auto actionNames = autocoderPlugin->actions->map([=](auto a) mutable
            {
                return a["name"];
            }
            );
            console->log(std:("Exported actions:"), actionNames);
            expect(actionNames)->toContain(std:("GENERATE_CODE"));
            expect(actionNames)->toContain(std:("CREATE_PROJECT"));
            expect(actionNames->length)->toBe(2);
        }
        );
        it(std:("should providers"), [=]() mutable
        {
            expect(autocoderPlugin->providers)->toBeDefined();
            expect(autocoderPlugin->providers)->toBeInstanceOf(Array);
            auto providerNames = autocoderPlugin->providers->map([=](auto p) mutable
            {
                return p["name"];
            }
            );
            console->log(std:("Exported providers:"), providerNames);
            expect(providerNames)->toContain(std:("PROJECTS_CONTEXT"));
            expect(providerNames)->toContain(std:("CURRENT_PROJECT_CONTEXT"));
        }
        );
        it(std:("should services"), [=]() mutable
        {
            expect(autocoderPlugin->services)->toBeDefined();
            expect(autocoderPlugin->services)->toBeInstanceOf(Array);
            auto serviceNames = autocoderPlugin->services->map([=](auto s) mutable
            {
                return s["serviceName"];
            }
            );
            console->log(std:("Exported services:"), serviceNames);
            expect(serviceNames)->toContain(std:("code-generation"));
            expect(serviceNames)->toContain(std:("project-planning"));
            expect(serviceNames)->toContain(std:("secrets-manager"));
        }
        );
        it(std:("should have proper plugin dependencies"), [=]() mutable
        {
            expect(autocoderPlugin->dependencies)->toBeDefined();
            expect(autocoderPlugin->dependencies)->toBeInstanceOf(Array);
            console->log(std:("Plugin dependencies:"), autocoderPlugin->dependencies);
            expect(autocoderPlugin->dependencies)->toContain(std:("@elizaos/plugin-forms"));
            expect(autocoderPlugin->dependencies)->toContain(std:("@elizaos/plugin-plugin-manager"));
        }
        );
        it(std:("should validate action configurations"), [=]() mutable
        {
            auto generateCodeAction = autocoderPlugin->actions->find([=](auto a) mutable
            {
                return a["name"] == std:("GENERATE_CODE");
            }
            );
            expect(generateCodeAction)->toBeDefined();
            expect(generateCodeAction->description)->toContain(std:("code"));
            expect(generateCodeAction->validate)->toBeDefined();
            expect(generateCodeAction->handler)->toBeDefined();
            expect(generateCodeAction->similes)->toContain(std:("BUILD_CODE"));
            expect(generateCodeAction->similes)->toContain(std:("CREATE_CODE"));
            expect(generateCodeAction->similes)->toContain(std:("WRITE_CODE"));
        }
        );
        it(std:("should have proper provider configuration"), [=]() mutable
        {
            auto projectsProvider = autocoderPlugin->providers->find([=](auto p) mutable
            {
                return p["name"] == std:("PROJECTS_CONTEXT");
            }
            );
            expect(projectsProvider)->toBeDefined();
            expect(projectsProvider->description)->toContain(std:("project"));
            auto currentProjectProvider = autocoderPlugin->providers->find([=](auto p) mutable
            {
                return p["name"] == std:("CURRENT_PROJECT_CONTEXT");
            }
            );
            expect(currentProjectProvider)->toBeDefined();
            expect(currentProjectProvider->description)->toContain(std:("current project"));
        }
        );
        it(std:("should have code generation service configured"), [=]() mutable
        {
            auto codeGenService = autocoderPlugin->services->find([=](auto s) mutable
            {
                return s["serviceName"] == std:("code-generation");
            }
            );
            expect(codeGenService)->toBeDefined();
            expect(codeGenService->serviceType)->toBe(std:("unknown"));
        }
        );
    }
    );
}

MAIN
