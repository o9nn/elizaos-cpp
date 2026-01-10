#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/utils/run-bun.h"

std::shared_ptr<Promise<void>> runBunCommand(array<string> args, string cwd)
{
    auto finalArgs = array<string>{ args };
    auto isInstallCommand = const_(args)[0] == std::string("install");
    auto isCI = OR((process->env->CI), (process->env->ELIZA_TEST_MODE == std::string("true")));
    if (AND((isCI), (isInstallCommand))) {
        if (!finalArgs->includes(std::string("--frozen-lockfile"))) {
            finalArgs->push(std::string("--frozen-lockfile"));
        }
        console->info(std::string("✅ Using CI-optimized flags for faster installation..."));
    }
    try
    {
        std::async([=]() { execa(std::string("bun"), finalArgs, object{
            object::pair{std::string("cwd"), std::string("cwd")}, 
            object::pair{std::string("stdio"), std::string("inherit")}
        }); });
    }
    catch (const any& error)
    {
        if (OR((error["code"] == std::string("ENOENT")), (error["message"]["includes"](std::string("bun: command not found"))))) {
            throw any(std::make_shared<Error>(std::string("Bun command not found. ") + displayBunInstallationTipCompact() + string_empty));
        }
        if (AND((AND((isCI), (isInstallCommand))), ((OR((error["message"]["includes"](std::string("frozen-lockfile"))), (error["message"]["includes"](std::string("install")))))))) {
            console->warn(std::string("CI-optimized install failed, retrying with basic args..."));
            std::async([=]() { execa(std::string("bun"), args, object{
                object::pair{std::string("cwd"), std::string("cwd")}, 
                object::pair{std::string("stdio"), std::string("inherit")}
            }); });
        } else {
            throw any(error);
        }
    }
    return std::shared_ptr<Promise<void>>();
};


