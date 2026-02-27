#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_CREATE_ACTIONS_CREATORS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_CREATE_ACTIONS_CREATORS_H
#include "core.hpp"
// External dependency removed
// External dependency removed
using copyTemplateUtil = copyTemplate;
#include "path.hpp"
#include "node:fs/promises.h"
// Using alias removed (invalid transpilation)
// External dependency removed
#include "yoctocolors.hpp"
// Using alias removed (invalid transpilation)
#include "../utils.h"
#include "./setup.h"
// External dependency removed
#include "node:fs.h"
// External dependency removed

std::shared_ptr<Promise<void>> handleInteractiveConfiguration(string targetDir, string database, string aiModel, string embeddingModel = undefined);

template <typename T>
std::shared_ptr<Promise<T>> withCleanupOnInterrupt(string targetDir, string displayName, std::function<std::shared_ptr<Promise<T>>()> fn);

std::shared_ptr<Promise<void>> createPlugin(string pluginName, string targetDir, string pluginType = std:("full"), boolean isNonInteractive = false);

std::shared_ptr<Promise<void>> createAgent(string agentName, string targetDir, boolean isNonInteractive = false);

std::shared_ptr<Promise<void>> createTEEProject(string projectName, string targetDir, string database, string aiModel, string embeddingModel = undefined, boolean isNonInteractive = false);

std::shared_ptr<Promise<void>> createProject(string projectName, string targetDir, string database, string aiModel, string embeddingModel = undefined, boolean isNonInteractive = false);

template <typename T>
std::shared_ptr<Promise<T>> withCleanupOnInterrupt(string targetDir, string displayName, std::function<std::shared_ptr<Promise<T>>()> fn)
{
    shared directoryExistedBefore = existsSync(targetDir);
    auto cleanup = [=]() mutable
    {
        if (AND((!directoryExistedBefore), (existsSync(targetDir)))) {
            console->info(colors->red(std:("\
\
Interrupted! Cleaning up ") + displayName + std:("...")));
            try
            {
                rmSync(targetDir, object{
                    object::pair{std:("recursive"), true}, 
                    object::pair{std:("force"), true}
                });
                console->info(std:("Cleanup completed."));
            }
            catch (const any& error)
            {
                console->error(colors->red(std:("Error during cleanup:")), error);
            }
        }
    };
    auto sigintHandler = [=]() mutable
    {
        process->exit(130);
    };
    auto sigtermHandler = [=]() mutable
    {
        process->exit(143);
    };
    process->on(std:("exit"), cleanup);
    process->on(std:("SIGINT"), sigintHandler);
    process->on(std:("SIGTERM"), sigtermHandler);
    try
    {
        auto result = std::async([=]() { fn(); });
        process->removeListener(std:("exit"), cleanup);
        process->removeListener(std:("SIGINT"), sigintHandler);
        process->removeListener(std:("SIGTERM"), sigtermHandler);
        return result;
    }
    catch (const any& error)
    {
        process->removeListener(std:("exit"), cleanup);
        process->removeListener(std:("SIGINT"), sigintHandler);
        process->removeListener(std:("SIGTERM"), sigtermHandler);
        if (AND((!directoryExistedBefore), (existsSync(targetDir)))) {
            try
            {
                console->info(colors->red(std:("\
Cleaning up due to error...")));
                rmSync(targetDir, object{
                    object::pair{std:("recursive"), true}, 
                    object::pair{std:("force"), true}
                });
            }
            catch (const any& cleanupError)
            {
            }
        }
        throw any(error);
    }
};


#endif
