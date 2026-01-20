#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/commands/plugins/utils/directory.h"

std::function<std::shared_ptr<Record<string, string>>(string)> getDependenciesFromDirectory = [=](auto cwd) mutable
{
    auto directoryInfo = detectDirectoryType(cwd);
    if (!directoryInfo->hasPackageJson) {
        return nullptr;
    }
    try
    {
        auto packageJsonPath = path->join(cwd, std::string("package.json"));
        auto packageJsonContent = readFileSync(packageJsonPath, std::string("utf-8"));
        auto packageJson = JSON->parse(packageJsonContent);
        auto dependencies = OR((packageJson["dependencies"]), (object{}));
        auto devDependencies = OR((packageJson["devDependencies"]), (object{}));
        return utils::assign(object{
        }, dependencies, devDependencies);
    }
    catch (const any& error)
    {
        if (is<SyntaxError>(error)) {
            logger->warn(std::string("Could not parse package.json: ") + error->message + string_empty);
        } else {
            logger->warn(std::string("Error reading package.json: ") + (is<Error>(error)) ? error->message : String(error) + string_empty);
        }
        return nullptr;
    }
};

void Main(void)
{
}

MAIN
