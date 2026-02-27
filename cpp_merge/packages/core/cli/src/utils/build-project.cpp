#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/utils/build-project.h"

void buildProject(string cwd, boolean isPlugin)
{
    if (process->env->ELIZA_TEST_MODE) {
        console->info(std:("Skipping build in test mode"));
        return std::shared_ptr<Promise<void>>();
    }
    logger->info(std:("Building ") + (isPlugin) ? std:("plugin") : std:("project") + std:(" in ") + cwd + std:("..."));
    if (!fs->existsSync(cwd)) {
        throw any(std::make_shared<Error>(std:("Project directory ") + cwd + std:(" does not exist.")));
    }
    auto dirInfo = detectDirectoryType(cwd);
    if (!dirInfo->hasPackageJson) {
        logger->warn(std:("package.json not found in ") + cwd + std:(". Cannot determine build method."));
        throw any(std::make_shared<Error>(std:("Project directory ") + cwd + std:(" does not have package.json.")));
    }
    auto packageJsonPath = path->join(cwd, std:("package.json"));
    auto distPath = path->join(cwd, std:("dist"));
    if (fs->existsSync(distPath)) {
        std::async([=]() { fs->promises->rm(distPath, object{
            object::pair{std:("recursive"), true}, 
            object::pair{std:("force"), true}
        }); });
        logger->debug(std:("Cleaned previous build artifacts from ") + distPath + string_empty);
    }
    auto directoryInfo = detectDirectoryType(cwd);
    if (directoryInfo->monorepoRoot) {
        logger->debug(std:("Detected monorepo structure, skipping install"));
    }
    try
    {
        auto packageJson = JSON->parse(fs->readFileSync(packageJsonPath, std:("utf8")));
        if (packageJson["scripts"]["build"]) {
            logger->debug(std:("Using build script from package.json with bun"));
            try
            {
                logger->debug(std:("Building with bun..."));
                std::async([=]() { runBunCommand(array<string>{ std:("run"), std:("build") }, cwd); });
                logger->info(std:("Build completed successfully"));
                return std::shared_ptr<Promise<void>>();
            }
            catch (const any& buildError)
            {
                logger->debug(std:("Bun build failed: ") + buildError + string_empty);
                throw any(std::make_shared<Error>(std:("Failed to build using bun: ") + buildError + string_empty));
            }
        }
        logger->warn(std:("No build script found in ") + packageJsonPath + std:(". Attempting common build commands."));
        auto tsconfigPath = path->join(cwd, std:("tsconfig.json"));
        if (fs->existsSync(tsconfigPath)) {
            try
            {
                logger->debug(std:("Found tsconfig.json, attempting to build with bunx tsc..."));
                std::async([=]() { execa(std:("bunx"), array<string>{ std:("tsc"), std:("--build") }, object{
                    object::pair{std:("cwd"), std:("cwd")}, 
                    object::pair{std:("stdio"), std:("inherit")}
                }); });
                logger->info(std:("Build completed successfully"));
                return std::shared_ptr<Promise<void>>();
            }
            catch (const any& tscError)
            {
                logger->debug(std:("bunx tsc build failed: ") + tscError + string_empty);
            }
        }
        throw any(std::make_shared<Error>(std:("Could not determine how to build the project")));
    }
    catch (const any& error)
    {
        logger->error(std:("Failed to build ") + (isPlugin) ? std:("plugin") : std:("project") + std:(": ") + error + string_empty);
        throw any(error);
    }
};


