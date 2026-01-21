#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-inference/src/__tests__/claude-proxy-simple.test.h"

void Main(void)
{
    describe(std::string("Claude Proxy Simple Tests"), [=]() mutable
    {
        shared<std::shared_ptr<MockRuntime>> mockRuntime;
        beforeAll([=]() mutable
        {
            auto dotenv = require(std::string("dotenv"));
            auto path = require(std::string("path"));
            auto envPath = path["join"](process->cwd(), std::string(".."), std::string(".."), std::string(".env"));
            dotenv["config"](object{
                object::pair{std::string("path"), envPath}
            });
        }
        );
        it(std::string("should detect available providers correctly"), [=]() mutable
        {
            mockRuntime = object{
                object::pair{std::string("getSetting"), [=](auto key) mutable
                {
                    if (key == std::string("OPENAI_API_KEY")) return process->env->OPENAI_API_KEY;
                    return undefined;
                }
                }, 
                object::pair{std::string("useModel"), [=](auto _modelType, auto params) mutable
                {
                    return std::string("OpenAI fallback working");
                }
                }
            };
            auto proxy = std::make_shared<ClaudeProxy>(mockRuntime);
            auto selectProvider = (as<any>(proxy))["selectProvider"]["bind"](proxy);
            auto provider = std::async([=]() { selectProvider(); });
            console->log(std::string("Selected provider:"), provider);
            expect(provider)->toBe(std::string("openai"));
        }
        );
        it(std::string("should convert Claude messages to OpenAI format correctly"), [=]() mutable
        {
            mockRuntime = object{
                object::pair{std::string("getSetting"), [=]() mutable
                {
                    return undefined;
                }
                }, 
                object::pair{std::string("useModel"), [=]() mutable
                {
                    return std::string("test response");
                }
                }
            };
            auto proxy = std::make_shared<ClaudeProxy>(mockRuntime);
            auto claudeRequest = object{
                object::pair{std::string("model"), std::string("claude-3-opus-20240229")}, 
                object::pair{std::string("system"), std::string("You are a helpful assistant.")}, 
                object::pair{std::string("messages"), array<object>{ object{
                    object::pair{std::string("role"), as<std::shared_ptr<const>>(std::string("user"))}, 
                    object::pair{std::string("content"), std::string("Hello!")}
                }, object{
                    object::pair{std::string("role"), as<std::shared_ptr<const>>(std::string("assistant"))}, 
                    object::pair{std::string("content"), std::string("Hi there! How can I help you?")}
                }, object{
                    object::pair{std::string("role"), as<std::shared_ptr<const>>(std::string("user"))}, 
                    object::pair{std::string("content"), array<object>{ object{
                        object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("text"))}, 
                        object::pair{std::string("text"), std::string("What is 2+2?")}
                    } }}
                } }}, 
                object::pair{std::string("max_tokens"), 100}
            };
            auto convertMethod = (as<any>(proxy))["convertClaudeToOpenAI"]["bind"](proxy);
            auto openAIMessages = convertMethod(claudeRequest);
            expect(openAIMessages)->toHaveLength(4);
            expect(const_(openAIMessages)[0]["role"])->toBe(std::string("system"));
            expect(const_(openAIMessages)[0]["content"])->toBe(std::string("You are a helpful assistant."));
            expect(const_(openAIMessages)[1]["role"])->toBe(std::string("user"));
            expect(const_(openAIMessages)[1]["content"])->toBe(std::string("Hello!"));
            expect(const_(openAIMessages)[2]["role"])->toBe(std::string("assistant"));
            expect(const_(openAIMessages)[2]["content"])->toBe(std::string("Hi there! How can I help you?"));
            expect(const_(openAIMessages)[3]["role"])->toBe(std::string("user"));
            expect(const_(openAIMessages)[3]["content"])->toBe(std::string("What is 2+2?"));
        }
        );
        it(std::string("should format Claude response structure correctly"), [=]() mutable
        {
            mockRuntime = object{
                object::pair{std::string("getSetting"), [=]() mutable
                {
                    return undefined;
                }
                }, 
                object::pair{std::string("useModel"), [=]() mutable
                {
                    return std::string("test response");
                }
                }
            };
            auto proxy = std::make_shared<ClaudeProxy>(mockRuntime);
            auto formatMethod = (as<any>(proxy))["formatClaudeResponse"]["bind"](proxy);
            auto claudeResponse = formatMethod(std::string("Hello from the proxy!"), std::string("claude-3-opus-20240229"));
            expect(claudeResponse)->toHaveProperty(std::string("id"));
            expect(claudeResponse["type"])->toBe(std::string("message"));
            expect(claudeResponse["role"])->toBe(std::string("assistant"));
            expect(claudeResponse["model"])->toBe(std::string("claude-3-opus-20240229"));
            expect(claudeResponse["content"])->toHaveLength(1);
            expect(const_(claudeResponse["content"])[0]["type"])->toBe(std::string("text"));
            expect(const_(claudeResponse["content"])[0]["text"])->toBe(std::string("Hello from the proxy!"));
            expect(claudeResponse["stop_reason"])->toBe(std::string("end_turn"));
            expect(claudeResponse["usage"])->toHaveProperty(std::string("input_tokens"));
            expect(claudeResponse["usage"])->toHaveProperty(std::string("output_tokens"));
        }
        );
        it(std::string("should make direct Anthropic proxy request when API key is available"), [=]() mutable
        {
            if (!process->env->ANTHROPIC_API_KEY) {
                console->log(std::string("Skipping test - ANTHROPIC_API_KEY not found"));
                return std::shared_ptr<Promise<void>>();
            }
            mockRuntime = object{
                object::pair{std::string("getSetting"), [=](auto key) mutable
                {
                    if (key == std::string("ANTHROPIC_API_KEY")) return process->env->ANTHROPIC_API_KEY;
                    if (key == std::string("ANTHROPIC_API_URL")) return std::string("https://api.anthropic.com");
                    return undefined;
                }
                }
            };
            auto proxy = std::make_shared<ClaudeProxy>(mockRuntime);
            auto claudeRequest = object{
                object::pair{std::string("model"), std::string("claude-3-haiku-20240307")}, 
                object::pair{std::string("messages"), array<object>{ object{
                    object::pair{std::string("role"), as<std::shared_ptr<const>>(std::string("user"))}, 
                    object::pair{std::string("content"), std::string("Reply with exactly: "Direct proxy test successful"")}
                } }}, 
                object::pair{std::string("max_tokens"), 50}, 
                object::pair{std::string("temperature"), 0.1}
            };
            auto makeProxyRequest = (as<any>(proxy))["makeAnthropicProxyRequest"]["bind"](proxy);
            auto response = std::async([=]() { makeProxyRequest(claudeRequest); });
            expect(response)->toBeDefined();
            expect(response["type"])->toBe(std::string("message"));
            expect(const_(response["content"])[0]["text"])->toContain(std::string("Direct proxy test successful"));
            console->log(std::string("Direct Anthropic proxy response:"), const_(response["content"])[0]["text"]);
        }
        );
        it(std::string("should handle errors gracefully"), [=]() mutable
        {
            mockRuntime = object{
                object::pair{std::string("getSetting"), [=]() mutable
                {
                    return undefined;
                }
                }, 
                object::pair{std::string("useModel"), [=]() mutable
                {
                    throw any(std::make_shared<Error>(std::string("Model not available")));
                }
                }
            };
            auto proxy = std::make_shared<ClaudeProxy>(mockRuntime);
            auto claudeRequest = object{
                object::pair{std::string("model"), std::string("claude-3-opus-20240229")}, 
                object::pair{std::string("messages"), array<object>{ object{
                    object::pair{std::string("role"), as<std::shared_ptr<const>>(std::string("user"))}, 
                    object::pair{std::string("content"), std::string("Test error handling")}
                } }}, 
                object::pair{std::string("max_tokens"), 50}
            };
            try
            {
                std::async([=]() { proxy->processRequest(claudeRequest); });
                expect(true)->toBe(false);
            }
            catch (const any& error)
            {
                expect(error)->toBeDefined();
                expect(error["message"])->toContain(std::string("Model not available"));
            }
        }
        );
    }
    );
}

MAIN
