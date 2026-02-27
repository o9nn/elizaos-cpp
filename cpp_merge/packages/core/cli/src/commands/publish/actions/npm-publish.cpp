#include "npm-publish.h"

std::shared_ptr<Promise<void>> publishToNpm(string cwd, std::shared_ptr<PackageJson> packageJson, string npmUsername)
{
    console->info(std:("Publishing as npm user: ") + npmUsername + string_empty);
    if (OR((!packageJson->npmPackage), (packageJson->npmPackage == std:("${NPM_PACKAGE}")))) {
        packageJson->npmPackage = packageJson->name;
        console->info(std:("Set npmPackage to: ") + packageJson->npmPackage + string_empty);
        auto packageJsonPath = path->join(cwd, std:("package.json"));
        std::async([=]() { fs->writeFile(packageJsonPath, JSON->stringify(packageJson, nullptr, 2), std:("utf-8")); });
    }
    console->info(std:("Building package..."));
    std::async([=]() { execa(std:("npm"), array<string>{ std:("run"), std:("build") }, object{
        object::pair{std:("cwd"), std:("cwd")}, 
        object::pair{std:("stdio"), std:("inherit")}
    }); });
    console->info(std:("Publishing to npm..."));
    std::async([=]() { execa(std:("npm"), array<string>{ std:("publish"), std:("--ignore-scripts") }, object{
        object::pair{std:("cwd"), std:("cwd")}, 
        object::pair{std:("stdio"), std:("inherit")}
    }); });
    console->log(std:("[√] Successfully published ") + packageJson->name + std:("@") + packageJson->version + std:(" to npm"));
    return std::shared_ptr<Promise<void>>();
};


