#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_ENVIRONMENT_REPO_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_ENVIRONMENT_REPO_H
#include "core.h"
#include "zod.h"
#include "path.h"
#include "fs.h"
#include "child_process.h"
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
std::shared_ptr<Repo> repoFromSimplifiedInput(string input, string baseCommit = std::string("HEAD"), P2 type = std::string("auto"));

template <typename P2>
std::shared_ptr<Repo> repoFromSimplifiedInput(string input, string baseCommit, P2 type)
{
    if (type == std::string("auto")) {
        if (input->includes(std::string("github.com"))) {
            type = std::string("github");
        } else if (fs::existsSync(input)) {
            type = std::string("local");
        } else {
            type = std::string("preexisting");
        }
    }
    static switch_type __switch6933_7354 = {
        { any(std::string("github")), 1 },
        { any(std::string("local")), 2 },
        { any(std::string("preexisting")), 3 }
    };
    switch (__switch6933_7354[type])
    {
    case 1:
        return std::make_shared<GithubRepo>(object{
            object::pair{std::string("githubUrl"), input}, 
            object::pair{std::string("baseCommit"), std::string("baseCommit")}, 
            object::pair{std::string("type"), std::string("github")}, 
            object::pair{std::string("cloneTimeout"), 500}
        });
    case 2:
        return std::make_shared<LocalRepo>(object{
            object::pair{std::string("path"), input}, 
            object::pair{std::string("baseCommit"), std::string("baseCommit")}, 
            object::pair{std::string("type"), std::string("local")}
        });
    case 3:
        return std::make_shared<PreExistingRepo>(object{
            object::pair{std::string("repoName"), input}, 
            object::pair{std::string("baseCommit"), std::string("baseCommit")}, 
            object::pair{std::string("type"), std::string("preexisting")}, 
            object::pair{std::string("reset"), true}
        });
    default:
        throw any(std::make_shared<Error>(std::string("Unknown repo type: ") + type + string_empty));
    }
};


#endif
