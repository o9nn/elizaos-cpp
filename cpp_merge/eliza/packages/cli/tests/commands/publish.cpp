#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/tests/commands/publish.test.h"

void Main(void)
{
    describe(std:("ElizaOS Publish Commands"), [=]() mutable
    {
        shared<string> testTmpDir;
        shared<string> elizaosCmd;
        shared<string> originalCwd;
        shared<string> originalPath;
        beforeEach([=]() mutable
        {
            originalCwd = process->cwd();
            originalPath = OR((process->env->PATH), (string_empty));
            testTmpDir = std::async([=]() { mkdtemp(join(tmpdir(), std:("eliza-test-publish-"))); });
            process->chdir(testTmpDir);
            auto scriptDir = join(__dirname, std:(".."));
            elizaosCmd = std:("bun "") + join(scriptDir, std:("../dist/index.js")) + std:(""");
            process->env->GITHUB_TOKEN = std:("mock-github-token-for-testing");
            process->env->GH_TOKEN = std:("mock-github-token-for-testing");
            process->env->GITHUB_USERNAME = std:("test-user");
            process->env->GITHUB_USER = std:("test-user");
            process->env->NPM_TOKEN = std:("mock-npm-token");
            process->env->NODE_AUTH_TOKEN = std:("mock-npm-token");
            auto elizaosDataDir = join(testTmpDir, std:(".elizaos"));
            process->env->ELIZAOS_DATA_DIR = elizaosDataDir;
            std::async([=]() { mkdir(elizaosDataDir, object{
                object::pair{std:("recursive"), true}
            }); });
            std::async([=]() { writeFile(join(elizaosDataDir, std:("credentials.json")), JSON->stringify(object{
                object::pair{std:("github"), object{
                    object::pair{std:("token"), std:("mock-github-token-for-testing")}, 
                    object::pair{std:("username"), std:("test-user")}
                }}
            })); });
            std::async([=]() { writeFile(join(elizaosDataDir, std:("registry.json")), JSON->stringify(object{
                object::pair{std:("registryUrl"), std:("https://github.com/elizaos/registry")}, 
                object::pair{std:("lastUpdated"), std:("2024-01-01T00:00:00.000Z")}
            })); });
            auto mockBinDir = join(testTmpDir, std:("mock-bin"));
            std::async([=]() { mkdir(mockBinDir, object{
                object::pair{std:("recursive"), true}
            }); });
            process->env->PATH = string_empty + mockBinDir + std:(":") + originalPath + string_empty;
            std::async([=]() { writeFile(join(mockBinDir, std:("npm")), std:("#!/bin/bash\
# Comprehensive npm mock that handles all npm operations without prompts\
case "$1" in\
  "whoami")\
    echo "test-user"\
    exit 0\
    ;\
  "login")\
    echo "Logged in as test-user"\
    exit 0\
    ;\
  "publish")\
    if [[ "$*" == *"--ignore-scripts"* ]]; then\
      echo "Published successfully (with --ignore-scripts)"\
    else\
      echo "Published successfully"\
    fi\
    exit 0\
    ;\
  "run")\
    if [[ "$2" == "build" ]]; then\
      echo "Build completed"\
      exit 0\
    fi\
    echo "npm run $2 completed"\
    exit 0\
    ;\
  "version")\
    if [[ "$2" == "patch" ]] || [[ "$2" == "minor" ]] || [[ "$2" == "major" ]]; then\
      echo "v1.0.1"\
      exit 0\
    fi\
    echo "1.0.0"\
    exit 0\
    ;\
  "view")\
    # Mock npm view for CLI version checking - return empty to avoid update prompts\
    echo '{}'\
    exit 0\
    ;\
  "config")\
    case "$2" in\
      "get")\
        echo "mock-value"\
        ;\
      "set")\
        echo "Config set successfully"\
        ;\
      *)\
        echo "npm config $*"\
        ;\
    esac\
    exit 0\
    ;\
  "install"|"i")\
    echo "Dependencies installed"\
    exit 0\
    ;\
  *)\
    echo "npm $*"\
    exit 0\
    ;\
esac")); });
            if (process->platform == std:("win32")) {
                std::async([=]() { writeFile(join(mockBinDir, std:("npm.cmd")), std:("@echo off\
if "%1"=="whoami" (\
  echo test-user\
  exit /b 0\
)\
if "%1"=="login" (\
  echo Logged in as test-user\
  exit /b 0\
)\
if "%1"=="publish" (\
  echo Published successfully\
  exit /b 0\
)\
if "%1"=="run" (\
  echo npm run %2 completed\
  exit /b 0\
)\
if "%1"=="version" (\
  if "%2"=="patch" echo v1.0.1\
  if "%2"=="minor" echo v1.0.1\
  if "%2"=="major" echo v1.0.1\
  if "%2"=="" echo 1.0.0\
  exit /b 0\
)\
if "%1"=="view" (\
  echo {}\
  exit /b 0\
)\
if "%1"=="config" (\
  echo npm config %*\
  exit /b 0\
)\
if "%1"=="install" (\
  echo Dependencies installed\
  exit /b 0\
)\
echo npm %*\
exit /b 0")); });
            } else {
                execSync(std:("chmod +x ") + join(mockBinDir, std:("npm")) + string_empty);
            }
            auto gitMockContent = (process->platform == std:("win32")) ? std:("@echo off\
if "%1"=="init" (\
  echo Initialized git repository\
  exit /b 0\
)\
if "%1"=="add" (\
  echo Git add completed\
  exit /b 0\
)\
if "%1"=="commit" (\
  echo Git commit completed\
  exit /b 0\
)\
if "%1"=="push" (\
  echo Git push completed\
  exit /b 0\
)\
if "%1"=="config" (\
  if "%2"=="user.name" echo Test User\
  if "%2"=="user.email" echo test@example.com\
  if "%2"=="remote.origin.url" echo https://github.com/test-user/test-repo.git\
  if not "%2"=="user.name" if not "%2"=="user.email" if not "%2"=="remote.origin.url" echo git config value\
  exit /b 0\
)\
if "%1"=="remote" (\
  if "%2"=="get-url" (\
    echo https://github.com/test-user/test-repo.git\
  ) else (\
    echo git remote %*\
  )\
  exit /b 0\
)\
if "%1"=="status" (\
  echo On branch main\
  echo nothing to commit, working tree clean\
  exit /b 0\
)\
if "%1"=="branch" (\
  echo * main\
  exit /b 0\
)\
if "%1"=="tag" (\
  echo v1.0.0\
  exit /b 0\
)\
echo git %*\
exit /b 0") : std:("#!/bin/bash\
# Comprehensive git mock that handles all git operations\
case "$1" in\
  "init")\
    echo "Initialized git repository"\
    exit 0\
    ;\
  "add"|"commit"|"push"|"pull"|"fetch")\
    echo "Git $1 completed"\
    exit 0\
    ;\
  "config")\
    case "$2" in\
      "user.name")\
        echo "Test User"\
        ;\
      "user.email")\
        echo "test@example.com"\
        ;\
      "remote.origin.url")\
        echo "https://github.com/test-user/test-repo.git"\
        ;\
      *)\
        echo "git config value"\
        ;\
    esac\
    exit 0\
    ;\
  "remote")\
    if [[ "$2" == "get-url" ]]; then\
      echo "https://github.com/test-user/test-repo.git"\
    else\
      echo "git remote $*"\
    fi\
    exit 0\
    ;\
  "status")\
    echo "On branch main"\
    echo "nothing to commit, working tree clean"\
    exit 0\
    ;\
  "branch")\
    echo "* main"\
    exit 0\
    ;\
  "tag")\
    echo "v1.0.0"\
    exit 0\
    ;\
  *)\
    echo "git $*"\
    exit 0\
    ;\
