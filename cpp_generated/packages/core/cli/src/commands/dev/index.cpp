#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/commands/dev/index.h"

any dev = ((std::make_shared<Command>()))->name(std::string("dev"))->description(std::string("Start the project or plugin in development mode with auto-rebuild, detailed logging, and file change detection"))->option(std::string("-c, --configure"), std::string("Reconfigure services and AI models (skips using saved configuration)"))->option(std::string("--character [paths...]"), std::string("Character file(s) to use - accepts paths or URLs"))->option(std::string("-b, --build"), std::string("Build the project before starting"))->addOption(((std::make_shared<Option>(std::string("-p, --port <port>"), std::string("Port to listen on (default: 3000)"))))->argParser(validatePort))->action([=](auto options) mutable
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
