#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-inference/src/__tests__/claude-proxy-openai.test.h"

void Main(void)
{
    describe(std::string("Claude Proxy with OpenAI Fallback"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> runtime;
        shared<any> claudeHandler;
        beforeAll([=]() mutable
        {
            auto dotenv = require(std::string("dotenv"));
            auto path = require(std::string("path"));
            auto envPath = path["join"](process->cwd(), std::string(".."), std::string(".."), std::string(".env"));
            dotenv["config"](object{
                object::pair{std::string("path"), envPath}
            });
            runtime = std::make_shared<AgentRuntime>(object{
                object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("character"), object{
                    object::pair{std::string("name"), std::string("Test Agent")}, 
                    object::pair{std::string("bio"), array<string>{ std::string("Test agent for Claude proxy OpenAI fallback") }}, 
                    object::pair{std::string("system"), std::string("You are a helpful assistant")}, 
                    object::pair{std::string("settings"), object{
                        object::pair{std::string("OPENAI_API_KEY"), process->env->OPENAI_API_KEY}
                    }}
                }}, 
                object::pair{std::string("plugins"), array<any>{ sqlPlugin, openaiPlugin, inferencePlugin }}
            });
            std::async([=]() { runtime->initialize(); });
            claudeHandler = createClaudeHandler(runtime);
        }
        );
        it(std::string("should fallback to OpenAI when Anthropic is not available"), [=]() mutable
        {
            if (!process->env->OPENAI_API_KEY) {
                console->log(std::string("Skipping test - OPENAI_API_KEY not found"));
                return std::shared_ptr<Promise<void>>();
            }
            console->log(std::string("Testing Claude proxy with OpenAI fallback..."));
            console->log(std::string("OpenAI API Key present:"), !!process->env->OPENAI_API_KEY);
            console->log(std::string("Anthropic API Key in runtime:"), !!runtime->getSetting(std::string("ANTHROPIC_API_KEY")));
            auto claudeRequest = object{
                object::pair{std::string("model"), std::string("claude-3-opus-20240229")}, 
                object::pair{std::string("messages"), array<object>{ object{
                    object::pair{std::string("role"), as<std::shared_ptr<const>>(std::string("user"))}, 
                    object::pair{std::string("content"), std::string("Reply with exactly: "OpenAI fallback working"")}
                } }}, 
                object::pair{std::string("max_tokens"), 50}, 
                object::pair{std::string("temperature"), 0.1}
            };
            auto response = std::async([=]() { claudeHandler(claudeRequest); });
            expect(response)->toBeDefined();
            expect(response["type"])->toBe(std::string("message"));
            expect(response["role"])->toBe(std::string("assistant"));
            expect(response["content"])->toBeInstanceOf(Array);
            expect(const_(response["content"])[0])->toHaveProperty(std::string("type"), std::string("text"));
            expect(const_(response["content"])[0]["text"])->toBeTruthy();
            console->log(std::string("OpenAI fallback response:"), const_(response["content"])[0]["text"]);
            auto responseText = OR((const_(response["content"])[0]["text"]["toLowerCase"]()), (string_empty));
            expect(responseText)->toContain(std::string("openai"));
            expect(responseText)->toContain(std::string("fallback"));
        }
        );
        it(std::string("should handle complex conversations through OpenAI"), [=]() mutable
        {
            if (!process->env->OPENAI_API_KEY) {
                console->log(std::string("Skipping test - OPENAI_API_KEY not found"));
                return std::shared_ptr<Promise<void>>();
            }
            auto claudeRequest = object{
                object::pair{std::string("model"), std::string("claude-3-opus-20240229")}, 
                object::pair{std::string("system"), std::string("You are a helpful math tutor.")}, 
                object::pair{std::string("messages"), array<object>{ object{
                    object::pair{std::string("role"), as<std::shared_ptr<const>>(std::string("user"))}, 
                    object::pair{std::string("content"), std::string("What is the square root of 144?")}
                }, object{
                    object::pair{std::string("role"), as<std::shared_ptr<const>>(std::string("assistant"))}, 
                    object::pair{std::string("content"), std::string("The square root of 144 is 12.")}
                }, object{
                    object::pair{std::string("role"), as<std::shared_ptr<const>>(std::string("user"))}, 
                    object::pair{std::string("content"), std::string("And what is 12 squared?")}
                } }}, 
                object::pair{std::string("max_tokens"), 100}, 
                object::pair{std::string("temperature"), 0.1}
            };
            auto response = std::async([=]() { claudeHandler(claudeRequest); });
            expect(response)->toBeDefined();
            auto responseText = OR((const_(response["content"])[0]["text"]), (string_empty));
            console->log(std::string("Math conversation response:"), responseText);
            expect(responseText)->toContain(std::string("144"));
        }
        );
        it(std::string("should properly format Claude response structure"), [=]() mutable
        {
            if (!process->env->OPENAI_API_KEY) {
                console->log(std::string("Skipping test - OPENAI_API_KEY not found"));
                return std::shared_ptr<Promise<void>>();
            }
            auto claudeRequest = object{
                object::pair{std::string("model"), std::string("claude-3-sonnet-20240229")}, 
                object::pair{std::string("messages"), array<object>{ object{
                    object::pair{std::string("role"), as<std::shared_ptr<const>>(std::string("user"))}, 
                    object::pair{std::string("content"), std::string("Say hello")}
                } }}, 
                object::pair{std::string("max_tokens"), 20}, 
                object::pair{std::string("temperature"), 0.5}
            };
            auto response = std::async([=]() { claudeHandler(claudeRequest); });
            expect(response)->toHaveProperty(std::string("id"));
            expect(response)->toHaveProperty(std::string("type"), std::string("message"));
            expect(response)->toHaveProperty(std::string("role"), std::string("assistant"));
            expect(response)->toHaveProperty(std::string("model"));
            expect(response)->toHaveProperty(std::string("content"));
            expect(response)->toHaveProperty(std::string("stop_reason"));
            expect(response)->toHaveProperty(std::string("usage"));
            expect(response["content"])->toBeInstanceOf(Array);
            expect(response["content"]["length"])->toBeGreaterThan(0);
            expect(const_(response["content"])[0])->toHaveProperty(std::string("type"), std::string("text"));
            expect(const_(response["content"])[0])->toHaveProperty(std::string("text"));
            expect(response["usage"])->toHaveProperty(std::string("input_tokens"));
            expect(response["usage"])->toHaveProperty(std::string("output_tokens"));
            console->log(std::string("Response structure verified:"), object{
                object::pair{std::string("id"), response["id"]}, 
                object::pair{std::string("model"), response["model"]}, 
                object::pair{std::string("content_length"), response["content"]["length"]}, 
                object::pair{std::string("text_preview"), const_(response["content"])[0]["text"]["substring"](0, 50)}
            });
        }
        );
    }
    );
}

MAIN
