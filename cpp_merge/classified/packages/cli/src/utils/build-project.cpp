#include "build-project.h"

void buildProject(string cwd, boolean isPlugin)
{
    if (process->env->ELIZA_TEST_MODE) {
        return std::shared_ptr<Promise<void>>();
    }
    if (!fs->existsSync(cwd)) {
        throw any(std::make_shared<Error>(std:("Project directory ") + cwd + std:(" does not exist.")));
    }
    auto dirInfo = detectDirectoryType(cwd);
    if (!dirInfo->hasPackageJson) {
        throw any(std::make_shared<Error>(std:("Project directory ") + cwd + std:(" does not have package.json.")));
    }
    auto packageJsonPath = path->join(cwd, std:("package.json"));
    auto distPath = path->join(cwd, std:("dist"));
    if (fs->existsSync(distPath)) {
        std::async([=]() { fs->promises->rm(distPath, object{
            object::pair{std:("recursive"), true}, 
            object::pair{std:("force"), true}
        }); });
    }
    auto projectType = (isPlugin) ? std:("plugin") : std:("project");
    try
    {
        auto packageJson = JSON->parse(fs->readFileSync(packageJsonPath, std:("utf8")));
        if (packageJson["scripts"]["build"]) {
            auto result = std::async([=]() { runBunWithSpinner(array<string>{ std:("run"), std:("build") }, cwd, object{
                object::pair{std:("spinnerText"), std:("Building ") + projectType + std:("...")}, 
                object::pair{std:("successText"), colors->green(std:("✓ ") + (projectType->charAt(0)->toUpperCase() + projectType->slice(1)) + std:(" built successfully"))}, 
                object::pair{std:("errorText"), std:("Failed to build ") + projectType + string_empty}
            }); });
            if (!result->success) {
                throw any(OR((result->error), (std::make_shared<Error>(std:("Failed to build using bun")))));
            }
            return std::shared_ptr<Promise<void>>();
        }
        auto tsconfigPath = path->join(cwd, std:("tsconfig.json"));
        if (fs->existsSync(tsconfigPath)) {
            try
            {
                auto result = std::async([=]() { bunExec(std:("bunx"), array<string>{ std:("tsc"), std:("--build") }, object{
                    object::pair{std:("cwd"), std:("cwd")}
                }); });
                if (result->success) {
                    return std::shared_ptr<Promise<void>>();
                } else {
                    throw any(std::make_shared<Error>(std:("bunx tsc build failed: ") + (OR((result->stderr), (result->stdout))) + string_empty));
                }
            }
            catch (const any& tscError)
            {
                throw any(std::make_shared<Error>(std:("bunx tsc build failed: ") + tscError + string_empty));
            }
        }
        throw any(std::make_shared<Error>(std:("Could not determine how to build the project")));
    }
    catch (const any& error)
    {
        throw any(error);
    }
};


