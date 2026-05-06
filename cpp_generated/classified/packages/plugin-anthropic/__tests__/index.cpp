#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-anthropic/__tests__/index.test.h"

any extractAndParseJSON = vi->fn([=](auto x) mutable
{
    return (object{
        object::pair{std::string("parsed"), x}
    });
}
);
any ensureReflectionProperties = vi->fn([=](auto obj, auto isRef) mutable
{
    return utils::assign(object{
        , 
        object::pair{std::string("_reflection"), isRef}
    }, obj);
}
);
object mockLogger = object{
    object::pair{std::string("debug"), vi->fn()}, 
    object::pair{std::string("error"), vi->fn()}, 
    object::pair{std::string("warn"), vi->fn()}, 
    object::pair{std::string("log"), vi->fn()}
};
any mockGenerateText = vi->fn();
object anthropicPlugin = object{
    object::pair{std::string("name"), std::string("anthropic")}, 
    object::pair{std::string("description"), std::string("Anthropic plugin (supports text generation only)")}, 
    object::pair{std::string("config"), object{
        object::pair{std::string("ANTHROPIC_API_KEY"), process->env->ANTHROPIC_API_KEY}, 
        object::pair{std::string("ANTHROPIC_SMALL_MODEL"), process->env->ANTHROPIC_SMALL_MODEL}, 
        object::pair{std::string("ANTHROPIC_LARGE_MODEL"), process->env->ANTHROPIC_LARGE_MODEL}
    }}, 
    , 
    object::pair{std::string("models"), object{
        object::pair{std::string("TEXT_SMALL"), [=](auto runtime, auto params) mutable
        {
            auto result = std::async([=]() { mockGenerateText(object{
                object::pair{std::string("model"), std::string("anthropic(") + (OR((runtime["getSetting"](std::string("ANTHROPIC_SMALL_MODEL"))), (std::string("claude-3-haiku-20240307")))) + std::string(")")}, 
                object::pair{std::string("prompt"), params["prompt"]}, 
                object::pair{std::string("system"), runtime["character"]["system"]}
            }); });
            return result->text;
        }
        }, 
        object::pair{std::string("TEXT_LARGE"), [=](auto runtime, auto params) mutable
        {
            auto result = std::async([=]() { mockGenerateText(object{
                object::pair{std::string("model"), std::string("anthropic(") + (OR((runtime["getSetting"](std::string("ANTHROPIC_LARGE_MODEL"))), (std::string("claude-3-5-sonnet-latest")))) + std::string(")")}, 
                object::pair{std::string("prompt"), params["prompt"]}, 
                object::pair{std::string("system"), runtime["character"]["system"]}
            }); });
            return result->text;
        }
        }, 
        object::pair{std::string("OBJECT_SMALL"), [=](auto runtime, auto params) mutable
        {
            auto isReflection = Boolean(AND((params["schema"]["facts"]), (params["schema"]["relationships"])));
            auto result = std::async([=]() { mockGenerateText(object{
                object::pair{std::string("model"), std::string("anthropic(") + (OR((runtime["getSetting"](std::string("ANTHROPIC_SMALL_MODEL"))), (std::string("claude-3-haiku-20240307")))) + std::string(")")}, 
                object::pair{std::string("prompt"), params["prompt"]}
            }); });
            try
            {
                auto parsed = extractAndParseJSON(result->text);
                return ensureReflectionProperties(parsed, isReflection);
            }
            catch (const any& e)
            {
                mockLogger["error"](std::string("Failed to parse JSON from Anthropic response:"), e);
                throw any(std::make_shared<Error>(std::string("Invalid JSON returned from Anthropic model")));
            }
        }
        }
    }}
};
std::function<object(object)> fakeRuntime = [=](auto settings = object{}) mutable
{
    return (object{
        object::pair{std::string("getSetting"), [=](auto key) mutable
        {
            return OR((const_(settings)[key]), (nullptr));
        }
        }, 
        object::pair{std::string("character"), object{
            object::pair{std::string("system"), std::string("sys")}
        }}
    });
};

