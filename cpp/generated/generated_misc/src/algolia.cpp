#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/lib/algolia.h"

void indexProject(std::any project)
{
    std::async([=]() { index->saveObject(utils::assign(object{
        object::pair{std::string("objectID"), project["id"]}
    }, project)); });
};


void removeProject(std::string projectId)
{
    std::async([=]() { index->deleteObject(projectId); });
};


std::any client = algoliasearch(process->env->NEXT_PUBLIC_ALGOLIA_APP_ID, process->env->ALGOLIA_ADMIN_KEY);
std::any index = client->initIndex(std::string("projects"));

void Main(void)
{
}

MAIN
