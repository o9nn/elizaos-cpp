#include "/home/runner/work/elizaos-cpp/elizaos-cpp/mcp-gateway/example-agents/ai16z-price-agent-sse.h"

std::shared_ptr<Promise<void>> main()
{
    any mcpClient;
    {
        utils::finally __finally7622_7881([&]() mutable
        {
            if (mcpClient) {
                try
                {
                    std::async([=]() { mcpClient["close"](); });
                    console->log(std::string("[Agent] MCP client closed"));
                }
                catch (const any& closeError)
                {
                    console->error(std::string("[Agent] Error closing MCP client:"), closeError);
                }
            }
        });
        try
        {
            auto walletKey = as<std::shared_ptr<Hex>>((OR((process->env->WALLET_PRIVATE_KEY), (DEFAULT_WALLET_KEY))));
            auto account = privateKeyToAccount(walletKey);
            console->log(std::string("[Agent] Payment wallet configured:"));
            console->log(std::string("[Agent] Address: ") + account->address + string_empty);
            console->log(std::string("[Agent] This wallet will pay the gateway $0.01 per tool call via x402\
"));
            console->log(std::string("[Agent] Connecting to MCP Gateway via SSE..."));
            console->log(std::string("[Agent] SSE URL: ") + GATEWAY_SSE_URL + string_empty);
            console->log(std::string("[Agent] Message URL: ") + GATEWAY_MESSAGE_URL + std::string("\
"));
            shared baseFetch = wrapFetchWithPayment(fetch, account);
            auto mcpCompatibleFetch = [=](auto input, auto init) mutable
            {
                try
                {
                    auto response = std::async([=]() { baseFetch(input, init); });
                    return response;
                }
                catch (const any& error)
                {
                    console->error(std::string("[Agent] Payment error:"), error);
                    throw any(error);
                }
            };
            mcpClient = std::make_shared<Client>(object{
                object::pair{std::string("name"), std::string("ai16z-price-agent-sse")}, 
                object::pair{std::string("version"), std::string("1.0.0")}
            }, object{
                object::pair{std::string("capabilities"), object{}}
            });
            auto transport = std::make_shared<SSEClientTransport>(std::make_shared<URL>(GATEWAY_SSE_URL), object{
                object::pair{std::string("fetch"), mcpCompatibleFetch}
            });
            std::async([=]() { mcpClient["connect"](transport); });
            console->log(std::string("[Agent] Connected to MCP Gateway via SSE"));
            auto toolsResult = std::async([=]() { mcpClient["listTools"](); });
            auto availableTools = toolsResult["tools"];
            console->log(std::string("[Agent] Available MCP tools: ") + availableTools["length"] + std::string(" tools discovered"));
            auto anthropic = std::make_shared<Anthropic>(object{
                object::pair{std::string("apiKey"), OR((process->env->ANTHROPIC_API_KEY), (std::string("required")))}, 
                object::pair{std::string("fetch"), wrapFetchWithPayment(fetch, account)}
            });
            auto tools = availableTools["map"]([=](auto tool) mutable
            {
                return (object{
                    object::pair{std::string("name"), tool["name"]->replace((new RegExp(std::string(":"))), std::string("_"))}, 
                    object::pair{std::string("description"), OR((tool["description"]), (string_empty))}, 
                    object::pair{std::string("input_schema"), utils::assign(object{
                        object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("object"))}
                    }, tool["inputSchema"])}
                });
            }
            );
            auto toolNameMap = std::make_shared<Map<string, string>>();
            for (auto& tool : availableTools)
            {
                toolNameMap->set(tool["name"]["replace"]((new RegExp(std::string(":"))), std::string("_")), tool["name"]);
            }
            console->log(std::string("[Agent] Tools ready for AI agent\
"));
            console->log(std::string("[Agent] Starting AI16Z price query...\
"));
            auto messages = array<std::shared_ptr<Anthropic::MessageParam>>{ object{
                object::pair{std::string("role"), std::string("user")}, 
                object::pair{std::string("content"), std::string("What is the current price of AI16Z token in USD?")}
            } };
            auto continueLoop = true;
            auto iterations = 0;
            auto maxIterations = 10;
            while (AND((continueLoop), (iterations < maxIterations)))
            {
                iterations++;
                auto response = std::async([=]() { anthropic->messages->create(object{
                    object::pair{std::string("model"), std::string("claude-3-7-sonnet-20250219")}, 
                    object::pair{std::string("max_tokens"), 4096}, 
                    object::pair{std::string("system"), std::string("You are a cryptocurrency price assistant with access to CoinGecko data.\
\
Your task is to fetch the current price of the AI16Z token (also known as ai16z).\
\
Available tools provide access to cryptocurrency information through CoinGecko.\
\
Steps to follow:\
1. Search for the AI16Z token using available tools\
2. Get its current price in USD\
3. Provide a clear, concise response with the price\
\
Be specific and accurate. The token might be listed as "ai16z", "AI16Z", or similar variations.")}, 
                    object::pair{std::string("messages"), std::string("messages")}, 
                    object::pair{std::string("tools"), std::string("tools")}
                }); });
                if (response->stop_reason == std::string("end_turn")) {
                    auto textContent = response->content->find([=](auto c) mutable
                    {
                        return c["type"] == std::string("text");
                    }
                    );
                    if (AND((textContent), (in(std::string("text"), textContent)))) {
                        console->log(std::string("\
[Agent] Result:\
"));
                        console->log(textContent->text);
                    }
                    continueLoop = false;
                    break;
                }
                if (response->stop_reason == std::string("tool_use")) {
                    auto toolResults = array<std::shared_ptr<Anthropic::ToolResultBlockParam>>();
                    for (auto& block : response->content)
                    {
                        if (block->type == std::string("tool_use")) {
                            console->log(std::string("[Agent] Calling tool: ") + block->name + string_empty);
                            try
                            {
                                auto originalToolName = OR((toolNameMap->get(block->name)), (block->name));
                                auto result = std::async([=]() { mcpClient["callTool"](object{
                                    object::pair{std::string("name"), originalToolName}, 
                                    object::pair{std::string("arguments"), as<Record<string, any>>(block->input)}
                                }); });
                                toolResults->push(object{
                                    object::pair{std::string("type"), std::string("tool_result")}, 
                                    object::pair{std::string("tool_use_id"), block->id}, 
                                    object::pair{std::string("content"), JSON->stringify(result["content"])}
                                });
                            }
                            catch (const any& toolError)
                            {
                                console->error(std::string("[Agent] Tool error:"), toolError);
                                toolResults->push(object{
                                    object::pair{std::string("type"), std::string("tool_result")}, 
                                    object::pair{std::string("tool_use_id"), block->id}, 
                                    object::pair{std::string("content"), std::string("Error: ") + (is<Error>(toolError)) ? toolError->message : String(toolError) + string_empty}
                                });
                            }
                        }
                    }
                    messages->push(object{
                        object::pair{std::string("role"), std::string("assistant")}, 
                        object::pair{std::string("content"), response->content}
                    });
                    messages->push(object{
                        object::pair{std::string("role"), std::string("user")}, 
                        object::pair{std::string("content"), toolResults}
                    });
                } else {
                    continueLoop = false;
                }
            }
            console->log(std::string("\
[Agent] Done!"));
        }
        catch (const any& error)
        {
            console->error(std::string("[Agent] Error:"), error);
            process->exit(1);
        }
    }
    return std::shared_ptr<Promise<void>>();
};


string DEFAULT_WALLET_KEY = std::string("default-key");
string GATEWAY_SSE_URL = std::string("http://localhost:8000/sse");
string GATEWAY_MESSAGE_URL = std::string("http://localhost:8000/message");

void Main(void)
{
    process->on(std::string("SIGINT"), [=]() mutable
    {
        console->log(std::string("\
[Agent] Received SIGINT, shutting down..."));
        process->exit(0);
    }
    );
    process->on(std::string("SIGTERM"), [=]() mutable
    {
        console->log(std::string("\
[Agent] Received SIGTERM, shutting down..."));
        process->exit(0);
    }
    );
    main()->_catch([=](auto error) mutable
    {
        console->error(std::string("[Agent] Fatal error:"), error);
        process->exit(1);
    }
    );
}

MAIN
