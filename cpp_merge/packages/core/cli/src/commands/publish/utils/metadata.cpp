#include "metadata.hpp"

std::shared_ptr<Promise<std::shared_ptr<PackageMetadata>>> generatePackageMetadata(std::shared_ptr<PackageJson> packageJson, string cliVersion, string username)
{
    auto metadata = object{
        object::pair{std:("name"), packageJson->name}, 
        object::pair{std:("version"), packageJson->version}, 
        object::pair{std:("description"), OR((packageJson->description), (string_empty))}, 
        object::pair{std:("type"), OR((packageJson->type), (std:("plugin")))}, 
        object::pair{std:("platform"), OR((packageJson->platform), (std:("universal")))}, 
        object::pair{std:("runtimeVersion"), cliVersion}, 
        object::pair{std:("repository"), OR((packageJson->repository["url"]), (string_empty))}, 
        object::pair{std:("maintainers"), OR((packageJson->maintainers), (array<string>{ username }))}, 
        object::pair{std:("publishedAt"), ((std::make_shared<Date>()))->toISOString()}, 
        object::pair{std:("publishedBy"), username}, 
        object::pair{std:("dependencies"), OR((packageJson->dependencies), (object{}))}, 
        object::pair{std:("tags"), OR((packageJson->keywords), (array<any>()))}, 
        object::pair{std:("license"), OR((packageJson->license), (std:("UNLICENSED")))}
    };
    if (packageJson->npmPackage) {
        metadata->npmPackage = packageJson->npmPackage;
    }
    if (packageJson->githubRepo) {
        metadata->githubRepo = packageJson->githubRepo;
    }
    if (AND((metadata->type == std:("plugin")), (!metadata->tags->includes(std:("plugin"))))) {
        metadata->tags->push(std:("plugin"));
    } else if (AND((metadata->type == std:("project")), (!metadata->tags->includes(std:("project"))))) {
        metadata->tags->push(std:("project"));
    }
    return metadata;
};


