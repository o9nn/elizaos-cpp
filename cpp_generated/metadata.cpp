#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/commands/publish/utils/metadata.h"

std::shared_ptr<Promise<std::shared_ptr<PackageMetadata>>> generatePackageMetadata(std::shared_ptr<PackageJson> packageJson, string cliVersion, string username)
{
    auto metadata = object{
        object::pair{std::string("name"), packageJson->name}, 
        object::pair{std::string("version"), packageJson->version}, 
        object::pair{std::string("description"), OR((packageJson->description), (string_empty))}, 
        object::pair{std::string("type"), OR((packageJson->type), (std::string("plugin")))}, 
        object::pair{std::string("platform"), OR((packageJson->platform), (std::string("universal")))}, 
        object::pair{std::string("runtimeVersion"), cliVersion}, 
        object::pair{std::string("repository"), OR((packageJson->repository["url"]), (string_empty))}, 
        object::pair{std::string("maintainers"), OR((packageJson->maintainers), (array<string>{ username }))}, 
        object::pair{std::string("publishedAt"), ((std::make_shared<Date>()))->toISOString()}, 
        object::pair{std::string("publishedBy"), username}, 
        object::pair{std::string("dependencies"), OR((packageJson->dependencies), (object{}))}, 
        object::pair{std::string("tags"), OR((packageJson->keywords), (array<any>()))}, 
        object::pair{std::string("license"), OR((packageJson->license), (std::string("UNLICENSED")))}
    };
    if (packageJson->npmPackage) {
        metadata->npmPackage = packageJson->npmPackage;
    }
    if (packageJson->githubRepo) {
        metadata->githubRepo = packageJson->githubRepo;
    }
    if (AND((metadata->type == std::string("plugin")), (!metadata->tags->includes(std::string("plugin"))))) {
        metadata->tags->push(std::string("plugin"));
    } else if (AND((metadata->type == std::string("project")), (!metadata->tags->includes(std::string("project"))))) {
        metadata->tags->push(std::string("project"));
    }
    return metadata;
};


