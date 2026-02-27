#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/src/environment/repo.h"

array<string> getGitResetCommands(string baseCommit)
{
    return array<string>{ std:("git fetch"), std:("git status"), std:("git restore ."), std:("git reset --hard"), std:("git checkout ") + baseCommit + string_empty, std:("git clean -fdq") };
};


PreExistingRepo::PreExistingRepo(PreExistingRepoConfig config) {
    this->repoName = config->repoName;
    this->baseCommit = config->baseCommit;
    this->reset = config->reset;
}

std::shared_ptr<Promise<void>> PreExistingRepo::copy(std::shared_ptr<AbstractDeployment> _deployment)
{
    logger->info(std:("Using pre-existing repository ") + this->repoName + string_empty);
    return std::shared_ptr<Promise<void>>();
}

array<string> PreExistingRepo::getResetCommands()
{
    if (!this->reset) {
        return array<any>();
    }
    return getGitResetCommands(this->baseCommit);
}

LocalRepo::LocalRepo(LocalRepoConfig config) {
    this->path = config->path;
    this->baseCommit = config->baseCommit;
    this->repoName = path->basename(this->path)->replace(std:(" "), std:("-"))->replace(std:("'"), string_empty);
}

void LocalRepo::checkValidRepo()
{
    if (!fs::existsSync(this->path)) {
        throw any(std::make_shared<Error>(std:("Could not find git repository at path=") + this->path + string_empty));
    }
    auto gitPath = path->join(this->path, std:(".git"));
    if (!fs::existsSync(gitPath)) {
        throw any(std::make_shared<Error>(string_empty + this->path + std:(" is not a git repository")));
    }
    auto status = execSync(std:("git status --porcelain"), object{
        object::pair{std:("cwd"), this->path}
    })->toString();
    if (AND((status->trim()), (!process->env->PYTEST_CURRENT_TEST))) {
        throw any(std::make_shared<Error>(std:("Local git repository ") + this->path + std:(" is dirty. Please commit or stash changes.")));
    }
}

std::shared_ptr<Promise<void>> LocalRepo::copy(std::shared_ptr<AbstractDeployment> deployment)
{
    this->checkValidRepo();
    logger->info(std:("Copying local repository from ") + this->path + string_empty);
    std::async([=]() { deployment->runtime->upload(as<std::shared_ptr<UploadRequest>>(object{
        object::pair{std:("sourcePath"), this->path}, 
        object::pair{std:("targetPath"), std:("/") + this->repoName + string_empty}
    })); });
    auto result = std::async([=]() { deployment->runtime->execute(as<std::shared_ptr<Command>>(object{
        object::pair{std:("command"), std:("chown -R root:root ") + this->repoName + string_empty}, 
        object::pair{std:("shell"), true}
    })); });
    if (result->exitCode != 0) {
        auto msg = std:("Failed to change permissions on copied repository (exit code: ") + result->exitCode + std:(", stdout: ") + result->stdout + std:(", stderr: ") + result->stderr + std:(")");
        throw any(std::make_shared<Error>(msg));
    }
    return std::shared_ptr<Promise<void>>();
}

array<string> LocalRepo::getResetCommands()
{
    return getGitResetCommands(this->baseCommit);
}

GithubRepo::GithubRepo(GithubRepoConfig config) {
    if (AND((config->githubUrl->split(std:("/"))->length == 2), (!config->githubUrl->includes(std:("://"))))) {
        this->githubUrl = std:("https://github.com/") + config->githubUrl + string_empty;
    } else {
        this->githubUrl = config->githubUrl;
    }
    this->baseCommit = config->baseCommit;
    this->cloneTimeout = config->cloneTimeout;
    auto parsed = parseGhRepoUrl(this->githubUrl);
    this->repoName = string_empty + parsed["owner"] + std:("__") + parsed["repo"] + string_empty;
}

string GithubRepo::getUrlWithToken(string token)
{
    if (!token) {
        return this->githubUrl;
    }
    if (this->githubUrl->includes(std:("@"))) {
        logger->warn(std:("Cannot prepend token to URL. "@" found in URL"));
        return this->githubUrl;
    }
    auto urlParts = this->githubUrl->split(std:("://"));
    if (urlParts->get_length() == 2) {
        return std:("https://") + token + std:("@") + const_(urlParts)[1] + string_empty;
    }
    return this->githubUrl;
}

std::shared_ptr<Promise<void>> GithubRepo::copy(std::shared_ptr<AbstractDeployment> deployment)
{
    logger->info(std:("Cloning GitHub repository ") + this->githubUrl + string_empty);
    auto token = OR((process->env->GITHUB_TOKEN), (string_empty));
    auto url = this->getUrlWithToken(token);
    auto commands = array<string>{ std:("mkdir /") + this->repoName + string_empty, std:("cd /") + this->repoName + string_empty, std:("git init"), std:("git remote add origin ") + url + string_empty, std:("git fetch --depth 1 origin ") + this->baseCommit + string_empty, std:("git checkout FETCH_HEAD"), std:("cd ..") };
    std::async([=]() { deployment->runtime->execute(as<std::shared_ptr<Command>>(object{
        object::pair{std:("command"), commands->join(std:(" && "))}, 
        object::pair{std:("timeout"), this->cloneTimeout}, 
        object::pair{std:("shell"), true}, 
        object::pair{std:("check"), true}
    })); });
    return std::shared_ptr<Promise<void>>();
}

array<string> GithubRepo::getResetCommands()
{
    return getGitResetCommands(this->baseCommit);
}

std::shared_ptr<AgentLogger> logger = getLogger(std:("repo"));
any PreExistingRepoConfigSchema = z->object(object{
    object::pair{std:("repoName"), z->string()->describe(std:("The repo name (must be at root of deployment)"))}, 
    object::pair{std:("baseCommit"), z->string()->default(std:("HEAD"))}, 
    object::pair{std:("type"), z->literal(std:("preexisting"))}, 
    object::pair{std:("reset"), z->boolean()->default(true)}
});
any LocalRepoConfigSchema = z->object(object{
    object::pair{std:("path"), z->string()->transform([=](auto p) mutable
    {
        return path->resolve(p);
    }
    )}, 
    object::pair{std:("baseCommit"), z->string()->default(std:("HEAD"))}, 
    object::pair{std:("type"), z->literal(std:("local"))}
});
any GithubRepoConfigSchema = z->object(object{
    object::pair{std:("githubUrl"), z->string()}, 
    object::pair{std:("baseCommit"), z->string()->default(std:("HEAD"))}, 
    object::pair{std:("cloneTimeout"), z->number()->default(500)}, 
    object::pair{std:("type"), z->literal(std:("github"))}
});
any RepoConfigSchema = z->discriminatedUnion(std:("type"), array<any>{ PreExistingRepoConfigSchema, LocalRepoConfigSchema, GithubRepoConfigSchema });

void Main(void)
{
}

MAIN
