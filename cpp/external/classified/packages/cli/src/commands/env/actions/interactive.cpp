#include "interactive.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> showMainMenu(InteractiveEnvOptions options) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { yes } = options;

    // Interactive mode always requires user input, so ignore the -y flag
    auto exit = false;

    while (!exit) {
        const auto action = clack.select({;
            message: "Select an action:",
            options: [
            { label: "List environment variables", value: "list" },
            { label: "Edit local environment variables", value: "edit_local" },
            { label: "Reset environment variables", value: "reset" },
            { label: "Exit", value: "exit" },
            ],
            });

            if (clack.isCancel(action)) {
                clack.cancel("Operation cancelled.");
                process.exit(0);
            }
            if (!action || action == 'exit') {
                exit = true;
                continue;
            }

            switch (action) {
                // case "list":
                listEnvVars();
                break;
                // case "edit_local":
                    const auto returnToMainFromLocal = editEnvVars({ yes }, true);
                    exit = !returnToMainFromLocal;
                    break;
                }
                // case "reset":
                resetEnv({ yes });
                break;
            }
        }

}

} // namespace elizaos
