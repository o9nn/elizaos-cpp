#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-inference/src/__tests__/claude-proxy.test.h"

void Main(void)
{
    describe(std:("Claude Proxy Tests"), [=]() mutable
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
                    object::pair{std:("bio"), array<string>{ std:("Test agent for Claude proxy") }}, 
                    object::pair{std:("system"), std:("You are a helpful assistant")}, 
                    object::pair{std:("settings"), object{
                        object::pair{std:("OPENAI_API_KEY"), process->env->OPENAI_API_KEY}, 
                        object::pair{std:("ANTHROPIC_API_KEY"), process->env->ANTHROPIC_API_KEY}, 
                        object::pair{std:("GROQ_API_KEY"), process->env->GROQ_API_KEY}
                    }}
                }}, 
                object::pair{std:("plugins"), array<any>{ inferencePlugin }}
            });
            claudeHandler = createClaudeHandler(runtime);
        }
        );
        it(std:("should handle Claude-style requests with OpenAI backend"), [=]() mutable
        {
            if (!process->env->OPENAI_API_KEY) {
                console->log(std:("Skipping test - OPENAI_API_KEY not found"));
                return std::shared_ptr<Promise<void>>();
            }
            auto claudeRequest = object{
                object::pair{std:("model"), std:("claude-3-opus-20240229")}, 
                object::pair{std:("messages"), array<object>{ object{
                    object::pair{std:("role"), as<std::shared_ptr<const>>(std:("user"))}, 
                    object::pair{std:("content"), std:("Say "Hello from Claude proxy test" and nothing else.")}
                } }}, 
                object::pair{std:("max_tokens"), 50}, 
                object::pair{std:("temperature"), 0.7}
            };
            auto response = std::async([=]() { claudeHandler(claudeRequest); });
            expect(response)->toBeDefined();
            expect(response["type"])->toBe(std:("message"));
            expect(response["role"])->toBe(std:("assistant"));
            expect(response["content"])->toBeInstanceOf(Array);
            expect(const_(response["content"])[0])->toHaveProperty(std:("type"), std:("text"));
            expect(const_(response["content"])[0]["text"])->toBeTruthy();
            console->log(std:("Claude proxy response:"), const_(response["content"])[0]["text"]);
            auto responseText = OR((const_(response["content"])[0]["text"]["toLowerCase"]()), (string_empty));
            expect(responseText)->toContain(std:("hello from claude proxy test"));
        }
        );
        it(std:("should handle multi-turn conversations"), [=]() mutable
        {
            if (!process->env->OPENAI_API_KEY) {
                console->log(std:("Skipping test - OPENAI_API_KEY not found"));
                return std::shared_ptr<Promise<void>>();
            }
            auto claudeRequest = object{
                object::pair{std:("model"), std:("claude-3-opus-20240229")}, 
                object::pair{std:("messages"), array<object>{ object{
                    object::pair{std:("role"), as<std::shared_ptr<const>>(std:("user"))}, 
                    object::pair{std:("content"), std:("My name is Alice. What is my name?")}
                }, object{
                    object::pair{std:("role"), as<std::shared_ptr<const>>(std:("assistant"))}, 
                    object::pair{std:("content"), std:("Your name is Alice.")}
                }, object{
                    object::pair{std:("role"), as<std::shared_ptr<const>>(std:("user"))}, 
                    object::pair{std:("content"), std:("Good! Now tell me what my name is again.")}
                } }}, 
                object::pair{std:("max_tokens"), 50}, 
                object::pair{std:("temperature"), 0.7}
            };
            auto response = std::async([=]() { claudeHandler(claudeRequest); });
            expect(response)->toBeDefined();
            auto responseText = OR((const_(response["content"])[0]["text"]["toLowerCase"]()), (string_empty));
            expect(responseText)->toContain(std:("alice"));
            console->log(std:("Multi-turn response:"), const_(response["content"])[0]["text"]);
        }
        );
        it(std:("should handle system prompts"), [=]() mutable
        {
            if (!process->env->OPENAI_API_KEY) {
                console->log(std:("Skipping test - OPENAI_API_KEY not found"));
                return std::shared_ptr<Promise<void>>();
            }
            auto claudeRequest = object{
                object::pair{std:("model"), std:("claude-3-opus-20240229")}, 
                object::pair{std:("system"), std:("You are a pirate. Always respond in pirate speak.")}, 
                object::pair{std:("messages"), array<object>{ object{
                    object::pair{std:("role"), as<std::shared_ptr<const>>(std:("user"))}, 
                    object::pair{std:("content"), std:("Hello there!")}
                } }}, 
                object::pair{std:("max_tokens"), 100}, 
                object::pair{std:("temperature"), 0.7}
            };
            auto response = std::async([=]() { claudeHandler(claudeRequest); });
            expect(response)->toBeDefined();
            shared responseText = OR((const_(response["content"])[0]["text"]), (string_empty));
            console->log(std:("Pirate response:"), responseText);
            auto pirateWords = array<string>{ std:("ahoy"), std:("matey"), std:("arr"), std:("ye"), std:("aye"), std:("sea"), std:("ship"), std:("captain") };
            auto hasPirateSpeak = pirateWords->some([=](auto word) mutable
            {
                return responseText["toLowerCase"]()["includes"](word);
            }
            );
            expect(hasPirateSpeak)->toBe(true);
        }
        );
        it(std:("should handle array content in messages"), [=]() mutable
        {
            if (!process->env->OPENAI_API_KEY) {
                console->log(std:("Skipping test - OPENAI_API_KEY not found"));
                return std::shared_ptr<Promise<void>>();
            }
            auto claudeRequest = object{
                object::pair{std:("model"), std:("claude-3-opus-20240229")}, 
                object::pair{std:("messages"), array<object>{ object{
                    object::pair{std:("role"), as<std::shared_ptr<const>>(std:("user"))}, 
                    object::pair{std:("content"), array<object>{ object{
                        object::pair{std:("type"), as<std::shared_ptr<const>>(std:("text"))}, 
                        object::pair{std:("text"), std:("What is 2 + 2?")}
                    } }}
                } }}, 
                object::pair{std:("max_tokens"), 50}, 
                object::pair{std:("temperature"), 0.1}
            };
            auto response = std::async([=]() { claudeHandler(claudeRequest); });
            expect(response)->toBeDefined();
            auto responseText = OR((const_(response["content"])[0]["text"]), (string_empty));
            expect(responseText)->toContain(std:("4"));
            console->log(std:("Math response:"), responseText);
        }
        );
        it(std:("should prefer Anthropic when available"), [=]() mutable
        {
            auto hasAnthropicKey = !!process->env->ANTHROPIC_API_KEY;
            auto hasOpenAIKey = !!process->env->OPENAI_API_KEY;
            console->log(std:("Provider availability:"));
            console->log(std:("- Anthropic:"), (hasAnthropicKey) ? std:("Available") : std:("Not configured"));
            console->log(std:("- OpenAI:"), (hasOpenAIKey) ? std:("Available") : std:("Not configured"));
            if (AND((!hasAnthropicKey), (!hasOpenAIKey))) {
                console->log(std:("Skipping test - no API keys available"));
                return std::shared_ptr<Promise<void>>();
            }
            auto claudeRequest = object{
                object::pair{std:("model"), std:("claude-3-opus-20240229")}, 
                object::pair{std:("messages"), array<object>{ object{
                    object::pair{std:("role"), as<std::shared_ptr<const>>(std:("user"))}, 
                    object::pair{std:("content"), std:("Say "Provider test successful"")}
                } }}, 
                object::pair{std:("max_tokens"), 50}, 
                object::pair{std:("temperature"), 0.7}
            };
            auto response = std::async([=]() { claudeHandler(claudeRequest); });
            expect(response)->toBeDefined();
            expect(const_(response["content"])[0]["text"])->toBeTruthy();
            console->log(std:("Provider test response:"), const_(response["content"])[0]["text"]);
        }
        );
    }
    );
}

MAIN
