#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/tests/rules.test.h"

void Main(void)
{
    describe(std::string("Rules Module"), [=]() mutable
    {
        describe(std::string("PythonValidator"), [=]() mutable
        {
            shared validator = std::make_shared<PythonValidator>();
            test(std::string("should detect missing type annotations"), [=]() mutable
            {
                auto code = std::string("\
def process_data(data):\
    return data * 2\
");
                auto result = validator->validate(code, std::string("test.py"));
                expect(result->valid)->toBe(false);
                expect(result->violations->some([=](auto v) mutable
                {
                    return v->rule == std::string("python-type-annotations");
                }
                ))->toBe(true);
            }
            );
            test(std::string("should detect os.path usage"), [=]() mutable
            {
                auto code = std::string("\
import os.path\
\
def get_file_path(filename: str) -> str:\
    return os.path.join('/tmp', filename)\
");
                auto result = validator->validate(code, std::string("test.py"));
                expect(result->valid)->toBe(false);
                expect(result->violations->some([=](auto v) mutable
                {
                    return v->rule == std::string("use-pathlib");
                }
                ))->toBe(true);
            }
            );
            test(std::string("should detect open() without pathlib"), [=]() mutable
            {
                auto code = std::string("\
def read_file(filename: str) -> str:\
    with open(filename, 'r') as f:\
        return f.read()\
");
                auto result = validator->validate(code, std::string("test.py"));
                expect(result->valid)->toBe(false);
                expect(result->violations->some([=](auto v) mutable
                {
                    return v->rule == std::string("use-pathlib");
                }
                ))->toBe(true);
            }
            );
            test(std::string("should pass valid Python code"), [=]() mutable
            {
                auto code = std::string("\
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
                auto result = validator->validate(code, std::string("test.py"));
                expect(result->valid)->toBe(true);
                expect(result->violations->filter([=](auto v) mutable
                {
                    return v->severity == std::string("error");
                }
                ))->toHaveLength(0);
            }
            );
        }
        );
        describe(std::string("TypeScriptValidator"), [=]() mutable
        {
            shared validator = std::make_shared<TypeScriptValidator>();
            test(std::string("should detect any type usage"), [=]() mutable
            {
                auto code = std::string("\
function processData(data: any): any {\
    return data;\
}\
");
                auto result = validator->validate(code, std::string("test.ts"));
                expect(result->valid)->toBe(false);
                expect(result->violations->some([=](auto v) mutable
                {
                    return v->rule == std::string("explicit-types");
                }
                ))->toBe(true);
            }
            );
            test(std::string("should detect synchronous fs usage"), [=]() mutable
            {
                auto code = std::string("\
import * as fs from 'fs';\
\
function readFile(path: string): string {\
    return fs.readFileSync(path, 'utf-8');\
}\
");
                auto result = validator->validate(code, std::string("test.ts"));
                expect(result->violations->some([=](auto v) mutable
                {
                    return v->rule == std::string("node-fs-promises");
                }
                ))->toBe(true);
            }
            );
            test(std::string("should pass valid TypeScript code"), [=]() mutable
            {
                auto code = std::string("\
import { promises as fs } from 'fs';\
\
/**\
 * Read a file asynchronously\
 */\
export async function readFile(path: string): Promise<string> {\
    return await fs.readFile(path, 'utf-8');\
}\
");
                auto result = validator->validate(code, std::string("test.ts"));
                expect(result->valid)->toBe(true);
                expect(result->violations->filter([=](auto v) mutable
                {
                    return v->severity == std::string("error");
                }
                ))->toHaveLength(0);
            }
            );
        }
        );
        describe(std::string("getValidator"), [=]() mutable
        {
            test(std::string("should return PythonValidator for python"), [=]() mutable
            {
                auto validator = getValidator(std::string("python"));
                expect(validator)->toBeInstanceOf(PythonValidator);
            }
            );
            test(std::string("should return TypeScriptValidator for typescript"), [=]() mutable
            {
                auto validator = getValidator(std::string("typescript"));
                expect(validator)->toBeInstanceOf(TypeScriptValidator);
            }
            );
        }
        );
        describe(std::string("getApplicableRules"), [=]() mutable
        {
            test(std::string("should return Python rules for .py files"), [=]() mutable
            {
                auto rules = getApplicableRules(std::string("test.py"));
                expect(rules)->toEqual(PYTHON_CODING_RULES);
            }
            );
            test(std::string("should return TypeScript rules for .ts files"), [=]() mutable
            {
                auto rules = getApplicableRules(std::string("test.ts"));
                expect(rules)->toEqual(TYPESCRIPT_CODING_RULES);
            }
            );
            test(std::string("should use provided language parameter"), [=]() mutable
            {
                auto rules = getApplicableRules(std::string("test.txt"), std::string("python"));
                expect(rules)->toEqual(PYTHON_CODING_RULES);
            }
            );
        }
        );
        describe(std::string("Project Structure"), [=]() mutable
        {
            test(std::string("should have correct main entry points"), [=]() mutable
            {
                expect(PROJECT_STRUCTURE->mainEntryPoints)->toHaveLength(2);
                expect(const_(PROJECT_STRUCTURE->mainEntryPoints)[0]->path)->toBe(std::string("sweagent/run/run_single.py"));
                expect(const_(PROJECT_STRUCTURE->mainEntryPoints)[1]->path)->toBe(std::string("sweagent/run/run_batch.py"));
            }
            );
            test(std::string("should have correct main class"), [=]() mutable
            {
                expect(PROJECT_STRUCTURE->mainClass->name)->toBe(std::string("Agent"));
                expect(PROJECT_STRUCTURE->mainClass->path)->toBe(std::string("sweagent/agent/agents.py"));
            }
            );
            test(std::string("should have correct execution environment"), [=]() mutable
            {
                expect(PROJECT_STRUCTURE->executionEnvironment->type)->toBe(std::string("docker"));
                expect(PROJECT_STRUCTURE->executionEnvironment->interfaceProject)->toBe(std::string("SWE-ReX"));
            }
            );
            test(std::string("should have correct inspectors"), [=]() mutable
            {
                expect(PROJECT_STRUCTURE->inspectors)->toHaveLength(2);
                auto cliInspector = PROJECT_STRUCTURE->inspectors->find([=](auto i) mutable
                {
                    return i->type == std::string("cli");
                }
                );
                auto webInspector = PROJECT_STRUCTURE->inspectors->find([=](auto i) mutable
                {
                    return i->type == std::string("web");
                }
                );
                expect(cliInspector)->toBeDefined();
                expect(webInspector)->toBeDefined();
            }
            );
        }
        );
        describe(std::string("getComponentByPath"), [=]() mutable
        {
            test(std::string("should return correct component for main agent"), [=]() mutable
            {
                auto component = getComponentByPath(std::string("sweagent/agent/agents.py"));
                expect(component)->not->toBeNull();
                expect(component["component"])->toBe(std::string("main-agent"));
            }
            );
            test(std::string("should return correct component for entry point"), [=]() mutable
            {
                auto component = getComponentByPath(std::string("sweagent/run/run_single.py"));
                expect(component)->not->toBeNull();
                expect(component["component"])->toBe(std::string("entry-point"));
            }
            );
            test(std::string("should return correct component for tool"), [=]() mutable
            {
                auto component = getComponentByPath(std::string("tools/search/search_file"));
                expect(component)->not->toBeNull();
                expect(component["component"])->toBe(std::string("tool"));
            }
            );
            test(std::string("should return null for unknown path"), [=]() mutable
            {
                auto component = getComponentByPath(std::string("unknown/path.py"));
                expect(component)->toBeNull();
            }
            );
        }
        );
        describe(std::string("exportAllRulesToCursor"), [=]() mutable
        {
            test(std::string("should export rules in Cursor format"), [=]() mutable
            {
                auto exported = exportAllRulesToCursor();
                expect(Object->keys(exported))->toContain(std::string("general.mdc"));
                expect(Object->keys(exported))->toContain(std::string("project-overview.mdc"));
            }
            );
            test(std::string("should include frontmatter in exported rules"), [=]() mutable
            {
                auto exported = exportAllRulesToCursor();
                auto generalRule = (*const_(exported))[std::string("general.mdc")];
                expect(generalRule)->toContain(std::string("---"));
                expect(generalRule)->toContain(std::string("alwaysApply: true"));
            }
            );
        }
        );
        describe(std::string("formatValidationResults"), [=]() mutable
        {
            test(std::string("should format empty results correctly"), [=]() mutable
            {
                auto results = array<std::shared_ptr<ValidationResult>>();
                auto formatted = formatValidationResults(results);
                expect(formatted)->toBe(std::string("All files passed validation!"));
            }
            );
            test(std::string("should format violations correctly"), [=]() mutable
            {
                auto results = array<object>{ object{
                    object::pair{std::string("valid"), false}, 
                    object::pair{std::string("file"), std::string("test.py")}, 
                    object::pair{std::string("violations"), array<object>{ object{
                        object::pair{std::string("rule"), std::string("test-rule")}, 
                        object::pair{std::string("line"), 10}, 
                        object::pair{std::string("message"), std::string("Test violation")}, 
                        object::pair{std::string("severity"), as<std::shared_ptr<const>>(std::string("error"))}
                    } }}, 
                    object::pair{std::string("warnings"), array<string>{ std::string("Test warning") }}
                } };
                auto formatted = formatValidationResults(results);
                expect(formatted)->toContain(std::string("test.py"));
                expect(formatted)->toContain(std::string("[ERROR:10]"));
                expect(formatted)->toContain(std::string("test-rule"));
                expect(formatted)->toContain(std::string("Test violation"));
                expect(formatted)->toContain(std::string("[WARNING]"));
                expect(formatted)->toContain(std::string("Test warning"));
            }
            );
        }
        );
    }
    );
}

MAIN
