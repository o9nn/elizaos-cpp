#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/lib/algolia.h"

void indexProject(any project)
{
    std::async([=]() { index->saveObject(utils::assign(object{
        object::pair{std::string("objectID"), project["id"]}
    }, project)); });
};


void removeProject(string projectId)
{
    std::async([=]() { index->deleteObject(projectId); });
};


any client = algoliasearch(process->env->NEXT_PUBLIC_ALGOLIA_APP_ID, process->env->ALGOLIA_ADMIN_KEY);
any index = client->initIndex(std::string("projects"));

void Main(void)
{
}

MAIN
