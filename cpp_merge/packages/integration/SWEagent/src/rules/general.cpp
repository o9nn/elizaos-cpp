#include "general.hpp"

object validateAgainstRules(string code, std::shared_ptr<CodingGuidelines> guidelines)
{
    auto violations = array<string>();
    if (guidelines->language == std:("python")) {
        if (AND((code->includes(std:("os.path"))), (!code->includes(std:("pathlib"))))) {
            violations->push(std:("Use pathlib instead of os.path"));
        }
        if (AND((code->match((new RegExp(std:("with\s+open\s*\"))))), (!code->includes(std:("Path"))))) {
            violations->push(std:("Use Path.read_text() over with/* spread: open */() constructs"));
        }
    }
    if (guidelines->language == std:("typescript")) {
        if (code->match((new RegExp(std:(":\s*any(?:\s|$|,|\)"))))) {
            violations->push(std:("Avoid using any type, use explicit types instead"));
        }
    }
    return object{
        object::pair{std:("valid"), violations->get_length() == 0}, 
        object::pair{std:("violations"), std:("violations")}
    };
};


array<std::shared_ptr<CodingRule>> PYTHON_CODING_RULES = array<std::shared_ptr<CodingRule>>{ object{
    object::pair{std:("id"), std:("python-type-annotations")}, 
    object::pair{std:("category"), std:("style")}, 
    object::pair{std:("rule"), std:("Use python with type annotations")}, 
    object::pair{std:("enforcement"), std:("required")}
}, object{
    object::pair{std:("id"), std:("python-version")}, 
    object::pair{std:("category"), std:("style")}, 
    object::pair{std:("rule"), std:("Target python 3.11 or higher")}, 
    object::pair{std:("enforcement"), std:("required")}
}, object{
    object::pair{std:("id"), std:("use-pathlib")}, 
    object::pair{std:("category"), std:("filesystem")}, 
    object::pair{std:("rule"), std:("Use pathlib instead of os.path")}, 
    object::pair{std:("enforcement"), std:("required")}, 
    object::pair{std:("alternatives"), array<string>{ std:("Path.read_text() over with/* spread: open */() constructs") }}
}, object{
    object::pair{std:("id"), std:("use-argparse")}, 
    object::pair{std:("category"), std:("api")}, 
    object::pair{std:("rule"), std:("Use argparse to add interfaces")}, 
    object::pair{std:("enforcement"), std:("required")}
}, object{
    object::pair{std:("id"), std:("minimal-comments")}, 
    object::pair{std:("category"), std:("documentation")}, 
    object::pair{std:("rule"), std:("Keep code comments to a minimum and only highlight particularly logically challenging things")}, 
    object::pair{std:("enforcement"), std:("recommended")}
}, object{
    object::pair{std:("id"), std:("no-readme-append")}, 
    object::pair{std:("category"), std:("documentation")}, 
    object::pair{std:("rule"), std:("Do not append to the README unless specifically requested")}, 
    object::pair{std:("enforcement"), std:("required")}
} };
array<std::shared_ptr<CodingRule>> TYPESCRIPT_CODING_RULES = array<std::shared_ptr<CodingRule>>{ object{
    object::pair{std:("id"), std:("typescript-strict")}, 
    object::pair{std:("category"), std:("style")}, 
    object::pair{std:("rule"), std:("Use strict TypeScript configuration")}, 
    object::pair{std:("enforcement"), std:("required")}
}, object{
    object::pair{std:("id"), std:("explicit-types")}, 
    object::pair{std:("category"), std:("style")}, 
    object::pair{std:("rule"), std:("Use explicit type annotations for function parameters and return types")}, 
    object::pair{std:("enforcement"), std:("required")}
}, object{
    object::pair{std:("id"), std:("node-fs-promises")}, 
    object::pair{std:("category"), std:("filesystem")}, 
    object::pair{std:("rule"), std:("Use fs.promises API for file operations")}, 
    object::pair{std:("enforcement"), std:("required")}, 
    object::pair{std:("alternatives"), array<string>{ std:("fs/promises module") }}
}, object{
    object::pair{std:("id"), std:("commander-or-yargs")}, 
    object::pair{std:("category"), std:("api")}, 
    object::pair{std:("rule"), std:("Use commander.js or yargs for CLI interfaces")}, 
    object::pair{std:("enforcement"), std:("recommended")}, 
    object::pair{std:("alternatives"), array<string>{ std:("commander"), std:("yargs") }}
}, object{
    object::pair{std:("id"), std:("minimal-comments")}, 
    object::pair{std:("category"), std:("documentation")}, 
    object::pair{std:("rule"), std:("Keep code comments to a minimum and only highlight particularly logically challenging things")}, 
    object::pair{std:("enforcement"), std:("recommended")}
}, object{
    object::pair{std:("id"), std:("jsdoc-public-api")}, 
    object::pair{std:("category"), std:("documentation")}, 
    object::pair{std:("rule"), std:("Use JSDoc comments for public APIs and exported functions")}, 
    object::pair{std:("enforcement"), std:("recommended")}
} };
std::shared_ptr<CodingGuidelines> GENERAL_CODING_GUIDELINES = object{
    object::pair{std:("language"), std:("python")}, 
    object::pair{std:("version"), std:("3.11")}, 
    object::pair{std:("rules"), PYTHON_CODING_RULES}
};
std::shared_ptr<CodingGuidelines> TYPESCRIPT_CODING_GUIDELINES = object{
    object::pair{std:("language"), std:("typescript")}, 
    object::pair{std:("version"), std:("5.0")}, 
    object::pair{std:("rules"), TYPESCRIPT_CODING_RULES}
};

void Main(void)
{
}

MAIN
