#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/utils/github.h"

any fetchGitHubData(string repo)
{
    auto [owner, repoName] = repo->split(std:("/"))->slice(-2);
    auto response = std::async([=]() { fetch(std:("https://api.github.com/repos/") + owner + std:("/") + repoName + string_empty, object{
        object::pair{std:("headers"), object{
            object::pair{std:("Authorization"), std:("token ") + process->env->GITHUB_TOKEN + string_empty}
        }}
    }); });
    return response->json();
};


