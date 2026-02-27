#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/autodoc/src/AIService/AIService.h"

AIService::AIService(std::shared_ptr<Configuration> configuration_) : configuration(configuration_)  {
    if (!process->env->OPENAI_API_KEY) {
        throw any(std::make_shared<Error>(std:("OPENAI_API_KEY is not set")));
    }
    this->chatModel = std::make_shared<ChatOpenAI>(object{
        object::pair{std:("apiKey"), process->env->OPENAI_API_KEY}
    });
    this->chatModelFAQ = std::make_shared<ChatOpenAI>(object{
        object::pair{std:("apiKey"), process->env->OPENAI_API_KEY}, 
        object::pair{std:("model"), std:("gpt-4o")}
    });
    this->codeFormatter = std::make_shared<CodeFormatter>();
}

std::shared_ptr<Promise<string>> AIService::generateComment(string prompt, boolean isFAQ)
{
    try
    {
        auto finalPrompt = prompt;
        if (!isFAQ) {
            finalPrompt = this->codeFormatter->truncateCodeBlock(prompt, 8000);
        }
        console->log(std:("Generating comment for prompt of length: ") + finalPrompt->get_length() + string_empty);
        try
        {
            any response;
            if (isFAQ) {
                response = std::async([=]() { this->chatModelFAQ->invoke(finalPrompt); });
            } else {
                response = std::async([=]() { this->chatModel->invoke(finalPrompt); });
            }
            return as<string>(response["content"]);
        }
        catch (const any& error)
        {
            if (AND((is<Error>(error)), (error->message->includes(std:("maximum context length"))))) {
                console->warn(std:("Token limit exceeded, attempting with further truncation..."));
                finalPrompt = this->codeFormatter->truncateCodeBlock(prompt, 4000);
                try
                {
                    auto response = std::async([=]() { this->chatModel->invoke(finalPrompt); });
                    return as<string>(response->content);
                }
                catch (const any& retryError)
                {
                    if (AND((is<Error>(retryError)), (retryError->message->includes(std:("maximum context length"))))) {
                        console->warn(std:("Still exceeding token limit, using minimal context..."));
                        finalPrompt = this->codeFormatter->truncateCodeBlock(prompt, 2000);
                        auto response = std::async([=]() { this->chatModel->invoke(finalPrompt); });
                        return as<string>(response->content);
                    }
                    throw any(retryError);
                }
            }
            throw any(error);
        }
    }
    catch (const any& error)
    {
        this->handleAPIError(as<std::shared_ptr<Error>>(error));
        return string_empty;
    }
}

void AIService::handleAPIError(std::shared_ptr<Error> error)
{
    console->error(std:("API Error:"), error->message);
    throw any(error);
}


void Main(void)
{
    dotenv->config();
}

MAIN
