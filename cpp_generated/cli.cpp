#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/src/rules/cli.h"

std::shared_ptr<Promise<array<string>>> findProjectFiles()
{
    shared files = array<string>();
    shared extensions = array<string>{ std::string(".py"), std::string(".ts"), std::string(".tsx") };
    auto walk = [=](auto dir) mutable
    {
        auto entries = std::async([=]() { fs::promises::readdir(dir, object{
            object::pair{std::string("withFileTypes"), true}
        }); });
        for (auto& entry : entries)
        {
            auto fullPath = path->join(dir, entry->name);
            if (OR((OR((OR((entry->name->startsWith(std::string("."))), (entry->name == std::string("node_modules")))), (entry->name == std::string("dist")))), (entry->name == std::string("build")))) {
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
            console->warn(std::string("Warning: Could not resolve pattern: ") + pattern + string_empty);
        }
    }
    return Array->from(std::make_shared<Set>(resolved));
};


std::shared_ptr<Promise<array<string>>> findFilesInDir(string dir)
{
    auto files = array<string>();
    auto extensions = array<string>{ std::string(".py"), std::string(".ts"), std::string(".tsx") };
    auto entries = std::async([=]() { fs::promises::readdir(dir, object{
        object::pair{std::string("withFileTypes"), true}
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
    program->name(std::string("swe-rules"))->description(std::string("SWE-agent rules validation and management CLI"))->version(std::string("1.0.0"));
    program->command(std::string("validate"))->description(std::string("Validate files against SWE-agent coding rules"))->argument(std::string("[files...]"), std::string("Files to validate (supports wildcards)"))->option(std::string("-l, --language <lang>"), std::string("Language to validate (python or typescript)"), std::string("auto"))->option(std::string("-s, --strict"), std::string("Exit with error code if validation fails"), false)->option(std::string("-q, --quiet"), std::string("Only show errors, no warnings"), false)->action([=](auto files, auto options) mutable
    {
        try
        {
            if (OR((!files), (files->get_length() == 0))) {
                files = std::async([=]() { findProjectFiles(); });
            }
            auto resolvedFiles = std::async([=]() { resolveFiles(files); });
            if (resolvedFiles->get_length() == 0) {
                console->log(std::string("No files found to validate"));
                process->exit(0);
            }
            console->log(std::string("Validating ") + resolvedFiles->get_length() + std::string(" file(s)..."));
            auto results = std::async([=]() { validateFiles(resolvedFiles); });
            auto filteredResults = (options["quiet"]) ? results->map([=](auto r) mutable
            {
                return (utils::assign(object{
                    , 
                    object::pair{std::string("warnings"), array<any>()}
                }, r));
            }
            ) : results;
            auto output = formatValidationResults(filteredResults);
            console->log(output);
            auto hasErrors = results->some([=](auto r) mutable
            {
                return r->violations->some([=](auto v) mutable
                {
                    return v["severity"] == std::string("error");
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
            console->error(std::string("Validation error:"), error);
            process->exit(1);
        }
    }
    );
    program->command(std::string("list-rules"))->description(std::string("List all available validation rules"))->option(std::string("-l, --language <lang>"), std::string("Filter by language (python or typescript)"))->action([=](auto options) mutable
    {
        auto languages = (options["language"]) ? array<string>{ options["language"] } : array<string>{ std::string("python"), std::string("typescript") };
        auto& __array2586_3224 = languages;
        for (auto __indx2586_3224 = 0_N; __indx2586_3224 < __array2586_3224->get_length(); __indx2586_3224++)
        {
            auto& lang = const_(__array2586_3224)[__indx2586_3224];
            {
                console->log(std::string("\
") + lang->toUpperCase() + std::string(" Rules:"));
                console->log(std::string("=")->repeat(50));
                auto rules = getApplicableRules(std::string("dummy.") + ((lang == std::string("python")) ? std::string("py") : std::string("ts")), as<any>(lang));
                auto& __array2831_3218 = rules;
                for (auto __indx2831_3218 = 0_N; __indx2831_3218 < __array2831_3218->get_length(); __indx2831_3218++)
                {
                    auto& rule = const_(__array2831_3218)[__indx2831_3218];
                    {
                        console->log(std::string("\
") + rule->id + std::string(":"));
                        console->log(std::string("  Category: ") + rule->category + string_empty);
                        console->log(std::string("  Rule: ") + rule->rule + string_empty);
                        console->log(std::string("  Enforcement: ") + rule->enforcement + string_empty);
                        if (AND((rule->alternatives), (rule->alternatives->get_length() > 0))) {
                            console->log(std::string("  Alternatives: ") + rule->alternatives->join(std::string(", ")) + string_empty);
                        }
                    }
                }
            }
        }
    }
    );
    program->command(std::string("export"))->description(std::string("Export rules to Cursor IDE format"))->option(std::string("-o, --output <dir>"), std::string("Output directory"), std::string(".cursor/rules"))->action([=](auto options) mutable
    {
        try
        {
            auto outputDir = path->resolve(OR((options["output"]), (std::string(".cursor/rules"))));
            std::async([=]() { fs::promises::mkdir(outputDir, object{
                object::pair{std::string("recursive"), true}
            }); });
            auto rules = exportAllRulesToCursor();
            for (auto& [filename, content] : Object->entries(rules))
            {
                auto filePath = path->join(outputDir, filename);
                std::async([=]() { fs::promises::writeFile(filePath, content, std::string("utf-8")); });
                console->log(std::string("Exported: ") + filePath + string_empty);
            }
            console->log(std::string("\
Successfully exported ") + Object->keys(rules)->get_length() + std::string(" rule file(s) to ") + outputDir + string_empty);
        }
        catch (const any& error)
        {
            console->error(std::string("Export error:"), error);
            process->exit(1);
        }
    }
    );
    program->command(std::string("info"))->description(std::string("Show information about SWE-agent project structure"))->action([=]() mutable
    {
        console->log(std::string("\
SWE-agent Project Structure"));
        console->log(std::string("=")->repeat(50));
        console->log(std::string("\
Main Entry Points:"));
        console->log(std::string("  - sweagent/run/run_single.py: Single agent instances"));
        console->log(std::string("  - sweagent/run/run_batch.py: Batch processing/benchmarking"));
        console->log(std::string("\
Core Components:"));
        console->log(std::string("  - Agent Class: sweagent/agent/agents.py"));
        console->log(std::string("  - Environment: sweagent/environment/swe_env.py"));
        console->log(std::string("  - Execution: Sandboxed Docker containers via SWE-ReX"));
        console->log(std::string("\
Tools:"));
        console->log(std::string("  - Location: tools/ directory"));
        console->log(std::string("  - Organization: Bundled and deployed to containers"));
        console->log(std::string("\
Inspectors:"));
        console->log(std::string("  - CLI: inspector_cli.py"));
        console->log(std::string("  - Web: sweagent/inspector/server.py"));
        console->log(std::string("\
TypeScript Conversion:"));
        console->log(std::string("  - Source: src/ directory"));
        console->log(std::string("  - Status: In progress"));
    }
    );
    program->parse(process->argv);
    if (!process->argv->slice(2)->get_length()) {
        program->outputHelp();
    }
}

MAIN
