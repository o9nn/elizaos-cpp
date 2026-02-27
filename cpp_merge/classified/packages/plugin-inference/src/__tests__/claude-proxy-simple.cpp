#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-inference/src/__tests__/claude-proxy-simple.test.h"

void Main(void)
{
    describe(std:("Claude Proxy Simple Tests"), [=]() mutable
    {
        shared<std::shared_ptr<MockRuntime>> mockRuntime;
        beforeAll([=]() mutable
        {
            auto dotenv = require(std:("dotenv"));
            auto path = require(std:("path"));
            auto envPath = path["join"](process->cwd(), std:(".."), std:(".."), std:(".env"));
            dotenv["config"](object{
                object::pair{std:("path"), envPath}
            });
        }
        );
        it(std:("should detect available providers correctly"), [=]() mutable
        {
            mockRuntime = object{
                object::pair{std:("getSetting"), [=](auto key) mutable
                {
                    if (key == std:("OPENAI_API_KEY")) return process->env->OPENAI_API_KEY;
                    return undefined;
                }
                }, 
                object::pair{std:("useModel"), [=](auto _modelType, auto params) mutable
                {
                    return std:("OpenAI fallback working");
                }
                }
            };
            auto proxy = std::make_shared<ClaudeProxy>(mockRuntime);
            auto selectProvider = (as<any>(proxy))["selectProvider"]["bind"](proxy);
            auto provider = std::async([=]() { selectProvider(); });
            console->log(std:("Selected provider:"), provider);
            expect(provider)->toBe(std:("openai"));
        }
        );
        it(std:("should convert Claude messages to OpenAI format correctly"), [=]() mutable
        {
            mockRuntime = object{
                object::pair{std:("getSetting"), [=]() mutable
                {
                    return undefined;
                }
                }, 
                object::pair{std:("useModel"), [=]() mutable
                {
                    return std:("test response");
                }
                }
            };
            auto proxy = std::make_shared<ClaudeProxy>(mockRuntime);
            auto claudeRequest = object{
                object::pair{std:("model"), std:("claude-3-opus-20240229")}, 
                object::pair{std:("system"), std:("You are a helpful assistant.")}, 
                object::pair{std:("messages"), array<object>{ object{
                    object::pair{std:("role"), as<std::shared_ptr<const>>(std:("user"))}, 
                    object::pair{std:("content"), std:("Hello!")}
                }, object{
                    object::pair{std:("role"), as<std::shared_ptr<const>>(std:("assistant"))}, 
                    object::pair{std:("content"), std:("Hi there! How can I help you?")}
                }, object{
                    object::pair{std:("role"), as<std::shared_ptr<const>>(std:("user"))}, 
                    object::pair{std:("content"), array<object>{ object{
                        object::pair{std:("type"), as<std::shared_ptr<const>>(std:("text"))}, 
                        object::pair{std:("text"), std:("What is 2+2?")}
                    } }}
                } }}, 
                object::pair{std:("max_tokens"), 100}
            };
            auto convertMethod = (as<any>(proxy))["convertClaudeToOpenAI"]["bind"](proxy);
            auto openAIMessages = convertMethod(claudeRequest);
            expect(openAIMessages)->toHaveLength(4);
            expect(const_(openAIMessages)[0]["role"])->toBe(std:("system"));
            expect(const_(openAIMessages)[0]["content"])->toBe(std:("You are a helpful assistant."));
            expect(const_(openAIMessages)[1]["role"])->toBe(std:("user"));
            expect(const_(openAIMessages)[1]["content"])->toBe(std:("Hello!"));
            expect(const_(openAIMessages)[2]["role"])->toBe(std:("assistant"));
            expect(const_(openAIMessages)[2]["content"])->toBe(std:("Hi there! How can I help you?"));
            expect(const_(openAIMessages)[3]["role"])->toBe(std:("user"));
            expect(const_(openAIMessages)[3]["content"])->toBe(std:("What is 2+2?"));
        }
        );
        it(std:("should format Claude response structure correctly"), [=]() mutable
        {
            mockRuntime = object{
                object::pair{std:("getSetting"), [=]() mutable
                {
                    return undefined;
                }
                }, 
                object::pair{std:("useModel"), [=]() mutable
                {
                    return std:("test response");
                }
                }
            };
            auto proxy = std::make_shared<ClaudeProxy>(mockRuntime);
            auto formatMethod = (as<any>(proxy))["formatClaudeResponse"]["bind"](proxy);
            auto claudeResponse = formatMethod(std:("Hello from the proxy!"), std:("claude-3-opus-20240229"));
            expect(claudeResponse)->toHaveProperty(std:("id"));
            expect(claudeResponse["type"])->toBe(std:("message"));
            expect(claudeResponse["role"])->toBe(std:("assistant"));
            expect(claudeResponse["model"])->toBe(std:("claude-3-opus-20240229"));
            expect(claudeResponse["content"])->toHaveLength(1);
            expect(const_(claudeResponse["content"])[0]["type"])->toBe(std:("text"));
            expect(const_(claudeResponse["content"])[0]["text"])->toBe(std:("Hello from the proxy!"));
            expect(claudeResponse["stop_reason"])->toBe(std:("end_turn"));
            expect(claudeResponse["usage"])->toHaveProperty(std:("input_tokens"));
            expect(claudeResponse["usage"])->toHaveProperty(std:("output_tokens"));
        }
        );
        it(std:("should make direct Anthropic proxy request when API key is available"), [=]() mutable
        {
            if (!process->env->ANTHROPIC_API_KEY) {
                console->log(std:("Skipping test - ANTHROPIC_API_KEY not found"));
                return std::shared_ptr<Promise<void>>();
            }
            mockRuntime = object{
                object::pair{std:("getSetting"), [=](auto key) mutable
                {
                    if (key == std:("ANTHROPIC_API_KEY")) return process->env->ANTHROPIC_API_KEY;
                    if (key == std:("ANTHROPIC_API_URL")) return std:("https://api.anthropic.com");
                    return undefined;
                }
                }
            };
            auto proxy = std::make_shared<ClaudeProxy>(mockRuntime);
            auto claudeRequest = object{
                object::pair{std:("model"), std:("claude-3-haiku-20240307")}, 
                object::pair{std:("messages"), array<object>{ object{
                    object::pair{std:("role"), as<std::shared_ptr<const>>(std:("user"))}, 
                    object::pair{std:("content"), std:("Reply with exactly: "Direct proxy test successful"")}
                } }}, 
                object::pair{std:("max_tokens"), 50}, 
                object::pair{std:("temperature"), 0.1}
            };
            auto makeProxyRequest = (as<any>(proxy))["makeAnthropicProxyRequest"]["bind"](proxy);
            auto response = std::async([=]() { makeProxyRequest(claudeRequest); });
            expect(response)->toBeDefined();
            expect(response["type"])->toBe(std:("message"));
            expect(const_(response["content"])[0]["text"])->toContain(std:("Direct proxy test successful"));
            console->log(std:("Direct Anthropic proxy response:"), const_(response["content"])[0]["text"]);
        }
        );
        it(std:("should handle errors gracefully"), [=]() mutable
        {
            mockRuntime = object{
                object::pair{std:("getSetting"), [=]() mutable
                {
                    return undefined;
                }
                }, 
                object::pair{std:("useModel"), [=]() mutable
                {
                    throw any(std::make_shared<Error>(std:("Model not available")));
                }
                }
            };
            auto proxy = std::make_shared<ClaudeProxy>(mockRuntime);
            auto claudeRequest = object{
                object::pair{std:("model"), std:("claude-3-opus-20240229")}, 
                object::pair{std:("messages"), array<object>{ object{
                    object::pair{std:("role"), as<std::shared_ptr<const>>(std:("user"))}, 
                    object::pair{std:("content"), std:("Test error handling")}
                } }}, 
                object::pair{std:("max_tokens"), 50}
            };
            try
            {
                std::async([=]() { proxy->processRequest(claudeRequest); });
                expect(true)->toBe(false);
            }
            catch (const any& error)
            {
                expect(error)->toBeDefined();
                expect(error["message"])->toContain(std:("Model not available"));
            }
        }
        );
    }
    );
}

MAIN
