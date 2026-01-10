#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/scripts/generate-unit-tests.h"

std::shared_ptr<Promise<array<string>>> findAllSourceFiles(string dir, array<string> files)
{
    auto entries = std::async([=]() { readdir(dir); });
    for (auto& entry : entries)
    {
        auto fullPath = join(dir, entry);
        auto stats = std::async([=]() { stat(fullPath); });
        if (stats->isDirectory()) {
            if (AND((AND((!entry->includes(std::string("node_modules"))), (!entry->includes(std::string("dist"))))), (!entry->includes(std::string("test"))))) {
                std::async([=]() { findAllSourceFiles(fullPath, files); });
            }
        } else if (AND((AND((AND((entry->endsWith(std::string(".ts"))), (!entry->endsWith(std::string(".test.ts"))))), (!entry->endsWith(std::string(".spec.ts"))))), (!entry->endsWith(std::string(".d.ts"))))) {
            files->push(fullPath);
        }
    }
    return files;
};


string getTestPath(string sourcePath, string category)
{
    auto relativePath = relative(join(process->cwd(), std::string("src")), sourcePath);
    auto testFileName = basename(sourcePath)->replace(std::string(".ts"), std::string(".test.ts"));
    auto dirPath = dirname(relativePath);
    if (category == std::string("commands")) {
        return join(process->cwd(), std::string("tests/commands"), testFileName);
    } else if (category == std::string("utils")) {
        return join(process->cwd(), std::string("tests/unit/utils"), dirPath->replace(std::string("utils/"), string_empty), testFileName);
    } else if (category == std::string("types")) {
        return join(process->cwd(), std::string("tests/unit/types"), testFileName);
    } else {
        return join(process->cwd(), std::string("tests/unit"), relativePath->replace(std::string(".ts"), std::string(".test.ts")));
    }
};


std::shared_ptr<Promise<string>> generateTestContent(string sourcePath)
{
    auto sourceContent = std::async([=]() { readFile(sourcePath, std::string("utf-8")); });
    auto relativePath = relative(process->cwd(), sourcePath);
    auto importPath = relativePath->replace(std::string("src/"), std::string("../../../src/"))->replace(std::string(".ts"), string_empty);
    auto exportMatches = sourceContent->matchAll((new RegExp(std::string("export\s+(async\s+)?function\s+(\w+)"))));
    auto classMatches = sourceContent->matchAll((new RegExp(std::string("export\s+class\s+(\w+)"))));
    auto constMatches = sourceContent->matchAll((new RegExp(std::string("export\s+const\s+(\w+)"))));
    auto functions = Array->from(exportMatches)->map([=](auto m) mutable
    {
        return const_(m)[2];
    }
    );
    auto classes = Array->from(classMatches)->map([=](auto m) mutable
    {
        return const_(m)[1];
    }
    );
    auto constants = Array->from(constMatches)->map([=](auto m) mutable
    {
        return const_(m)[1];
    }
    );
    auto testContent = std::string("import { describe, it, expect, mock, beforeEach, afterEach } from 'bun:test';\
\
// TODO: Import the functions/classes to test\
// import { ") + (array<any>{ functions, classes, constants })->join(std::string(", ")) + std::string(" } from '") + importPath + std::string("';\
\
describe('") + basename(sourcePath)->replace(std::string(".ts"), string_empty) + std::string("', () => {\
  beforeEach(() => {\
    mock.restore();\
  });\
\
  afterEach(() => {\
    mock.restore();\
  });\
");
    for (auto& func : functions)
    {
        testContent += std::string("\
  describe('") + func + std::string("', () => {\
    it.todo('should ") + func + std::string(" correctly');\
    \
    it.todo('should handle errors in ") + func + std::string("');\
  });\
");
    }
    for (auto& cls : classes)
    {
        testContent += std::string("\
  describe('") + cls + std::string("', () => {\
    it.todo('should create instance of ") + cls + std::string("');\
    \
    it.todo('should test ") + cls + std::string(" methods');\
  });\
");
    }
    for (auto& cnst : constants)
    {
        testContent += std::string("\
  describe('") + cnst + std::string("', () => {\
    it.todo('should export ") + cnst + std::string(" correctly');\
  });\
");
    }
    if (AND((AND((functions->get_length() == 0), (classes->get_length() == 0))), (constants->get_length() == 0))) {
        testContent += std::string("\
  it.todo('should test module functionality');\
");
    }
    testContent += std::string("});");
    return testContent;
};


void main()
{
    console->log(std::string("🧪 Generating Unit Test Skeletons for elizaOS CLI...\
"));
    auto srcPath = join(process->cwd(), std::string("src"));
    auto sourceFiles = std::async([=]() { findAllSourceFiles(srcPath); });
    auto filesToTest = array<std::shared_ptr<FileToTest>>();
    auto skipped = 0;
    for (auto& sourcePath : sourceFiles)
    {
        auto category = categorizeFile(sourcePath);
        auto testPath = getTestPath(sourcePath, category);
        if (existsSync(testPath)) {
            skipped++;
            continue;
        }
        filesToTest->push(object{
            object::pair{std::string("sourcePath"), std::string("sourcePath")}, 
            object::pair{std::string("testPath"), std::string("testPath")}, 
            object::pair{std::string("category"), std::string("category")}
        });
    }
    console->log(std::string("📊 Found ") + sourceFiles->get_length() + std::string(" source files"));
    console->log(std::string("   ") + skipped + std::string(" already have tests"));
    console->log(std::string("   ") + filesToTest->get_length() + std::string(" need test files\
"));
    if (filesToTest->get_length() == 0) {
        console->log(std::string("✅ All files already have tests!"));
        return std::shared_ptr<Promise<void>>();
    }
    console->log(std::string("Would you like to generate test skeletons for all untested files?"));
    console->log(std::string("(This will create .todo tests that you can implement later)\
"));
    auto answer = prompt(std::string("Generate tests? (y/n): "));
    if (answer->toLowerCase() != std::string("y")) {
        console->log(std::string("Cancelled."));
        return std::shared_ptr<Promise<void>>();
    }
    auto created = 0;
    auto failed = 0;
    for (auto& file : filesToTest)
    {
        try
        {
            auto testDir = dirname(file->testPath);
            std::async([=]() { mkdir(testDir, object{
                object::pair{std::string("recursive"), true}
            }); });
            auto testContent = std::async([=]() { generateTestContent(file->sourcePath); });
            std::async([=]() { writeFile(file->testPath, testContent); });
            console->log(std::string("✅ Created: ") + relative(process->cwd(), file->testPath) + string_empty);
            created++;
        }
        catch (const any& error)
        {
            console->error(std::string("❌ Failed: ") + relative(process->cwd(), file->testPath) + string_empty);
            console->error(std::string("   Error: ") + error["message"] + string_empty);
            failed++;
        }
    }
    console->log(std::string("\
📊 Summary:"));
    console->log(std::string("   ✅ Created: ") + created + std::string(" test files"));
    console->log(std::string("   ❌ Failed: ") + failed + std::string(" files"));
    console->log(std::string("\
💡 Next steps:"));
    console->log(std::string("   1. Run 'bun test' to see all .todo tests"));
    console->log(std::string("   2. Implement tests by replacing it.todo with it"));
    console->log(std::string("   3. Run coverage report to track progress"));
};



void Main(void)
{
    main()->_catch(console->error);
}

MAIN
