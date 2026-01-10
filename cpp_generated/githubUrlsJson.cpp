#include "/home/runner/work/elizaos-cpp/elizaos-cpp/plugins-automation/src/githubUrlsJson.h"

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
    if (TEST_MODE) {
        console->log(std::string("🧪 TEST MODE: Only processing repository '") + TEST_REPO + std::string("'"));
    }
    auto repos = std::async([=]() { octokit->paginate(octokit->repos->listForOrg, object{
        object::pair{std::string("org"), ORG_NAME}, 
        object::pair{std::string("per_page"), 100}
    }); });
    auto& __array771_4729 = repos;
    for (auto __indx771_4729 = 0_N; __indx771_4729 < __array771_4729->get_length(); __indx771_4729++)
    {
        auto& repo = const_(__array771_4729)[__indx771_4729];
        {
            if (AND((TEST_MODE), (repo->name != TEST_REPO))) {
                continue;
            }
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
            auto expectedRepositoryUrl = std::string("https://github.com/") + ORG_NAME + std::string("/") + repo->name + std::string(".git");
            auto needsRepositoryUpdate = false;
            if (!pkg["repository"]) {
                needsRepositoryUpdate = true;
            } else if (type_of(pkg["repository"]) == std::string("string")) {
                needsRepositoryUpdate = true;
            } else if (type_of(pkg["repository"]) == std::string("object")) {
                if (OR((OR((!pkg["repository"]["url"]), (pkg["repository"]["url"] == string_empty))), (pkg["repository"]["url"] != expectedRepositoryUrl))) {
                    needsRepositoryUpdate = true;
                }
            }
            if (!needsRepositoryUpdate) {
                console->log(std::string("Skipping ") + ORG_NAME + std::string("/") + repo->name + std::string(" (repository field is already correct)"));
                continue;
            }
            pkg["repository"] = object{
                object::pair{std::string("type"), std::string("git")}, 
                object::pair{std::string("url"), expectedRepositoryUrl}
            };
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
                object::pair{std::string("message"), std::string("chore: update repository URL and bump version in package.json")}, 
                object::pair{std::string("content"), updated}, 
                object::pair{std::string("sha"), fileData["sha"]}
            }); });
            console->log(std::string("Updated repository URL and bumped version in package.json for ") + ORG_NAME + std::string("/") + repo->name + std::string(" on ") + TARGET_BRANCH + std::string(" branch"));
        }
    }
    return std::shared_ptr<Promise<void>>();
};


string ORG_NAME = std::string("elizaos-plugins");
string TARGET_BRANCH = std::string("1.x");
boolean TEST_MODE = false;
string TEST_REPO = std::string("plugin-knowledge");

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
