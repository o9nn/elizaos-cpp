#include "config.hpp"

string exportToCursorFormat(std::shared_ptr<Rule> rule)
{
    auto frontmatter = (array<string>{ std:("---"), std:("description: ") + (OR((rule->description), (string_empty))) + string_empty, std:("globs: ") + (OR((rule->globs->join(std:(", "))), (string_empty))) + string_empty, std:("alwaysApply: ") + rule->alwaysApply + string_empty, std:("---"), string_empty })->join(std:("\
"));
    auto content = string_empty;
    if (type_of(rule->content) == std:("string")) {
        content = rule->content;
    } else if (rule->content) {
        if (rule->content->title) {
            content += std:("# ") + rule->content->title + std:("\
\
");
        }
        if (rule->content->overview) {
            content += string_empty + rule->content->overview + std:("\
\
");
        }
        if (rule->content->guidelines) {
            content += rule->content->guidelines->map([=](auto g) mutable
            {
                return std:("- ") + g + string_empty;
            }
            )->join(std:("\
"));
        }
        if (rule->content->projectStructure) {
            content += std:("\
\
Project Structure:\
");
            content += std:("- Main entry points: ") + rule->content->projectStructure->mainEntryPoints->map([=](auto e) mutable
            {
                return e->path;
            }
            )->join(std:(", ")) + std:("\
");
            content += std:("- Main class: ") + rule->content->projectStructure->mainClass->name + std:(" (") + rule->content->projectStructure->mainClass->path + std:(")\
");
            content += std:("- Execution: ") + rule->content->projectStructure->executionEnvironment->description + std:("\
");
            content += std:("- Tools: Located in ") + rule->content->projectStructure->tools->location + std:("\
");
            content += std:("- Inspectors: ") + rule->content->projectStructure->inspectors->map([=](auto i) mutable
            {
                return i->name;
            }
            )->join(std:(", ")) + std:("\
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
        exported[string_empty + rule->name + std:(".mdc")] = exportToCursorFormat(rule);
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
        if (OR((glob == std:("*")), (filePath->includes(glob->replace(std:("*"), string_empty))))) {
            return true;
        }
    }
    return false;
};


array<std::shared_ptr<Rule>> CURSOR_RULES = array<std::shared_ptr<Rule>>{ object{
    object::pair{std:("name"), std:("general")}, 
    object::pair{std:("description"), std:("General coding guidelines")}, 
    object::pair{std:("globs"), array<any>()}, 
    object::pair{std:("alwaysApply"), true}, 
    object::pair{std:("content"), object{
        object::pair{std:("title"), std:("General Coding Rules")}, 
        object::pair{std:("guidelines"), array<string>{ std:("Use python with type annotations"), std:("Target python 3.11 or higher"), std:("Use pathlib instead of os.path. Also use Path.read_text() over with/* spread: open */() constructs"), std:("Use argparse to add interfaces"), std:("Keep code comments to a minimum and only highlight particularly logically challenging things"), std:("Do not append to the README unless specifically requested") }}
    }}
}, object{
    object::pair{std:("name"), std:("project-overview")}, 
    object::pair{std:("description"), std:("SWE-agent project structure and overview")}, 
    object::pair{std:("globs"), array<any>()}, 
    object::pair{std:("alwaysApply"), true}, 
    object::pair{std:("content"), object{
        object::pair{std:("title"), std:("SWE-agent Overview")}, 
        object::pair{std:("overview"), std:("SWE-agent implements an AI software engineering agent that uses language models to fix github issues.")}, 
        object::pair{std:("projectStructure"), PROJECT_STRUCTURE}
    }}
} };
std::shared_ptr<RulesConfig> RULES_CONFIG = object{
    object::pair{std:("general"), GENERAL_CODING_GUIDELINES}, 
    object::pair{std:("projectOverview"), PROJECT_STRUCTURE}, 
    object::pair{std:("cursorRules"), CURSOR_RULES}
};

void Main(void)
{
}

MAIN
