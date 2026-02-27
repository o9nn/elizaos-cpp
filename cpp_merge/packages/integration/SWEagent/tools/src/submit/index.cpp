#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/tools/src/submit/index.h"

void submit()
{
    auto repoRoot = String(registry->get(std:("ROOT"), OR((process->env->ROOT), (std:(".")))));
    try
    {
        if (fs::existsSync(TEST_PATCH_PATH)) {
            auto testPatch = fs::readFileSync(TEST_PATCH_PATH, std:("utf-8"));
            if (testPatch->trim()) {
                try
                {
                    execSync(std:("git apply -R < "") + TEST_PATCH_PATH + std:("""), object{
                        object::pair{std:("cwd"), as<string>(repoRoot)}, 
                        object::pair{std:("stdio"), std:("ignore")}
                    });
                }
                catch (const any& error)
                {
                    console->error(std:("Warning: Failed to reverse test patch"));
                }
            }
        }
        execSync(std:("git add -A"), object{
            object::pair{std:("cwd"), as<string>(repoRoot)}, 
            object::pair{std:("stdio"), std:("ignore")}
        });
        auto patch = execSync(std:("git diff --cached"), object{
            object::pair{std:("cwd"), as<string>(repoRoot)}, 
            object::pair{std:("encoding"), std:("utf-8")}, 
            object::pair{std:("maxBuffer"), 10 * 1024 * 1024}
        });
        fs::writeFileSync(PATCH_PATH, patch);
        console->log(std:("<<SWE_AGENT_SUBMISSION>>"));
        console->log(patch);
        console->log(std:("<<SWE_AGENT_SUBMISSION>>"));
    }
    catch (const any& error)
    {
        console->error(std:("Error creating submission:"), error);
        process->exit(1);
    }
};


string rootDir = OR((OR((process->env->ROOT), (process->env->TMPDIR))), (std:("/tmp")));
string PATCH_PATH = path->join(rootDir, std:("model.patch"));
string TEST_PATCH_PATH = path->join(rootDir, std:("test.patch"));

void Main(void)
{
    if (require->main == module) {
        program->name(std:("submit"))->description(std:("Submit the current changes as a patch"))->version(std:("1.0.0"))->action([=]() mutable
        {
            submit();
        }
        );
        if (process->argv->get_length() == 2) {
            submit();
        } else {
            program->parse(process->argv);
        }
    }
}

MAIN
