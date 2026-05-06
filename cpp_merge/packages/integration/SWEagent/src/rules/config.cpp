#include "config.hpp"
#include <string>

string exportToCursorFormat(std::shared_ptr<Rule> rule)
{
    auto frontmatter = (array<string>{ std::string("---"), std::string("description: ") + (OR((rule->description), (string_empty))) + string_empty, std::string("globs: ") + (OR((rule->globs->join(std::string(", "))), (string_empty))) + string_empty, std::string("alwaysApply: ") + rule->alwaysApply + string_empty, std::string("---"), string_empty })->join(std::string("\
"));
    auto content = string_empty;
    if (type_of(rule->content) == std::string("string")) {
        content = rule->content;
    } else if (rule->content) {
        if (rule->content->title) {
            content += std::string("# ") + rule->content->title + std::string("\
\
");
        }
        if (rule->content->overview) {
            content += string_empty + rule->content->overview + std::string("\
\
");
        }
        if (rule->content->guidelines) {
            content += rule->content->guidelines->map([=](auto g) mutable
            {
                return std::string("- ") + g + string_empty;
            }
            )->join(std::string("\
"));
        }
        if (rule->content->projectStructure) {
            content += std::string("\
\
Project Structure:\
");
            content += std::string("- Main entry points: ") + rule->content->projectStructure->mainEntryPoints->map([=](auto e) mutable
            {
                return e->path;
            }
            )->join(std::string(", ")) + std::string("\
");
            content += std::string("- Main class: ") + rule->content->projectStructure->mainClass->name + std::string(" (") + rule->content->projectStructure->mainClass->path + std::string(")\
");
            content += std::string("- Execution: ") + rule->content->projectStructure->executionEnvironment->description + std::string("\
");
            content += std::string("- Tools: Located in ") + rule->content->projectStructure->tools->location + std::string("\
");
            content += std::string("- Inspectors: ") + rule->content->projectStructure->inspectors->map([=](auto i) mutable
            {
                return i->name;
            }
            )->join(std::string(", ")) + std::string("\
");
        }
    }
    return frontmatter + content;
};


Record<string, string> exportAllRulesToCursor()
{
    auto exported = object{};
    for (auto& rule : CURSOR_RULES)
    {
        exported[string_empty + rule->name + std::string(".mdc")] = exportToCursorFormat(rule);
    }
    return exported;
};


boolean shouldApplyRules(string filePath, std::shared_ptr<Rule> rule)
{
    if (rule->alwaysApply) {
        return true;
    }
    if (OR((!rule->globs), (rule->globs->get_length() == 0))) {
        return false;
    }
    for (auto& glob : rule->globs)
    {
        if (OR((glob == std::string("*")), (filePath->includes(glob->replace(std::string("*"), string_empty))))) {
            return true;
        }
    }
    return false;
};


array<std::shared_ptr<Rule>> CURSOR_RULES = array<std::shared_ptr<Rule>>{ object{
    object::pair{std::string("name"), std::string("general")}, 
    object::pair{std::string("description"), std::string("General coding guidelines")}, 
    object::pair{std::string("globs"), array<any>()}, 
    object::pair{std::string("alwaysApply"), true}, 
    object::pair{std::string("content"), object{
        object::pair{std::string("title"), std::string("General Coding Rules")}, 
        object::pair{std::string("guidelines"), array<string>{ std::string("Use python with type annotations"), std::string("Target python 3.11 or higher"), std::string("Use pathlib instead of os.path. Also use Path.read_text() over with/* spread: open */() constructs"), std::string("Use argparse to add interfaces"), std::string("Keep code comments to a minimum and only highlight particularly logically challenging things"), std::string("Do not append to the README unless specifically requested") }}
    }}
}, object{
    object::pair{std::string("name"), std::string("project-overview")}, 
    object::pair{std::string("description"), std::string("SWE-agent project structure and overview")}, 
    object::pair{std::string("globs"), array<any>()}, 
    object::pair{std::string("alwaysApply"), true}, 
    object::pair{std::string("content"), object{
        object::pair{std::string("title"), std::string("SWE-agent Overview")}, 
        object::pair{std::string("overview"), std::string("SWE-agent implements an AI software engineering agent that uses language models to fix github issues.")}, 
        object::pair{std::string("projectStructure"), PROJECT_STRUCTURE}
    }}
} };
std::shared_ptr<RulesConfig> RULES_CONFIG = object{
    object::pair{std::string("general"), GENERAL_CODING_GUIDELINES}, 
    object::pair{std::string("projectOverview"), PROJECT_STRUCTURE}, 
    object::pair{std::string("cursorRules"), CURSOR_RULES}
};

void Main(void)
{
}

MAIN
