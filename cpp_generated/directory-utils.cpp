#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/commands/update/utils/directory-utils.h"

void handleInvalidDirectory(std::shared_ptr<DirectoryInfo> directoryInfo)
{
    auto messages = object{
        object::pair{std::string("non-elizaos-dir"), (array<string>{ std::string("This directory doesn't appear to be an ElizaOS project."), AND((directoryInfo->packageName), (std::string("Found package: ") + directoryInfo->packageName + string_empty)), std::string("ElizaOS update only works in ElizaOS projects, plugins, the ElizaOS monorepo, and ElizaOS infrastructure packages (e.g. client, cli)."), std::string("To create a new ElizaOS project, use: elizaos create <project-name>") })->filter(Boolean)}, 
        object::pair{std::string("invalid"), (array<string>{ std::string("Cannot update packages in this directory."), (!directoryInfo->hasPackageJson) ? std::string("No package.json found. This doesn't appear to be a valid project directory.") : std::string("The package.json file appears to be invalid or unreadable."), std::string("To create a new ElizaOS project, use: elizaos create <project-name>") })->filter(Boolean)}
    };
    auto messageList = const_(messages)[directoryInfo->type];
    if (messageList) {
        messageList->forEach([=](auto msg) mutable
        {
            return console->info(msg);
        }
        );
    } else {
        console->error(std::string("Unexpected directory type: ") + directoryInfo->type + string_empty);
    }
};


