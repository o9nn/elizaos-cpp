#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/utils/run-bun.h"

std::shared_ptr<Promise<void>> runBunCommand(array<string> args, string cwd)
{
    auto finalArgs = array<string>{ args };
    auto isInstallCommand = const_(args)[0] == std:("install");
    auto isCI = OR((process->env->CI), (process->env->ELIZA_TEST_MODE == std:("true")));
    if (AND((isCI), (isInstallCommand))) {
        if (!finalArgs->includes(std:("--frozen-lockfile"))) {
            finalArgs->push(std:("--frozen-lockfile"));
        }
        console->info(std:("✅ Using CI-optimized flags for faster installation..."));
    }
    try
    {
        std::async([=]() { execa(std:("bun"), finalArgs, object{
            object::pair{std:("cwd"), std:("cwd")}, 
            object::pair{std:("stdio"), std:("inherit")}
        }); });
    }
    catch (const any& error)
    {
        if (OR((error["code"] == std:("ENOENT")), (error["message"]["includes"](std:("bun: command not found"))))) {
            throw any(std::make_shared<Error>(std:("Bun command not found. ") + displayBunInstallationTipCompact() + string_empty));
        }
        if (AND((AND((isCI), (isInstallCommand))), ((OR((error["message"]["includes"](std:("frozen-lockfile"))), (error["message"]["includes"](std:("install")))))))) {
            console->warn(std:("CI-optimized install failed, retrying with basic args..."));
            std::async([=]() { execa(std:("bun"), args, object{
                object::pair{std:("cwd"), std:("cwd")}, 
                object::pair{std:("stdio"), std:("inherit")}
            }); });
        } else {
            throw any(error);
        }
    }
    return std::shared_ptr<Promise<void>>();
};


