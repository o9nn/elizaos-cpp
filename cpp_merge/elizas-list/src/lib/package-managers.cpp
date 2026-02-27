#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/lib/package-managers.h"

std::shared_ptr<Promise<std::shared_ptr<PackageInfo>>> PackageManagerClient::getNpmInfo(string packageName)
{
    auto response = std::async([=]() { axios->get(std:("https://registry.npmjs.org/") + packageName + string_empty); });
    auto latest = const_(response->data)[std:("dist-tags")]->latest;
    auto info = const_(response->data->versions)[latest];
    auto downloads = std::async([=]() { axios->get(std:("https://api.npmjs.org/downloads/point/last-month/") + packageName + string_empty); });
    return object{
        object::pair{std:("name"), packageName}, 
        object::pair{std:("version"), latest}, 
        object::pair{std:("description"), info->description}, 
        object::pair{std:("downloads"), downloads->data->downloads}, 
        object::pair{std:("repository"), info->repository->url}, 
        object::pair{std:("dependencies"), Object->keys(OR((info->dependencies), (object{})))}
    };
}

std::shared_ptr<Promise<std::shared_ptr<PackageInfo>>> PackageManagerClient::getPyPiInfo(string packageName)
{
    auto response = std::async([=]() { axios->get(std:("https://pypi.org/pypi/") + packageName + std:("/json")); });
    auto info = response->data->info;
    return object{
        object::pair{std:("name"), packageName}, 
        object::pair{std:("version"), info->version}, 
        object::pair{std:("description"), info->summary}, 
        object::pair{std:("downloads"), 0}, 
        object::pair{std:("repository"), info->project_urls->Repository}, 
        object::pair{std:("dependencies"), array<any>()}
    };
}

