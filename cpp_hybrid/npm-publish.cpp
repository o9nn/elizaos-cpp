#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/commands/publish/actions/npm-publish.h"

std::shared_ptr<Promise<void>> publishToNpm(string cwd, std::shared_ptr<PackageJson> packageJson, string npmUsername)
{
    console->info(std::string("Publishing as npm user: ") + npmUsername + string_empty);
    if (OR((!packageJson->npmPackage), (packageJson->npmPackage == std::string("${NPM_PACKAGE}")))) {
        packageJson->npmPackage = packageJson->name;
        console->info(std::string("Set npmPackage to: ") + packageJson->npmPackage + string_empty);
        auto packageJsonPath = path->join(cwd, std::string("package.json"));
        std::async([=]() { fs->writeFile(packageJsonPath, JSON->stringify(packageJson, nullptr, 2), std::string("utf-8")); });
    }
    console->info(std::string("Building package..."));
    std::async([=]() { execa(std::string("npm"), array<string>{ std::string("run"), std::string("build") }, object{
        object::pair{std::string("cwd"), std::string("cwd")}, 
        object::pair{std::string("stdio"), std::string("inherit")}
    }); });
    console->info(std::string("Publishing to npm..."));
    std::async([=]() { execa(std::string("npm"), array<string>{ std::string("publish"), std::string("--ignore-scripts") }, object{
        object::pair{std::string("cwd"), std::string("cwd")}, 
        object::pair{std::string("stdio"), std::string("inherit")}
    }); });
    console->log(std::string("[√] Successfully published ") + packageJson->name + std::string("@") + packageJson->version + std::string(" to npm"));
    return std::shared_ptr<Promise<void>>();
};


