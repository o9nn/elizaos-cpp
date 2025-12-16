#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;

/**
 * Handle invalid directory scenarios
 */
`,
      'ElizaOS update only works in ElizaOS projects, plugins, the ElizaOS monorepo, and ElizaOS infrastructure packages (e.g. client, cli).',
      'To create a new ElizaOS project, use: elizaos create <project-name>',
    ].filter(Boolean),
    invalid: [
      'Cannot update packages in this directory.',
      !directoryInfo.hasPackageJson
        ? "No package.json found. This doesn't appear to be a valid project directory."
        : 'The package.json file appears to be invalid or unreadable.',
      'To create a new ElizaOS project, use: elizaos create <project-name>',
    ].filter(Boolean),
  };

  const messageList = messages[directoryInfo.type];
  if (messageList) {
    messageList.forEach((msg) => console.info(msg));
  } else {
    console.error(`Unexpected directory type: ${directoryInfo.type}`);
  }
}

} // namespace elizaos
