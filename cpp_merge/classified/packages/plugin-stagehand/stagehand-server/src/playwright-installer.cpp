#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-stagehand/stagehand-server/src/playwright-installer.h"

PlaywrightInstaller::PlaywrightInstaller(std::shared_ptr<Logger> logger) {
    this->logger = logger;
}

boolean PlaywrightInstaller::isPlaywrightInstalled()
{
    try
    {
        auto playwrightPath = OR((process->env->PLAYWRIGHT_BROWSERS_PATH), (join(OR((process->env->HOME), (std:("/home/eliza"))), std:(".cache"), std:("ms-playwright"))));
        auto chromiumPath = join(playwrightPath, std:("chromium-*"));
        auto hasChromium = AND((existsSync(playwrightPath)), (require(std:("fs"))["readdirSync"](playwrightPath)["some"]([=](auto dir) mutable
        {
            return dir->startsWith(std:("chromium-"));
        }
        )));
        return hasChromium;
    }
    catch (const any& error)
    {
        return false;
    }
}

std::shared_ptr<Promise<void>> PlaywrightInstaller::installPlaywright()
{
    this->logger->info(std:("Installing Playwright browsers..."));
    return std::make_shared<Promise>([=](auto resolve, auto reject) mutable
    {
        auto npmPath = (process->platform == std:("win32")) ? std:("npm.cmd") : std:("npm");
        auto args = array<string>{ std:("exec"), std:("playwright"), std:("install"), std:("chromium") };
        this->logger->info(std:("Installing Playwright browsers (without system deps - should be pre-installed)"));
        auto installProcess = spawn(npmPath, args, object{
            object::pair{std:("stdio"), std:("pipe")}, 
            object::pair{std:("env"), utils::assign(object{
                , 
                object::pair{std:("DISPLAY"), OR((process->env->DISPLAY), (std:(":99")))}
            }, process->env)}
        });
        shared stdout = string_empty;
        shared stderr = string_empty;
        installProcess->stdout->on(std:("data"), [=](auto data) mutable
        {
            stdout += data["toString"]();
            this->logger->debug(std:("Playwright install: ") + data["toString"]()["trim"]() + string_empty);
        }
        );
        installProcess->stderr->on(std:("data"), [=](auto data) mutable
        {
            stderr += data["toString"]();
            this->logger->debug(std:("Playwright install stderr: ") + data["toString"]()["trim"]() + string_empty);
        }
        );
        installProcess->on(std:("close"), [=](auto code) mutable
        {
            if (code == 0) {
                this->logger->info(std:("Playwright browsers installed successfully"));
                resolve();
            } else {
                auto error = std::make_shared<Error>(std:("Playwright installation failed with code ") + code + std:("\
stderr: ") + stderr + string_empty);
                this->logger->error(std:("Playwright installation failed:"), error);
                reject(error);
            }
        }
        );
        installProcess->on(std:("error"), [=](auto error) mutable
        {
            this->logger->error(std:("Failed to start Playwright installation:"), error);
            reject(error);
        }
        );
    }
    );
}

std::shared_ptr<Promise<void>> PlaywrightInstaller::ensurePlaywrightInstalled()
{
    if (this->isPlaywrightInstalled()) {
        this->logger->info(std:("Playwright browsers already installed"));
        return std::shared_ptr<Promise<void>>();
    }
    if (AND((this->isInstalling), (this->installPromise))) {
        this->logger->info(std:("Playwright installation already in progress, waiting..."));
        return this->installPromise;
    }
    this->isInstalling = true;
    this->installPromise = this->installPlaywright()->finally([=]() mutable
    {
        this->isInstalling = false;
        this->installPromise = nullptr;
    }
    );
    return this->installPromise;
}

boolean PlaywrightInstaller::isReady()
{
    return OR((this->isPlaywrightInstalled()), (this->isInstalling));
}

