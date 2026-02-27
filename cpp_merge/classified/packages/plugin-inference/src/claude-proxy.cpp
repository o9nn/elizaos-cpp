#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-inference/src/claude-proxy.h"

ClaudeProxy::ClaudeProxy(std::shared_ptr<IAgentRuntime> runtime_) : runtime(runtime_)  {
}

std::shared_ptr<Promise<std::shared_ptr<ClaudeResponse>>> ClaudeProxy::processRequest(std::shared_ptr<ClaudeMessagesRequest> claudeRequest)
{
    auto provider = std::async([=]() { this->selectProvider(); });
    logger->info(std:("[CLAUDE_PROXY] Routing request to provider: ") + provider + string_empty);
    switch (provider)
    {
    case InferenceProvider::ANTHROPIC:
        return this->makeAnthropicProxyRequest(claudeRequest);
    case InferenceProvider::OPENAI:
        return this->handleOpenAIRequest(claudeRequest);
    case InferenceProvider::GROQ:
        return this->handleGroqRequest(claudeRequest);
    case InferenceProvider::OLLAMA:
        return this->handleOllamaRequest(claudeRequest);
    default:
        throw any(std::make_shared<Error>(std:("Unsupported provider: ") + provider + string_empty));
    }
}

std::shared_ptr<Promise<std::shared_ptr<ClaudeResponse>>> ClaudeProxy::makeAnthropicProxyRequest(std::shared_ptr<ClaudeMessagesRequest> claudeRequest)
{
    auto apiKey = this->runtime->getSetting(std:("ANTHROPIC_API_KEY"));
    if (!apiKey) {
        throw any(std::make_shared<Error>(std:("ANTHROPIC_API_KEY not configured")));
    }
    auto baseUrl = OR((this->runtime->getSetting(std:("ANTHROPIC_API_URL"))), (std:("https://api.anthropic.com")));
    try
    {
        auto response = std::async([=]() { fetch(string_empty + baseUrl + std:("/v1/messages"), object{
            object::pair{std:("method"), std:("POST")}, 
            object::pair{std:("headers"), object{
                object::pair{std:("Content-Type"), std:("application/json")}, 
                object::pair{std:("x-api-key"), apiKey}, 
                object::pair{std:("anthropic-version"), std:("2023-06-01")}
            }}, 
            object::pair{std:("body"), JSON->stringify(claudeRequest)}
        }); });
        if (!response->ok) {
            auto errorBody = std::async([=]() { response->text(); });
            throw any(std::make_shared<Error>(std:("Anthropic API error: ") + response->status + std:(" - ") + errorBody + string_empty));
        }
        auto data = as<std::shared_ptr<ClaudeResponse>>((std::async([=]() { response->json(); })));
        return data;
    }
    catch (const any& error)
    {
        logger->error(std:("[CLAUDE_PROXY] Error making Anthropic proxy request:"), error);
        throw any(error);
    }
}

std::shared_ptr<Promise<InferenceProvider>> ClaudeProxy::selectProvider()
{
    auto preferences = array<InferenceProvider>{ InferenceProvider::ANTHROPIC, InferenceProvider::OPENAI, InferenceProvider::GROQ, InferenceProvider::OLLAMA };
    for (auto& provider : preferences)
    {
        if (std::async([=]() { this->isProviderAvailable(provider); })) {
            return provider;
        }
    }
    throw any(std::make_shared<Error>(std:("No inference providers available")));
}

std::shared_ptr<Promise<boolean>> ClaudeProxy::isProviderAvailable(InferenceProvider provider)
{
    switch (provider)
    {
    case InferenceProvider::ANTHROPIC:
        return !!this->runtime->getSetting(std:("ANTHROPIC_API_KEY"));
    case InferenceProvider::OPENAI:
        return !!this->runtime->getSetting(std:("OPENAI_API_KEY"));
    case InferenceProvider::GROQ:
        return !!this->runtime->getSetting(std:("GROQ_API_KEY"));
    case InferenceProvider::OLLAMA:
        return true;
    default:
        return false;
    }
}

std::shared_ptr<Promise<std::shared_ptr<ClaudeResponse>>> ClaudeProxy::handleOpenAIRequest(std::shared_ptr<ClaudeMessagesRequest> claudeRequest)
{
    auto openAIMessages = this->convertClaudeToOpenAI(claudeRequest);
    auto prompt = this->formatOpenAIPrompt(openAIMessages, claudeRequest->system);
    auto response = std::async([=]() { this->runtime->useModel(ModelType->TEXT_LARGE, object{
        object::pair{std:("runtime"), this->runtime}, 
        object::pair{std:("prompt"), std:("prompt")}, 
        object::pair{std:("modelType"), ModelType->TEXT_LARGE}, 
        object::pair{std:("maxTokens"), claudeRequest->max_tokens}, 
        object::pair{std:("temperature"), claudeRequest->temperature}, 
        object::pair{std:("stopSequences"), claudeRequest->stop_sequences}
    }); });
    return this->formatClaudeResponse(response, claudeRequest->model);
}

std::shared_ptr<Promise<std::shared_ptr<ClaudeResponse>>> ClaudeProxy::handleGroqRequest(std::shared_ptr<ClaudeMessagesRequest> claudeRequest)
{
    return this->handleOpenAIRequest(claudeRequest);
}

