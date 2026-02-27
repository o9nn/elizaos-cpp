#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/src/index.h"

void gracefulShutdown(string signal)
{
    if (!shutdownState["tryInitiateShutdown"]()) {
        logger->debug(std:("Ignoring ") + signal + std:(" - shutdown already in progress"));
        return std::shared_ptr<Promise<void>>();
    }
    logger->info(std:("Received ") + signal + std:(", shutting down gracefully..."));
    try
    {
        auto serverWasStopped = std::async([=]() { stopServer(); });
        if (serverWasStopped) {
            logger->info(std:("Server stopped successfully"));
        }
    }
    catch (const any& error)
    {
        auto errorMessage = (is<Error>(error)) ? error->message : String(error);
        logger->error(std:("Error stopping server: ") + errorMessage + string_empty);
        logger->debug(std:("Full error details:"), error);
    }
    auto exitCode = (signal == std:("SIGINT")) ? any(130) ((signal == std:("SIGTERM")) ? 143 : 0);
    process->exit(exitCode);
};


void main()
{
    auto delegated = std::async([=]() { tryDelegateToLocalCli(); });
    if (delegated) {
        return std::shared_ptr<Promise<void>>();
    }
    if (process->argv->includes(std:("--no-emoji"))) {
        configureEmojis(object{
            object::pair{std:("forceDisable"), true}
        });
    }
    if (process->argv->includes(std:("--no-auto-install"))) {
        process->env->ELIZA_NO_AUTO_INSTALL = std:("true");
    }
    auto version = getVersion();
    auto args = process->argv->slice(2);
    auto isUpdateCommand = args->includes(std:("update"));
    auto willShowBanner = args->get_length() == 0;
    if (AND((!willShowBanner), (!isUpdateCommand))) {
        auto currentVersion = getVersion();
        std::async([=]() { checkAndShowUpdateNotification(currentVersion); });
    }
    auto program = ((std::make_shared<Command>()))->name(std:("elizaos"))->version(version, std:("-v, --version"), std:("output the version number"))->option(std:("--no-emoji"), std:("Disable emoji output"))->option(std:("--no-auto-install"), std:("Disable automatic Bun installation"));
    program->addCommand(create)->addCommand(monorepo)->addCommand(plugins)->addCommand(agent)->addCommand(tee)->addCommand(start)->addCommand(update)->addCommand(test)->addCommand(env)->addCommand(dev)->addCommand(publish);
    if (process->argv->get_length() == 2) {
        std::async([=]() { displayBanner(false); });
    }
    std::async([=]() { program->parseAsync(); });
};


object shutdownState = object{
    object::pair{std:("isShuttingDown"), false}, 
};

void Main(void)
{
    process->env->NODE_OPTIONS = std:("--no-deprecation");
    process->env->NODE_NO_WARNINGS = std:("1");
    process->env->QUIET_MODE = OR((process->env->QUIET_MODE), (std:("true")));
    process->on(std:("SIGINT"), [=]() mutable
    {
        return gracefulShutdown(std:("SIGINT"));
    }
    );
    process->on(std:("SIGTERM"), [=]() mutable
    {
        return gracefulShutdown(std:("SIGTERM"));
    }
    );
    main()->_catch([=](auto error) mutable
    {
        logger->error(std:("An error occurred:"), error);
        process->exit(1);
    }
    );
}

MAIN
