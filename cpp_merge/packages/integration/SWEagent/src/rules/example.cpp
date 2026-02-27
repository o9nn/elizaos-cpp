#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/src/rules/example.h"

void validatePythonExample()
{
    console->log(std:("== Python Validation Example =="));
    auto pythonCode = std:("\
def process_data(data):\
    # This function needs type annotations\
    with open('data.txt') as f:\
        content = f.read()\
    return content\
");
    auto validator = std::make_shared<PythonValidator>();
    auto result = validator->validate(pythonCode, std:("example.py"));
    console->log(std:("Valid:"), result->valid);
    console->log(std:("Violations:"), result->violations);
    console->log(std:("Warnings:"), result->warnings);
};


void validateTypeScriptExample()
{
    console->log(std:("\
== TypeScript Validation Example =="));
    auto tsCode = std:("\
function processData(data: unknown) {\
  const fs = require('fs');\
  const content = fs.readFileSync('data.txt', 'utf-8');\
  return content;\
}\
");
    auto validator = std::make_shared<TypeScriptValidator>();
    auto result = validator->validate(tsCode, std:("example.ts"));
    console->log(std:("Valid:"), result->valid);
    console->log(std:("Violations:"), result->violations);
    console->log(std:("Warnings:"), result->warnings);
};


void projectStructureExample()
{
    console->log(std:("\
== Project Structure Example =="));
    auto agentInfo = getComponentByPath(std:("sweagent/agent/agents.py"));
    console->log(std:("Agent component:"), agentInfo);
    console->log(std:("\
Main entry points:"));
    PROJECT_STRUCTURE->mainEntryPoints->forEach([=](auto entry) mutable
    {
        console->log(std:("  - ") + entry->path + std:(": ") + entry->description + string_empty);
    }
    );
    console->log(std:("\
Inspectors:"));
    PROJECT_STRUCTURE->inspectors->forEach([=](auto inspector) mutable
    {
        console->log(std:("  - ") + inspector->name + std:(" (") + inspector->type + std:("): ") + inspector->path + string_empty);
    }
    );
};


void rulesExample()
{
    console->log(std:("\
== Rules Example =="));
    console->log(std:("Python rules:"));
    auto pythonRules = getApplicableRules(std:("example.py"));
    pythonRules->forEach([=](auto rule) mutable
    {
        console->log(std:("  - ") + rule->id + std:(": ") + rule->rule + string_empty);
    }
    );
    console->log(std:("\
TypeScript rules:"));
    auto tsRules = getApplicableRules(std:("example.ts"));
    tsRules->forEach([=](auto rule) mutable
    {
        console->log(std:("  - ") + rule->id + std:(": ") + rule->rule + string_empty);
    }
    );
};


void exportExample()
{
    console->log(std:("\
== Export Example =="));
    shared cursorRules = exportAllRulesToCursor();
    console->log(std:("Exported rules:"));
    Object->keys(cursorRules)->forEach([=](auto filename) mutable
    {
        console->log(std:("  - ") + filename + std:(" (") + (*const_(cursorRules))[filename]->get_length() + std:(" bytes)"));
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
