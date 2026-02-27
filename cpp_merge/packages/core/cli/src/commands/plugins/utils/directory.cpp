#include "directory.hpp"

std::function<std::shared_ptr<Record<string, string>>(string)> getDependenciesFromDirectory = [=](auto cwd) mutable
{
    auto directoryInfo = detectDirectoryType(cwd);
    if (!directoryInfo->hasPackageJson) {
        return nullptr;
    }
    try
    {
        auto packageJsonPath = path->join(cwd, std:("package.json"));
        auto packageJsonContent = readFileSync(packageJsonPath, std:("utf-8"));
        auto packageJson = JSON->parse(packageJsonContent);
        auto dependencies = OR((packageJson["dependencies"]), (object{}));
        auto devDependencies = OR((packageJson["devDependencies"]), (object{}));
        return utils::assign(object{
        }, dependencies, devDependencies);
    }
    catch (const any& error)
    {
        if (is<SyntaxError>(error)) {
            logger->warn(std:("Could not parse package.json: ") + error->message + string_empty);
        } else {
            logger->warn(std:("Error reading package.json: ") + (is<Error>(error)) ? error->message : String(error) + string_empty);
        }
        return nullptr;
    }
};

void Main(void)
{
}

MAIN
