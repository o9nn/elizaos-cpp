#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/src/utils/run-bun.h"

std::shared_ptr<Promise<void>> runBunCommand(array<string> args, string cwd, boolean silent)
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
        auto result = (silent) ? std::async([=]() { bunExec(std:("bun"), finalArgs, object{
            object::pair{std:("cwd"), std:("cwd")}
        }); }) : std::async([=]() { bunExecInherit(std:("bun"), finalArgs, object{
            object::pair{std:("cwd"), std:("cwd")}
        }); });
        if (AND((silent), (!result->success))) {
            throw any(std::make_shared<Error>(std:("Bun command failed with exit code ") + result->exitCode + std:(": ") + (OR((result->stderr), (result->stdout))) + string_empty));
        }
    }
    catch (const any& error)
    {
        if (OR((error["code"] == std:("ENOENT")), (error["message"]["includes"](std:("bun: command not found"))))) {
            throw any(std::make_shared<Error>(std:("Bun command not found. ") + displayBunInstallationTipCompact() + string_empty));
        }
        if (AND((AND((isCI), (isInstallCommand))), ((OR((error["message"]["includes"](std:("frozen-lockfile"))), (error["message"]["includes"](std:("install")))))))) {
            console->warn(std:("CI-optimized install failed, retrying with basic args..."));
            auto retryResult = (silent) ? std::async([=]() { bunExec(std:("bun"), args, object{
                object::pair{std:("cwd"), std:("cwd")}
            }); }) : std::async([=]() { bunExecInherit(std:("bun"), args, object{
                object::pair{std:("cwd"), std:("cwd")}
            }); });
            if (AND((silent), (!retryResult->success))) {
                throw any(std::make_shared<Error>(std:("Bun command failed with exit code ") + retryResult->exitCode + std:(": ") + (OR((retryResult->stderr), (retryResult->stdout))) + string_empty));
            }
        } else {
            throw any(error);
        }
    }
    return std::shared_ptr<Promise<void>>();
};


