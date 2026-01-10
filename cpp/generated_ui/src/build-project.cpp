#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/utils/build-project.h"

void buildProject(string cwd, boolean isPlugin)
{
    if (process->env->ELIZA_TEST_MODE) {
        console->info(std::string("Skipping build in test mode"));
        return std::shared_ptr<Promise<void>>();
    }
    logger->info(std::string("Building ") + (isPlugin) ? std::string("plugin") : std::string("project") + std::string(" in ") + cwd + std::string("..."));
    if (!fs->existsSync(cwd)) {
        throw any(std::make_shared<Error>(std::string("Project directory ") + cwd + std::string(" does not exist.")));
    }
    auto dirInfo = detectDirectoryType(cwd);
    if (!dirInfo->hasPackageJson) {
        logger->warn(std::string("package.json not found in ") + cwd + std::string(". Cannot determine build method."));
        throw any(std::make_shared<Error>(std::string("Project directory ") + cwd + std::string(" does not have package.json.")));
    }
    auto packageJsonPath = path->join(cwd, std::string("package.json"));
    auto distPath = path->join(cwd, std::string("dist"));
    if (fs->existsSync(distPath)) {
        std::async([=]() { fs->promises->rm(distPath, object{
            object::pair{std::string("recursive"), true}, 
            object::pair{std::string("force"), true}
        }); });
        logger->debug(std::string("Cleaned previous build artifacts from ") + distPath + string_empty);
    }
    auto directoryInfo = detectDirectoryType(cwd);
    if (directoryInfo->monorepoRoot) {
        logger->debug(std::string("Detected monorepo structure, skipping install"));
    }
    try
    {
        auto packageJson = JSON->parse(fs->readFileSync(packageJsonPath, std::string("utf8")));
        if (packageJson["scripts"]["build"]) {
            logger->debug(std::string("Using build script from package.json with bun"));
            try
            {
                logger->debug(std::string("Building with bun..."));
                std::async([=]() { runBunCommand(array<string>{ std::string("run"), std::string("build") }, cwd); });
                logger->info(std::string("Build completed successfully"));
                return std::shared_ptr<Promise<void>>();
            }
            catch (const any& buildError)
            {
                logger->debug(std::string("Bun build failed: ") + buildError + string_empty);
                throw any(std::make_shared<Error>(std::string("Failed to build using bun: ") + buildError + string_empty));
            }
        }
        logger->warn(std::string("No build script found in ") + packageJsonPath + std::string(". Attempting common build commands."));
        auto tsconfigPath = path->join(cwd, std::string("tsconfig.json"));
        if (fs->existsSync(tsconfigPath)) {
            try
            {
                logger->debug(std::string("Found tsconfig.json, attempting to build with bunx tsc..."));
                std::async([=]() { execa(std::string("bunx"), array<string>{ std::string("tsc"), std::string("--build") }, object{
                    object::pair{std::string("cwd"), std::string("cwd")}, 
                    object::pair{std::string("stdio"), std::string("inherit")}
                }); });
                logger->info(std::string("Build completed successfully"));
                return std::shared_ptr<Promise<void>>();
            }
            catch (const any& tscError)
            {
                logger->debug(std::string("bunx tsc build failed: ") + tscError + string_empty);
            }
        }
        throw any(std::make_shared<Error>(std::string("Could not determine how to build the project")));
    }
    catch (const any& error)
    {
        logger->error(std::string("Failed to build ") + (isPlugin) ? std::string("plugin") : std::string("project") + std::string(": ") + error + string_empty);
        throw any(error);
    }
};


