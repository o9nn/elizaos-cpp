#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/tests/test-fixtures.h"

object withTmpEnvFile()
{
    shared tmpDir = fs::mkdtempSync(path->join(os::tmpdir(), std::string("test-env-")));
    auto envFile = path->join(tmpDir, std::string(".swe-agent-env"));
    fs::writeFileSync(envFile, std::string("{}"), std::string("utf-8"));
    shared originalEnvFile = process->env->SWE_AGENT_ENV_FILE;
    process->env->SWE_AGENT_ENV_FILE = envFile;
    auto cleanup = [=]() mutable
    {
        if (originalEnvFile != undefined) {
            process->env->SWE_AGENT_ENV_FILE = originalEnvFile;
        } else {
            process->env.Delete("SWE_AGENT_ENV_FILE");
        }
        if (fs::existsSync(tmpDir)) {
            fs::rmSync(tmpDir, object{
                object::pair{std::string("recursive"), true}, 
                object::pair{std::string("force"), true}
            });
        }
    };
    return object{
        object::pair{std::string("envFile"), std::string("envFile")}, 
        object::pair{std::string("tmpDir"), std::string("tmpDir")}, 
        object::pair{std::string("cleanup"), std::string("cleanup")}
    };
};


object createTestFileWithContent(string envFile, string content, string fileName)
{
    auto tmpDir = path->dirname(envFile);
    auto testPath = path->join(tmpDir, fileName);
    fs::writeFileSync(testPath, content, std::string("utf-8"));
    auto registry = object{
        object::pair{std::string("CURRENT_FILE"), testPath}, 
        object::pair{std::string("FIRST_LINE"), std::string("1")}, 
        object::pair{std::string("WINDOW"), std::string("10")}
    };
    fs::writeFileSync(envFile, JSON->stringify(registry), std::string("utf-8"));
    return object{
        object::pair{std::string("testPath"), std::string("testPath")}, 
        object::pair{std::string("registry"), std::string("registry")}
    };
};


void makePythonToolImportable(string filePath, string moduleName)
{
    console->log(std::string("Would import Python tool ") + filePath + std::string(" as ") + moduleName + std::string(" (mocked)"));
};


