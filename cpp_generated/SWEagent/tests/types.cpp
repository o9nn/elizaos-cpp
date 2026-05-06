#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/tests/types.test.h"

void Main(void)
{
    describe(std::string("StepOutput"), [=]() mutable
    {
        it(std::string("should create with default values"), [=]() mutable
        {
            auto step = std::make_shared<StepOutputImpl>();
            expect(step->query)->toEqual(array<object>{ object{} });
            expect(step->thought)->toBe(string_empty);
            expect(step->action)->toBe(string_empty);
            expect(step->output)->toBe(string_empty);
            expect(step->observation)->toBe(string_empty);
            expect(step->executionTime)->toBe(0);
            expect(step->done)->toBe(false);
            expect(step->exitStatus)->toBeNull();
            expect(step->submission)->toBeNull();
            expect(step->state)->toEqual(object{});
            expect(step->extraInfo)->toEqual(object{});
        }
        );
        it(std::string("should convert to template format dict"), [=]() mutable
        {
            auto step = std::make_shared<StepOutputImpl>();
            step->thought = std::string("Test thought");
            step->action = std::string("Test action");
            step->observation = std::string("Test observation");
            step->exitStatus = 0;
            step->submission = std::string("Test submission");
            step->done = true;
            step->executionTime = 1.5;
            auto dict = step->toTemplateFormatDict();
            expect(dict)->toEqual(object{
                object::pair{std::string("thought"), std::string("Test thought")}, 
                object::pair{std::string("action"), std::string("Test action")}, 
                object::pair{std::string("output"), string_empty}, 
                object::pair{std::string("observation"), std::string("Test observation")}, 
                object::pair{std::string("exitStatus"), 0}, 
                object::pair{std::string("submission"), std::string("Test submission")}, 
                object::pair{std::string("done"), true}, 
                object::pair{std::string("executionTime"), 1.5}
            });
        }
        );
    }
    );
    describe(std::string("TrajectoryStep"), [=]() mutable
    {
        it(std::string("should have correct structure"), [=]() mutable
        {
            auto step = object{
                object::pair{std::string("action"), std::string("test action")}, 
                object::pair{std::string("observation"), std::string("test observation")}, 
                object::pair{std::string("response"), std::string("test response")}, 
                object::pair{std::string("state"), object{
                    object::pair{std::string("key"), std::string("value")}
                }}, 
                object::pair{std::string("thought"), std::string("test thought")}, 
                object::pair{std::string("executionTime"), 2.5}, 
                object::pair{std::string("query"), array<object>{ object{
                    object::pair{std::string("test"), std::string("query")}
                } }}, 
                object::pair{std::string("extraInfo"), object{
                    object::pair{std::string("extra"), std::string("info")}
                }}
            };
            expect(step->action)->toBe(std::string("test action"));
            expect(step->state)->toHaveProperty(std::string("key"), std::string("value"));
            expect(step->executionTime)->toBe(2.5);
        }
        );
    }
    );
    describe(std::string("HistoryItem"), [=]() mutable
    {
        it(std::string("should handle optional fields"), [=]() mutable
        {
            auto item = object{
                object::pair{std::string("role"), std::string("user")}, 
                object::pair{std::string("content"), std::string("Test content")}, 
                object::pair{std::string("messageType"), std::string("thought")}
            };
            expect(item->role)->toBe(std::string("user"));
            expect(item->content)->toBe(std::string("Test content"));
            expect(item->messageType)->toBe(std::string("thought"));
            expect(item->agent)->toBeUndefined();
            expect(item->isDemo)->toBeUndefined();
        }
        );
        it(std::string("should handle all fields"), [=]() mutable
        {
            auto item = object{
                object::pair{std::string("role"), std::string("assistant")}, 
                object::pair{std::string("content"), array<object>{ object{
                    object::pair{std::string("type"), std::string("text")}, 
                    object::pair{std::string("text"), std::string("Hello")}
                } }}, 
                object::pair{std::string("messageType"), std::string("action")}, 
                object::pair{std::string("agent"), std::string("test-agent")}, 
                object::pair{std::string("isDemo"), true}, 
                object::pair{std::string("thought"), std::string("Thinking...")}, 
                object::pair{std::string("action"), std::string("Do something")}, 
                object::pair{std::string("toolCalls"), array<object>{ object{
                    object::pair{std::string("id"), std::string("id1")}, 
                    object::pair{std::string("type"), std::string("function")}, 
                    object::pair{std::string("function"), object{
                        object::pair{std::string("name"), std::string("tool1")}, 
                        object::pair{std::string("arguments"), std::string("{}")}
                    }}
                } }}, 
                object::pair{std::string("toolCallIds"), array<string>{ std::string("id1") }}, 
                object::pair{std::string("tags"), array<string>{ std::string("test"), std::string("demo") }}, 
                object::pair{std::string("cacheControl"), object{
                    object::pair{std::string("type"), std::string("ephemeral")}
                }}, 
                object::pair{std::string("thinkingBlocks"), array<object>{ object{
                    object::pair{std::string("type"), std::string("thinking")}, 
                    object::pair{std::string("content"), std::string("thinking")}
                } }}
            };
            expect(item->agent)->toBe(std::string("test-agent"));
            expect(item->isDemo)->toBe(true);
            expect(item->tags)->toContain(std::string("test"));
            expect(item->toolCallIds)->toHaveLength(1);
        }
        );
    }
    );
    describe(std::string("AgentInfo"), [=]() mutable
    {
        it(std::string("should handle optional fields"), [=]() mutable
        {
            auto info = object{};
            expect(info->modelStats)->toBeUndefined();
            expect(info->exitStatus)->toBeUndefined();
            expect(info->submission)->toBeUndefined();
        }
        );
        it(std::string("should handle all fields"), [=]() mutable
        {
            auto info = object{
                object::pair{std::string("modelStats"), object{
                    object::pair{std::string("cost"), 0.5}, 
                    object::pair{std::string("tokens"), 100}
                }}, 
                object::pair{std::string("exitStatus"), std::string("success")}, 
                object::pair{std::string("submission"), std::string("Solution submitted")}, 
                object::pair{std::string("review"), object{
                    object::pair{std::string("score"), 0.8}
                }}, 
                object::pair{std::string("editedFiles30"), std::string("file1.py")}, 
                object::pair{std::string("editedFiles50"), std::string("file1.py, file2.py")}, 
                object::pair{std::string("editedFiles70"), std::string("file1.py, file2.py, file3.py")}, 
                object::pair{std::string("summarizer"), object{
                    object::pair{std::string("summary"), std::string("Test summary")}
                }}, 
                object::pair{std::string("sweAgentHash"), std::string("abc123")}, 
                object::pair{std::string("sweAgentVersion"), std::string("1.0.0")}, 
                object::pair{std::string("sweRexVersion"), std::string("0.1.0")}, 
                object::pair{std::string("sweRexHash"), std::string("def456")}
            };
            expect(info->modelStats)->toHaveProperty(std::string("cost"), 0.5);
            expect(info->exitStatus)->toBe(std::string("success"));
            expect(info->review)->toHaveProperty(std::string("score"), 0.8);
        }
        );
    }
    );
}

MAIN
