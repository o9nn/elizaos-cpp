#include "directory-utils.h"

void handleInvalidDirectory(std::shared_ptr<DirectoryInfo> directoryInfo)
{
    auto messages = object{
        object::pair{std:("non-elizaos-dir"), (array<string>{ std:("This directory doesn't appear to be an ElizaOS project."), AND((directoryInfo->packageName), (std:("Found package: ") + directoryInfo->packageName + string_empty)), std:("ElizaOS update only works in ElizaOS projects, plugins, the ElizaOS monorepo, and ElizaOS infrastructure packages (e.g. client, cli)."), std:("To create a new ElizaOS project, use: elizaos create <project-name>") })->filter(Boolean)}, 
        object::pair{std:("invalid"), (array<string>{ std:("Cannot update packages in this directory."), (!directoryInfo->hasPackageJson) ? std:("No package.json found. This doesn't appear to be a valid project directory.") : std:("The package.json file appears to be invalid or unreadable."), std:("To create a new ElizaOS project, use: elizaos create <project-name>") })->filter(Boolean)}
    };
    auto messageList = const_(messages)[directoryInfo->type];
    if (messageList) {
        messageList->forEach([=](auto msg) mutable
        {
            return console->info(msg);
        }
        );
    } else {
        console->error(std:("Unexpected directory type: ") + directoryInfo->type + string_empty);
    }
};


