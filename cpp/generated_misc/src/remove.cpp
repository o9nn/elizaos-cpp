#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/commands/plugins/actions/remove.h"

std::shared_ptr<Promise<void>> removePlugin(string plugin)
{
    auto cwd = process->cwd();
    auto directoryInfo = detectDirectoryType(cwd);
    if (OR((!directoryInfo), (!directoryInfo->hasPackageJson))) {
        console->error(std::string("Could not read or parse package.json. This directory is: ") + (OR((directoryInfo->type), (std::string("invalid or inaccessible")))) + string_empty);
        process->exit(1);
    }
    auto allDependencies = getDependenciesFromDirectory(cwd);
    if (!allDependencies) {
        console->error(std::string("Could not read dependencies from package.json. Cannot determine which package to remove."));
        process->exit(1);
    }
    auto packageNameToRemove = findPluginPackageName(plugin, allDependencies);
    if (!packageNameToRemove) {
        logger->warn(std::string("Plugin matching "") + plugin + std::string("" not found in project dependencies."));
        console->info(std::string("\
Check installed plugins using: elizaos plugins installed-plugins"));
        process->exit(0);
    }
    console->info(std::string("Removing ") + packageNameToRemove + std::string("..."));
    try
    {
        std::async([=]() { execa(std::string("bun"), array<string>{ std::string("remove"), packageNameToRemove }, object{
            object::pair{std::string("cwd"), std::string("cwd")}, 
            object::pair{std::string("stdio"), std::string("inherit")}
        }); });
    }
    catch (const any& execError)
    {
        logger->error(std::string("Failed to run 'bun remove ") + packageNameToRemove + std::string("': ") + (is<Error>(execError)) ? execError->message : String(execError) + string_empty);
        if (AND((AND((AND((AND((execError), (type_of(execError) == std::string("object")))), (in(std::string("stderr"), execError)))), (type_of(execError["stderr"]) == std::string("string")))), (execError["stderr"]["includes"](std::string("not found"))))) {
            logger->info(std::string("'bun remove' indicated package was not found. Continuing with directory removal attempt."));
        } else {
            handleError(execError);
            process->exit(1);
        }
    }
    auto baseName = packageNameToRemove;
    if (packageNameToRemove->includes(std::string("/"))) {
        auto parts = packageNameToRemove->split(std::string("/"));
        baseName = const_(parts)[parts->get_length() - 1];
    }
    baseName = baseName->replace((new RegExp(std::string("^plugin"))), string_empty);
    auto dirNameToRemove = std::string("plugin-") + baseName + string_empty;
    auto pluginDir = path->join(cwd, dirNameToRemove);
    if (existsSync(pluginDir)) {
        try
        {
            rmSync(pluginDir, object{
                object::pair{std::string("recursive"), true}, 
                object::pair{std::string("force"), true}
            });
        }
        catch (const any& rmError)
        {
            logger->error(std::string("Failed to remove directory ") + pluginDir + std::string(": ") + (is<Error>(rmError)) ? rmError->message : String(rmError) + string_empty);
        }
    } else {
        auto nonPrefixedDir = path->join(cwd, baseName);
        if (existsSync(nonPrefixedDir)) {
            try
            {
                rmSync(nonPrefixedDir, object{
                    object::pair{std::string("recursive"), true}, 
                    object::pair{std::string("force"), true}
                });
            }
            catch (const any& rmError)
            {
                logger->error(std::string("Failed to remove directory ") + nonPrefixedDir + std::string(": ") + (is<Error>(rmError)) ? rmError->message : String(rmError) + string_empty);
            }
        }
    }
    console->log(std::string("Successfully removed ") + packageNameToRemove + string_empty);
    return std::shared_ptr<Promise<void>>();
};


