#include "find-file.h"

void findFile(string fileName, string dir)
{
    if (!fs::existsSync(dir)) {
        console->error(std:("Directory ") + dir + std:(" not found"));
        process->exit(1);
    }
    auto absDir = path->resolve(dir);
    try
    {
        auto matches = array<string>();
        if (OR((OR((fileName->includes(std:("*"))), (fileName->includes(std:("?"))))), (fileName->includes(std:("["))))) {
            matches = glob->sync(std:("**/") + fileName + string_empty, object{
                object::pair{std:("cwd"), absDir}, 
                object::pair{std:("absolute"), true}, 
                object::pair{std:("nodir"), true}, 
                object::pair{std:("ignore"), array<string>{ std:("**/node_modules/**"), std:("**/.git/**") }}
            });
        } else {
            try
            {
                auto findCmd = std:("find "") + absDir + std:("" -type f -name "") + fileName + std:("" 2>/dev/null");
                auto result = execSync(findCmd, object{
                    object::pair{std:("encoding"), std:("utf-8")}
                });
                matches = result->trim()->split(std:("\
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
            console->log(std:("No matches found for "") + fileName + std:("" in ") + absDir + string_empty);
            return;
        }
        console->log(std:("Found ") + matches->get_length() + std:(" matches for "") + fileName + std:("" in ") + absDir + std:(":"));
        matches->forEach([=](auto file) mutable
        {
            auto relPath = path->relative(process->cwd(), file);
            console->log(relPath);
        }
        );
    }
    catch (const any& error)
    {
        console->error(std:("Error finding files: ") + error + string_empty);
        process->exit(1);
    }
};


void setupCLI()
{
    program->name(std:("find-file"))->description(std:("Find all files with a given name or pattern in a directory"))->version(std:("1.0.0"))->argument(std:("<file-name>"), std:("The name or pattern to search for (supports wildcards)"))->argument(std:("[dir]"), std:("The directory to search in (default: current directory)"), std:("./"))->action([=](auto fileName, auto dir) mutable
    {
        findFile(fileName, dir);
    }
    );
    program->parse(process->argv);
};



void Main(void)
{
    if (OR((require->main == module), (require->main->filename->endsWith(std:("/bin/find_file"))))) {
        setupCLI();
    }
}

MAIN
