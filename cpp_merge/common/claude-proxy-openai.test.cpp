#include "claude-proxy-openai.test.h"

void Main(void)
{
    describe(std:("Claude Proxy with OpenAI Fallback"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> runtime;
        shared<any> claudeHandler;
        beforeAll([=]() mutable
        {
            auto dotenv = require(std:("dotenv"));
            auto path = require(std:("path"));
            auto envPath = path["join"](process->cwd(), std:(".."), std:(".."), std:(".env"));
            dotenv["config"](object{
                object::pair{std:("path"), envPath}
            });
            runtime = std::make_shared<AgentRuntime>(object{
                object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("character"), object{
                    object::pair{std:("name"), std:("Test Agent")}, 
                    object::pair{std:("bio"), array<string>{ std:("Test agent for Claude proxy OpenAI fallback") }}, 
                    object::pair{std:("system"), std:("You are a helpful assistant")}, 
                    object::pair{std:("settings"), object{
                        object::pair{std:("OPENAI_API_KEY"), process->env->OPENAI_API_KEY}
                    }}
                }}, 
                object::pair{std:("plugins"), array<any>{ sqlPlugin, openaiPlugin, inferencePlugin }}
            });
            std::async([=]() { runtime->initialize(); });
            claudeHandler = createClaudeHandler(runtime);
        }
        );
        it(std:("should fallback to OpenAI when Anthropic is not available"), [=]() mutable
        {
            if (!process->env->OPENAI_API_KEY) {
                console->log(std:("Skipping test - OPENAI_API_KEY not found"));
                return std::shared_ptr<Promise<void>>();
            }
            console->log(std:("Testing Claude proxy with OpenAI fallback..."));
            console->log(std:("OpenAI API Key present:"), !!process->env->OPENAI_API_KEY);
            console->log(std:("Anthropic API Key in runtime:"), !!runtime->getSetting(std:("ANTHROPIC_API_KEY")));
            auto claudeRequest = object{
                object::pair{std:("model"), std:("claude-3-opus-20240229")}, 
                object::pair{std:("messages"), array<object>{ object{
                    object::pair{std:("role"), as<std::shared_ptr<const>>(std:("user"))}, 
                    object::pair{std:("content"), std:("Reply with exactly: "OpenAI fallback working"")}
                } }}, 
                object::pair{std:("max_tokens"), 50}, 
                object::pair{std:("temperature"), 0.1}
            };
            auto response = std::async([=]() { claudeHandler(claudeRequest); });
            expect(response)->toBeDefined();
            expect(response["type"])->toBe(std:("message"));
            expect(response["role"])->toBe(std:("assistant"));
            expect(response["content"])->toBeInstanceOf(Array);
            expect(const_(response["content"])[0])->toHaveProperty(std:("type"), std:("text"));
            expect(const_(response["content"])[0]["text"])->toBeTruthy();
            console->log(std:("OpenAI fallback response:"), const_(response["content"])[0]["text"]);
            auto responseText = OR((const_(response["content"])[0]["text"]["toLowerCase"]()), (string_empty));
            expect(responseText)->toContain(std:("openai"));
            expect(responseText)->toContain(std:("fallback"));
        }
        );
        it(std:("should handle complex conversations through OpenAI"), [=]() mutable
        {
            if (!process->env->OPENAI_API_KEY) {
                console->log(std:("Skipping test - OPENAI_API_KEY not found"));
                return std::shared_ptr<Promise<void>>();
            }
            auto claudeRequest = object{
                object::pair{std:("model"), std:("claude-3-opus-20240229")}, 
                object::pair{std:("system"), std:("You are a helpful math tutor.")}, 
                object::pair{std:("messages"), array<object>{ object{
                    object::pair{std:("role"), as<std::shared_ptr<const>>(std:("user"))}, 
                    object::pair{std:("content"), std:("What is the square root of 144?")}
                }, object{
                    object::pair{std:("role"), as<std::shared_ptr<const>>(std:("assistant"))}, 
                    object::pair{std:("content"), std:("The square root of 144 is 12.")}
                }, object{
                    object::pair{std:("role"), as<std::shared_ptr<const>>(std:("user"))}, 
                    object::pair{std:("content"), std:("And what is 12 squared?")}
                } }}, 
                object::pair{std:("max_tokens"), 100}, 
                object::pair{std:("temperature"), 0.1}
            };
            auto response = std::async([=]() { claudeHandler(claudeRequest); });
            expect(response)->toBeDefined();
            auto responseText = OR((const_(response["content"])[0]["text"]), (string_empty));
            console->log(std:("Math conversation response:"), responseText);
            expect(responseText)->toContain(std:("144"));
        }
        );
        it(std:("should properly format Claude response structure"), [=]() mutable
        {
            if (!process->env->OPENAI_API_KEY) {
                console->log(std:("Skipping test - OPENAI_API_KEY not found"));
                return std::shared_ptr<Promise<void>>();
            }
            auto claudeRequest = object{
                object::pair{std:("model"), std:("claude-3-sonnet-20240229")}, 
                object::pair{std:("messages"), array<object>{ object{
                    object::pair{std:("role"), as<std::shared_ptr<const>>(std:("user"))}, 
                    object::pair{std:("content"), std:("Say hello")}
                } }}, 
                object::pair{std:("max_tokens"), 20}, 
                object::pair{std:("temperature"), 0.5}
            };
            auto response = std::async([=]() { claudeHandler(claudeRequest); });
            expect(response)->toHaveProperty(std:("id"));
            expect(response)->toHaveProperty(std:("type"), std:("message"));
            expect(response)->toHaveProperty(std:("role"), std:("assistant"));
            expect(response)->toHaveProperty(std:("model"));
            expect(response)->toHaveProperty(std:("content"));
            expect(response)->toHaveProperty(std:("stop_reason"));
            expect(response)->toHaveProperty(std:("usage"));
            expect(response["content"])->toBeInstanceOf(Array);
            expect(response["content"]["length"])->toBeGreaterThan(0);
            expect(const_(response["content"])[0])->toHaveProperty(std:("type"), std:("text"));
            expect(const_(response["content"])[0])->toHaveProperty(std:("text"));
            expect(response["usage"])->toHaveProperty(std:("input_tokens"));
            expect(response["usage"])->toHaveProperty(std:("output_tokens"));
            console->log(std:("Response structure verified:"), object{
                object::pair{std:("id"), response["id"]}, 
                object::pair{std:("model"), response["model"]}, 
                object::pair{std:("content_length"), response["content"]["length"]}, 
                object::pair{std:("text_preview"), const_(response["content"])[0]["text"]["substring"](0, 50)}
            });
        }
        );
    }
    );
}

MAIN
