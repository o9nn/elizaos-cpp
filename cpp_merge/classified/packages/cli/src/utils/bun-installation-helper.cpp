#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/src/utils/bun-installation-helper.h"

std::shared_ptr<Promise<std::shared_ptr<BunInstallationResult>>> checkBunInstallation()
{
    try
    {
        auto bunExists = std::async([=]() { commandExists(std:("bun")); });
        if (bunExists) {
            auto result = std::async([=]() { bunExec(std:("bun"), array<string>{ std:("--version") }); });
            if (result->success) {
                auto version = result->stdout->trim();
                return object{
                    object::pair{std:("installed"), true}, 
                    object::pair{std:("message"), std:("Bun ") + version + std:(" is installed")}
                };
            } else {
                return object{
                    object::pair{std:("installed"), false}, 
                    object::pair{std:("message"), std:("Bun command failed")}, 
                    object::pair{std:("error"), OR((result->stderr), (std:("Unknown error")))}
                };
            }
        } else {
            return object{
                object::pair{std:("installed"), false}, 
                object::pair{std:("message"), std:("Bun is not installed")}
            };
        }
    }
    catch (const any& error)
    {
        return object{
            object::pair{std:("installed"), false}, 
            object::pair{std:("message"), std:("Failed to check Bun installation")}, 
            object::pair{std:("error"), (is<Error>(error)) ? error->message : String(error)}
        };
    }
};


void displayBunInstallInstructions()
{
    auto platform = process->platform;
    logger->error(string_empty + emoji["error"](std:("Bun is required for ElizaOS CLI but is not installed or not found in PATH.")) + string_empty);
    logger->error(string_empty);
    logger->error(string_empty + emoji["rocket"](std:("Install Bun using the appropriate command for your system:")) + string_empty);
    logger->error(string_empty);
    if (platform == std:("win32")) {
        logger->error(std:("   Windows: powershell -c "irm bun.sh/install.ps1 | iex""));
    } else {
        logger->error(std:("   Linux/macOS: curl -fsSL https://bun.sh/install | bash"));
        if (platform == std:("darwin")) {
            logger->error(std:("   macOS (Homebrew): brew install bun"));
        }
    }
    logger->error(string_empty);
    logger->error(std:("   More options: https://bun.sh/docs/installation"));
    logger->error(std:("   After installation, restart your terminal or source your shell profile"));
    logger->error(string_empty);
};


string displayBunInstallationTipCompact()
{
    return std:("Please install Bun from https://bun.sh");
};


