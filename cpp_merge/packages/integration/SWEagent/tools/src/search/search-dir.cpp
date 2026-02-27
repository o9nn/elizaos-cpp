#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/tools/src/search/search-dir.h"

void searchDir(string searchTerm, string dir)
{
    if (!fs::existsSync(dir)) {
        console->error(std:("Directory ") + dir + std:(" not found"));
        process->exit(1);
    }
    auto absDir = path->resolve(dir);
    try
    {
        auto grepCmd = std:("find "") + absDir + std:("" -type f ! -path '*/.*' -exec grep -nIH -- "") + searchTerm + std:("" {} + 2>/dev/null | cut -d: -f1 | sort | uniq -c");
        string matches;
        try
        {
            matches = execSync(grepCmd, object{
                object::pair{std:("encoding"), std:("utf-8")}, 
                object::pair{std:("stdio"), array<string>{ std:("pipe"), std:("pipe"), std:("ignore") }}
            });
        }
        catch (const any& error)
        {
            auto execError = as<object>(error);
            if (OR((execError["status"] == 1), (!execError["stdout"]))) {
                console->log(std:("No matches found for "") + searchTerm + std:("" in ") + absDir + string_empty);
                return;
            }
            throw any(error);
        }
        if (!matches->trim()) {
            console->log(std:("No matches found for "") + searchTerm + std:("" in ") + absDir + string_empty);
            return;
        }
        auto matchLines = matches->trim()->split(std:("\
"));
        shared fileMatches = array<object>();
        matchLines->forEach([=](auto line) mutable
        {
            auto match = line->trim()->match((new RegExp(std:("^\s*(\d+)\s+(.+)"))));
            if (match) {
                auto count = parseInt((*const_(match))[1], 10);
                auto file = (*const_(match))[2];
                fileMatches->push(object{
                    object::pair{std:("file"), std:("file")}, 
                    object::pair{std:("count"), std:("count")}
                });
            }
        }
        );
        if (fileMatches->get_length() > 100) {
            console->error(std:("More than ") + fileMatches->get_length() + std:(" files matched for "") + searchTerm + std:("" in ") + absDir + std:(". Please narrow your search."));
            return;
        }
        auto totalMatches = fileMatches->reduce([=](auto sum, auto fm) mutable
        {
            return sum + fm["count"];
        }
        , 0);
        console->log(std:("Found ") + totalMatches + std:(" matches for "") + searchTerm + std:("" in ") + absDir + std:(":"));
        fileMatches->forEach([=](auto fm) mutable
        {
            auto relPath = path->relative(process->cwd(), fm["file"]);
            console->log(string_empty + relPath + std:(" (") + fm["count"] + std:(" matches)"));
        }
        );
        console->log(std:("End of matches for "") + searchTerm + std:("" in ") + absDir + string_empty);
    }
    catch (const any& error)
    {
        console->error(std:("Error searching directory: ") + error + string_empty);
        process->exit(1);
    }
};


void setupCLI()
{
    program->name(std:("search-dir"))->description(std:("Search for a term in all files within a directory"))->version(std:("1.0.0"))->argument(std:("<search-term>"), std:("The term to search for"))->argument(std:("[dir]"), std:("The directory to search in (default: current directory)"), std:("./"))->action([=](auto searchTerm, auto dir) mutable
    {
        searchDir(searchTerm, dir);
    }
    );
    program->parse(process->argv);
};



void Main(void)
{
    if (OR((require->main == module), (require->main->filename->endsWith(std:("/bin/search_dir"))))) {
        setupCLI();
    }
}

MAIN
