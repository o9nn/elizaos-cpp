#include "authentication.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::string> getNpmUsername() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    console.info(
    "NPM authentication required for registry compliance (package name must match potential NPM package).";
    );

    try {
        // Check if already logged in
        const auto { stdout } = execa("npm", ["whoami"]);
        const auto currentUser = stdout.trim();
        std::cout << "Found existing NPM login: " + currentUser << std::endl;

        // Ask if they want to use this account or login with a different one
        const auto useExisting = clack.confirm({;
            "message: " + "Use NPM account \"" + currentUser + "\" for package naming?"
            initialValue: true,
            });

            if (clack.isCancel(useExisting)) {
                clack.cancel("Operation cancelled.");
                process.exit(0);
            }

            if (useExisting) {
                return currentUser;
                } else {
                    // They want to use a different account, prompt for login
                    std::cout << "Please login with your desired NPM account..." << std::endl;
                    execa("npm", ["login"], { stdio: "inherit" });

                    // Get the new username after login
                    const auto { stdout: newStdout } = execa("npm", ["whoami"]);
                    const auto newUser = newStdout.trim();
                    std::cout << "Logged in as: " + newUser << std::endl;
                    return newUser;
                }
                } catch (error) {
                    // Not logged in, prompt for login
                    std::cout << "Not logged into NPM. Please login to continue..." << std::endl;
                    try {
                        execa("npm", ["login"], { stdio: "inherit" });

                        // Get username after successful login
                        const auto { stdout } = execa("npm", ["whoami"]);
                        const auto username = stdout.trim();
                        std::cout << "Successfully logged in as: " + username << std::endl;
                        return username;
                        } catch (loginError) {
                            std::cerr << "NPM login failed. Registry compliance requires a valid NPM account." << std::endl;
                            process.exit(1);
                        }
                    }

}

} // namespace elizaos
