#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/utils/handle-error.h"

void handleError(any error)
{
    auto isNoSpace = OR(((AND((is<Error>(error)), ((OR((error->message->includes(std::string("no space left on device"))), (error->message->includes(std::string("ENOSPC"))))))))), ((AND((type_of(error) == std::string("string")), ((OR((error->includes(std::string("no space left on device"))), (error->includes(std::string("ENOSPC"))))))))));
    if (isNoSpace) {
        logger->error(colors->red(std::string("ERROR: No space left on device! Please free up disk space and try again.")));
        if (is<Error>(error)) {
            logger->error(colors->red(error->message));
            logger->error(colors->red(OR((error->stack), (string_empty))));
        } else {
            logger->error(colors->red(String(error)));
        }
    } else {
        logger->error(std::string("An error occurred:"), error);
        if (is<Error>(error)) {
            logger->error(std::string("Error details:"), error->message);
            logger->error(std::string("Stack trace:"), error->stack);
        } else {
            logger->error(std::string("Unknown error type:"), type_of(error));
            logger->error(std::string("Error value:"), error);
        }
    }
    process->exit(1);
};


void checkServer(std::shared_ptr<OptionValues> opts)
{
    try
    {
        auto response = std::async([=]() { fetch(string_empty + getAgentRuntimeUrl(opts) + std::string("/api/agents")); });
        if (!response->ok) {
            throw any(std::make_shared<Error>(std::string("Server responded with ") + response->status + std::string(": ") + response->statusText + string_empty));
        }
        logger->success(std::string("ElizaOS server is running"));
    }
    catch (const any& error)
    {
        logger->error(std::string("Unable to connect to ElizaOS server, likely not running or not accessible!"));
        process->exit(1);
    }
};