std::shared_ptr<Promise<std::shared_ptr<ClaudeResponse>>> ClaudeProxy::handleOllamaRequest(std::shared_ptr<ClaudeMessagesRequest> claudeRequest)
{
    auto prompt = this->convertClaudeToPrompt(claudeRequest);
    auto response = std::async([=]() { this->runtime->useModel(ModelType->TEXT_LARGE, object{
        object::pair{std:("runtime"), this->runtime}, 
        object::pair{std:("prompt"), std:("prompt")}, 
        object::pair{std:("modelType"), ModelType->TEXT_LARGE}, 
        object::pair{std:("maxTokens"), claudeRequest->max_tokens}, 
        object::pair{std:("temperature"), claudeRequest->temperature}, 
        object::pair{std:("stopSequences"), claudeRequest->stop_sequences}
    }); });
    return this->formatClaudeResponse(response, claudeRequest->model);
}

array<std::shared_ptr<OpenAIMessage>> ClaudeProxy::convertClaudeToOpenAI(std::shared_ptr<ClaudeMessagesRequest> claudeRequest)
{
    auto openAIMessages = array<std::shared_ptr<OpenAIMessage>>();
    if (claudeRequest->system) {
        openAIMessages->push(object{
            object::pair{std:("role"), std:("system")}, 
            object::pair{std:("content"), claudeRequest->system}
        });
    }
    for (auto& message : claudeRequest->messages)
    {
        if (message->role == std:("user")) {
            if (Array->isArray(message->content)) {
                auto content = message->content->filter([=](auto c) mutable
                {
                    return c["type"] == std:("text");
                }
                )->map([=](auto c) mutable
                {
                    return c["text"];
                }
                )->join(std:("\
"));
                openAIMessages->push(object{
                    object::pair{std:("role"), std:("user")}, 
                    object::pair{std:("content"), std:("content")}
                });
            } else {
                openAIMessages->push(object{
                    object::pair{std:("role"), std:("user")}, 
                    object::pair{std:("content"), message->content}
                });
            }
        } else if (message->role == std:("assistant")) {
            if (Array->isArray(message->content)) {
                auto textContent = message->content->filter([=](auto c) mutable
                {
                    return c["type"] == std:("text");
                }
                )->map([=](auto c) mutable
                {
                    return c["text"];
                }
                )->join(std:("\
"));
                openAIMessages->push(object{
                    object::pair{std:("role"), std:("assistant")}, 
                    object::pair{std:("content"), OR((textContent), (string_empty))}
                });
            } else {
                openAIMessages->push(object{
                    object::pair{std:("role"), std:("assistant")}, 
                    object::pair{std:("content"), message->content}
                });
            }
        }
    }
    return openAIMessages;
}

string ClaudeProxy::convertClaudeToPrompt(std::shared_ptr<ClaudeMessagesRequest> claudeRequest)
{
    auto prompt = string_empty;
    if (claudeRequest->system) {
        prompt += std:("System: ") + claudeRequest->system + std:("\
\
");
    }
    for (auto& message : claudeRequest->messages)
    {
        auto role = (message->role == std:("user")) ? std:("User") : std:("Assistant");
        if (Array->isArray(message->content)) {
            auto textContent = message->content->filter([=](auto c) mutable
            {
                return c["type"] == std:("text");
            }
            )->map([=](auto c) mutable
            {
                return c["text"];
            }
            )->join(std:("\
"));
            prompt += string_empty + role + std:(": ") + textContent + std:("\
\
");
        } else {
            prompt += string_empty + role + std:(": ") + message->content + std:("\
\
");
        }
    }
    return prompt->trim();
}

string ClaudeProxy::formatOpenAIPrompt(array<std::shared_ptr<OpenAIMessage>> messages, string system)
{
    auto prompt = string_empty;
    if (system) {
        prompt += string_empty + system + std:("\
\
");
    }
    for (auto& message : messages)
    {
        if (message->role == std:("system")) {
            continue;
        }
        auto role = (message->role == std:("user")) ? std:("User") : std:("Assistant");
        auto content = (type_of(message->content) == std:("string")) ? message->content : message->content->map([=](auto c) mutable
        {
            return c["text"];
        }
        )->join(std:("\
"));
        prompt += string_empty + role + std:(": ") + content + std:("\
\
");
    }
    return prompt->trim();
}

std::shared_ptr<ClaudeResponse> ClaudeProxy::formatClaudeResponse(string text, string model)
{
    return object{
        object::pair{std:("id"), std:("msg_") + Math->random()->toString(36)->substr(2, 9) + string_empty}, 
        object::pair{std:("type"), std:("message")}, 
        object::pair{std:("role"), std:("assistant")}, 
        object::pair{std:("model"), std:("model")}, 
        object::pair{std:("content"), array<object>{ object{
            object::pair{std:("type"), std:("text")}, 
            object::pair{std:("text"), std:("text")}
        } }}, 
        object::pair{std:("stop_reason"), std:("end_turn")}, 
        object::pair{std:("usage"), object{
            object::pair{std:("input_tokens"), 0}, 
            object::pair{std:("output_tokens"), 0}
        }}
    };
}

any createClaudeHandler(std::shared_ptr<IAgentRuntime> runtime)
{
    shared proxy = std::make_shared<ClaudeProxy>(runtime);
    return [=](auto request) mutable
    {
        try
        {
            return std::async([=]() { proxy->processRequest(request); });
        }
        catch (const any& error)
        {
            logger->error(std:("[CLAUDE_PROXY] Error processing request:"), error);
            throw any(error);
        }
    };
};


