#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-anthropic/__tests__/index.test.h"

any extractAndParseJSON = vi->fn([=](auto x) mutable
{
    return (object{
        object::pair{std:("parsed"), x}
    });
}
);
any ensureReflectionProperties = vi->fn([=](auto obj, auto isRef) mutable
{
    return utils::assign(object{
        , 
        object::pair{std:("_reflection"), isRef}
    }, obj);
}
);
object mockLogger = object{
    object::pair{std:("debug"), vi->fn()}, 
    object::pair{std:("error"), vi->fn()}, 
    object::pair{std:("warn"), vi->fn()}, 
    object::pair{std:("log"), vi->fn()}
};
any mockGenerateText = vi->fn();
object anthropicPlugin = object{
    object::pair{std:("name"), std:("anthropic")}, 
    object::pair{std:("description"), std:("Anthropic plugin (supports text generation only)")}, 
    object::pair{std:("config"), object{
        object::pair{std:("ANTHROPIC_API_KEY"), process->env->ANTHROPIC_API_KEY}, 
        object::pair{std:("ANTHROPIC_SMALL_MODEL"), process->env->ANTHROPIC_SMALL_MODEL}, 
        object::pair{std:("ANTHROPIC_LARGE_MODEL"), process->env->ANTHROPIC_LARGE_MODEL}
    }}, 
    , 
    object::pair{std:("models"), object{
        object::pair{std:("TEXT_SMALL"), [=](auto runtime, auto params) mutable
        {
            auto result = std::async([=]() { mockGenerateText(object{
                object::pair{std:("model"), std:("anthropic(") + (OR((runtime["getSetting"](std:("ANTHROPIC_SMALL_MODEL"))), (std:("claude-3-haiku-20240307")))) + std:(")")}, 
                object::pair{std:("prompt"), params["prompt"]}, 
                object::pair{std:("system"), runtime["character"]["system"]}
            }); });
            return result->text;
        }
        }, 
        object::pair{std:("TEXT_LARGE"), [=](auto runtime, auto params) mutable
        {
            auto result = std::async([=]() { mockGenerateText(object{
                object::pair{std:("model"), std:("anthropic(") + (OR((runtime["getSetting"](std:("ANTHROPIC_LARGE_MODEL"))), (std:("claude-3-5-sonnet-latest")))) + std:(")")}, 
                object::pair{std:("prompt"), params["prompt"]}, 
                object::pair{std:("system"), runtime["character"]["system"]}
            }); });
            return result->text;
        }
        }, 
        object::pair{std:("OBJECT_SMALL"), [=](auto runtime, auto params) mutable
        {
            auto isReflection = Boolean(AND((params["schema"]["facts"]), (params["schema"]["relationships"])));
            auto result = std::async([=]() { mockGenerateText(object{
                object::pair{std:("model"), std:("anthropic(") + (OR((runtime["getSetting"](std:("ANTHROPIC_SMALL_MODEL"))), (std:("claude-3-haiku-20240307")))) + std:(")")}, 
                object::pair{std:("prompt"), params["prompt"]}
            }); });
            try
            {
                auto parsed = extractAndParseJSON(result->text);
                return ensureReflectionProperties(parsed, isReflection);
            }
            catch (const any& e)
            {
                mockLogger["error"](std:("Failed to parse JSON from Anthropic response:"), e);
                throw any(std::make_shared<Error>(std:("Invalid JSON returned from Anthropic model")));
            }
        }
        }
    }}
};
std::function<object(object)> fakeRuntime = [=](auto settings = object{}) mutable
{
    return (object{
        object::pair{std:("getSetting"), [=](auto key) mutable
        {
            return OR((const_(settings)[key]), (nullptr));
        }
        }, 
        object::pair{std:("character"), object{
            object::pair{std:("system"), std:("sys")}
        }}
    });
};

