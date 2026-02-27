#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/tests/types.test.h"

void Main(void)
{
    describe(std:("StepOutput"), [=]() mutable
    {
        it(std:("should create with default values"), [=]() mutable
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
        it(std:("should convert to template format dict"), [=]() mutable
        {
            auto step = std::make_shared<StepOutputImpl>();
            step->thought = std:("Test thought");
            step->action = std:("Test action");
            step->observation = std:("Test observation");
            step->exitStatus = 0;
            step->submission = std:("Test submission");
            step->done = true;
            step->executionTime = 1.5;
            auto dict = step->toTemplateFormatDict();
            expect(dict)->toEqual(object{
                object::pair{std:("thought"), std:("Test thought")}, 
                object::pair{std:("action"), std:("Test action")}, 
                object::pair{std:("output"), string_empty}, 
                object::pair{std:("observation"), std:("Test observation")}, 
                object::pair{std:("exitStatus"), 0}, 
                object::pair{std:("submission"), std:("Test submission")}, 
                object::pair{std:("done"), true}, 
                object::pair{std:("executionTime"), 1.5}
            });
        }
        );
    }
    );
    describe(std:("TrajectoryStep"), [=]() mutable
    {
        it(std:("should have correct structure"), [=]() mutable
        {
            auto step = object{
                object::pair{std:("action"), std:("test action")}, 
                object::pair{std:("observation"), std:("test observation")}, 
                object::pair{std:("response"), std:("test response")}, 
                object::pair{std:("state"), object{
                    object::pair{std:("key"), std:("value")}
                }}, 
                object::pair{std:("thought"), std:("test thought")}, 
                object::pair{std:("executionTime"), 2.5}, 
                object::pair{std:("query"), array<object>{ object{
                    object::pair{std:("test"), std:("query")}
                } }}, 
                object::pair{std:("extraInfo"), object{
                    object::pair{std:("extra"), std:("info")}
                }}
            };
            expect(step->action)->toBe(std:("test action"));
            expect(step->state)->toHaveProperty(std:("key"), std:("value"));
            expect(step->executionTime)->toBe(2.5);
        }
        );
    }
    );
    describe(std:("HistoryItem"), [=]() mutable
    {
        it(std:("should handle optional fields"), [=]() mutable
        {
            auto item = object{
                object::pair{std:("role"), std:("user")}, 
                object::pair{std:("content"), std:("Test content")}, 
                object::pair{std:("messageType"), std:("thought")}
            };
            expect(item->role)->toBe(std:("user"));
            expect(item->content)->toBe(std:("Test content"));
            expect(item->messageType)->toBe(std:("thought"));
            expect(item->agent)->toBeUndefined();
            expect(item->isDemo)->toBeUndefined();
        }
        );
        it(std:("should handle all fields"), [=]() mutable
        {
            auto item = object{
                object::pair{std:("role"), std:("assistant")}, 
                object::pair{std:("content"), array<object>{ object{
                    object::pair{std:("type"), std:("text")}, 
                    object::pair{std:("text"), std:("Hello")}
                } }}, 
                object::pair{std:("messageType"), std:("action")}, 
                object::pair{std:("agent"), std:("test-agent")}, 
                object::pair{std:("isDemo"), true}, 
                object::pair{std:("thought"), std:("Thinking...")}, 
                object::pair{std:("action"), std:("Do something")}, 
                object::pair{std:("toolCalls"), array<object>{ object{
                    object::pair{std:("id"), std:("id1")}, 
                    object::pair{std:("type"), std:("function")}, 
                    object::pair{std:("function"), object{
                        object::pair{std:("name"), std:("tool1")}, 
                        object::pair{std:("arguments"), std:("{}")}
                    }}
                } }}, 
                object::pair{std:("toolCallIds"), array<string>{ std:("id1") }}, 
                object::pair{std:("tags"), array<string>{ std:("test"), std:("demo") }}, 
                object::pair{std:("cacheControl"), object{
                    object::pair{std:("type"), std:("ephemeral")}
                }}, 
                object::pair{std:("thinkingBlocks"), array<object>{ object{
                    object::pair{std:("type"), std:("thinking")}, 
                    object::pair{std:("content"), std:("thinking")}
                } }}
            };
            expect(item->agent)->toBe(std:("test-agent"));
            expect(item->isDemo)->toBe(true);
            expect(item->tags)->toContain(std:("test"));
            expect(item->toolCallIds)->toHaveLength(1);
        }
        );
    }
    );
    describe(std:("AgentInfo"), [=]() mutable
    {
        it(std:("should handle optional fields"), [=]() mutable
        {
            auto info = object{};
            expect(info->modelStats)->toBeUndefined();
            expect(info->exitStatus)->toBeUndefined();
            expect(info->submission)->toBeUndefined();
        }
        );
        it(std:("should handle all fields"), [=]() mutable
        {
            auto info = object{
                object::pair{std:("modelStats"), object{
                    object::pair{std:("cost"), 0.5}, 
                    object::pair{std:("tokens"), 100}
                }}, 
                object::pair{std:("exitStatus"), std:("success")}, 
                object::pair{std:("submission"), std:("Solution submitted")}, 
                object::pair{std:("review"), object{
                    object::pair{std:("score"), 0.8}
                }}, 
                object::pair{std:("editedFiles30"), std:("file1.py")}, 
                object::pair{std:("editedFiles50"), std:("file1.py, file2.py")}, 
                object::pair{std:("editedFiles70"), std:("file1.py, file2.py, file3.py")}, 
                object::pair{std:("summarizer"), object{
                    object::pair{std:("summary"), std:("Test summary")}
                }}, 
                object::pair{std:("sweAgentHash"), std:("abc123")}, 
                object::pair{std:("sweAgentVersion"), std:("1.0.0")}, 
                object::pair{std:("sweRexVersion"), std:("0.1.0")}, 
                object::pair{std:("sweRexHash"), std:("def456")}
            };
            expect(info->modelStats)->toHaveProperty(std:("cost"), 0.5);
            expect(info->exitStatus)->toBe(std:("success"));
            expect(info->review)->toHaveProperty(std:("score"), 0.8);
        }
        );
    }
    );
}

MAIN
