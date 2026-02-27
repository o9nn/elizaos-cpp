#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_SRC_ENVIRONMENT_REPO_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_SRC_ENVIRONMENT_REPO_H
#include "core.hpp"
#include "zod.hpp"
#include "path.hpp"
#include "fs.hpp"
#include "child_process.hpp"
#include "../utils/github.h"
#include "../utils/log.h"
#include "./deployment.h"
#include "./runtime.h"

typedef z::infer<PreExistingRepoConfigSchema> PreExistingRepoConfig;

typedef z::infer<LocalRepoConfigSchema> LocalRepoConfig;

typedef z::infer<GithubRepoConfigSchema> GithubRepoConfig;

typedef z::infer<RepoConfigSchema> RepoConfig;

class Repo;
class PreExistingRepo;
class LocalRepo;
class GithubRepo;

extern std::shared_ptr<AgentLogger> logger;
class Repo : public object, public std::enable_shared_from_this<Repo> {
public:
    using std::enable_shared_from_this<Repo>::shared_from_this;
    string baseCommit;

    string repoName;

    virtual std::shared_ptr<Promise<void>> copy(std::shared_ptr<AbstractDeployment> deployment) = 0;
    virtual array<string> getResetCommands() = 0;
};

array<string> getGitResetCommands(string baseCommit);

extern any PreExistingRepoConfigSchema;
class PreExistingRepo : public Repo, public std::enable_shared_from_this<PreExistingRepo> {
public:
    using std::enable_shared_from_this<PreExistingRepo>::shared_from_this;
    string repoName;

    string baseCommit;

    boolean reset;

    PreExistingRepo(PreExistingRepoConfig config);
    virtual std::shared_ptr<Promise<void>> copy(std::shared_ptr<AbstractDeployment> _deployment);
    virtual array<string> getResetCommands();
};

extern any LocalRepoConfigSchema;
class LocalRepo : public Repo, public std::enable_shared_from_this<LocalRepo> {
public:
    using std::enable_shared_from_this<LocalRepo>::shared_from_this;
    string path;

    string baseCommit;

    string repoName;

    LocalRepo(LocalRepoConfig config);
    virtual void checkValidRepo();
    virtual std::shared_ptr<Promise<void>> copy(std::shared_ptr<AbstractDeployment> deployment);
    virtual array<string> getResetCommands();
};

extern any GithubRepoConfigSchema;
class GithubRepo : public Repo, public std::enable_shared_from_this<GithubRepo> {
public:
    using std::enable_shared_from_this<GithubRepo>::shared_from_this;
    string githubUrl;

    string baseCommit;

    double cloneTimeout;

    string repoName;

    GithubRepo(GithubRepoConfig config);
    virtual string getUrlWithToken(string token);
    virtual std::shared_ptr<Promise<void>> copy(std::shared_ptr<AbstractDeployment> deployment);
    virtual array<string> getResetCommands();
};

extern any RepoConfigSchema;
template <typename P2>
std::shared_ptr<Repo> repoFromSimplifiedInput(string input, string baseCommit = std:("HEAD"), P2 type = std:("auto"));

template <typename P2>
std::shared_ptr<Repo> repoFromSimplifiedInput(string input, string baseCommit, P2 type)
{
    if (type == std:("auto")) {
        if (input->includes(std:("github.com"))) {
            type = std:("github");
        } else if (fs::existsSync(input)) {
            type = std:("local");
        } else {
            type = std:("preexisting");
        }
    }
    static switch_type __switch6933_7354 = {
        { any(std:("github")), 1 },
        { any(std:("local")), 2 },
        { any(std:("preexisting")), 3 }
    };
    switch (__switch6933_7354[type])
    {
    case 1:
        return std::make_shared<GithubRepo>(object{
            object::pair{std:("githubUrl"), input}, 
            object::pair{std:("baseCommit"), std:("baseCommit")}, 
            object::pair{std:("type"), std:("github")}, 
            object::pair{std:("cloneTimeout"), 500}
        });
    case 2:
        return std::make_shared<LocalRepo>(object{
            object::pair{std:("path"), input}, 
            object::pair{std:("baseCommit"), std:("baseCommit")}, 
            object::pair{std:("type"), std:("local")}
        });
    case 3:
        return std::make_shared<PreExistingRepo>(object{
            object::pair{std:("repoName"), input}, 
            object::pair{std:("baseCommit"), std:("baseCommit")}, 
            object::pair{std:("type"), std:("preexisting")}, 
            object::pair{std:("reset"), true}
        });
    default:
        throw any(std::make_shared<Error>(std:("Unknown repo type: ") + type + string_empty));
    }
};


#endif
