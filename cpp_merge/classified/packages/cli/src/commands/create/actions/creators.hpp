#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_CREATE_ACTIONS_CREATORS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_CREATE_ACTIONS_CREATORS_H
#include "core.hpp"
#include <string>
// External dependency removed
// External dependency removed
using copyTemplateUtil = copyTemplate;
#include "path.hpp"
// Node.js module removed - use C++ filesystem
// Using alias removed (invalid transpilation)
// External dependency removed
#include "yoctocolors.hpp"
// Using alias removed (invalid transpilation)
#include "../utils.h"
#include "./setup.h"
// External dependency removed
// Node.js module removed - use C++ filesystem
// External dependency removed

std::shared_ptr<Promise<void>> handleInteractiveConfiguration(string targetDir, string database, string aiModel, string embeddingModel = std::nullopt);

template <typename T>
std::shared_ptr<Promise<T>> withCleanupOnInterrupt(string targetDir, string displayName, std::function<std::shared_ptr<Promise<T>>()> fn);

std::shared_ptr<Promise<void>> createPlugin(string pluginName, string targetDir, string pluginType = std::string("full"), boolean isNonInteractive = false);

std::shared_ptr<Promise<void>> createAgent(string agentName, string targetDir, boolean isNonInteractive = false);

std::shared_ptr<Promise<void>> createTEEProject(string projectName, string targetDir, string database, string aiModel, string embeddingModel = std::nullopt, boolean isNonInteractive = false);

std::shared_ptr<Promise<void>> createProject(string projectName, string targetDir, string database, string aiModel, string embeddingModel = std::nullopt, boolean isNonInteractive = false);

template <typename T>
std::shared_ptr<Promise<T>> withCleanupOnInterrupt(string targetDir, string displayName, std::function<std::shared_ptr<Promise<T>>()> fn)
{
    shared directoryExistedBefore = existsSync(targetDir);
    auto cleanup = [=]() mutable
    {
        if (AND((!directoryExistedBefore), (existsSync(targetDir)))) {
            console->info(colors->red(std::string("\
\
Interrupted! Cleaning up ") + displayName + std::string("...")));
            try
            {
                rmSync(targetDir, object{
                    object::pair{std::string("recursive"), true}, 
                    object::pair{std::string("force"), true}
                });
                console->info(std::string("Cleanup completed."));
            }
            catch (const any& error)
            {
                console->error(colors->red(std::string("Error during cleanup:")), error);
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
    process->on(std::string("exit"), cleanup);
    process->on(std::string("SIGINT"), sigintHandler);
    process->on(std::string("SIGTERM"), sigtermHandler);
    try
    {
        auto result = std::async([=]() { fn(); });
        process->removeListener(std::string("exit"), cleanup);
        process->removeListener(std::string("SIGINT"), sigintHandler);
        process->removeListener(std::string("SIGTERM"), sigtermHandler);
        return result;
    }
    catch (const any& error)
    {
        process->removeListener(std::string("exit"), cleanup);
        process->removeListener(std::string("SIGINT"), sigintHandler);
        process->removeListener(std::string("SIGTERM"), sigtermHandler);
        if (AND((!directoryExistedBefore), (existsSync(targetDir)))) {
            try
            {
                console->info(colors->red(std::string("\
Cleaning up due to error...")));
                rmSync(targetDir, object{
                    object::pair{std::string("recursive"), true}, 
                    object::pair{std::string("force"), true}
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
