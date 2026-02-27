#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/tests/test-tools-integration.test.h"

void Main(void)
{
    describe(std::string("Tools Integration Tests"), [=]() mutable
    {
        shared<string> tmpDir;
        shared toolsDir = path->join(__dirname, std::string(".."), std::string("tools"));
        beforeAll([=]() mutable
        {
            tmpDir = fs::mkdtempSync(path->join(os::tmpdir(), std::string("tools-test-")));
            if (!fs::existsSync(path->join(toolsDir, std::string("dist")))) {
                console->log(std::string("Building tools..."));
                execSync(std::string("npm run build"), object{
                    object::pair{std::string("cwd"), toolsDir}
                });
            }
        }
        );
        afterAll([=]() mutable
        {
            if (fs::existsSync(tmpDir)) {
                fs::rmSync(tmpDir, object{
                    object::pair{std::string("recursive"), true}
                });
            }
        }
        );
        describe(std::string("Registry Tool"), [=]() mutable
        {
            shared registryPath = path->join(toolsDir, std::string("bin"), std::string("registry"));
            it(std::string("should set and get values"), [=]() mutable
            {
                process->env->SWE_AGENT_ENV_FILE = path->join(tmpDir, std::string("test-env.json"));
                execSync(string_empty + registryPath + std::string(" set TEST_VAR "test value""));
                auto result = execSync(string_empty + registryPath + std::string(" get TEST_VAR"))->toString()->trim();
                expect(result)->toBe(std::string("test value"));
                process->env.Delete("SWE_AGENT_ENV_FILE");
            }
            );
            it(std::string("should handle _read_env and _write_env"), [=]() mutable
            {
                process->env->SWE_AGENT_ENV_FILE = path->join(tmpDir, std::string("test-env2.json"));
                auto writeEnv = path->join(toolsDir, std::string("bin"), std::string("_write_env"));
                auto readEnv = path->join(toolsDir, std::string("bin"), std::string("_read_env"));
                execSync(string_empty + writeEnv + std::string(" MY_VAR "my value""));
                auto result = execSync(string_empty + readEnv + std::string(" MY_VAR"))->toString()->trim();
                expect(result)->toBe(std::string("my value"));
                auto resultWithDefault = execSync(string_empty + readEnv + std::string(" NONEXISTENT "default""))->toString()->trim();
                expect(resultWithDefault)->toBe(std::string("default"));
                process->env.Delete("SWE_AGENT_ENV_FILE");
            }
            );
        }
        );
        describe(std::string("Search Tools"), [=]() mutable
        {
            shared<string> testFile;
            beforeAll([=]() mutable
            {
                testFile = path->join(tmpDir, std::string("search-test.txt"));
                fs::writeFileSync(testFile, std::string("line one\
line two with test\
line three\
test line four"));
            }
            );
            it(std::string("should search within file"), [=]() mutable
            {
                auto searchFile = path->join(toolsDir, std::string("bin"), std::string("search_file"));
                auto result = execSync(string_empty + searchFile + std::string(" test ") + testFile + string_empty)->toString();
                expect(result)->toContain(std::string("Found"));
                expect(result)->toContain(std::string("matches for "test""));
                expect(result)->toContain(std::string("line two with test"));
                expect(result)->toContain(std::string("test line four"));
            }
            );
            it(std::string("should find files by pattern"), [=]() mutable
            {
                auto findFile = path->join(toolsDir, std::string("bin"), std::string("find_file"));
                fs::writeFileSync(path->join(tmpDir, std::string("test1.txt")), std::string("content"));
                fs::writeFileSync(path->join(tmpDir, std::string("test2.txt")), std::string("content"));
                fs::writeFileSync(path->join(tmpDir, std::string("other.md")), std::string("content"));
                auto result = execSync(string_empty + findFile + std::string(" "*.txt" ") + tmpDir + string_empty)->toString();
                expect(result)->toContain(std::string("test1.txt"));
                expect(result)->toContain(std::string("test2.txt"));
                expect(result)->not->toContain(std::string("other.md"));
            }
            );
            it(std::string("should search directory"), [=]() mutable
            {
                auto searchDir = path->join(toolsDir, std::string("bin"), std::string("search_dir"));
                fs::writeFileSync(path->join(tmpDir, std::string("file1.txt")), std::string("contains search term"));
                fs::writeFileSync(path->join(tmpDir, std::string("file2.txt")), std::string("also has search in it"));
                fs::writeFileSync(path->join(tmpDir, std::string("file3.txt")), std::string("nothing here"));
                auto result = execSync(string_empty + searchDir + std::string(" search ") + tmpDir + string_empty)->toString();
                expect(result)->toContain(std::string("file1.txt"));
                expect(result)->toContain(std::string("file2.txt"));
                expect(result)->not->toContain(std::string("file3.txt"));
            }
            );
        }
        );
        describe(std::string("Image Tools"), [=]() mutable
        {
            it(std::string("should convert image to base64"), [=]() mutable
            {
                auto viewImage = path->join(toolsDir, std::string("bin"), std::string("view_image"));
                auto pngData = Buffer::from(std::string("iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVR42mNkYPhfDwAChwGA60e6kgAAAABJRU5ErkJggg=="), std::string("base64"));
                auto imagePath = path->join(tmpDir, std::string("test.png"));
                fs::writeFileSync(imagePath, pngData);
                auto result = execSync(string_empty + viewImage + std::string(" ") + imagePath + string_empty)->toString();
                expect(result)->toContain(std::string("!["));
                expect(result)->toContain(std::string("](data:image/png;base64,"));
            }
            );
        }
        );
        describe(std::string("Submit Tool"), [=]() mutable
        {
            it(std::string("should create submission markers"), [=]() mutable
            {
                auto submit = path->join(toolsDir, std::string("bin"), std::string("submit"));
                process->env->ROOT = tmpDir;
                execSync(std::string("git init"), object{
                    object::pair{std::string("cwd"), tmpDir}
                });
                execSync(std::string("git config user.email "test@example.com""), object{
                    object::pair{std::string("cwd"), tmpDir}
                });
                execSync(std::string("git config user.name "Test User""), object{
                    object::pair{std::string("cwd"), tmpDir}
                });
                fs::writeFileSync(path->join(tmpDir, std::string("test.txt")), std::string("test content"));
                execSync(std::string("git add ."), object{
                    object::pair{std::string("cwd"), tmpDir}
                });
                auto result = execSync(submit, object{
                    object::pair{std::string("maxBuffer"), 10 * 1024 * 1024}
                })->toString();
                expect(result)->toContain(std::string("<<SWE_AGENT_SUBMISSION>>"));
                process->env.Delete("ROOT");
            }
            );
        }
        );
        describe(std::string("Forfeit Tool"), [=]() mutable
        {
            it(std::string("should output forfeit marker"), [=]() mutable
            {
                auto forfeit = path->join(toolsDir, std::string("bin"), std::string("exit_forfeit"));
                auto result = execSync(forfeit)->toString();
                expect(result)->toContain(std::string("###SWE-AGENT-EXIT-FORFEIT###"));
            }
            );
        }
        );
        describe(std::string("Windowed Editor Tools"), [=]() mutable
        {
            shared<string> testFile;
            beforeAll([=]() mutable
            {
                testFile = path->join(tmpDir, std::string("windowed-test.txt"));
                auto content = Array->from(object{
                    object::pair{std::string("length"), 50}
                }, [=](auto _, auto i) mutable
                {
                    return std::string("line ") + (i + 1) + string_empty;
                }
                )->join(std::string("\
"));
                fs::writeFileSync(testFile, content);
            }
            );
            it(std::string("should create new file"), [=]() mutable
            {
                auto create = path->join(toolsDir, std::string("bin"), std::string("create"));
                auto newFile = path->join(tmpDir, std::string("new-file.txt"));
                process->env->SWE_AGENT_ENV_FILE = path->join(tmpDir, std::string("windowed-env.json"));
                execSync(string_empty + create + std::string(" ") + newFile + string_empty);
                expect(fs::existsSync(newFile))->toBe(true);
                process->env.Delete("SWE_AGENT_ENV_FILE");
            }
            );
            it(std::string("should open and display file"), [=]() mutable
            {
                auto open = path->join(toolsDir, std::string("bin"), std::string("open"));
                process->env->SWE_AGENT_ENV_FILE = path->join(tmpDir, std::string("windowed-env2.json"));
                auto result = execSync(string_empty + open + std::string(" ") + testFile + string_empty)->toString();
                expect(result)->toContain(std::string("Opened"));
                expect(result)->toContain(std::string("line 1"));
                process->env.Delete("SWE_AGENT_ENV_FILE");
            }
            );
            it(std::string("should navigate with goto"), [=]() mutable
            {
                auto goto = path->join(toolsDir, std::string("bin"), std::string("goto"));
                process->env->SWE_AGENT_ENV_FILE = path->join(tmpDir, std::string("windowed-env3.json"));
                auto open = path->join(toolsDir, std::string("bin"), std::string("open"));
                execSync(string_empty + open + std::string(" ") + testFile + string_empty);
                auto result = execSync(string_empty + goto + std::string(" 25"))->toString();
                expect(result)->toContain(std::string("line 25"));
                process->env.Delete("SWE_AGENT_ENV_FILE");
            }
            );
        }
        );
        describe(std::string("Diff State Tool"), [=]() mutable
        {
            it(std::string("should update diff state"), [=]() mutable
            {
                shared diffState = path->join(toolsDir, std::string("bin"), std::string("_state_diff_state"));
                process->env->ROOT = tmpDir;
                auto gitDir = path->join(tmpDir, std::string("git-test"));
                fs::mkdirSync(gitDir, object{
                    object::pair{std::string("recursive"), true}
                });
                execSync(std::string("git init"), object{
                    object::pair{std::string("cwd"), gitDir}
                });
                execSync(std::string("git config user.email "test@example.com""), object{
                    object::pair{std::string("cwd"), gitDir}
                });
                execSync(std::string("git config user.name "Test User""), object{
                    object::pair{std::string("cwd"), gitDir}
                });
                fs::writeFileSync(path->join(gitDir, std::string("test.txt")), std::string("content"));
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
        describe(std::string("String Replace Editor"), [=]() mutable
        {
            shared<string> testFile;
            beforeAll([=]() mutable
            {
                testFile = path->join(tmpDir, std::string("edit-test.py"));
                fs::writeFileSync(testFile, std::string("def hello():\
    print("world")\
    return 42"));
            }
            );
            it(std::string("should view file"), [=]() mutable
            {
                auto editor = path->join(toolsDir, std::string("bin"), std::string("str_replace_editor"));
                auto result = execSync(string_empty + editor + std::string(" view ") + testFile + string_empty)->toString();
                expect(result)->toContain(std::string("def hello():"));
                expect(result)->toContain(std::string("print("world")"));
            }
            );
            it(std::string("should create new file"), [=]() mutable
            {
                auto editor = path->join(toolsDir, std::string("bin"), std::string("str_replace_editor"));
                auto newFile = path->join(tmpDir, std::string("new-edit-file.py"));
                execSync(string_empty + editor + std::string(" create ") + newFile + std::string(" --file-text "# New file\nprint('created')""));
                expect(fs::existsSync(newFile))->toBe(true);
                auto content = fs::readFileSync(newFile, std::string("utf-8"));
                expect(content)->toContain(std::string("# New file"));
                expect(content)->toContain(std::string("print('created')"));
            }
            );
        }
        );
    }
    );
    describe(std::string("Tool Executability"), [=]() mutable
    {
        shared toolsDir = path->join(__dirname, std::string(".."), std::string("tools"), std::string("bin"));
        it(std::string("should have all tools as executable scripts"), [=]() mutable
        {
            auto expectedTools = array<string>{ std::string("registry"), std::string("_read_env"), std::string("_write_env"), std::string("_state_diff_state"), std::string("str_replace_editor"), std::string("view_image"), std::string("search_file"), std::string("search_dir"), std::string("find_file"), std::string("submit"), std::string("exit_forfeit"), std::string("filemap"), std::string("open"), std::string("goto"), std::string("scroll_up"), std::string("scroll_down"), std::string("create"), std::string("edit") };
            expectedTools->forEach([=](auto tool) mutable
            {
                auto toolPath = path->join(toolsDir, tool);
                expect(fs::existsSync(toolPath))->toBe(true);
                if (process->platform != std::string("win32")) {
                    auto stats = fs::statSync(toolPath);
                    expect(std::bit_and()(stats->mode, 64))->toBeTruthy();
                }
            }
            );
        }
        );
        it(std::string("should have proper shebang in all tools"), [=]() mutable
        {
            auto tools = fs::readdirSync(toolsDir);
            tools->forEach([=](auto tool) mutable
            {
                if (tool == std::string("setup.sh")) {
                    return;
                }
                auto toolPath = path->join(toolsDir, tool);
                if (fs::statSync(toolPath)->isFile()) {
                    auto content = fs::readFileSync(toolPath, std::string("utf-8"));
                    expect(content)->toMatch((new RegExp(std::string("^#!\/usr\/bin\/env nod"))));
                }
            }
            );
        }
        );
    }
    );
}

MAIN
