#include "/home/runner/work/elizaos-cpp/elizaos-cpp/plugins-automation/src/packageNames.h"

std::shared_ptr<Promise<void>> main()
{
    auto token = process->env->GITHUB_TOKEN;
    if (!token) {
        console->error(std::string("Error: GITHUB_TOKEN environment variable is not set"));
        process->exit(1);
    }
    auto octokit = std::make_shared<Octokit>(object{
        object::pair{std::string("auth"), token}
    });
    auto repos = std::async([=]() { octokit->paginate(octokit->repos->listForOrg, object{
        object::pair{std::string("org"), ORG_NAME}, 
        object::pair{std::string("per_page"), 100}
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
                    object::pair{std::string("owner"), ORG_NAME}, 
                    object::pair{std::string("repo"), repo->name}, 
                    object::pair{std::string("branch"), TARGET_BRANCH}
                }); });
            }
            catch (const any& error)
            {
                if (error["status"] == 404) {
                    console->log(std::string("Skipping ") + ORG_NAME + std::string("/") + repo->name + std::string(" (no ") + TARGET_BRANCH + std::string(" branch)"));
                    continue;
                }
                throw any(error);
            }
            try
            {
                auto response = std::async([=]() { octokit->repos->getContent(object{
                    object::pair{std::string("owner"), ORG_NAME}, 
                    object::pair{std::string("repo"), repo->name}, 
                    object::pair{std::string("path"), std::string("package.json")}, 
                    object::pair{std::string("ref"), TARGET_BRANCH}
                }); });
                if (OR((Array->isArray(response->data)), (!(in(std::string("content"), response->data))))) {
                    continue;
                }
                fileData = response->data;
            }
            catch (const any& error)
            {
                if (error["status"] == 404) {
                    console->log(std::string("Skipping ") + ORG_NAME + std::string("/") + repo->name + std::string(" (no package.json on ") + TARGET_BRANCH + std::string(")"));
                    continue;
                }
                throw any(error);
            }
            auto raw = Buffer::from(fileData["content"], std::string("base64"))->toString(std::string("utf8"));
            auto pkg = as<object>(JSON->parse(raw));
            auto oldName = pkg["name"];
            if (AND((oldName), (oldName->startsWith(std::string("@elizaos/"))))) {
                console->log(std::string("Skipping ") + ORG_NAME + std::string("/") + repo->name + std::string(" (package name already correct: ") + oldName + std::string(")"));
                continue;
            }
            if (AND((oldName), (!oldName->startsWith(std::string("@elizaos-plugins/"))))) {
                console->log(std::string("Skipping ") + ORG_NAME + std::string("/") + repo->name + std::string(" (package name doesn't match expected pattern: ") + oldName + std::string(")"));
                continue;
            }
            string newName;
            if (!oldName) {
                newName = std::string("@elizaos/") + repo->name + string_empty;
                console->log(std::string("Adding missing package name for ") + ORG_NAME + std::string("/") + repo->name + std::string(": ") + newName + string_empty);
            } else {
                newName = oldName->replace((new RegExp(std::string("^@elizaos-plugins\"))), std::string("@elizaos/"));
                console->log(std::string("Renaming ") + oldName + std::string(" to ") + newName + std::string(" for ") + ORG_NAME + std::string("/") + repo->name + string_empty);
            }
            pkg["name"] = newName;
            if (pkg["version"]) {
                if (pkg["version"]->includes(std::string("-beta."))) {
                    auto betaMatch = pkg["version"]->match((new RegExp(std::string("^(.+)-beta\.(\d+)"))));
                    if (AND((AND((betaMatch), ((*const_(betaMatch))[1]))), ((*const_(betaMatch))[2]))) {
                        auto baseVersion = (*const_(betaMatch))[1];
                        auto betaNumber = parseInt((*const_(betaMatch))[2], 10);
                        if (!isNaN(betaNumber)) {
                            pkg["version"] = string_empty + baseVersion + std::string("-beta.") + (betaNumber + 1) + string_empty;
                        }
                    }
                } else {
                    auto versionParts = pkg["version"]->split(std::string("."));
                    if (AND((versionParts->get_length() >= 3), (const_(versionParts)[2]))) {
                        auto patchVersion = parseInt(const_(versionParts)[2], 10);
                        if (!isNaN(patchVersion)) {
                            versionParts[2] = (patchVersion + 1)->toString();
                            pkg["version"] = versionParts->join(std::string("."));
                        }
                    }
                }
            }
            auto updated = Buffer::from(JSON->stringify(pkg, nullptr, 2) + std::string("\
"), std::string("utf8"))->toString(std::string("base64"));
            std::async([=]() { octokit->repos->createOrUpdateFileContents(object{
                object::pair{std::string("owner"), ORG_NAME}, 
                object::pair{std::string("repo"), repo->name}, 
                object::pair{std::string("path"), std::string("package.json")}, 
                object::pair{std::string("branch"), TARGET_BRANCH}, 
                object::pair{std::string("message"), std::string("chore: rename scope to @elizaos and bump version in package.json")}, 
                object::pair{std::string("content"), updated}, 
                object::pair{std::string("sha"), fileData["sha"]}
            }); });
            console->log(std::string("Updated package.json in ") + ORG_NAME + std::string("/") + repo->name + std::string(" on ") + TARGET_BRANCH + std::string(" branch - renamed ") + oldName + std::string(" to ") + newName + std::string(" and bumped version to ") + pkg["version"] + string_empty);
        }
    }
    return std::shared_ptr<Promise<void>>();
};


string ORG_NAME = std::string("elizaos-plugins");
string TARGET_BRANCH = std::string("1.x");

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
