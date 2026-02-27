#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/utils/github.h"

any fetchGitHubData(string repo)
{
    auto [owner, repoName] = repo->split(std::string("/"))->slice(-2);
    auto response = std::async([=]() { fetch(std::string("https://api.github.com/repos/") + owner + std::string("/") + repoName + string_empty, object{
        object::pair{std::string("headers"), object{
            object::pair{std::string("Authorization"), std::string("token ") + process->env->GITHUB_TOKEN + string_empty}
        }}
    }); });
    return response->json();
};


