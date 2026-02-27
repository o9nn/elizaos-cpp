#include "auto-install-bun.h"

void updatePathForBun()
{
    auto home = homedir();
    auto bunBinPath = join(home, std:(".bun"), std:("bin"));
    auto currentPath = OR((process->env->PATH), (string_empty));
    auto pathSeparator = (process->platform == std:("win32")) ? std:(";") : std:(":");
    auto pathDirs = currentPath->split(pathSeparator);
    shared bunBinPathNormalized = bunBinPath->replace((new RegExp(std:("[/\\]+"))), string_empty);
    auto isInPath = pathDirs->some([=](auto dir) mutable
    {
        auto dirNormalized = dir->replace((new RegExp(std:("[/\\]+"))), string_empty);
        return dirNormalized == bunBinPathNormalized;
    }
    );
    if (!isInPath) {
        process->env->PATH = string_empty + bunBinPath + string_empty + pathSeparator + string_empty + currentPath + string_empty;
        logger->debug(std:("Added ") + bunBinPath + std:(" to PATH for current process"));
    } else {
        logger->debug(string_empty + bunBinPath + std:(" is already in PATH"));
    }
};


std::shared_ptr<Promise<boolean>> isBunInstalled()
{
    try
    {
        auto exists = std::async([=]() { commandExists(std:("bun")); });
        if (!exists) {
            return false;
        }
        auto result = std::async([=]() { bunExec(std:("bun"), array<string>{ std:("--version") }, object{
            object::pair{std:("stdio"), std:("ignore")}
        }); });
        return result->success;
    }
    catch (const any& error)
    {
        logger->debug(std:("Error checking if Bun is installed:"), error);
        return false;
    }
};


std::shared_ptr<Promise<boolean>> autoInstallBun()
{
    if (std::async([=]() { isBunInstalled(); })) {
        logger->info(std:("Bun is already installed."));
        return true;
    }
    auto platform = process->platform;
    try
    {
        if (platform == std:("win32")) {
            std::async([=]() { bunExecInherit(std:("powershell"), array<string>{ std:("-c"), std:("irm bun.sh/install.ps1 | iex") }); });
        } else {
            std::async([=]() { bunExecInherit(std:("sh"), array<string>{ std:("-c"), std:("curl -fsSL https://bun.sh/install | bash") }); });
        }
        logger->info(std:("Bun installation script executed successfully."));
        updatePathForBun();
        std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
        {
            return setTimeout(resolve, INSTALLATION_VERIFICATION_DELAY_MS);
        }
        ); });
        if (std::async([=]() { isBunInstalled(); })) {
            logger->success(std:("Bun has been successfully installed!"));
            return true;
        } else {
            logger->error(std:("Bun installation completed but the command is not available in PATH."));
            logger->error(std:("Please restart your terminal or source your shell profile."));
            return false;
        }
    }
    catch (const any& error)
    {
        logger->error(std:("Failed to auto-install Bun: ") + (is<Error>(error)) ? error->message : String(error) + string_empty);
        return false;
    }
};


boolean shouldAutoInstall()
{
    auto autoInstallEnv = process->env->ELIZA_AUTO_INSTALL_BUN;
    if (OR((autoInstallEnv == std:("false")), (autoInstallEnv == std:("0")))) {
        return false;
    }
    if (OR((process->env->CI), (process->env->CONTINUOUS_INTEGRATION))) {
        return false;
    }
    if (OR((process->env->CONTAINER), (process->env->DOCKER_CONTAINER))) {
        return false;
    }
    return true;
};


double INSTALLATION_VERIFICATION_DELAY_MS = 2000;

void Main(void)
{
}

MAIN
