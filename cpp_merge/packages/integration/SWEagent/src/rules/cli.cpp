#include "cli.hpp"

std::shared_ptr<Promise<array<string>>> findProjectFiles()
{
    shared files = array<string>();
    shared extensions = array<string>{ std:(".py"), std:(".ts"), std:(".tsx") };
    auto walk = [=](auto dir) mutable
    {
        auto entries = std::async([=]() { fs::promises::readdir(dir, object{
            object::pair{std:("withFileTypes"), true}
        }); });
        for (auto& entry : entries)
        {
            auto fullPath = path->join(dir, entry->name);
            if (OR((OR((OR((entry->name->startsWith(std:("."))), (entry->name == std:("node_modules")))), (entry->name == std:("dist")))), (entry->name == std:("build")))) {
                continue;
            }
            if (entry->isDirectory()) {
                std::async([=]() { walk(fullPath); });
            } else if (entry->isFile()) {
                auto ext = path->extname(entry->name);
                if (extensions->includes(ext)) {
                    files->push(fullPath);
                }
            }
        }
    };

    std::async([=]() { walk(process->cwd()); });
    return files;
};


std::shared_ptr<Promise<array<string>>> resolveFiles(array<string> patterns)
{
    auto resolved = array<string>();
    for (auto& pattern : patterns)
    {
        auto absPath = path->resolve(pattern);
        try
        {
            auto stat = std::async([=]() { fs::promises::stat(absPath); });
            if (stat->isFile()) {
                resolved->push(absPath);
            } else if (stat->isDirectory()) {
                auto dirFiles = std::async([=]() { findFilesInDir(absPath); });
                resolved->push(const_(dirFiles)[0]);
            }
        }
        catch (const any& error)
        {
            console->warn(std:("Warning: Could not resolve pattern: ") + pattern + string_empty);
        }
    }
    return Array->from(std::make_shared<Set>(resolved));
};


std::shared_ptr<Promise<array<string>>> findFilesInDir(string dir)
{
    auto files = array<string>();
    auto extensions = array<string>{ std:(".py"), std:(".ts"), std:(".tsx") };
    auto entries = std::async([=]() { fs::promises::readdir(dir, object{
        object::pair{std:("withFileTypes"), true}
    }); });
    for (auto& entry : entries)
    {
        auto fullPath = path->join(dir, entry->name);
        if (entry->isFile()) {
            auto ext = path->extname(entry->name);
            if (extensions->includes(ext)) {
                files->push(fullPath);
            }
        }
    }
    return files;
};


any program = std::make_shared<Command>();

