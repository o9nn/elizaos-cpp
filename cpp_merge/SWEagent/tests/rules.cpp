#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/tests/rules.test.h"

void Main(void)
{
    describe(std:("Rules Module"), [=]() mutable
    {
        describe(std:("PythonValidator"), [=]() mutable
        {
            shared validator = std::make_shared<PythonValidator>();
            test(std:("should detect missing type annotations"), [=]() mutable
            {
                auto code = std:("\
def process_data(data):\
    return data * 2\
");
                auto result = validator->validate(code, std:("test.py"));
                expect(result->valid)->toBe(false);
                expect(result->violations->some([=](auto v) mutable
                {
                    return v->rule == std:("python-type-annotations");
                }
                ))->toBe(true);
            }
            );
            test(std:("should detect os.path usage"), [=]() mutable
            {
                auto code = std:("\
import os.path\
\
def get_file_path(filename: str) -> str:\
    return os.path.join('/tmp', filename)\
");
                auto result = validator->validate(code, std:("test.py"));
                expect(result->valid)->toBe(false);
                expect(result->violations->some([=](auto v) mutable
                {
                    return v->rule == std:("use-pathlib");
                }
                ))->toBe(true);
            }
            );
            test(std:("should detect open() without pathlib"), [=]() mutable
            {
                auto code = std:("\
def read_file(filename: str) -> str:\
    with open(filename, 'r') as f:\
        return f.read()\
");
                auto result = validator->validate(code, std:("test.py"));
                expect(result->valid)->toBe(false);
                expect(result->violations->some([=](auto v) mutable
                {
                    return v->rule == std:("use-pathlib");
                }
                ))->toBe(true);
            }
            );
            test(std:("should pass valid Python code"), [=]() mutable
            {
                auto code = std:("\
from pathlib import Path\
from typing import List\
\
def read_files(filenames: List[str]) -> List[str]:\
    """Read multiple files using pathlib."""\
    results = []\
    for filename in filenames:\
        content = Path(filename).read_text()\
        results.append(content)\
    return results\
");
                auto result = validator->validate(code, std:("test.py"));
                expect(result->valid)->toBe(true);
                expect(result->violations->filter([=](auto v) mutable
                {
                    return v->severity == std:("error");
                }
                ))->toHaveLength(0);
            }
            );
        }
        );
        describe(std:("TypeScriptValidator"), [=]() mutable
        {
            shared validator = std::make_shared<TypeScriptValidator>();
            test(std:("should detect any type usage"), [=]() mutable
            {
                auto code = std:("\
function processData(data) {\
    return data;\
}\
");
                auto result = validator->validate(code, std:("test.ts"));
                expect(result->valid)->toBe(false);
                expect(result->violations->some([=](auto v) mutable
                {
                    return v->rule == std:("explicit-types");
                }
                ))->toBe(true);
            }
            );
            test(std:("should detect synchronous fs usage"), [=]() mutable
            {
                auto code = std:("\
import * as fs from 'fs';\
\
function readFile(path) {\
    return fs.readFileSync(path, 'utf-8');\
}\
");
                auto result = validator->validate(code, std:("test.ts"));
                expect(result->violations->some([=](auto v) mutable
                {
                    return v->rule == std:("node-fs-promises");
                }
                ))->toBe(true);
            }
            );
            test(std:("should pass valid TypeScript code"), [=]() mutable
            {
                auto code = std:("\
import { promises as fs } from 'fs';\
\
/**\
 * Read a file asynchronously\
 */\
async function readFile(path): Promise<string> {\
    return fs.readFile(path, 'utf-8');\
}\
");
                auto result = validator->validate(code, std:("test.ts"));
                expect(result->valid)->toBe(true);
                expect(result->violations->filter([=](auto v) mutable
                {
                    return v->severity == std:("error");
                }
                ))->toHaveLength(0);
            }
            );
        }
        );
        describe(std:("getValidator"), [=]() mutable
        {
            test(std:("should return PythonValidator for python"), [=]() mutable
            {
                auto validator = getValidator(std:("python"));
                expect(validator)->toBeInstanceOf(PythonValidator);
            }
            );
            test(std:("should return TypeScriptValidator for typescript"), [=]() mutable
            {
                auto validator = getValidator(std:("typescript"));
                expect(validator)->toBeInstanceOf(TypeScriptValidator);
            }
            );
        }
        );
        describe(std:("getApplicableRules"), [=]() mutable
        {
            test(std:("should return Python rules for .py files"), [=]() mutable
            {
                auto rules = getApplicableRules(std:("test.py"));
                expect(rules)->toEqual(PYTHON_CODING_RULES);
            }
            );
            test(std:("should return TypeScript rules for .ts files"), [=]() mutable
            {
                auto rules = getApplicableRules(std:("test.ts"));
                expect(rules)->toEqual(TYPESCRIPT_CODING_RULES);
            }
            );
            test(std:("should use provided language parameter"), [=]() mutable
            {
                auto rules = getApplicableRules(std:("test.txt"), std:("python"));
                expect(rules)->toEqual(PYTHON_CODING_RULES);
            }
            );
        }
        );
        describe(std:("Project Structure"), [=]() mutable
        {
            test(std:("should have correct main entry points"), [=]() mutable
            {
                expect(PROJECT_STRUCTURE->mainEntryPoints)->toHaveLength(2);
                expect(const_(PROJECT_STRUCTURE->mainEntryPoints)[0]->path)->toBe(std:("sweagent/run/run_single.py"));
                expect(const_(PROJECT_STRUCTURE->mainEntryPoints)[1]->path)->toBe(std:("sweagent/run/run_batch.py"));
            }
            );
            test(std:("should have correct main class"), [=]() mutable
            {
                expect(PROJECT_STRUCTURE->mainClass->name)->toBe(std:("Agent"));
                expect(PROJECT_STRUCTURE->mainClass->path)->toBe(std:("sweagent/agent/agents.py"));
            }
            );
            test(std:("should have correct execution environment"), [=]() mutable
            {
                expect(PROJECT_STRUCTURE->executionEnvironment->type)->toBe(std:("docker"));
                expect(PROJECT_STRUCTURE->executionEnvironment->interfaceProject)->toBe(std:("SWE-ReX"));
            }
            );
            test(std:("should have correct inspectors"), [=]() mutable
            {
                expect(PROJECT_STRUCTURE->inspectors)->toHaveLength(2);
                auto cliInspector = PROJECT_STRUCTURE->inspectors->find([=](auto i) mutable
                {
                    return i->type == std:("cli");
                }
                );
                auto webInspector = PROJECT_STRUCTURE->inspectors->find([=](auto i) mutable
                {
                    return i->type == std:("web");
                }
                );
                expect(cliInspector)->toBeDefined();
                expect(webInspector)->toBeDefined();
            }
            );
        }
        );
        describe(std:("getComponentByPath"), [=]() mutable
        {
            test(std:("should return correct component for main agent"), [=]() mutable
            {
                auto component = getComponentByPath(std:("sweagent/agent/agents.py"));
                expect(component)->not->toBeNull();
                expect(component["component"])->toBe(std:("main-agent"));
            }
            );
            test(std:("should return correct component for entry point"), [=]() mutable
            {
                auto component = getComponentByPath(std:("sweagent/run/run_single.py"));
                expect(component)->not->toBeNull();
                expect(component["component"])->toBe(std:("entry-point"));
            }
            );
            test(std:("should return correct component for tool"), [=]() mutable
            {
                auto component = getComponentByPath(std:("tools/search/search_file"));
                expect(component)->not->toBeNull();
                expect(component["component"])->toBe(std:("tool"));
            }
            );
            test(std:("should return null for unknown path"), [=]() mutable
            {
                auto component = getComponentByPath(std:("unknown/path.py"));
                expect(component)->toBeNull();
            }
            );
        }
        );
        describe(std:("exportAllRulesToCursor"), [=]() mutable
        {
            test(std:("should rules in Cursor format"), [=]() mutable
            {
                auto exported = exportAllRulesToCursor();
                expect(Object->keys(exported))->toContain(std:("general.mdc"));
                expect(Object->keys(exported))->toContain(std:("project-overview.mdc"));
            }
            );
            test(std:("should include frontmatter in exported rules"), [=]() mutable
            {
                auto exported = exportAllRulesToCursor();
                auto generalRule = (*const_(exported))[std:("general.mdc")];
                expect(generalRule)->toContain(std:("---"));
                expect(generalRule)->toContain(std:("alwaysApply: true"));
            }
            );
        }
        );
        describe(std:("formatValidationResults"), [=]() mutable
        {
            test(std:("should format empty results correctly"), [=]() mutable
            {
                auto results = array<std::shared_ptr<ValidationResult>>();
                auto formatted = formatValidationResults(results);
                expect(formatted)->toBe(std:("All files passed validation!"));
            }
            );
            test(std:("should format violations correctly"), [=]() mutable
            {
                auto results = array<object>{ object{
                    object::pair{std:("valid"), false}, 
                    object::pair{std:("file"), std:("test.py")}, 
                    object::pair{std:("violations"), array<object>{ object{
                        object::pair{std:("rule"), std:("test-rule")}, 
                        object::pair{std:("line"), 10}, 
                        object::pair{std:("message"), std:("Test violation")}, 
                        object::pair{std:("severity"), as<std::shared_ptr<const>>(std:("error"))}
                    } }}, 
                    object::pair{std:("warnings"), array<string>{ std:("Test warning") }}
                } };
                auto formatted = formatValidationResults(results);
                expect(formatted)->toContain(std:("test.py"));
                expect(formatted)->toContain(std:("[ERROR:10]"));
                expect(formatted)->toContain(std:("test-rule"));
                expect(formatted)->toContain(std:("Test violation"));
                expect(formatted)->toContain(std:("[WARNING]"));
                expect(formatted)->toContain(std:("Test warning"));
            }
            );
        }
        );
    }
    );
}

MAIN
