#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-INFERENCE_SRC_CLAUDE-PROXY_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-INFERENCE_SRC_CLAUDE-PROXY_H
#include "core.h"
#include "@elizaos/core.h"
#include "./index.h"

typedef any ClaudeContent;

class ClaudeTool;
class ClaudeMessage;
class ClaudeMessagesRequest;
class ClaudeResponse;
class OpenAIMessage;
class ClaudeProxy;

class ClaudeTool : public object, public std::enable_shared_from_this<ClaudeTool> {
public:
    using std::enable_shared_from_this<ClaudeTool>::shared_from_this;
    string name;

    string description;

    any input_schema;
};

class ClaudeMessage : public object, public std::enable_shared_from_this<ClaudeMessage> {
public:
    using std::enable_shared_from_this<ClaudeMessage>::shared_from_this;
    any role;

    ClaudeContent content;
};

class ClaudeMessagesRequest : public object, public std::enable_shared_from_this<ClaudeMessagesRequest> {
public:
    using std::enable_shared_from_this<ClaudeMessagesRequest>::shared_from_this;
    string model;

    array<std::shared_ptr<ClaudeMessage>> messages;

    string system;

    double max_tokens;

    array<string> stop_sequences;

    boolean stream;

    double temperature;

    double top_p;

    double top_k;

    array<std::shared_ptr<ClaudeTool>> tools;

    object tool_choice;
};

class ClaudeResponse : public object, public std::enable_shared_from_this<ClaudeResponse> {
public:
    using std::enable_shared_from_this<ClaudeResponse>::shared_from_this;
    string id;

    string type;

    string role;

    string model;

    array<object> content;

    string stop_reason;

    object usage;
};

class OpenAIMessage : public object, public std::enable_shared_from_this<OpenAIMessage> {
public:
    using std::enable_shared_from_this<OpenAIMessage>::shared_from_this;
    any role;

    any content;

    array<object> tool_calls;

    string tool_call_id;
};

class ClaudeProxy : public object, public std::enable_shared_from_this<ClaudeProxy> {
public:
    using std::enable_shared_from_this<ClaudeProxy>::shared_from_this;
    std::shared_ptr<IAgentRuntime> runtime;

    ClaudeProxy(std::shared_ptr<IAgentRuntime> runtime_);
    virtual std::shared_ptr<Promise<std::shared_ptr<ClaudeResponse>>> processRequest(std::shared_ptr<ClaudeMessagesRequest> claudeRequest);
    virtual std::shared_ptr<Promise<std::shared_ptr<ClaudeResponse>>> makeAnthropicProxyRequest(std::shared_ptr<ClaudeMessagesRequest> claudeRequest);
    virtual std::shared_ptr<Promise<InferenceProvider>> selectProvider();
    virtual std::shared_ptr<Promise<boolean>> isProviderAvailable(InferenceProvider provider);
    virtual std::shared_ptr<Promise<std::shared_ptr<ClaudeResponse>>> handleOpenAIRequest(std::shared_ptr<ClaudeMessagesRequest> claudeRequest);
    virtual std::shared_ptr<Promise<std::shared_ptr<ClaudeResponse>>> handleGroqRequest(std::shared_ptr<ClaudeMessagesRequest> claudeRequest);
    virtual std::shared_ptr<Promise<std::shared_ptr<ClaudeResponse>>> handleOllamaRequest(std::shared_ptr<ClaudeMessagesRequest> claudeRequest);
    virtual array<std::shared_ptr<OpenAIMessage>> convertClaudeToOpenAI(std::shared_ptr<ClaudeMessagesRequest> claudeRequest);
    virtual string convertClaudeToPrompt(std::shared_ptr<ClaudeMessagesRequest> claudeRequest);
    virtual string formatOpenAIPrompt(array<std::shared_ptr<OpenAIMessage>> messages, string system = undefined);
    virtual std::shared_ptr<ClaudeResponse> formatClaudeResponse(string text, string model);
};

any createClaudeHandler(std::shared_ptr<IAgentRuntime> runtime);

#endif
