#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/project-starter/src/__tests__/config.test.h"

any initPlugin = plugin->init;

void Main(void)
{
    mock->module(std:("@elizaos/core"), [=]() mutable
    {
        auto actual = require(std:("@elizaos/core"));
        return utils::assign(object{
            , 
            object::pair{std:("logger"), object{
                object::pair{std:("info"), mock()}, 
                object::pair{std:("error"), mock()}, 
                object::pair{std:("warn"), mock()}
            }}
        }, actual);
    }
    );
    describe(std:("Plugin Configuration Schema"), [=]() mutable
    {
        shared originalEnv = utils::assign(object{
        }, process->env);
        beforeEach([=]() mutable
        {
            mock->restore();
            process->env = utils::assign(object{
            }, originalEnv);
        }
        );
        afterEach([=]() mutable
        {
            process->env = utils::assign(object{
            }, originalEnv);
        }
        );
        it(std:("should accept valid configuration"), [=]() mutable
        {
            auto validConfig = object{
                object::pair{std:("EXAMPLE_PLUGIN_VARIABLE"), std:("valid-value")}
            };
            if (initPlugin) {
                auto error = nullptr;
                try
                {
                    std::async([=]() { initPlugin(validConfig, createMockRuntime()); });
                }
                catch (const any& e)
                {
                    error = as<std::shared_ptr<Error>>(e);
                }
                expect(error)->toBeNull();
            }
        }
        );
        it(std:("should accept empty configuration"), [=]() mutable
        {
            auto emptyConfig = object{};
            if (initPlugin) {
                auto error = nullptr;
                try
                {
                    std::async([=]() { initPlugin(emptyConfig, createMockRuntime()); });
                }
                catch (const any& e)
                {
                    error = as<std::shared_ptr<Error>>(e);
                }
                expect(error)->toBeNull();
            }
        }
        );
        it(std:("should accept configuration with additional properties"), [=]() mutable
        {
            auto configWithExtra = object{
                object::pair{std:("EXAMPLE_PLUGIN_VARIABLE"), std:("valid-value")}, 
                object::pair{std:("EXTRA_PROPERTY"), std:("should be ignored")}
            };
            if (initPlugin) {
                auto error = nullptr;
                try
                {
                    std::async([=]() { initPlugin(configWithExtra, createMockRuntime()); });
                }
                catch (const any& e)
                {
                    error = as<std::shared_ptr<Error>>(e);
                }
                expect(error)->toBeNull();
            }
        }
        );
        it(std:("should reject invalid configuration"), [=]() mutable
        {
            auto invalidConfig = object{
                object::pair{std:("EXAMPLE_PLUGIN_VARIABLE"), string_empty}
            };
            if (initPlugin) {
                auto error = nullptr;
                try
                {
                    std::async([=]() { initPlugin(invalidConfig, createMockRuntime()); });
                }
                catch (const any& e)
                {
                    error = as<std::shared_ptr<Error>>(e);
                }
                expect(error)->not->toBeNull();
            }
        }
        );
        it(std:("should set environment variables from valid config"), [=]() mutable
        {
            auto testConfig = object{
                object::pair{std:("EXAMPLE_PLUGIN_VARIABLE"), std:("test-value")}
            };
            if (initPlugin) {
                process->env.Delete("EXAMPLE_PLUGIN_VARIABLE");
                std::async([=]() { initPlugin(testConfig, createMockRuntime()); });
                expect(process->env->EXAMPLE_PLUGIN_VARIABLE)->toBe(std:("test-value"));
            }
        }
        );
        it(std:("should not override existing environment variables"), [=]() mutable
        {
            process->env->EXAMPLE_PLUGIN_VARIABLE = std:("pre-existing-value");
            auto testConfig = object{};
            if (initPlugin) {
                std::async([=]() { initPlugin(testConfig, createMockRuntime()); });
                expect(process->env->EXAMPLE_PLUGIN_VARIABLE)->toBe(std:("pre-existing-value"));
            }
        }
        );
        it(std:("should handle zod validation errors gracefully"), [=]() mutable
        {
            auto mockZodError = std::make_shared<z->ZodError>(array<object>{ object{
                object::pair{std:("code"), z->ZodIssueCode->too_small}, 
                object::pair{std:("minimum"), 1}, 
                object::pair{std:("type"), std:("string")}, 
                object::pair{std:("inclusive"), true}, 
                object::pair{std:("message"), std:("Example plugin variable is too short")}, 
                object::pair{std:("path"), array<string>{ std:("EXAMPLE_PLUGIN_VARIABLE") }}
            } });
            auto schema = z->object(object{
                object::pair{std:("EXAMPLE_PLUGIN_VARIABLE"), z->string()->min(1)}
            });
            auto originalParseAsync = schema->parseAsync;
            schema->parseAsync = mock()->mockRejectedValue(mockZodError);
            try
            {
                std::async([=]() { schema->parseAsync(object{}); });
                expect(true)->toBe(false);
            }
            catch (const any& error)
            {
                expect(error)->toBe(mockZodError);
            }
            schema->parseAsync = originalParseAsync;
        }
        );
        it(std:("should rethrow non-zod errors"), [=]() mutable
        {
            auto genericError = std::make_shared<Error>(std:("Something went wrong"));
            auto schema = z->object(object{
                object::pair{std:("EXAMPLE_PLUGIN_VARIABLE"), z->string()->min(1)}
            });
            auto originalParseAsync = schema->parseAsync;
            schema->parseAsync = mock()->mockRejectedValue(genericError);
            try
            {
                std::async([=]() { schema->parseAsync(object{}); });
                expect(true)->toBe(false);
            }
            catch (const any& error)
            {
                expect(error)->toBe(genericError);
            }
            schema->parseAsync = originalParseAsync;
        }
        );
    }
    );
}

MAIN
