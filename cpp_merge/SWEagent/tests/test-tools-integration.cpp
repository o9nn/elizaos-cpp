#include "test-tools-integration.test.h"

void Main(void)
{
    describe(std:("Tools Integration Tests"), [=]() mutable
    {
        shared<string> tmpDir;
        shared toolsDir = path->join(__dirname, std:(".."), std:("tools"));
        beforeAll([=]() mutable
        {
            tmpDir = fs::mkdtempSync(path->join(os::tmpdir(), std:("tools-test-")));
            if (!fs::existsSync(path->join(toolsDir, std:("dist")))) {
                console->log(std:("Building tools..."));
                execSync(std:("npm run build"), object{
                    object::pair{std:("cwd"), toolsDir}
                });
            }
        }
        );
        afterAll([=]() mutable
        {
            if (fs::existsSync(tmpDir)) {
                fs::rmSync(tmpDir, object{
                    object::pair{std:("recursive"), true}
                });
            }
        }
        );
        describe(std:("Registry Tool"), [=]() mutable
        {
            shared registryPath = path->join(toolsDir, std:("bin"), std:("registry"));
            it(std:("should set and get values"), [=]() mutable
            {
                process->env->SWE_AGENT_ENV_FILE = path->join(tmpDir, std:("test-env.json"));
                execSync(string_empty + registryPath + std:(" set TEST_VAR "test value""));
                auto result = execSync(string_empty + registryPath + std:(" get TEST_VAR"))->toString()->trim();
                expect(result)->toBe(std:("test value"));
                process->env.Delete("SWE_AGENT_ENV_FILE");
            }
            );
            it(std:("should handle _read_env and _write_env"), [=]() mutable
            {
                process->env->SWE_AGENT_ENV_FILE = path->join(tmpDir, std:("test-env2.json"));
                auto writeEnv = path->join(toolsDir, std:("bin"), std:("_write_env"));
                auto readEnv = path->join(toolsDir, std:("bin"), std:("_read_env"));
                execSync(string_empty + writeEnv + std:(" MY_VAR "my value""));
                auto result = execSync(string_empty + readEnv + std:(" MY_VAR"))->toString()->trim();
                expect(result)->toBe(std:("my value"));
                auto resultWithDefault = execSync(string_empty + readEnv + std:(" NONEXISTENT "default""))->toString()->trim();
                expect(resultWithDefault)->toBe(std:("default"));
                process->env.Delete("SWE_AGENT_ENV_FILE");
            }
            );
        }
        );
        describe(std:("Search Tools"), [=]() mutable
        {
            shared<string> testFile;
            beforeAll([=]() mutable
            {
                testFile = path->join(tmpDir, std:("search-test.txt"));
                fs::writeFileSync(testFile, std:("line one\
line two with test\
line three\
test line four"));
            }
            );
            it(std:("should search within file"), [=]() mutable
            {
                auto searchFile = path->join(toolsDir, std:("bin"), std:("search_file"));
                auto result = execSync(string_empty + searchFile + std:(" test ") + testFile + string_empty)->toString();
                expect(result)->toContain(std:("Found"));
                expect(result)->toContain(std:("matches for "test""));
                expect(result)->toContain(std:("line two with test"));
                expect(result)->toContain(std:("test line four"));
            }
            );
            it(std:("should find files by pattern"), [=]() mutable
            {
                auto findFile = path->join(toolsDir, std:("bin"), std:("find_file"));
                fs::writeFileSync(path->join(tmpDir, std:("test1.txt")), std:("content"));
                fs::writeFileSync(path->join(tmpDir, std:("test2.txt")), std:("content"));
                fs::writeFileSync(path->join(tmpDir, std:("other.md")), std:("content"));
                auto result = execSync(string_empty + findFile + std:(" "*.txt" ") + tmpDir + string_empty)->toString();
                expect(result)->toContain(std:("test1.txt"));
                expect(result)->toContain(std:("test2.txt"));
                expect(result)->not->toContain(std:("other.md"));
            }
            );
            it(std:("should search directory"), [=]() mutable
            {
                auto searchDir = path->join(toolsDir, std:("bin"), std:("search_dir"));
                fs::writeFileSync(path->join(tmpDir, std:("file1.txt")), std:("contains search term"));
                fs::writeFileSync(path->join(tmpDir, std:("file2.txt")), std:("also has search in it"));
                fs::writeFileSync(path->join(tmpDir, std:("file3.txt")), std:("nothing here"));
                auto result = execSync(string_empty + searchDir + std:(" search ") + tmpDir + string_empty)->toString();
                expect(result)->toContain(std:("file1.txt"));
                expect(result)->toContain(std:("file2.txt"));
                expect(result)->not->toContain(std:("file3.txt"));
            }
            );
        }
        );
        describe(std:("Image Tools"), [=]() mutable
        {
            it(std:("should convert image to base64"), [=]() mutable
            {
                auto viewImage = path->join(toolsDir, std:("bin"), std:("view_image"));
                auto pngData = Buffer::from(std:("iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVR42mNkYPhfDwAChwGA60e6kgAAAABJRU5ErkJggg=="), std:("base64"));
                auto imagePath = path->join(tmpDir, std:("test.png"));
                fs::writeFileSync(imagePath, pngData);
                auto result = execSync(string_empty + viewImage + std:(" ") + imagePath + string_empty)->toString();
                expect(result)->toContain(std:("!["));
                expect(result)->toContain(std:("](data:image/png;base64,"));
            }
            );
        }
        );
        describe(std:("Submit Tool"), [=]() mutable
        {
            it(std:("should create submission markers"), [=]() mutable
            {
                auto submit = path->join(toolsDir, std:("bin"), std:("submit"));
                process->env->ROOT = tmpDir;
                execSync(std:("git init"), object{
                    object::pair{std:("cwd"), tmpDir}
                });
                execSync(std:("git config user.email "test@example.com""), object{
                    object::pair{std:("cwd"), tmpDir}
                });
                execSync(std:("git config user.name "Test User""), object{
                    object::pair{std:("cwd"), tmpDir}
                });
                fs::writeFileSync(path->join(tmpDir, std:("test.txt")), std:("test content"));
                execSync(std:("git add ."), object{
                    object::pair{std:("cwd"), tmpDir}
                });
                auto result = execSync(submit, object{
                    object::pair{std:("maxBuffer"), 10 * 1024 * 1024}
                })->toString();
                expect(result)->toContain(std:("<<SWE_AGENT_SUBMISSION>>"));
                process->env.Delete("ROOT");
            }
            );
        }
        );
        describe(std:("Forfeit Tool"), [=]() mutable
        {
            it(std:("should output forfeit marker"), [=]() mutable
            {
                auto forfeit = path->join(toolsDir, std:("bin"), std:("exit_forfeit"));
                auto result = execSync(forfeit)->toString();
                expect(result)->toContain(std:("###SWE-AGENT-EXIT-FORFEIT###"));
            }
            );
        }
        );
        describe(std:("Windowed Editor Tools"), [=]() mutable
        {
            shared<string> testFile;
            beforeAll([=]() mutable
            {
                testFile = path->join(tmpDir, std:("windowed-test.txt"));
                auto content = Array->from(object{
                    object::pair{std:("length"), 50}
                }, [=](auto _, auto i) mutable
                {
                    return std:("line ") + (i + 1) + string_empty;
                }
                )->join(std:("\
"));
                fs::writeFileSync(testFile, content);
            }
            );
            it(std:("should create new file"), [=]() mutable
            {
                auto create = path->join(toolsDir, std:("bin"), std:("create"));
                auto newFile = path->join(tmpDir, std:("new-file.txt"));
                process->env->SWE_AGENT_ENV_FILE = path->join(tmpDir, std:("windowed-env.json"));
                execSync(string_empty + create + std:(" ") + newFile + string_empty);
                expect(fs::existsSync(newFile))->toBe(true);
                process->env.Delete("SWE_AGENT_ENV_FILE");
            }
            );
            it(std:("should open and display file"), [=]() mutable
            {
                auto open = path->join(toolsDir, std:("bin"), std:("open"));
                process->env->SWE_AGENT_ENV_FILE = path->join(tmpDir, std:("windowed-env2.json"));
                auto result = execSync(string_empty + open + std:(" ") + testFile + string_empty)->toString();
                expect(result)->toContain(std:("Opened"));
                expect(result)->toContain(std:("line 1"));
                process->env.Delete("SWE_AGENT_ENV_FILE");
            }
            );
            it(std:("should navigate with goto"), [=]() mutable
            {
                auto goto = path->join(toolsDir, std:("bin"), std:("goto"));
                process->env->SWE_AGENT_ENV_FILE = path->join(tmpDir, std:("windowed-env3.json"));
                auto open = path->join(toolsDir, std:("bin"), std:("open"));
                execSync(string_empty + open + std:(" ") + testFile + string_empty);
                auto result = execSync(string_empty + goto + std:(" 25"))->toString();
                expect(result)->toContain(std:("line 25"));
                process->env.Delete("SWE_AGENT_ENV_FILE");
            }
            );
        }
        );
        describe(std:("Diff State Tool"), [=]() mutable
        {
            it(std:("should update diff state"), [=]() mutable
            {
                shared diffState = path->join(toolsDir, std:("bin"), std:("_state_diff_state"));
                process->env->ROOT = tmpDir;
                auto gitDir = path->join(tmpDir, std:("git-test"));
                fs::mkdirSync(gitDir, object{
                    object::pair{std:("recursive"), true}
                });
                execSync(std:("git init"), object{
                    object::pair{std:("cwd"), gitDir}
                });
                execSync(std:("git config user.email "test@example.com""), object{
                    object::pair{std:("cwd"), gitDir}
                });
                execSync(std:("git config user.name "Test User""), object{
                    object::pair{std:("cwd"), gitDir}
                });
                fs::writeFileSync(path->join(gitDir, std:("test.txt")), std:("content"));
                process->env->ROOT = gitDir;
                expect([=]() mutable
                {
                    return execSync(diffState);
                }
                )->not->toThrow();
                process->env.Delete("ROOT");
            }
            );
        }
        );
        describe(std:("String Replace Editor"), [=]() mutable
        {
            shared<string> testFile;
            beforeAll([=]() mutable
            {
                testFile = path->join(tmpDir, std:("edit-test.py"));
                fs::writeFileSync(testFile, std:("def hello():\
    print("world")\
    return 42"));
            }
            );
            it(std:("should view file"), [=]() mutable
            {
                auto editor = path->join(toolsDir, std:("bin"), std:("str_replace_editor"));
                auto result = execSync(string_empty + editor + std:(" view ") + testFile + string_empty)->toString();
                expect(result)->toContain(std:("def hello():"));
                expect(result)->toContain(std:("print("world")"));
            }
            );
            it(std:("should create new file"), [=]() mutable
            {
                auto editor = path->join(toolsDir, std:("bin"), std:("str_replace_editor"));
                auto newFile = path->join(tmpDir, std:("new-edit-file.py"));
                execSync(string_empty + editor + std:(" create ") + newFile + std:(" --file-text "# New file\nprint('created')""));
                expect(fs::existsSync(newFile))->toBe(true);
                auto content = fs::readFileSync(newFile, std:("utf-8"));
                expect(content)->toContain(std:("# New file"));
                expect(content)->toContain(std:("print('created')"));
            }
            );
        }
        );
    }
    );
    describe(std:("Tool Executability"), [=]() mutable
    {
        shared toolsDir = path->join(__dirname, std:(".."), std:("tools"), std:("bin"));
        it(std:("should have all tools as executable scripts"), [=]() mutable
        {
            auto expectedTools = array<string>{ std:("registry"), std:("_read_env"), std:("_write_env"), std:("_state_diff_state"), std:("str_replace_editor"), std:("view_image"), std:("search_file"), std:("search_dir"), std:("find_file"), std:("submit"), std:("exit_forfeit"), std:("filemap"), std:("open"), std:("goto"), std:("scroll_up"), std:("scroll_down"), std:("create"), std:("edit") };
            expectedTools->forEach([=](auto tool) mutable
            {
                auto toolPath = path->join(toolsDir, tool);
                expect(fs::existsSync(toolPath))->toBe(true);
                if (process->platform != std:("win32")) {
                    auto stats = fs::statSync(toolPath);
                    expect(std::bit_and()(stats->mode, 64))->toBeTruthy();
                }
            }
            );
        }
        );
        it(std:("should have proper shebang in all tools"), [=]() mutable
        {
            auto tools = fs::readdirSync(toolsDir);
            tools->forEach([=](auto tool) mutable
            {
                if (tool == std:("setup.sh")) {
                    return;
                }
                auto toolPath = path->join(toolsDir, tool);
                if (fs::statSync(toolPath)->isFile()) {
                    auto content = fs::readFileSync(toolPath, std:("utf-8"));
                    expect(content)->toMatch((new RegExp(std:("^#!\/usr\/bin\/env nod"))));
                }
            }
            );
        }
        );
    }
    );
}

MAIN
