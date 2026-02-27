#include "search-file.h"

void searchFile(string searchTerm, string filePath)
{
    if (!filePath) {
        filePath = String(registry->get(std:("CURRENT_FILE"), string_empty));
        if (!filePath) {
            console->error(std:("No file open. Use the open command first or provide a file path."));
            process->exit(1);
        }
    }
    if (!fs::existsSync(filePath)) {
        console->error(std:("Error: File name ") + filePath + std:(" not found. Please provide a valid file name."));
        process->exit(1);
    }
    filePath = path->resolve(filePath);
    try
    {
        shared content = fs::readFileSync(filePath, std:("utf-8"));
        auto lines = content->split(std:("\
"));
        shared matches = array<object>();
        lines->forEach([=](auto line, auto index) mutable
        {
            if (line->includes(searchTerm)) {
                matches->push(object{
                    object::pair{std:("line"), index + 1}, 
                    object::pair{std:("content"), line}
                });
            }
        }
        );
        if (matches->get_length() == 0) {
            console->log(std:("No matches found for "") + searchTerm + std:("" in ") + filePath + string_empty);
            return;
        }
        auto uniqueLines = std::make_shared<Set>(matches->map([=](auto m) mutable
        {
            return m["line"];
        }
        ));
        if (uniqueLines->size > 100) {
            console->error(std:("More than ") + uniqueLines->size + std:(" lines matched for "") + searchTerm + std:("" in ") + filePath + std:(". Please narrow your search."));
            return;
        }
        console->log(std:("Found ") + matches->get_length() + std:(" matches for "") + searchTerm + std:("" in ") + filePath + std:(":"));
        matches->forEach([=](auto match) mutable
        {
            console->log(std:("Line ") + match["line"] + std:(":") + match["content"] + string_empty);
        }
        );
        console->log(std:("End of matches for "") + searchTerm + std:("" in ") + filePath + string_empty);
    }
    catch (const any& error)
    {
        console->error(std:("Error reading file: ") + error + string_empty);
        process->exit(1);
    }
};


void setupCLI()
{
    program->name(std:("search-file"))->description(std:("Search for a term within a file"))->version(std:("1.0.0"))->argument(std:("<search-term>"), std:("The term to search for"))->argument(std:("[file]"), std:("The file to search in (if not provided, uses current open file)"))->action([=](auto searchTerm, auto file) mutable
    {
        searchFile(searchTerm, file);
    }
    );
    program->parse(process->argv);
};



void Main(void)
{
    if (OR((require->main == module), (require->main->filename->endsWith(std:("/bin/search_file"))))) {
        setupCLI();
    }
}

MAIN
