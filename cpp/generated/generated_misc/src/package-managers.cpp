#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/lib/package-managers.h"

std::shared_ptr<Promise<std::shared_ptr<PackageInfo>>> PackageManagerClient::getNpmInfo(string packageName)
{
    auto response = std::async([=]() { axios->get(std::string("https://registry.npmjs.org/") + packageName + string_empty); });
    auto latest = const_(response->data)[std::string("dist-tags")]->latest;
    auto info = const_(response->data->versions)[latest];
    auto downloads = std::async([=]() { axios->get(std::string("https://api.npmjs.org/downloads/point/last-month/") + packageName + string_empty); });
    return object{
        object::pair{std::string("name"), packageName}, 
        object::pair{std::string("version"), latest}, 
        object::pair{std::string("description"), info->description}, 
        object::pair{std::string("downloads"), downloads->data->downloads}, 
        object::pair{std::string("repository"), info->repository->url}, 
        object::pair{std::string("dependencies"), Object->keys(OR((info->dependencies), (object{})))}
    };
}

std::shared_ptr<Promise<std::shared_ptr<PackageInfo>>> PackageManagerClient::getPyPiInfo(string packageName)
{
    auto response = std::async([=]() { axios->get(std::string("https://pypi.org/pypi/") + packageName + std::string("/json")); });
    auto info = response->data->info;
    return object{
        object::pair{std::string("name"), packageName}, 
        object::pair{std::string("version"), info->version}, 
        object::pair{std::string("description"), info->summary}, 
        object::pair{std::string("downloads"), 0}, 
        object::pair{std::string("repository"), info->project_urls->Repository}, 
        object::pair{std::string("dependencies"), array<any>()}
    };
}