void Main(void)
{
    describe(std:("anthropicPlugin"), [=]() mutable
    {
        beforeEach([=]() mutable
        {
            mockGenerateText->mockReset();
            Object->values(mockLogger)->forEach([=](auto fn) mutable
            {
                return fn["mockReset"]();
            }
            );
            extractAndParseJSON->mockClear();
            ensureReflectionProperties->mockClear();
        }
        );
        it(std:("should the correct name and config"), [=]() mutable
        {
            expect(anthropicPlugin["name"])->toBe(std:("anthropic"));
            expect(anthropicPlugin["config"])->toHaveProperty(std:("ANTHROPIC_API_KEY"));
        }
        );
        it(std:("init warns if API key missing"), [=]() mutable
        {
            auto orig = process->env->ANTHROPIC_API_KEY;
            process->env.Delete("ANTHROPIC_API_KEY");
            std::async([=]() { anthropicPlugin["init"](object{}); });
            expect(mockLogger["warn"])->toHaveBeenCalled();
            if (orig) process->env->ANTHROPIC_API_KEY = orig;
        }
        );
        it(std:("TEXT_SMALL model calls generateText and returns text"), [=]() mutable
        {
            mockGenerateText->mockResolvedValue(object{
                object::pair{std:("text"), std:("result")}
            });
            auto result = std::async([=]() { anthropicPlugin["models"]["TEXT_SMALL"](fakeRuntime(), object{
                object::pair{std:("prompt"), std:("hi")}
            }); });
            expect(result)->toBe(std:("result"));
            expect(mockGenerateText)->toHaveBeenCalled();
        }
        );
        it(std:("TEXT_LARGE model calls generateText and returns text"), [=]() mutable
        {
            mockGenerateText->mockResolvedValue(object{
                object::pair{std:("text"), std:("large")}
            });
            auto result = std::async([=]() { anthropicPlugin["models"]["TEXT_LARGE"](fakeRuntime(), object{
                object::pair{std:("prompt"), std:("hi")}
            }); });
            expect(result)->toBe(std:("large"));
            expect(mockGenerateText)->toHaveBeenCalled();
        }
        );
        it(std:("OBJECT_SMALL model parses and ensures reflection"), [=]() mutable
        {
            mockGenerateText->mockResolvedValue(object{
                object::pair{std:("text"), std:("{"foo":1}")}
            });
            auto params = object{
                object::pair{std:("prompt"), std:("p")}, 
                object::pair{std:("schema"), object{
                    object::pair{std:("facts"), array<any>()}, 
                    object::pair{std:("relationships"), array<any>()}
                }}
            };
            extractAndParseJSON->mockReturnValue(object{
                object::pair{std:("foo"), 1}
            });
            ensureReflectionProperties->mockReturnValue(object{
                object::pair{std:("foo"), 1}, 
                object::pair{std:("_reflection"), true}
            });
            auto result = std::async([=]() { anthropicPlugin["models"]["OBJECT_SMALL"](fakeRuntime(), params); });
            expect(result)->toEqual(object{
                object::pair{std:("foo"), 1}, 
                object::pair{std:("_reflection"), true}
            });
            expect(extractAndParseJSON)->toHaveBeenCalledWith(std:("{"foo":1}"));
            expect(ensureReflectionProperties)->toHaveBeenCalledWith(object{
                object::pair{std:("foo"), 1}
            }, true);
        }
        );
        it(std:("OBJECT_SMALL model throws on parse error"), [=]() mutable
        {
            mockGenerateText->mockResolvedValue(object{
                object::pair{std:("text"), std:("{"foo":1}")}
            });
            extractAndParseJSON->mockImplementation([=]() mutable
            {
                throw any(std::make_shared<Error>(std:("bad")));
            }
            );
            auto params = object{
                object::pair{std:("prompt"), std:("p")}, 
                object::pair{std:("schema"), object{
                    object::pair{std:("facts"), array<any>()}, 
                    object::pair{std:("relationships"), array<any>()}
                }}
            };
            std::async([=]() { expect(anthropicPlugin["models"]["OBJECT_SMALL"](fakeRuntime(), params))->rejects->toThrow(std:("Invalid JSON returned from Anthropic model")); });
            expect(mockLogger["error"])->toHaveBeenCalled();
        }
        );
    }
    );
}

MAIN
