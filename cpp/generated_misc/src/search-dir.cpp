#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/tools/src/search/search-dir.h"

void searchDir(string searchTerm, string dir)
{
    if (!fs::existsSync(dir)) {
        console->error(std::string("Directory ") + dir + std::string(" not found"));
        process->exit(1);
    }
    auto absDir = path->resolve(dir);
    try
    {
        auto grepCmd = std::string("find "") + absDir + std::string("" -type f ! -path '*/.*' -exec grep -nIH -- "") + searchTerm + std::string("" {} + 2>/dev/null | cut -d: -f1 | sort | uniq -c");
        string matches;
        try
        {
            matches = execSync(grepCmd, object{
                object::pair{std::string("encoding"), std::string("utf-8")}, 
                object::pair{std::string("stdio"), array<string>{ std::string("pipe"), std::string("pipe"), std::string("ignore") }}
            });
        }
        catch (const any& error)
        {
            auto execError = as<object>(error);
            if (OR((execError["status"] == 1), (!execError["stdout"]))) {
                console->log(std::string("No matches found for "") + searchTerm + std::string("" in ") + absDir + string_empty);
                return;
            }
            throw any(error);
        }
        if (!matches->trim()) {
            console->log(std::string("No matches found for "") + searchTerm + std::string("" in ") + absDir + string_empty);
            return;
        }
        auto matchLines = matches->trim()->split(std::string("\
"));
        shared fileMatches = array<object>();
        matchLines->forEach([=](auto line) mutable
        {
            auto match = line->trim()->match((new RegExp(std::string("^\s*(\d+)\s+(.+)"))));
            if (match) {
                auto count = parseInt((*const_(match))[1], 10);
                auto file = (*const_(match))[2];
                fileMatches->push(object{
                    object::pair{std::string("file"), std::string("file")}, 
                    object::pair{std::string("count"), std::string("count")}
                });
            }
        }
        );
        if (fileMatches->get_length() > 100) {
            console->error(std::string("More than ") + fileMatches->get_length() + std::string(" files matched for "") + searchTerm + std::string("" in ") + absDir + std::string(". Please narrow your search."));
            return;
        }
        auto totalMatches = fileMatches->reduce([=](auto sum, auto fm) mutable
        {
            return sum + fm["count"];
        }
        , 0);
        console->log(std::string("Found ") + totalMatches + std::string(" matches for "") + searchTerm + std::string("" in ") + absDir + std::string(":"));
        fileMatches->forEach([=](auto fm) mutable
        {
            auto relPath = path->relative(process->cwd(), fm["file"]);
            console->log(string_empty + relPath + std::string(" (") + fm["count"] + std::string(" matches)"));
        }
        );
        console->log(std::string("End of matches for "") + searchTerm + std::string("" in ") + absDir + string_empty);
    }
    catch (const any& error)
    {
        console->error(std::string("Error searching directory: ") + error + string_empty);
        process->exit(1);
    }
};


void setupCLI()
{
    program->name(std::string("search-dir"))->description(std::string("Search for a term in all files within a directory"))->version(std::string("1.0.0"))->argument(std::string("<search-term>"), std::string("The term to search for"))->argument(std::string("[dir]"), std::string("The directory to search in (default: current directory)"), std::string("./"))->action([=](auto searchTerm, auto dir) mutable
    {
        searchDir(searchTerm, dir);
    }
    );
    program->parse(process->argv);
};



void Main(void)
{
    if (OR((require->main == module), (require->main->filename->endsWith(std::string("/bin/search_dir"))))) {
        setupCLI();
    }
}

MAIN
