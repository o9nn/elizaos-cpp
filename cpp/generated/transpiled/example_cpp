#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/src/rules/example.h"

void validatePythonExample()
{
    console->log(std::string("=== Python Validation Example ==="));
    auto pythonCode = std::string("\
def process_data(data):\
    # This function needs type annotations\
    with open('data.txt') as f:\
        content = f.read()\
    return content\
");
    auto validator = std::make_shared<PythonValidator>();
    auto result = validator->validate(pythonCode, std::string("example.py"));
    console->log(std::string("Valid:"), result->valid);
    console->log(std::string("Violations:"), result->violations);
    console->log(std::string("Warnings:"), result->warnings);
};


void validateTypeScriptExample()
{
    console->log(std::string("\
=== TypeScript Validation Example ==="));
    auto tsCode = std::string("\
function processData(data: unknown) {\
  const fs = require('fs');\
  const content = fs.readFileSync('data.txt', 'utf-8');\
  return content;\
}\
");
    auto validator = std::make_shared<TypeScriptValidator>();
    auto result = validator->validate(tsCode, std::string("example.ts"));
    console->log(std::string("Valid:"), result->valid);
    console->log(std::string("Violations:"), result->violations);
    console->log(std::string("Warnings:"), result->warnings);
};


void projectStructureExample()
{
    console->log(std::string("\
=== Project Structure Example ==="));
    auto agentInfo = getComponentByPath(std::string("sweagent/agent/agents.py"));
    console->log(std::string("Agent component:"), agentInfo);
    console->log(std::string("\
Main entry points:"));
    PROJECT_STRUCTURE->mainEntryPoints->forEach([=](auto entry) mutable
    {
        console->log(std::string("  - ") + entry->path + std::string(": ") + entry->description + string_empty);
    }
    );
    console->log(std::string("\
Inspectors:"));
    PROJECT_STRUCTURE->inspectors->forEach([=](auto inspector) mutable
    {
        console->log(std::string("  - ") + inspector->name + std::string(" (") + inspector->type + std::string("): ") + inspector->path + string_empty);
    }
    );
};


void rulesExample()
{
    console->log(std::string("\
=== Rules Example ==="));
    console->log(std::string("Python rules:"));
    auto pythonRules = getApplicableRules(std::string("example.py"));
    pythonRules->forEach([=](auto rule) mutable
    {
        console->log(std::string("  - ") + rule->id + std::string(": ") + rule->rule + string_empty);
    }
    );
    console->log(std::string("\
TypeScript rules:"));
    auto tsRules = getApplicableRules(std::string("example.ts"));
    tsRules->forEach([=](auto rule) mutable
    {
        console->log(std::string("  - ") + rule->id + std::string(": ") + rule->rule + string_empty);
    }
    );
};


void exportExample()
{
    console->log(std::string("\
=== Export Example ==="));
    shared cursorRules = exportAllRulesToCursor();
    console->log(std::string("Exported rules:"));
    Object->keys(cursorRules)->forEach([=](auto filename) mutable
    {
        console->log(std::string("  - ") + filename + std::string(" (") + (*const_(cursorRules))[filename]->get_length() + std::string(" bytes)"));
    }
    );
};


void runExamples()
{
    std::async([=]() { validatePythonExample(); });
    std::async([=]() { validateTypeScriptExample(); });
    projectStructureExample();
    rulesExample();
    exportExample();
};



void Main(void)
{
    if (require->main == module) {
        runExamples()->_catch(console->error);
    }
}

MAIN