void Main(void)
{
    program->name(std:("swe-rules"))->description(std:("SWE-agent rules validation and management CLI"))->version(std:("1.0.0"));
    program->command(std:("validate"))->description(std:("Validate files against SWE-agent coding rules"))->argument(std:("[files...]"), std:("Files to validate (supports wildcards)"))->option(std:("-l, --language <lang>"), std:("Language to validate (python or typescript)"), std:("auto"))->option(std:("-s, --strict"), std:("Exit with error code if validation fails"), false)->option(std:("-q, --quiet"), std:("Only show errors, no warnings"), false)->action([=](auto files, auto options) mutable
    {
        try
        {
            if (OR((!files), (files->get_length() == 0))) {
                files = std::async([=]() { findProjectFiles(); });
            }
            auto resolvedFiles = std::async([=]() { resolveFiles(files); });
            if (resolvedFiles->get_length() == 0) {
                console->log(std:("No files found to validate"));
                process->exit(0);
            }
            console->log(std:("Validating ") + resolvedFiles->get_length() + std:(" file(s)..."));
            auto results = std::async([=]() { validateFiles(resolvedFiles); });
            auto filteredResults = (options["quiet"]) ? results->map([=](auto r) mutable
            {
                return (utils::assign(object{
                    , 
                    object::pair{std:("warnings"), array<any>()}
                }, r));
            }
            ) : results;
            auto output = formatValidationResults(filteredResults);
            console->log(output);
            auto hasErrors = results->some([=](auto r) mutable
            {
                return r->violations->some([=](auto v) mutable
                {
                    return v["severity"] == std:("error");
                }
                );
            }
            );
            if (AND((options["strict"]), (hasErrors))) {
                process->exit(1);
            }
        }
        catch (const any& error)
        {
            console->error(std:("Validation error:"), error);
            process->exit(1);
        }
    }
    );
    program->command(std:("list-rules"))->description(std:("List all available validation rules"))->option(std:("-l, --language <lang>"), std:("Filter by language (python or typescript)"))->action([=](auto options) mutable
    {
        auto languages = (options["language"]) ? array<string>{ options["language"] } : array<string>{ std:("python"), std:("typescript") };
        auto& __array2586_3224 = languages;
        for (auto __indx2586_3224 = 0_N; __indx2586_3224 < __array2586_3224->get_length(); __indx2586_3224++)
        {
            auto& lang = const_(__array2586_3224)[__indx2586_3224];
            {
                console->log(std:("\
") + lang->toUpperCase() + std:(" Rules:"));
                console->log(std:("=")->repeat(50));
                auto rules = getApplicableRules(std:("dummy.") + ((lang == std:("python")) ? std:("py") : std:("ts")), as<any>(lang));
                auto& __array2831_3218 = rules;
                for (auto __indx2831_3218 = 0_N; __indx2831_3218 < __array2831_3218->get_length(); __indx2831_3218++)
                {
                    auto& rule = const_(__array2831_3218)[__indx2831_3218];
                    {
                        console->log(std:("\
") + rule->id + std:(":"));
                        console->log(std:("  Category: ") + rule->category + string_empty);
                        console->log(std:("  Rule: ") + rule->rule + string_empty);
                        console->log(std:("  Enforcement: ") + rule->enforcement + string_empty);
                        if (AND((rule->alternatives), (rule->alternatives->get_length() > 0))) {
                            console->log(std:("  Alternatives: ") + rule->alternatives->join(std:(", ")) + string_empty);
                        }
                    }
                }
            }
        }
    }
    );
    program->command(std:("export"))->description(std:("Export rules to Cursor IDE format"))->option(std:("-o, --output <dir>"), std:("Output directory"), std:(".cursor/rules"))->action([=](auto options) mutable
    {
        try
        {
            auto outputDir = path->resolve(OR((options["output"]), (std:(".cursor/rules"))));
            std::async([=]() { fs::promises::mkdir(outputDir, object{
                object::pair{std:("recursive"), true}
            }); });
            auto rules = exportAllRulesToCursor();
            for (auto& [filename, content] : Object->entries(rules))
            {
                auto filePath = path->join(outputDir, filename);
                std::async([=]() { fs::promises::writeFile(filePath, content, std:("utf-8")); });
                console->log(std:("Exported: ") + filePath + string_empty);
            }
            console->log(std:("\
Successfully exported ") + Object->keys(rules)->get_length() + std:(" rule file(s) to ") + outputDir + string_empty);
        }
        catch (const any& error)
        {
            console->error(std:("Export error:"), error);
            process->exit(1);
        }
    }
    );
    program->command(std:("info"))->description(std:("Show information about SWE-agent project structure"))->action([=]() mutable
    {
        console->log(std:("\
SWE-agent Project Structure"));
        console->log(std:("=")->repeat(50));
        console->log(std:("\
Main Entry Points:"));
        console->log(std:("  - sweagent/run/run_single.py: Single agent instances"));
        console->log(std:("  - sweagent/run/run_batch.py: Batch processing/benchmarking"));
        console->log(std:("\
Core Components:"));
        console->log(std:("  - Agent Class: sweagent/agent/agents.py"));
        console->log(std:("  - Environment: sweagent/environment/swe_env.py"));
        console->log(std:("  - Execution: Sandboxed Docker containers via SWE-ReX"));
        console->log(std:("\
Tools:"));
        console->log(std:("  - Location: tools/ directory"));
        console->log(std:("  - Organization: Bundled and deployed to containers"));
        console->log(std:("\
Inspectors:"));
        console->log(std:("  - CLI: inspector_cli.py"));
        console->log(std:("  - Web: sweagent/inspector/server.py"));
        console->log(std:("\
TypeScript Conversion:"));
        console->log(std:("  - Source: src/ directory"));
        console->log(std:("  - Status: In progress"));
    }
    );
    program->parse(process->argv);
    if (!process->argv->slice(2)->get_length()) {
        program->outputHelp();
    }
}

MAIN
