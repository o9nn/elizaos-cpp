#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/src/commands/dev/index.h"

any dev = ((std::make_shared<Command>()))->name(std:("dev"))->description(std:("Start the project or plugin in development mode with auto-rebuild, detailed logging, and file change detection"))->option(std:("-c, --configure"), std:("Reconfigure services and AI models (skips using saved configuration)"))->option(std:("--character [paths...]"), std:("Character file(s) to use - accepts paths or URLs"))->option(std:("-b, --build"), std:("Build the project before starting"))->addOption(((std::make_shared<Option>(std:("-p, --port <port>"), std:("Port to listen on (default: 3000)"))))->argParser([=](auto value) mutable
{
    auto port = Number->parseInt(value, 10);
    if (OR((OR((Number->isNaN(port)), (port < 0))), (port > 65535))) {
        throw any(std::make_shared<Error>(std:("Port must be a number between 0 and 65535")));
    }
    return port;
}
))->action([=](auto options) mutable
{
    try
    {
        std::async([=]() { startDevMode(options); });
    }
    catch (const any& error)
    {
        handleError(error);
    }
}
);

void Main(void)
{
}

MAIN
