#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/src/utils/bun-installation-helper.h"

std::shared_ptr<Promise<std::shared_ptr<BunInstallationResult>>> checkBunInstallation()
{
    try
    {
        auto bunExists = std::async([=]() { commandExists(std::string("bun")); });
        if (bunExists) {
            auto result = std::async([=]() { bunExec(std::string("bun"), array<string>{ std::string("--version") }); });
            if (result->success) {
                auto version = result->stdout->trim();
                return object{
                    object::pair{std::string("installed"), true}, 
                    object::pair{std::string("message"), std::string("Bun ") + version + std::string(" is installed")}
                };
            } else {
                return object{
                    object::pair{std::string("installed"), false}, 
                    object::pair{std::string("message"), std::string("Bun command failed")}, 
                    object::pair{std::string("error"), OR((result->stderr), (std::string("Unknown error")))}
                };
            }
        } else {
            return object{
                object::pair{std::string("installed"), false}, 
                object::pair{std::string("message"), std::string("Bun is not installed")}
            };
        }
    }
    catch (const any& error)
    {
        return object{
            object::pair{std::string("installed"), false}, 
            object::pair{std::string("message"), std::string("Failed to check Bun installation")}, 
            object::pair{std::string("error"), (is<Error>(error)) ? error->message : String(error)}
        };
    }
};


void displayBunInstallInstructions()
{
    auto platform = process->platform;
    logger->error(string_empty + emoji["error"](std::string("Bun is required for ElizaOS CLI but is not installed or not found in PATH.")) + string_empty);
    logger->error(string_empty);
    logger->error(string_empty + emoji["rocket"](std::string("Install Bun using the appropriate command for your system:")) + string_empty);
    logger->error(string_empty);
    if (platform == std::string("win32")) {
        logger->error(std::string("   Windows: powershell -c "irm bun.sh/install.ps1 | iex""));
    } else {
        logger->error(std::string("   Linux/macOS: curl -fsSL https://bun.sh/install | bash"));
        if (platform == std::string("darwin")) {
            logger->error(std::string("   macOS (Homebrew): brew install bun"));
        }
    }
    logger->error(string_empty);
    logger->error(std::string("   More options: https://bun.sh/docs/installation"));
    logger->error(std::string("   After installation, restart your terminal or source your shell profile"));
    logger->error(string_empty);
};


string displayBunInstallationTipCompact()
{
    return std::string("Please install Bun from https://bun.sh");
};


