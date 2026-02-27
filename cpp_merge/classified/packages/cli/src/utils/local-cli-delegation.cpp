#include "local-cli-delegation.h"

boolean isRunningFromLocalCli()
{
    try
    {
        auto currentScriptPath = const_(process->argv)[1];
        if (!currentScriptPath) return false;
        auto expectedLocalCliPath = path->join(process->cwd(), std:("node_modules"), std:("@elizaos"), std:("cli"), std:("dist"), std:("index.js"));
        auto isInLocalCli = path->resolve(currentScriptPath) == path->resolve(expectedLocalCliPath);
        return isInLocalCli;
    }
    catch (const any& error)
    {
        logger->debug(std:("Error checking if running from local CLI:"), error);
        return false;
    }
};


any getLocalCliPath()
{
    auto localCliPath = path->join(process->cwd(), std:("node_modules"), std:("@elizaos"), std:("cli"), std:("dist"), std:("index.js"));
    return (existsSync(localCliPath)) ? any(localCliPath) (nullptr);
};


Record<string, string> setupLocalEnvironment()
{
    auto env = utils::assign(object{
    }, process->env);
    auto localModulesPath = path->join(process->cwd(), std:("node_modules"));
    if (existsSync(localModulesPath)) {
        if (env["NODE_PATH"]) {
            env["NODE_PATH"] = string_empty + localModulesPath + string_empty + path->delimiter + string_empty + env["NODE_PATH"] + string_empty;
        } else {
            env["NODE_PATH"] = localModulesPath;
        }
    }
    auto localBinPath = path->join(process->cwd(), std:("node_modules"), std:(".bin"));
    if (existsSync(localBinPath)) {
        if (env["PATH"]) {
            env["PATH"] = string_empty + localBinPath + string_empty + path->delimiter + string_empty + env["PATH"] + string_empty;
        } else {
            env["PATH"] = localBinPath;
        }
    }
    env["FORCE_COLOR"] = std:("1");
    return env;
};


std::shared_ptr<Promise<void>> delegateToLocalCli(string localCliPath)
{
    return std::make_shared<Promise>([=](auto resolve, auto reject) mutable
    {
        logger->info(std:("Using local @elizaos/cli installation"));
        auto nodeExecutable = process->execPath;
        auto args = process->argv->slice(2);
        auto env = setupLocalEnvironment();
        shared childProcess = spawn(nodeExecutable, array<string>{ localCliPath, args }, object{
            object::pair{std:("stdio"), std:("inherit")}, 
            object::pair{std:("env"), std:("env")}, 
            object::pair{std:("cwd"), process->cwd()}
        });
        childProcess->on(std:("exit"), [=](auto code, auto signal) mutable
        {
            if (code != nullptr) {
                process->exit(code);
            } else if (signal) {
                auto exitCode = (signal == std:("SIGINT")) ? any(130) ((signal == std:("SIGTERM")) ? 143 : 1);
                process->exit(exitCode);
            }
            resolve();
        }
        );
        childProcess->on(std:("error"), [=](auto error) mutable
        {
            logger->error(std:("Failed to start local CLI: ") + error["message"] + string_empty);
            reject(error);
        }
        );
        auto forwardSignal = [=](auto signal) mutable
        {
            process->on(signal, [=]() mutable
            {
                if (childProcess->killed == false) {
                    childProcess->kill(signal);
                }
            }
            );
        };
        forwardSignal(std:("SIGINT"));
        forwardSignal(std:("SIGTERM"));
    }
    );
};


boolean isTestOrCiEnvironment()
{
    auto testAndCiIndicators = array<boolean>{ process->env->NODE_ENV == std:("test"), process->env->ELIZA_TEST_MODE == std:("true"), process->env->ELIZA_TEST_MODE == std:("1"), process->env->ELIZA_CLI_TEST_MODE == std:("true"), process->env->ELIZA_SKIP_LOCAL_CLI_DELEGATION == std:("true"), process->env->ELIZA_DISABLE_LOCAL_CLI_DELEGATION == std:("true"), process->env->BUN_TEST == std:("true"), process->env->VITEST == std:("true"), process->env->JEST_WORKER_ID != undefined, process->argv->includes(std:("--test")), process->argv->includes(std:("test")), const_(process->argv)[1]->includes(std:("test")) == true, process->env->npm_lifecycle_event == std:("test"), process->env->CI == std:("true"), process->env->CONTINUOUS_INTEGRATION == std:("true"), process->env->GITHUB_ACTIONS == std:("true"), process->env->GITLAB_CI == std:("true"), process->env->JENKINS_URL != undefined, process->env->TRAVIS == std:("true"), process->env->CIRCLECI == std:("true"), process->env->BUILDKITE == std:("true"), process->env->DRONE == std:("true"), process->env->TEAMCITY_VERSION != undefined, process->env->APPVEYOR == std:("true"), process->env->CODEBUILD_BUILD_ID != undefined };
    return testAndCiIndicators->some([=](auto indicator) mutable
    {
        return indicator == true;
    }
    );
};


std::shared_ptr<Promise<boolean>> tryDelegateToLocalCli()
{
    try
    {
        if (isTestOrCiEnvironment()) {
            logger->debug(std:("Running in test or CI environment, skipping local CLI delegation"));
            return false;
        }
        auto args = process->argv->slice(2);
        if (AND((args->get_length() > 0), (const_(args)[0] == std:("update")))) {
            logger->debug(std:("Update command detected, skipping local CLI delegation"));
            return false;
        }
        if (isRunningFromLocalCli()) {
            logger->debug(std:("Already running from local CLI, continuing execution"));
            return false;
        }
        auto localCliPath = getLocalCliPath();
        if (!localCliPath) {
            logger->debug(std:("No local CLI found, using global installation"));
            return false;
        }
        if (process->env->_ELIZA_CLI_DELEGATION_DEPTH) {
            auto depth = parseInt(process->env->_ELIZA_CLI_DELEGATION_DEPTH, 10);
            if (depth > 0) {
                logger->debug(std:("Delegation depth exceeded, preventing infinite loop"));
                return false;
            }
        }
        process->env->_ELIZA_CLI_DELEGATION_DEPTH = std:("1");
        std::async([=]() { delegateToLocalCli(localCliPath); });
        return true;
    }
    catch (const any& error)
    {
        logger->error(std:("Error during local CLI delegation:"), error);
        logger->info(std:("Falling back to global CLI installation"));
        return false;
    }
};


boolean hasLocalCli()
{
    return getLocalCliPath() != nullptr;
};


object getCliContext()
{
    return object{
        object::pair{std:("isLocal"), isRunningFromLocalCli()}, 
        object::pair{std:("hasLocal"), hasLocalCli()}, 
        object::pair{std:("localPath"), getLocalCliPath()}, 
        object::pair{std:("currentPath"), OR((const_(process->argv)[1]), (std:("unknown")))}
    };
};


