#include "index.hpp"
#include <string>

void submit()
{
    auto repoRoot = String(registry->get(std::string("ROOT"), OR((process->env->ROOT), (std::string(".")))));
    try
    {
        if (fs::existsSync(TEST_PATCH_PATH)) {
            auto testPatch = fs::readFileSync(TEST_PATCH_PATH, std::string("utf-8"));
            if (testPatch->trim()) {
                try
                {
                    execSync(std::string("git apply -R < "") + TEST_PATCH_PATH + std::string("""), object{
                        object::pair{std::string("cwd"), as<string>(repoRoot)}, 
                        object::pair{std::string("stdio"), std::string("ignore")}
                    });
                }
                catch (const any& error)
                {
                    console->error(std::string("Warning: Failed to reverse test patch"));
                }
            }
        }
        execSync(std::string("git add -A"), object{
            object::pair{std::string("cwd"), as<string>(repoRoot)}, 
            object::pair{std::string("stdio"), std::string("ignore")}
        });
        auto patch = execSync(std::string("git diff --cached"), object{
            object::pair{std::string("cwd"), as<string>(repoRoot)}, 
            object::pair{std::string("encoding"), std::string("utf-8")}, 
            object::pair{std::string("maxBuffer"), 10 * 1024 * 1024}
        });
        fs::writeFileSync(PATCH_PATH, patch);
        console->log(std::string("<<SWE_AGENT_SUBMISSION>>"));
        console->log(patch);
        console->log(std::string("<<SWE_AGENT_SUBMISSION>>"));
    }
    catch (const any& error)
    {
        console->error(std::string("Error creating submission:"), error);
        process->exit(1);
    }
};


string rootDir = OR((OR((process->env->ROOT), (process->env->TMPDIR))), (std::string("/tmp")));
string PATCH_PATH = path->join(rootDir, std::string("model.patch"));
string TEST_PATCH_PATH = path->join(rootDir, std::string("test.patch"));

void Main(void)
{
    if (require->main == module) {
        program->name(std::string("submit"))->description(std::string("Submit the current changes as a patch"))->version(std::string("1.0.0"))->action([=]() mutable
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
