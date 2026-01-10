#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-stagehand/stagehand-server/src/playwright-installer.h"

PlaywrightInstaller::PlaywrightInstaller(std::shared_ptr<Logger> logger) {
    this->logger = logger;
}

boolean PlaywrightInstaller::isPlaywrightInstalled()
{
    try
    {
        auto playwrightPath = OR((process->env->PLAYWRIGHT_BROWSERS_PATH), (join(OR((process->env->HOME), (std::string("/home/eliza"))), std::string(".cache"), std::string("ms-playwright"))));
        auto chromiumPath = join(playwrightPath, std::string("chromium-*"));
        auto hasChromium = AND((existsSync(playwrightPath)), (require(std::string("fs"))["readdirSync"](playwrightPath)["some"]([=](auto dir) mutable
        {
            return dir->startsWith(std::string("chromium-"));
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
    this->logger->info(std::string("Installing Playwright browsers..."));
    return std::make_shared<Promise>([=](auto resolve, auto reject) mutable
    {
        auto npmPath = (process->platform == std::string("win32")) ? std::string("npm.cmd") : std::string("npm");
        auto args = array<string>{ std::string("exec"), std::string("playwright"), std::string("install"), std::string("chromium") };
        this->logger->info(std::string("Installing Playwright browsers (without system deps - should be pre-installed)"));
        auto installProcess = spawn(npmPath, args, object{
            object::pair{std::string("stdio"), std::string("pipe")}, 
            object::pair{std::string("env"), utils::assign(object{
                , 
                object::pair{std::string("DISPLAY"), OR((process->env->DISPLAY), (std::string(":99")))}
            }, process->env)}
        });
        shared stdout = string_empty;
        shared stderr = string_empty;
        installProcess->stdout->on(std::string("data"), [=](auto data) mutable
        {
            stdout += data["toString"]();
            this->logger->debug(std::string("Playwright install: ") + data["toString"]()["trim"]() + string_empty);
        }
        );
        installProcess->stderr->on(std::string("data"), [=](auto data) mutable
        {
            stderr += data["toString"]();
            this->logger->debug(std::string("Playwright install stderr: ") + data["toString"]()["trim"]() + string_empty);
        }
        );
        installProcess->on(std::string("close"), [=](auto code) mutable
        {
            if (code == 0) {
                this->logger->info(std::string("Playwright browsers installed successfully"));
                resolve();
            } else {
                auto error = std::make_shared<Error>(std::string("Playwright installation failed with code ") + code + std::string("\
stderr: ") + stderr + string_empty);
                this->logger->error(std::string("Playwright installation failed:"), error);
                reject(error);
            }
        }
        );
        installProcess->on(std::string("error"), [=](auto error) mutable
        {
            this->logger->error(std::string("Failed to start Playwright installation:"), error);
            reject(error);
        }
        );
    }
    );
}

std::shared_ptr<Promise<void>> PlaywrightInstaller::ensurePlaywrightInstalled()
{
    if (this->isPlaywrightInstalled()) {
        this->logger->info(std::string("Playwright browsers already installed"));
        return std::shared_ptr<Promise<void>>();
    }
    if (AND((this->isInstalling), (this->installPromise))) {
        this->logger->info(std::string("Playwright installation already in progress, waiting..."));
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