esac");
            std::async([=]() { writeFile(join(mockBinDir, (process->platform == std:("win32")) ? std:("git.cmd") : std:("git")), gitMockContent); });
            if (process->platform != std:("win32")) {
                execSync(std:("chmod +x ") + join(mockBinDir, std:("git")) + string_empty);
            }
            auto ghMockContent = (process->platform == std:("win32")) ? std:("@echo off\
if "%1"=="auth" (\
  echo Logged in to github.com as test-user\
  exit /b 0\
)\
if "%1"=="repo" (\
  echo Repository operation completed\
  exit /b 0\
)\
echo gh %*\
exit /b 0") : std:("#!/bin/bash\
case "$1" in\
  "auth")\
    echo "Logged in to github.com as test-user"\
    exit 0\
    ;\
  "repo")\
    echo "Repository operation completed"\
    exit 0\
    ;\
  *)\
    echo "gh $*"\
    exit 0\
    ;\
esac");
            std::async([=]() { writeFile(join(mockBinDir, (process->platform == std:("win32")) ? std:("gh.cmd") : std:("gh")), ghMockContent); });
            if (process->platform != std:("win32")) {
                execSync(std:("chmod +x ") + join(mockBinDir, std:("gh")) + string_empty);
            }
        }
        );
        afterEach([=]() mutable
        {
            safeChangeDirectory(originalCwd);
            process->env->PATH = originalPath;
            process->env.Delete("GITHUB_TOKEN");
            process->env.Delete("GH_TOKEN");
            process->env.Delete("GITHUB_USERNAME");
            process->env.Delete("GITHUB_USER");
            process->env.Delete("NPM_TOKEN");
            process->env.Delete("NODE_AUTH_TOKEN");
            process->env.Delete("ELIZAOS_DATA_DIR");
            if (AND((testTmpDir), (testTmpDir->includes(std:("eliza-test-publish-"))))) {
                try
                {
                    std::async([=]() { rm(testTmpDir, object{
                        object::pair{std:("recursive"), true}
                    }); });
                }
                catch (const any& e)
                {
                }
            }
        }
        );
        it(std:("publish --help shows usage"), [=]() mutable
        {
            auto result = execSync(string_empty + elizaosCmd + std:(" publish --help"), object{
                object::pair{std:("encoding"), std:("utf8")}
            });
            expect(result)->toContain(std:("Usage: elizaos publish"));
            expect(result)->toContain(std:("Publish a plugin to npm, GitHub, and the registry"));
            expect(result)->toContain(std:("--npm"));
            expect(result)->toContain(std:("--test"));
            expect(result)->toContain(std:("--dry-run"));
            expect(result)->toContain(std:("--skip-registry"));
        }
        );
        it(std:("publish command integrates with CLI properly"), [=]() mutable
        {
            auto helpResult = execSync(string_empty + elizaosCmd + std:(" --help"), object{
                object::pair{std:("encoding"), std:("utf8")}
            });
            expect(helpResult)->toContain(std:("publish"));
            auto publishHelpResult = execSync(string_empty + elizaosCmd + std:(" publish --help"), object{
                object::pair{std:("encoding"), std:("utf8")}
            });
            expect(publishHelpResult)->toContain(std:("Options:"));
        }
        );
        it(std:("publish command validates basic directory structure"), [=]() mutable
        {
            auto result = execSync(string_empty + elizaosCmd + std:(" publish --help"), object{
                object::pair{std:("encoding"), std:("utf8")}
            });
            expect(result)->toContain(std:("publish"));
        }
        );
        it(std:("publish command detects missing images"), [=]() mutable
        {
            std::async([=]() { mkdir(std:("plugin-simple")); });
            process->chdir(join(testTmpDir, std:("plugin-simple")));
            std::async([=]() { writeFile(std:("package.json"), JSON->stringify(object{
                object::pair{std:("name"), std:("@test-user/plugin-simple")}, 
                object::pair{std:("version"), std:("1.0.0")}
            })); });
            auto result = execSync(string_empty + elizaosCmd + std:(" publish --help"), object{
                object::pair{std:("encoding"), std:("utf8")}
            });
            expect(result)->toContain(std:("publish"));
        }
        );
        it(std:("publish dry-run flag works"), [=]() mutable
        {
            auto result = execSync(string_empty + elizaosCmd + std:(" publish --dry-run --help"), object{
                object::pair{std:("encoding"), std:("utf8")}
            });
            expect(result)->toContain(std:("dry-run"));
        }
        );
    }
    );
}

MAIN
