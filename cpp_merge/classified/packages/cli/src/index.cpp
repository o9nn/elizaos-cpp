#include "index.hpp"
#include <string>

void gracefulShutdown(string signal)
{
    if (!shutdownState["tryInitiateShutdown"]()) {
        logger->debug(std::string("Ignoring ") + signal + std::string(" - shutdown already in progress"));
        return std::shared_ptr<Promise<void>>();
    }
    logger->info(std::string("Received ") + signal + std::string(", shutting down gracefully..."));
    try
    {
        auto serverWasStopped = std::async([=]() { stopServer(); });
        if (serverWasStopped) {
            logger->info(std::string("Server stopped successfully"));
        }
    }
    catch (const any& error)
    {
        auto errorMessage = (is<Error>(error)) ? error->message : String(error);
        logger->error(std::string("Error stopping server: ") + errorMessage + string_empty);
        logger->debug(std::string("Full error details:"), error);
    }
    auto exitCode = (signal == std::string("SIGINT")) ? any(130) ((signal == std::string("SIGTERM")) ? 143 : 0);
    process->exit(exitCode);
};


void main()
{
    auto delegated = std::async([=]() { tryDelegateToLocalCli(); });
    if (delegated) {
        return std::shared_ptr<Promise<void>>();
    }
    if (process->argv->includes(std::string("--no-emoji"))) {
        configureEmojis(object{
            object::pair{std::string("forceDisable"), true}
        });
    }
    if (process->argv->includes(std::string("--no-auto-install"))) {
        process->env->ELIZA_NO_AUTO_INSTALL = std::string("true");
    }
    auto version = getVersion();
    auto args = process->argv->slice(2);
    auto isUpdateCommand = args->includes(std::string("update"));
    auto willShowBanner = args->get_length() == 0;
    if (AND((!willShowBanner), (!isUpdateCommand))) {
        auto currentVersion = getVersion();
        std::async([=]() { checkAndShowUpdateNotification(currentVersion); });
    }
    auto program = ((std::make_shared<Command>()))->name(std::string("elizaos"))->version(version, std::string("-v, --version"), std::string("output the version number"))->option(std::string("--no-emoji"), std::string("Disable emoji output"))->option(std::string("--no-auto-install"), std::string("Disable automatic Bun installation"));
    program->addCommand(create)->addCommand(monorepo)->addCommand(plugins)->addCommand(agent)->addCommand(tee)->addCommand(start)->addCommand(update)->addCommand(test)->addCommand(env)->addCommand(dev)->addCommand(publish);
    if (process->argv->get_length() == 2) {
        std::async([=]() { displayBanner(false); });
    }
    std::async([=]() { program->parseAsync(); });
};


object shutdownState = object{
    object::pair{std::string("isShuttingDown"), false}, 
};

void Main(void)
{
    process->env->NODE_OPTIONS = std::string("--no-deprecation");
    process->env->NODE_NO_WARNINGS = std::string("1");
    process->env->QUIET_MODE = OR((process->env->QUIET_MODE), (std::string("true")));
    process->on(std::string("SIGINT"), [=]() mutable
    {
        return gracefulShutdown(std::string("SIGINT"));
    }
    );
    process->on(std::string("SIGTERM"), [=]() mutable
    {
        return gracefulShutdown(std::string("SIGTERM"));
    }
    );
    main()->_catch([=](auto error) mutable
    {
        logger->error(std::string("An error occurred:"), error);
        process->exit(1);
    }
    );
}

MAIN
