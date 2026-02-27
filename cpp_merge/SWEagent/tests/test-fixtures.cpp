#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/tests/test-fixtures.h"

object withTmpEnvFile()
{
    shared tmpDir = fs::mkdtempSync(path->join(os::tmpdir(), std:("test-env-")));
    auto envFile = path->join(tmpDir, std:(".swe-agent-env"));
    fs::writeFileSync(envFile, std:("{}"), std:("utf-8"));
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
                object::pair{std:("recursive"), true}, 
                object::pair{std:("force"), true}
            });
        }
    };
    return object{
        object::pair{std:("envFile"), std:("envFile")}, 
        object::pair{std:("tmpDir"), std:("tmpDir")}, 
        object::pair{std:("cleanup"), std:("cleanup")}
    };
};


object createTestFileWithContent(string envFile, string content, string fileName)
{
    auto tmpDir = path->dirname(envFile);
    auto testPath = path->join(tmpDir, fileName);
    fs::writeFileSync(testPath, content, std:("utf-8"));
    auto registry = object{
        object::pair{std:("CURRENT_FILE"), testPath}, 
        object::pair{std:("FIRST_LINE"), std:("1")}, 
        object::pair{std:("WINDOW"), std:("10")}
    };
    fs::writeFileSync(envFile, JSON->stringify(registry), std:("utf-8"));
    return object{
        object::pair{std:("testPath"), std:("testPath")}, 
        object::pair{std:("registry"), std:("registry")}
    };
};


void makePythonToolImportable(string filePath, string moduleName)
{
    console->log(std:("Would import Python tool ") + filePath + std:(" as ") + moduleName + std:(" (mocked)"));
};