void Main(void)
{
    describe(std::string("anthropicPlugin"), [=]() mutable
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
        it(std::string("should the correct name and config"), [=]() mutable
        {
            expect(anthropicPlugin["name"])->toBe(std::string("anthropic"));
            expect(anthropicPlugin["config"])->toHaveProperty(std::string("ANTHROPIC_API_KEY"));
        }
        );
        it(std::string("init warns if API key missing"), [=]() mutable
        {
            auto orig = process->env->ANTHROPIC_API_KEY;
            process->env.Delete("ANTHROPIC_API_KEY");
            std::async([=]() { anthropicPlugin["init"](object{}); });
            expect(mockLogger["warn"])->toHaveBeenCalled();
            if (orig) process->env->ANTHROPIC_API_KEY = orig;
        }
        );
        it(std::string("TEXT_SMALL model calls generateText and returns text"), [=]() mutable
        {
            mockGenerateText->mockResolvedValue(object{
                object::pair{std::string("text"), std::string("result")}
            });
            auto result = std::async([=]() { anthropicPlugin["models"]["TEXT_SMALL"](fakeRuntime(), object{
                object::pair{std::string("prompt"), std::string("hi")}
            }); });
            expect(result)->toBe(std::string("result"));
            expect(mockGenerateText)->toHaveBeenCalled();
        }
        );
        it(std::string("TEXT_LARGE model calls generateText and returns text"), [=]() mutable
        {
            mockGenerateText->mockResolvedValue(object{
                object::pair{std::string("text"), std::string("large")}
            });
            auto result = std::async([=]() { anthropicPlugin["models"]["TEXT_LARGE"](fakeRuntime(), object{
                object::pair{std::string("prompt"), std::string("hi")}
            }); });
            expect(result)->toBe(std::string("large"));
            expect(mockGenerateText)->toHaveBeenCalled();
        }
        );
        it(std::string("OBJECT_SMALL model parses and ensures reflection"), [=]() mutable
        {
            mockGenerateText->mockResolvedValue(object{
                object::pair{std::string("text"), std::string("{"foo":1}")}
            });
            auto params = object{
                object::pair{std::string("prompt"), std::string("p")}, 
                object::pair{std::string("schema"), object{
                    object::pair{std::string("facts"), array<any>()}, 
                    object::pair{std::string("relationships"), array<any>()}
                }}
            };
            extractAndParseJSON->mockReturnValue(object{
                object::pair{std::string("foo"), 1}
            });
            ensureReflectionProperties->mockReturnValue(object{
                object::pair{std::string("foo"), 1}, 
                object::pair{std::string("_reflection"), true}
            });
            auto result = std::async([=]() { anthropicPlugin["models"]["OBJECT_SMALL"](fakeRuntime(), params); });
            expect(result)->toEqual(object{
                object::pair{std::string("foo"), 1}, 
                object::pair{std::string("_reflection"), true}
            });
            expect(extractAndParseJSON)->toHaveBeenCalledWith(std::string("{"foo":1}"));
            expect(ensureReflectionProperties)->toHaveBeenCalledWith(object{
                object::pair{std::string("foo"), 1}
            }, true);
        }
        );
        it(std::string("OBJECT_SMALL model throws on parse error"), [=]() mutable
        {
            mockGenerateText->mockResolvedValue(object{
                object::pair{std::string("text"), std::string("{"foo":1}")}
            });
            extractAndParseJSON->mockImplementation([=]() mutable
            {
                throw any(std::make_shared<Error>(std::string("bad")));
            }
            );
            auto params = object{
                object::pair{std::string("prompt"), std::string("p")}, 
                object::pair{std::string("schema"), object{
                    object::pair{std::string("facts"), array<any>()}, 
                    object::pair{std::string("relationships"), array<any>()}
                }}
            };
            std::async([=]() { expect(anthropicPlugin["models"]["OBJECT_SMALL"](fakeRuntime(), params))->rejects->toThrow(std::string("Invalid JSON returned from Anthropic model")); });
            expect(mockLogger["error"])->toHaveBeenCalled();
        }
        );
    }
    );
}

MAIN
