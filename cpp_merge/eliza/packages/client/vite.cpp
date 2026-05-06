#include "vite.config.h"
#include <string>

std::function<string()> getVersionAndWriteInfo = [=]() mutable
{
    auto lernaPath = path->resolve(__dirname, std::string("../../lerna.json"));
    auto packageJsonPath = path->resolve(__dirname, std::string("../../package.json"));
    auto infoJsonDir = path->resolve(__dirname, std::string("src/lib"));
    auto infoJsonPath = path->resolve(infoJsonDir, std::string("info.json"));
    auto version = std::string("0.0.0-error");
    try
    {
        if (fs->existsSync(lernaPath)) {
            auto lernaContent = fs->readFileSync(lernaPath, std::string("utf-8"));
            auto lernaConfig = JSON->parse(lernaContent);
            version = OR((lernaConfig["version"]), (version));
        } else {
            console->warn(std::string("Warning: ") + lernaPath + std::string(" does not exist. Trying package.json..."));
            if (fs->existsSync(packageJsonPath)) {
                auto packageContent = fs->readFileSync(packageJsonPath, std::string("utf-8"));
                auto packageConfig = JSON->parse(packageContent);
                version = OR((packageConfig["version"]), (version));
            }
        }
        if (!fs->existsSync(infoJsonDir)) {
            fs->mkdirSync(infoJsonDir, object{
                object::pair{std::string("recursive"), true}
            });
        }
        fs->writeFileSync(infoJsonPath, JSON->stringify(object{
            object::pair{std::string("version"), std::string("version")}
        }));
        console->log(std::string("Version ") + version + std::string(" written to ") + infoJsonPath + string_empty);
        return version;
    }
    catch (const any& error)
    {
        console->error(std::string("Error processing version:"), error);
        if (!fs->existsSync(infoJsonDir)) {
            fs->mkdirSync(infoJsonDir, object{
                object::pair{std::string("recursive"), true}
            });
        }
        fs->writeFileSync(infoJsonPath, JSON->stringify(object{
            object::pair{std::string("version"), std::string("version")}
        }));
        console->warn(std::string("Fallback version ") + version + std::string(" written to ") + infoJsonPath + std::string(" due to error."));
        return version;
    }
};
std::function<any()> versionPlugin = [=]() mutable
{
    shared<string> appVersion;
    return object{
        object::pair{std::string("name"), std::string("eliza-version-plugin")}, 
        object::pair{std::string("config"), [=]() mutable
        {
            appVersion = getVersionAndWriteInfo();
            return object{
                object::pair{std::string("define"), object{
                    object::pair{std::string(""".VITE_APP_VERSION"), JSON->stringify(appVersion)}
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
