#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/scripts/generate-unit-tests.h"

std::shared_ptr<Promise<array<string>>> findAllSourceFiles(string dir, array<string> files)
{
    auto entries = std::async([=]() { readdir(dir); });
    for (auto& entry : entries)
    {
        auto fullPath = join(dir, entry);
        auto stats = std::async([=]() { stat(fullPath); });
        if (stats->isDirectory()) {
            if (AND((AND((!entry->includes(std:("node_modules"))), (!entry->includes(std:("dist"))))), (!entry->includes(std:("test"))))) {
                std::async([=]() { findAllSourceFiles(fullPath, files); });
            }
        } else if (AND((AND((AND((entry->endsWith(std:(".ts"))), (!entry->endsWith(std:(".test.ts"))))), (!entry->endsWith(std:(".spec.ts"))))), (!entry->endsWith(std:(".d.ts"))))) {
            files->push(fullPath);
        }
    }
    return files;
};


string getTestPath(string sourcePath, string category)
{
    auto relativePath = relative(join(process->cwd(), std:("src")), sourcePath);
    auto testFileName = basename(sourcePath)->replace(std:(".ts"), std:(".test.ts"));
    auto dirPath = dirname(relativePath);
    if (category == std:("commands")) {
        return join(process->cwd(), std:("tests/commands"), testFileName);
    } else if (category == std:("utils")) {
        return join(process->cwd(), std:("tests/unit/utils"), dirPath->replace(std:("utils/"), string_empty), testFileName);
    } else if (category == std:("types")) {
        return join(process->cwd(), std:("tests/unit/types"), testFileName);
    } else {
        return join(process->cwd(), std:("tests/unit"), relativePath->replace(std:(".ts"), std:(".test.ts")));
    }
};


std::shared_ptr<Promise<string>> generateTestContent(string sourcePath)
{
    auto sourceContent = std::async([=]() { readFile(sourcePath, std:("utf-8")); });
    auto relativePath = relative(process->cwd(), sourcePath);
    auto importPath = relativePath->replace(std:("src/"), std:("../../../src/"))->replace(std:(".ts"), string_empty);
    auto exportMatches = sourceContent->matchAll((new RegExp(std:("export\s+(async\s+)?function\s+(\w+)"))));
    auto classMatches = sourceContent->matchAll((new RegExp(std:("export\s+class\s+(\w+)"))));
    auto constMatches = sourceContent->matchAll((new RegExp(std:("export\s+const\s+(\w+)"))));
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
    auto testContent = std:("import { describe, it, expect, mock, beforeEach, afterEach } from 'bun:test';\
\
// TODO: Import the functions/classes to test\
// import { ") + (array<any>{ functions, classes, constants })->join(std:(", ")) + std:(" } from '") + importPath + std:("';\
\
describe('") + basename(sourcePath)->replace(std:(".ts"), string_empty) + std:("', [&]() {\
  beforeEach([&]() {\
    mock.restore();\
  });\
\
  afterEach([&]() {\
    mock.restore();\
  });\
");
    for (auto& func : functions)
    {
        testContent += std:("\
  describe('") + func + std:("', [&]() {\
    it.todo('should ") + func + std:(" correctly');\
    \
    it.todo('should handle errors in ") + func + std:("');\
  });\
");
    }
    for (auto& cls : classes)
    {
        testContent += std:("\
  describe('") + cls + std:("', [&]() {\
    it.todo('should create instance of ") + cls + std:("');\
    \
    it.todo('should test ") + cls + std:(" methods');\
  });\
");
    }
    for (auto& cnst : constants)
    {
        testContent += std:("\
  describe('") + cnst + std:("', [&]() {\
    it.todo('should ") + cnst + std:(" correctly');\
  });\
");
    }
    if (AND((AND((functions->get_length() == 0), (classes->get_length() == 0))), (constants->get_length() == 0))) {
        testContent += std:("\
  it.todo('should test module functionality');\
");
    }
    testContent += std:("});");
    return testContent;
};


void main()
{
    console->log(std:("🧪 Generating Unit Test Skeletons for elizaos CLI...\
"));
    auto srcPath = join(process->cwd(), std:("src"));
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
            object::pair{std:("sourcePath"), std:("sourcePath")}, 
            object::pair{std:("testPath"), std:("testPath")}, 
            object::pair{std:("category"), std:("category")}
        });
    }
    console->log(std:("📊 Found ") + sourceFiles->get_length() + std:(" source files"));
    console->log(std:("   ") + skipped + std:(" already have tests"));
    console->log(std:("   ") + filesToTest->get_length() + std:(" need test files\
"));
    if (filesToTest->get_length() == 0) {
        console->log(std:("✅ All files already have tests!"));
        return std::shared_ptr<Promise<void>>();
    }
    console->log(std:("Would you like to generate test skeletons for all untested files?"));
    console->log(std:("(This will create .todo tests that you can implement later)\
"));
    auto shouldGenerateTests = std::async([=]() { clack->confirm(object{
        object::pair{std:("message"), std:("Generate test skeletons for all untested files?")}, 
        object::pair{std:("initialValue"), true}
    }); });
    if (clack->isCancel(shouldGenerateTests)) {
        clack->cancel(std:("Operation cancelled."));
        return std::shared_ptr<Promise<void>>();
    }
    if (!shouldGenerateTests) {
        clack->outro(std:("Test generation skipped."));
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
                object::pair{std:("recursive"), true}
            }); });
            auto testContent = std::async([=]() { generateTestContent(file->sourcePath); });
            std::async([=]() { writeFile(file->testPath, testContent); });
            console->log(std:("✅ Created: ") + relative(process->cwd(), file->testPath) + string_empty);
            created++;
        }
        catch (const any& error)
        {
            console->error(std:("❌ Failed: ") + relative(process->cwd(), file->testPath) + string_empty);
            console->error(std:("   Error: ") + error["message"] + string_empty);
            failed++;
        }
    }
    console->log(std:("\
📊 Summary:"));
    console->log(std:("   ✅ Created: ") + created + std:(" test files"));
    console->log(std:("   ❌ Failed: ") + failed + std:(" files"));
    console->log(std:("\
💡 Next steps:"));
    console->log(std:("   1. Run 'bun test' to see all .todo tests"));
    console->log(std:("   2. Implement tests by replacing it.todo with it"));
    console->log(std:("   3. Run coverage report to track progress"));
};



void Main(void)
{
    main()->_catch(console->error);
}

MAIN
