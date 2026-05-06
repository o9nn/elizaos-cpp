#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/src/utils/build-project.h"

void buildProject(string cwd, boolean isPlugin)
{
    if (process->env->ELIZA_TEST_MODE) {
        return std::shared_ptr<Promise<void>>();
    }
    if (!fs->existsSync(cwd)) {
        throw any(std::make_shared<Error>(std::string("Project directory ") + cwd + std::string(" does not exist.")));
    }
    auto dirInfo = detectDirectoryType(cwd);
    if (!dirInfo->hasPackageJson) {
        throw any(std::make_shared<Error>(std::string("Project directory ") + cwd + std::string(" does not have package.json.")));
    }
    auto packageJsonPath = path->join(cwd, std::string("package.json"));
    auto distPath = path->join(cwd, std::string("dist"));
    if (fs->existsSync(distPath)) {
        std::async([=]() { fs->promises->rm(distPath, object{
            object::pair{std::string("recursive"), true}, 
            object::pair{std::string("force"), true}
        }); });
    }
    auto projectType = (isPlugin) ? std::string("plugin") : std::string("project");
    try
    {
        auto packageJson = JSON->parse(fs->readFileSync(packageJsonPath, std::string("utf8")));
        if (packageJson["scripts"]["build"]) {
            auto result = std::async([=]() { runBunWithSpinner(array<string>{ std::string("run"), std::string("build") }, cwd, object{
                object::pair{std::string("spinnerText"), std::string("Building ") + projectType + std::string("...")}, 
                object::pair{std::string("successText"), colors->green(std::string("✓ ") + (projectType->charAt(0)->toUpperCase() + projectType->slice(1)) + std::string(" built successfully"))}, 
                object::pair{std::string("errorText"), std::string("Failed to build ") + projectType + string_empty}
            }); });
            if (!result->success) {
                throw any(OR((result->error), (std::make_shared<Error>(std::string("Failed to build using bun")))));
            }
            return std::shared_ptr<Promise<void>>();
        }
        auto tsconfigPath = path->join(cwd, std::string("tsconfig.json"));
        if (fs->existsSync(tsconfigPath)) {
            try
            {
                auto result = std::async([=]() { bunExec(std::string("bunx"), array<string>{ std::string("tsc"), std::string("--build") }, object{
                    object::pair{std::string("cwd"), std::string("cwd")}
                }); });
                if (result->success) {
                    return std::shared_ptr<Promise<void>>();
                } else {
                    throw any(std::make_shared<Error>(std::string("bunx tsc build failed: ") + (OR((result->stderr), (result->stdout))) + string_empty));
                }
            }
            catch (const any& tscError)
            {
                throw any(std::make_shared<Error>(std::string("bunx tsc build failed: ") + tscError + string_empty));
            }
        }
        throw any(std::make_shared<Error>(std::string("Could not determine how to build the project")));
    }
    catch (const any& error)
    {
        throw any(error);
    }
};


