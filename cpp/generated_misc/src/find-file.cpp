#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/tools/src/search/find-file.h"

void findFile(string fileName, string dir)
{
    if (!fs::existsSync(dir)) {
        console->error(std::string("Directory ") + dir + std::string(" not found"));
        process->exit(1);
    }
    auto absDir = path->resolve(dir);
    try
    {
        auto matches = array<string>();
        if (OR((OR((fileName->includes(std::string("*"))), (fileName->includes(std::string("?"))))), (fileName->includes(std::string("["))))) {
            matches = glob->sync(std::string("**/") + fileName + string_empty, object{
                object::pair{std::string("cwd"), absDir}, 
                object::pair{std::string("absolute"), true}, 
                object::pair{std::string("nodir"), true}, 
                object::pair{std::string("ignore"), array<string>{ std::string("**/node_modules/**"), std::string("**/.git/**") }}
            });
        } else {
            try
            {
                auto findCmd = std::string("find "") + absDir + std::string("" -type f -name "") + fileName + std::string("" 2>/dev/null");
                auto result = execSync(findCmd, object{
                    object::pair{std::string("encoding"), std::string("utf-8")}
                });
                matches = result->trim()->split(std::string("\
"))->filter([=](auto line) mutable
                {
                    return line->get_length() > 0;
                }
                );
            }
            catch (const any& error)
            {
                matches = array<any>();
            }
        }
        if (matches->get_length() == 0) {
            console->log(std::string("No matches found for "") + fileName + std::string("" in ") + absDir + string_empty);
            return;
        }
        console->log(std::string("Found ") + matches->get_length() + std::string(" matches for "") + fileName + std::string("" in ") + absDir + std::string(":"));
        matches->forEach([=](auto file) mutable
        {
            auto relPath = path->relative(process->cwd(), file);
            console->log(relPath);
        }
        );
    }
    catch (const any& error)
    {
        console->error(std::string("Error finding files: ") + error + string_empty);
        process->exit(1);
    }
};


void setupCLI()
{
    program->name(std::string("find-file"))->description(std::string("Find all files with a given name or pattern in a directory"))->version(std::string("1.0.0"))->argument(std::string("<file-name>"), std::string("The name or pattern to search for (supports wildcards)"))->argument(std::string("[dir]"), std::string("The directory to search in (default: current directory)"), std::string("./"))->action([=](auto fileName, auto dir) mutable
    {
        findFile(fileName, dir);
    }
    );
    program->parse(process->argv);
};



void Main(void)
{
    if (OR((require->main == module), (require->main->filename->endsWith(std::string("/bin/find_file"))))) {
        setupCLI();
    }
}

MAIN
