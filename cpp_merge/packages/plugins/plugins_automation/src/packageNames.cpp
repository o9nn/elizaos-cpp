#include "packageNames.hpp"

std::shared_ptr<Promise<void>> main()
{
    auto token = process->env->GITHUB_TOKEN;
    if (!token) {
        console->error(std:("Error: GITHUB_TOKEN environment variable is not set"));
        process->exit(1);
    }
    auto octokit = std::make_shared<Octokit>(object{
        object::pair{std:("auth"), token}
    });
    auto repos = std::async([=]() { octokit->paginate(octokit->repos->listForOrg, object{
        object::pair{std:("org"), ORG_NAME}, 
        object::pair{std:("per_page"), 100}
    }); });
    auto& __array542_4459 = repos;
    for (auto __indx542_4459 = 0_N; __indx542_4459 < __array542_4459->get_length(); __indx542_4459++)
    {
        auto& repo = const_(__array542_4459)[__indx542_4459];
        {
            any fileData;
            try
            {
                std::async([=]() { octokit->repos->getBranch(object{
                    object::pair{std:("owner"), ORG_NAME}, 
                    object::pair{std:("repo"), repo->name}, 
                    object::pair{std:("branch"), TARGET_BRANCH}
                }); });
            }
            catch (const any& error)
            {
                if (error["status"] == 404) {
                    console->log(std:("Skipping ") + ORG_NAME + std:("/") + repo->name + std:(" (no ") + TARGET_BRANCH + std:(" branch)"));
                    continue;
                }
                throw any(error);
            }
            try
            {
                auto response = std::async([=]() { octokit->repos->getContent(object{
                    object::pair{std:("owner"), ORG_NAME}, 
                    object::pair{std:("repo"), repo->name}, 
                    object::pair{std:("path"), std:("package.json")}, 
                    object::pair{std:("ref"), TARGET_BRANCH}
                }); });
                if (OR((Array->isArray(response->data)), (!(in(std:("content"), response->data))))) {
                    continue;
                }
                fileData = response->data;
            }
            catch (const any& error)
            {
                if (error["status"] == 404) {
                    console->log(std:("Skipping ") + ORG_NAME + std:("/") + repo->name + std:(" (no package.json on ") + TARGET_BRANCH + std:(")"));
                    continue;
                }
                throw any(error);
            }
            auto raw = Buffer::from(fileData["content"], std:("base64"))->toString(std:("utf8"));
            auto pkg = as<object>(JSON->parse(raw));
            auto oldName = pkg["name"];
            if (AND((oldName), (oldName->startsWith(std:("@elizaos/"))))) {
                console->log(std:("Skipping ") + ORG_NAME + std:("/") + repo->name + std:(" (package name already correct: ") + oldName + std:(")"));
                continue;
            }
            if (AND((oldName), (!oldName->startsWith(std:("@elizaos-plugins/"))))) {
                console->log(std:("Skipping ") + ORG_NAME + std:("/") + repo->name + std:(" (package name doesn't match expected pattern: ") + oldName + std:(")"));
                continue;
            }
            string newName;
            if (!oldName) {
                newName = std:("@elizaos/") + repo->name + string_empty;
                console->log(std:("Adding missing package name for ") + ORG_NAME + std:("/") + repo->name + std:(": ") + newName + string_empty);
            } else {
                newName = oldName->replace((new RegExp(std:("^@elizaos-plugins\"))), std:("@elizaos/"));
                console->log(std:("Renaming ") + oldName + std:(" to ") + newName + std:(" for ") + ORG_NAME + std:("/") + repo->name + string_empty);
            }
            pkg["name"] = newName;
            if (pkg["version"]) {
                if (pkg["version"]->includes(std:("-beta."))) {
                    auto betaMatch = pkg["version"]->match((new RegExp(std:("^(.+)-beta\.(\d+)"))));
                    if (AND((AND((betaMatch), ((*const_(betaMatch))[1]))), ((*const_(betaMatch))[2]))) {
                        auto baseVersion = (*const_(betaMatch))[1];
                        auto betaNumber = parseInt((*const_(betaMatch))[2], 10);
                        if (!isNaN(betaNumber)) {
                            pkg["version"] = string_empty + baseVersion + std:("-beta.") + (betaNumber + 1) + string_empty;
                        }
                    }
                } else {
                    auto versionParts = pkg["version"]->split(std:("."));
                    if (AND((versionParts->get_length() >= 3), (const_(versionParts)[2]))) {
                        auto patchVersion = parseInt(const_(versionParts)[2], 10);
                        if (!isNaN(patchVersion)) {
                            versionParts[2] = (patchVersion + 1)->toString();
                            pkg["version"] = versionParts->join(std:("."));
                        }
                    }
                }
            }
            auto updated = Buffer::from(JSON->stringify(pkg, nullptr, 2) + std:("\
"), std:("utf8"))->toString(std:("base64"));
            std::async([=]() { octokit->repos->createOrUpdateFileContents(object{
                object::pair{std:("owner"), ORG_NAME}, 
                object::pair{std:("repo"), repo->name}, 
                object::pair{std:("path"), std:("package.json")}, 
                object::pair{std:("branch"), TARGET_BRANCH}, 
                object::pair{std:("message"), std:("chore: rename scope to @elizaos and bump version in package.json")}, 
                object::pair{std:("content"), updated}, 
                object::pair{std:("sha"), fileData["sha"]}
            }); });
            console->log(std:("Updated package.json in ") + ORG_NAME + std:("/") + repo->name + std:(" on ") + TARGET_BRANCH + std:(" branch - renamed ") + oldName + std:(" to ") + newName + std:(" and bumped version to ") + pkg["version"] + string_empty);
        }
    }
    return std::shared_ptr<Promise<void>>();
};


string ORG_NAME = std:("elizaos-plugins");
string TARGET_BRANCH = std:("1.x");

void Main(void)
{
    dotenv->config();
    main()->_catch([=](auto error) mutable
    {
        console->error(error);
        process->exit(1);
    }
    );
}

MAIN
