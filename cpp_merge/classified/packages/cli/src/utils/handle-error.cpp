#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/src/utils/handle-error.h"

void handleError(any error)
{
    auto isNoSpace = OR(((AND((is<Error>(error)), ((OR((error->message->includes(std:("no space left on device"))), (error->message->includes(std:("ENOSPC"))))))))), ((AND((type_of(error) == std:("string")), ((OR((error->includes(std:("no space left on device"))), (error->includes(std:("ENOSPC"))))))))));
    if (isNoSpace) {
        logger->error(colors->red(std:("ERROR: No space left on device! Please free up disk space and try again.")));
        if (is<Error>(error)) {
            logger->error(colors->red(error->message));
            logger->error(colors->red(OR((error->stack), (string_empty))));
        } else {
            logger->error(colors->red(String(error)));
        }
    } else {
        logger->error(std:("An error occurred:"), error);
        if (is<Error>(error)) {
            logger->error(std:("Error details:"), error->message);
            logger->error(std:("Stack trace:"), error->stack);
        } else {
            logger->error(std:("Unknown error type:"), type_of(error));
            logger->error(std:("Error value:"), error);
        }
    }
    process->exit(1);
};


void checkServer(std::shared_ptr<OptionValues> opts)
{
    try
    {
        auto response = std::async([=]() { fetch(string_empty + getAgentRuntimeUrl(opts) + std:("/api/agents")); });
        if (!response->ok) {
            throw any(std::make_shared<Error>(std:("Server responded with ") + response->status + std:(": ") + response->statusText + string_empty));
        }
        logger->success(std:("ElizaOS server is running"));
    }
    catch (const any& error)
    {
        logger->error(std:("Unable to connect to ElizaOS server, likely not running or not accessible!"));
        process->exit(1);
    }
};


