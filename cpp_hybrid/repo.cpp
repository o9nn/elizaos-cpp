#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/src/environment/repo.h"

array<string> getGitResetCommands(string baseCommit)
{
    return array<string>{ std::string("git fetch"), std::string("git status"), std::string("git restore ."), std::string("git reset --hard"), std::string("git checkout ") + baseCommit + string_empty, std::string("git clean -fdq") };
};


PreExistingRepo::PreExistingRepo(PreExistingRepoConfig config) {
    this->repoName = config->repoName;
    this->baseCommit = config->baseCommit;
    this->reset = config->reset;
}

std::shared_ptr<Promise<void>> PreExistingRepo::copy(std::shared_ptr<AbstractDeployment> _deployment)
{
    logger->info(std::string("Using pre-existing repository ") + this->repoName + string_empty);
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
    this->repoName = path->basename(this->path)->replace(std::string(" "), std::string("-"))->replace(std::string("'"), string_empty);
}

void LocalRepo::checkValidRepo()
{
    if (!fs::existsSync(this->path)) {
        throw any(std::make_shared<Error>(std::string("Could not find git repository at path=") + this->path + string_empty));
    }
    auto gitPath = path->join(this->path, std::string(".git"));
    if (!fs::existsSync(gitPath)) {
        throw any(std::make_shared<Error>(string_empty + this->path + std::string(" is not a git repository")));
    }
    auto status = execSync(std::string("git status --porcelain"), object{
        object::pair{std::string("cwd"), this->path}
    })->toString();
    if (AND((status->trim()), (!process->env->PYTEST_CURRENT_TEST))) {
        throw any(std::make_shared<Error>(std::string("Local git repository ") + this->path + std::string(" is dirty. Please commit or stash changes.")));
    }
}

std::shared_ptr<Promise<void>> LocalRepo::copy(std::shared_ptr<AbstractDeployment> deployment)
{
    this->checkValidRepo();
    logger->info(std::string("Copying local repository from ") + this->path + string_empty);
    std::async([=]() { deployment->runtime->upload(as<std::shared_ptr<UploadRequest>>(object{
        object::pair{std::string("sourcePath"), this->path}, 
        object::pair{std::string("targetPath"), std::string("/") + this->repoName + string_empty}
    })); });
    auto result = std::async([=]() { deployment->runtime->execute(as<std::shared_ptr<Command>>(object{
        object::pair{std::string("command"), std::string("chown -R root:root ") + this->repoName + string_empty}, 
        object::pair{std::string("shell"), true}
    })); });
    if (result->exitCode != 0) {
        auto msg = std::string("Failed to change permissions on copied repository (exit code: ") + result->exitCode + std::string(", stdout: ") + result->stdout + std::string(", stderr: ") + result->stderr + std::string(")");
        throw any(std::make_shared<Error>(msg));
    }
    return std::shared_ptr<Promise<void>>();
}

array<string> LocalRepo::getResetCommands()
{
    return getGitResetCommands(this->baseCommit);
}

GithubRepo::GithubRepo(GithubRepoConfig config) {
    if (AND((config->githubUrl->split(std::string("/"))->length == 2), (!config->githubUrl->includes(std::string("://"))))) {
        this->githubUrl = std::string("https://github.com/") + config->githubUrl + string_empty;
    } else {
        this->githubUrl = config->githubUrl;
    }
    this->baseCommit = config->baseCommit;
    this->cloneTimeout = config->cloneTimeout;
    auto parsed = parseGhRepoUrl(this->githubUrl);
    this->repoName = string_empty + parsed["owner"] + std::string("__") + parsed["repo"] + string_empty;
}

string GithubRepo::getUrlWithToken(string token)
{
    if (!token) {
        return this->githubUrl;
    }
    if (this->githubUrl->includes(std::string("@"))) {
        logger->warn(std::string("Cannot prepend token to URL. "@" found in URL"));
        return this->githubUrl;
    }
    auto urlParts = this->githubUrl->split(std::string("://"));
    if (urlParts->get_length() == 2) {
        return std::string("https://") + token + std::string("@") + const_(urlParts)[1] + string_empty;
    }
    return this->githubUrl;
}

std::shared_ptr<Promise<void>> GithubRepo::copy(std::shared_ptr<AbstractDeployment> deployment)
{
    logger->info(std::string("Cloning GitHub repository ") + this->githubUrl + string_empty);
    auto token = OR((process->env->GITHUB_TOKEN), (string_empty));
    auto url = this->getUrlWithToken(token);
    auto commands = array<string>{ std::string("mkdir /") + this->repoName + string_empty, std::string("cd /") + this->repoName + string_empty, std::string("git init"), std::string("git remote add origin ") + url + string_empty, std::string("git fetch --depth 1 origin ") + this->baseCommit + string_empty, std::string("git checkout FETCH_HEAD"), std::string("cd ..") };
    std::async([=]() { deployment->runtime->execute(as<std::shared_ptr<Command>>(object{
        object::pair{std::string("command"), commands->join(std::string(" && "))}, 
        object::pair{std::string("timeout"), this->cloneTimeout}, 
        object::pair{std::string("shell"), true}, 
        object::pair{std::string("check"), true}
    })); });
    return std::shared_ptr<Promise<void>>();
}

array<string> GithubRepo::getResetCommands()
{
    return getGitResetCommands(this->baseCommit);
}

std::shared_ptr<AgentLogger> logger = getLogger(std::string("repo"));
any PreExistingRepoConfigSchema = z->object(object{
    object::pair{std::string("repoName"), z->string()->describe(std::string("The repo name (must be at root of deployment)"))}, 
    object::pair{std::string("baseCommit"), z->string()->default(std::string("HEAD"))}, 
    object::pair{std::string("type"), z->literal(std::string("preexisting"))}, 
    object::pair{std::string("reset"), z->boolean()->default(true)}
});
any LocalRepoConfigSchema = z->object(object{
    object::pair{std::string("path"), z->string()->transform([=](auto p) mutable
    {
        return path->resolve(p);
    }
    )}, 
    object::pair{std::string("baseCommit"), z->string()->default(std::string("HEAD"))}, 
    object::pair{std::string("type"), z->literal(std::string("local"))}
});
any GithubRepoConfigSchema = z->object(object{
    object::pair{std::string("githubUrl"), z->string()}, 
    object::pair{std::string("baseCommit"), z->string()->default(std::string("HEAD"))}, 
    object::pair{std::string("cloneTimeout"), z->number()->default(500)}, 
    object::pair{std::string("type"), z->literal(std::string("github"))}
});
any RepoConfigSchema = z->discriminatedUnion(std::string("type"), array<any>{ PreExistingRepoConfigSchema, LocalRepoConfigSchema, GithubRepoConfigSchema });

void Main(void)
{
}

MAIN
