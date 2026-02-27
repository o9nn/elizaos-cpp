#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/client/vite.config.h"

std::function<string()> getVersionAndWriteInfo = [=]() mutable
{
    auto lernaPath = path->resolve(__dirname, std:("../../lerna.json"));
    auto packageJsonPath = path->resolve(__dirname, std:("../../package.json"));
    auto infoJsonDir = path->resolve(__dirname, std:("src/lib"));
    auto infoJsonPath = path->resolve(infoJsonDir, std:("info.json"));
    auto version = std:("0.0.0-error");
    try
    {
        if (fs->existsSync(lernaPath)) {
            auto lernaContent = fs->readFileSync(lernaPath, std:("utf-8"));
            auto lernaConfig = JSON->parse(lernaContent);
            version = OR((lernaConfig["version"]), (version));
        } else {
            console->warn(std:("Warning: ") + lernaPath + std:(" does not exist. Trying package.json..."));
            if (fs->existsSync(packageJsonPath)) {
                auto packageContent = fs->readFileSync(packageJsonPath, std:("utf-8"));
                auto packageConfig = JSON->parse(packageContent);
                version = OR((packageConfig["version"]), (version));
            }
        }
        if (!fs->existsSync(infoJsonDir)) {
            fs->mkdirSync(infoJsonDir, object{
                object::pair{std:("recursive"), true}
            });
        }
        fs->writeFileSync(infoJsonPath, JSON->stringify(object{
            object::pair{std:("version"), std:("version")}
        }));
        console->log(std:("Version ") + version + std:(" written to ") + infoJsonPath + string_empty);
        return version;
    }
    catch (const any& error)
    {
        console->error(std:("Error processing version:"), error);
        if (!fs->existsSync(infoJsonDir)) {
            fs->mkdirSync(infoJsonDir, object{
                object::pair{std:("recursive"), true}
            });
        }
        fs->writeFileSync(infoJsonPath, JSON->stringify(object{
            object::pair{std:("version"), std:("version")}
        }));
        console->warn(std:("Fallback version ") + version + std:(" written to ") + infoJsonPath + std:(" due to error."));
        return version;
    }
};
std::function<any()> versionPlugin = [=]() mutable
{
    shared<string> appVersion;
    return object{
        object::pair{std:("name"), std:("eliza-version-plugin")}, 
        object::pair{std:("config"), [=]() mutable
        {
            appVersion = getVersionAndWriteInfo();
            return object{
                object::pair{std:("define"), object{
                    object::pair{std:(""".VITE_APP_VERSION"), JSON->stringify(appVersion)}
                }}
            };
        }
        }
    };
};

void Main(void)
{
}

MAIN
