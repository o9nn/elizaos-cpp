#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/src/commands/plugins/actions/remove.h"

std::shared_ptr<Promise<void>> removePlugin(string plugin)
{
    auto cwd = process->cwd();
    auto directoryInfo = detectDirectoryType(cwd);
    if (OR((!directoryInfo), (!directoryInfo->hasPackageJson))) {
        console->error(std:("Could not read or parse package.json. This directory is: ") + (OR((directoryInfo->type), (std:("invalid or inaccessible")))) + string_empty);
        process->exit(1);
    }
    auto allDependencies = getDependenciesFromDirectory(cwd);
    if (!allDependencies) {
        console->error(std:("Could not read dependencies from package.json. Cannot determine which package to remove."));
        process->exit(1);
    }
    auto packageNameToRemove = findPluginPackageName(plugin, allDependencies);
    if (!packageNameToRemove) {
        logger->warn(std:("Plugin matching "") + plugin + std:("" not found in project dependencies."));
        console->info(std:("\
Check installed plugins using: elizaos plugins installed-plugins"));
        process->exit(0);
    }
    console->info(std:("Removing ") + packageNameToRemove + std:("..."));
    try
    {
        std::async([=]() { bunExecInherit(std:("bun"), array<string>{ std:("remove"), packageNameToRemove }, object{
            object::pair{std:("cwd"), std:("cwd")}
        }); });
    }
    catch (const any& execError)
    {
        logger->error(std:("Failed to run 'bun remove ") + packageNameToRemove + std:("': ") + (is<Error>(execError)) ? execError->message : String(execError) + string_empty);
        if (AND((AND((AND((AND((execError), (type_of(execError) == std:("object")))), (in(std:("stderr"), execError)))), (type_of(execError["stderr"]) == std:("string")))), (execError["stderr"]["includes"](std:("not found"))))) {
            logger->info(std:("'bun remove' indicated package was not found. Continuing with directory removal attempt."));
        } else {
            handleError(execError);
            process->exit(1);
        }
    }
    auto baseName = packageNameToRemove;
    if (packageNameToRemove->includes(std:("/"))) {
        auto parts = packageNameToRemove->split(std:("/"));
        baseName = const_(parts)[parts->get_length() - 1];
    }
    baseName = baseName->replace((new RegExp(std:("^plugin"))), string_empty);
    auto dirNameToRemove = std:("plugin-") + baseName + string_empty;
    auto pluginDir = path->join(cwd, dirNameToRemove);
    if (existsSync(pluginDir)) {
        try
        {
            rmSync(pluginDir, object{
                object::pair{std:("recursive"), true}, 
                object::pair{std:("force"), true}
            });
        }
        catch (const any& rmError)
        {
            logger->error(std:("Failed to remove directory ") + pluginDir + std:(": ") + (is<Error>(rmError)) ? rmError->message : String(rmError) + string_empty);
        }
    } else {
        auto nonPrefixedDir = path->join(cwd, baseName);
        if (existsSync(nonPrefixedDir)) {
            try
            {
                rmSync(nonPrefixedDir, object{
                    object::pair{std:("recursive"), true}, 
                    object::pair{std:("force"), true}
                });
            }
            catch (const any& rmError)
            {
                logger->error(std:("Failed to remove directory ") + nonPrefixedDir + std:(": ") + (is<Error>(rmError)) ? rmError->message : String(rmError) + string_empty);
            }
        }
    }
    console->log(std:("Successfully removed ") + packageNameToRemove + string_empty);
    return std::shared_ptr<Promise<void>>();
};


