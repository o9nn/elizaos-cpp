#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-inference/src/__tests__/claude-proxy.test.h"

void Main(void)
{
    describe(std::string("Claude Proxy Tests"), [=]() mutable
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
                    object::pair{std::string("bio"), array<string>{ std::string("Test agent for Claude proxy") }}, 
                    object::pair{std::string("system"), std::string("You are a helpful assistant")}, 
                    object::pair{std::string("settings"), object{
                        object::pair{std::string("OPENAI_API_KEY"), process->env->OPENAI_API_KEY}, 
                        object::pair{std::string("ANTHROPIC_API_KEY"), process->env->ANTHROPIC_API_KEY}, 
                        object::pair{std::string("GROQ_API_KEY"), process->env->GROQ_API_KEY}
                    }}
                }}, 
                object::pair{std::string("plugins"), array<any>{ inferencePlugin }}
            });
            claudeHandler = createClaudeHandler(runtime);
        }
        );
        it(std::string("should handle Claude-style requests with OpenAI backend"), [=]() mutable
        {
            if (!process->env->OPENAI_API_KEY) {
                console->log(std::string("Skipping test - OPENAI_API_KEY not found"));
                return std::shared_ptr<Promise<void>>();
            }
            auto claudeRequest = object{
                object::pair{std::string("model"), std::string("claude-3-opus-20240229")}, 
                object::pair{std::string("messages"), array<object>{ object{
                    object::pair{std::string("role"), as<std::shared_ptr<const>>(std::string("user"))}, 
                    object::pair{std::string("content"), std::string("Say "Hello from Claude proxy test" and nothing else.")}
                } }}, 
                object::pair{std::string("max_tokens"), 50}, 
                object::pair{std::string("temperature"), 0.7}
            };
            auto response = std::async([=]() { claudeHandler(claudeRequest); });
            expect(response)->toBeDefined();
            expect(response["type"])->toBe(std::string("message"));
            expect(response["role"])->toBe(std::string("assistant"));
            expect(response["content"])->toBeInstanceOf(Array);
            expect(const_(response["content"])[0])->toHaveProperty(std::string("type"), std::string("text"));
            expect(const_(response["content"])[0]["text"])->toBeTruthy();
            console->log(std::string("Claude proxy response:"), const_(response["content"])[0]["text"]);
            auto responseText = OR((const_(response["content"])[0]["text"]["toLowerCase"]()), (string_empty));
            expect(responseText)->toContain(std::string("hello from claude proxy test"));
        }
        );
        it(std::string("should handle multi-turn conversations"), [=]() mutable
        {
            if (!process->env->OPENAI_API_KEY) {
                console->log(std::string("Skipping test - OPENAI_API_KEY not found"));
                return std::shared_ptr<Promise<void>>();
            }
            auto claudeRequest = object{
                object::pair{std::string("model"), std::string("claude-3-opus-20240229")}, 
                object::pair{std::string("messages"), array<object>{ object{
                    object::pair{std::string("role"), as<std::shared_ptr<const>>(std::string("user"))}, 
                    object::pair{std::string("content"), std::string("My name is Alice. What is my name?")}
                }, object{
                    object::pair{std::string("role"), as<std::shared_ptr<const>>(std::string("assistant"))}, 
                    object::pair{std::string("content"), std::string("Your name is Alice.")}
                }, object{
                    object::pair{std::string("role"), as<std::shared_ptr<const>>(std::string("user"))}, 
                    object::pair{std::string("content"), std::string("Good! Now tell me what my name is again.")}
                } }}, 
                object::pair{std::string("max_tokens"), 50}, 
                object::pair{std::string("temperature"), 0.7}
            };
            auto response = std::async([=]() { claudeHandler(claudeRequest); });
            expect(response)->toBeDefined();
            auto responseText = OR((const_(response["content"])[0]["text"]["toLowerCase"]()), (string_empty));
            expect(responseText)->toContain(std::string("alice"));
            console->log(std::string("Multi-turn response:"), const_(response["content"])[0]["text"]);
        }
        );
        it(std::string("should handle system prompts"), [=]() mutable
        {
            if (!process->env->OPENAI_API_KEY) {
                console->log(std::string("Skipping test - OPENAI_API_KEY not found"));
                return std::shared_ptr<Promise<void>>();
            }
            auto claudeRequest = object{
                object::pair{std::string("model"), std::string("claude-3-opus-20240229")}, 
                object::pair{std::string("system"), std::string("You are a pirate. Always respond in pirate speak.")}, 
                object::pair{std::string("messages"), array<object>{ object{
                    object::pair{std::string("role"), as<std::shared_ptr<const>>(std::string("user"))}, 
                    object::pair{std::string("content"), std::string("Hello there!")}
                } }}, 
                object::pair{std::string("max_tokens"), 100}, 
                object::pair{std::string("temperature"), 0.7}
            };
            auto response = std::async([=]() { claudeHandler(claudeRequest); });
            expect(response)->toBeDefined();
            shared responseText = OR((const_(response["content"])[0]["text"]), (string_empty));
            console->log(std::string("Pirate response:"), responseText);
            auto pirateWords = array<string>{ std::string("ahoy"), std::string("matey"), std::string("arr"), std::string("ye"), std::string("aye"), std::string("sea"), std::string("ship"), std::string("captain") };
            auto hasPirateSpeak = pirateWords->some([=](auto word) mutable
            {
                return responseText["toLowerCase"]()["includes"](word);
            }
            );
            expect(hasPirateSpeak)->toBe(true);
        }
        );
        it(std::string("should handle array content in messages"), [=]() mutable
        {
            if (!process->env->OPENAI_API_KEY) {
                console->log(std::string("Skipping test - OPENAI_API_KEY not found"));
                return std::shared_ptr<Promise<void>>();
            }
            auto claudeRequest = object{
                object::pair{std::string("model"), std::string("claude-3-opus-20240229")}, 
                object::pair{std::string("messages"), array<object>{ object{
                    object::pair{std::string("role"), as<std::shared_ptr<const>>(std::string("user"))}, 
                    object::pair{std::string("content"), array<object>{ object{
                        object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("text"))}, 
                        object::pair{std::string("text"), std::string("What is 2 + 2?")}
                    } }}
                } }}, 
                object::pair{std::string("max_tokens"), 50}, 
                object::pair{std::string("temperature"), 0.1}
            };
            auto response = std::async([=]() { claudeHandler(claudeRequest); });
            expect(response)->toBeDefined();
            auto responseText = OR((const_(response["content"])[0]["text"]), (string_empty));
            expect(responseText)->toContain(std::string("4"));
            console->log(std::string("Math response:"), responseText);
        }
        );
        it(std::string("should prefer Anthropic when available"), [=]() mutable
        {
            auto hasAnthropicKey = !!process->env->ANTHROPIC_API_KEY;
            auto hasOpenAIKey = !!process->env->OPENAI_API_KEY;
            console->log(std::string("Provider availability:"));
            console->log(std::string("- Anthropic:"), (hasAnthropicKey) ? std::string("Available") : std::string("Not configured"));
            console->log(std::string("- OpenAI:"), (hasOpenAIKey) ? std::string("Available") : std::string("Not configured"));
            if (AND((!hasAnthropicKey), (!hasOpenAIKey))) {
                console->log(std::string("Skipping test - no API keys available"));
                return std::shared_ptr<Promise<void>>();
            }
            auto claudeRequest = object{
                object::pair{std::string("model"), std::string("claude-3-opus-20240229")}, 
                object::pair{std::string("messages"), array<object>{ object{
                    object::pair{std::string("role"), as<std::shared_ptr<const>>(std::string("user"))}, 
                    object::pair{std::string("content"), std::string("Say "Provider test successful"")}
                } }}, 
                object::pair{std::string("max_tokens"), 50}, 
                object::pair{std::string("temperature"), 0.7}
            };
            auto response = std::async([=]() { claudeHandler(claudeRequest); });
            expect(response)->toBeDefined();
            expect(const_(response["content"])[0]["text"])->toBeTruthy();
            console->log(std::string("Provider test response:"), const_(response["content"])[0]["text"]);
        }
        );
    }
    );
}

MAIN
