#include "directory-utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void handleInvalidDirectory(DirectoryInfo directoryInfo) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::variant<Record<string, (string, undefined)[]>> messages = {;
        "non-elizaos-dir": [
        "This directory doesn't appear to be an ElizaOS project.",
        "directoryInfo.packageName && " + "Found package: " + directoryInfo.packageName
        "ElizaOS update only works in ElizaOS projects, plugins, the ElizaOS monorepo, and ElizaOS infrastructure packages (e.g. client, cli).",
        "To create a new ElizaOS project, use: elizaos create <project-name>",
        ].filter(Boolean),
        invalid: [
        "Cannot update packages in this directory.",
        !directoryInfo.hasPackageJson;
        ? "No package.json found. This doesn't appear to be a valid project directory.";
        : "The package.json file appears to be invalid or unreadable.",
        "To create a new ElizaOS project, use: elizaos create <project-name>",
        ].filter(Boolean),
        };

        const auto messageList = messages[directoryInfo.type];
        if (messageList) {
            messageList.forEach((msg) => console.info(msg));
            } else {
                std::cerr << "Unexpected directory type: " + directoryInfo.type << std::endl;
            }

}

} // namespace elizaos
