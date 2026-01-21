#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/tools/src/search/search-file.h"

void searchFile(string searchTerm, string filePath)
{
    if (!filePath) {
        filePath = String(registry->get(std::string("CURRENT_FILE"), string_empty));
        if (!filePath) {
            console->error(std::string("No file open. Use the open command first or provide a file path."));
            process->exit(1);
        }
    }
    if (!fs::existsSync(filePath)) {
        console->error(std::string("Error: File name ") + filePath + std::string(" not found. Please provide a valid file name."));
        process->exit(1);
    }
    filePath = path->resolve(filePath);
    try
    {
        shared content = fs::readFileSync(filePath, std::string("utf-8"));
        auto lines = content->split(std::string("\
"));
        shared matches = array<object>();
        lines->forEach([=](auto line, auto index) mutable
        {
            if (line->includes(searchTerm)) {
                matches->push(object{
                    object::pair{std::string("line"), index + 1}, 
                    object::pair{std::string("content"), line}
                });
            }
        }
        );
        if (matches->get_length() == 0) {
            console->log(std::string("No matches found for "") + searchTerm + std::string("" in ") + filePath + string_empty);
            return;
        }
        auto uniqueLines = std::make_shared<Set>(matches->map([=](auto m) mutable
        {
            return m["line"];
        }
        ));
        if (uniqueLines->size > 100) {
            console->error(std::string("More than ") + uniqueLines->size + std::string(" lines matched for "") + searchTerm + std::string("" in ") + filePath + std::string(". Please narrow your search."));
            return;
        }
        console->log(std::string("Found ") + matches->get_length() + std::string(" matches for "") + searchTerm + std::string("" in ") + filePath + std::string(":"));
        matches->forEach([=](auto match) mutable
        {
            console->log(std::string("Line ") + match["line"] + std::string(":") + match["content"] + string_empty);
        }
        );
        console->log(std::string("End of matches for "") + searchTerm + std::string("" in ") + filePath + string_empty);
    }
    catch (const any& error)
    {
        console->error(std::string("Error reading file: ") + error + string_empty);
        process->exit(1);
    }
};


void setupCLI()
{
    program->name(std::string("search-file"))->description(std::string("Search for a term within a file"))->version(std::string("1.0.0"))->argument(std::string("<search-term>"), std::string("The term to search for"))->argument(std::string("[file]"), std::string("The file to search in (if not provided, uses current open file)"))->action([=](auto searchTerm, auto file) mutable
    {
        searchFile(searchTerm, file);
    }
    );
    program->parse(process->argv);
};



void Main(void)
{
    if (OR((require->main == module), (require->main->filename->endsWith(std::string("/bin/search_file"))))) {
        setupCLI();
    }
}

MAIN
