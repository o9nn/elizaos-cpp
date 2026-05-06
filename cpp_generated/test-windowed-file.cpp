#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/tests/test-windowed-file.test.h"

void Main(void)
{
    describe(std::string("WindowedFile"), [=]() mutable
    {
        shared<string> tmpDir;
        shared<string> testFile;
        shared<string> envFile;
        shared<any> windowedFile;
        beforeEach([=]() mutable
        {
            tmpDir = fs::mkdtempSync(path->join(os::tmpdir(), std::string("test-windowed-")));
            testFile = path->join(tmpDir, std::string("test.py"));
            envFile = path->join(tmpDir, std::string(".swe-agent-env"));
            process->env->SWE_AGENT_ENV_FILE = envFile;
            auto content = Array->from(object{
                object::pair{std::string("length"), 100}
            }, [=](auto _, auto i) mutable
            {
                return String(i);
            }
            )->join(std::string("\
"));
            fs::writeFileSync(testFile, content);
            auto registryData = object{
                object::pair{std::string("CURRENT_FILE"), testFile}, 
                object::pair{std::string("FIRST_LINE"), std::string("0")}, 
                object::pair{std::string("WINDOW"), std::string("10")}
            };
            fs::writeFileSync(envFile, JSON->stringify(registryData));
            windowedFile = std::make_shared<WindowedFile>(object{
                object::pair{std::string("path"), testFile}, 
                object::pair{std::string("exitOnException"), false}
            });
            windowedFile["offsetMultiplier"] = 0.25;
        }
        );
        afterEach([=]() mutable
        {
            if (fs::existsSync(tmpDir)) {
                fs::rmSync(tmpDir, object{
                    object::pair{std::string("recursive"), true}
                });
            }
            process->env.Delete("SWE_AGENT_ENV_FILE");
        }
        );
        describe(std::string("Basic operations"), [=]() mutable
        {
            it(std::string("should initialize with correct properties"), [=]() mutable
            {
                expect(windowedFile["firstLine"])->toBe(0);
                expect(windowedFile["window"])->toBe(10);
                expect(windowedFile["nLines"])->toBe(100);
            }
            );
            it(std::string("should calculate line range correctly"), [=]() mutable
            {
                windowedFile["firstLine"] = 10;
                auto [start, end] = windowedFile["lineRange"];
                expect(end - start)->toBe(windowedFile["window"] - 1);
                expect(windowedFile["lineRange"])->toEqual(array<double>{ 10, 19 });
            }
            );
            it(std::string("should get window text"), [=]() mutable
            {
                windowedFile["firstLine"] = 10;
                auto windowText = windowedFile["getWindowText"]();
                expect(windowText)->toContain(std::string("10"));
                expect(windowText)->toContain(std::string("19"));
            }
            );
        }
        );
        describe(std::string("Text replacement"), [=]() mutable
        {
            it(std::string("should replace text in window"), [=]() mutable
            {
                windowedFile["firstLine"] = 10;
                windowedFile["replaceInWindow"](std::string("10"), std::string("Hello, world!"));
                expect(windowedFile["nLines"])->toBe(100);
                expect(windowedFile["lineRange"])->toEqual(array<double>{ 7, 16 });
                auto windowText = windowedFile["getWindowText"]();
                expect(windowText)->toContain(std::string("Hello, world!"));
            }
            );
            it(std::string("should handle multiple replacements"), [=]() mutable
            {
                windowedFile["firstLine"] = 50;
                windowedFile["replaceInWindow"](std::string("50"), std::string("Hello, world!"));
                expect(windowedFile["lineRange"])->toEqual(array<double>{ 47, 56 });
                auto windowText = windowedFile["getWindowText"]();
                expect(windowText)->toContain(std::string("Hello, world!"));
            }
            );
            it(std::string("should throw error when text not found"), [=]() mutable
            {
                expect([=]() mutable
                {
                    windowedFile["replaceInWindow"](std::string("asdf"), std::string("Hello, world!"));
                }
                )->toThrow(std::string("Text not found"));
            }
            );
        }
        );
        describe(std::string("Navigation"), [=]() mutable
        {
            it(std::string("should goto line correctly"), [=]() mutable
            {
                windowedFile["goto"](0, std::string("top"));
                expect(const_(windowedFile["lineRange"])[0])->toBe(0);
                windowedFile["goto"](50, std::string("top"));
                expect(const_(windowedFile["lineRange"])[0])->toBe(48);
                windowedFile["goto"](100, std::string("top"));
                expect(const_(windowedFile["lineRange"])[1])->toBe(99);
            }
            );
            it(std::string("should scroll correctly"), [=]() mutable
            {
                windowedFile["firstLine"] = 10;
                windowedFile["scroll"](10);
                expect(windowedFile["firstLine"])->toBe(20);
                windowedFile["scroll"](-10);
                expect(windowedFile["firstLine"])->toBe(10);
                windowedFile["scroll"](-100);
                expect(windowedFile["firstLine"])->toBe(0);
                windowedFile["scroll"](100);
                expect(const_(windowedFile["lineRange"])[1])->toBe(99);
            }
            );
        }
        );
        describe(std::string("Window output"), [=]() mutable
        {
            it(std::string("should print window with correct format"), [=]() mutable
            {
                windowedFile["firstLine"] = 10;
                auto output = windowedFile["getWindowText"](true, true, true);
                expect(output)->toContain(std::string("[File: ") + testFile + std::string(" (100 lines total)]"));
                expect(output)->toContain(std::string("(10 more lines above)"));
                expect(output)->toContain(std::string("11:10"));
                expect(output)->toContain(std::string("20:19"));
                expect(output)->toContain(std::string("(80 more lines below)"));
            }
            );
            it(std::string("should handle new file with single line"), [=]() mutable
            {
                auto newFile = path->join(tmpDir, std::string("new.py"));
                fs::writeFileSync(newFile, std::string("\
"));
                auto registryData = object{
                    object::pair{std::string("CURRENT_FILE"), newFile}, 
                    object::pair{std::string("FIRST_LINE"), std::string("0")}, 
                    object::pair{std::string("WINDOW"), std::string("10")}
                };
                fs::writeFileSync(envFile, JSON->stringify(registryData));
                auto newWindowedFile = std::make_shared<WindowedFile>(object{
                    object::pair{std::string("path"), newFile}, 
                    object::pair{std::string("exitOnException"), false}
                });
                expect(newWindowedFile->get_nLines())->toBe(1);
                expect(newWindowedFile->get_lineRange())->toEqual(array<double>{ 0, 0 });
                auto output = newWindowedFile->getWindowText(true, true, true);
                expect(output)->toContain(std::string("[File: ") + newFile + std::string(" (1 lines total)]"));
                expect(output)->toContain(std::string("1:"));
            }
            );
        }
        );
        describe(std::string("Text operations"), [=]() mutable
        {
            it(std::string("should find all occurrences"), [=]() mutable
            {
                auto content = std::string("test\
test\
other\
test");
                fs::writeFileSync(testFile, content);
                auto wf = std::make_shared<WindowedFile>(object{
                    object::pair{std::string("path"), testFile}, 
                    object::pair{std::string("exitOnException"), false}
                });
                auto occurrences = wf->findAllOccurrences(std::string("test"), false);
                expect(occurrences)->toEqual(array<double>{ 1, 2, 4 });
                auto zeroBasedOccurrences = wf->findAllOccurrences(std::string("test"), true);
                expect(zeroBasedOccurrences)->toEqual(array<double>{ 0, 1, 3 });
            }
            );
            it(std::string("should handle global replacement"), [=]() mutable
            {
                auto content = std::string("old\
old\
other\
old");
                fs::writeFileSync(testFile, content);
                auto wf = std::make_shared<WindowedFile>(object{
                    object::pair{std::string("path"), testFile}, 
                    object::pair{std::string("exitOnException"), false}
                });
                auto info = wf->replace(std::string("old"), std::string("new"));
                expect(info["nReplacements"])->toBe(3);
                expect(info["firstReplacedLine"])->toBe(1);
                auto newContent = fs::readFileSync(testFile, std::string("utf-8"));
                expect(newContent)->toBe(std::string("new\
new\
other\
new"));
            }
            );
            it(std::string("should handle insertion"), [=]() mutable
            {
                auto content = std::string("line1\
line2\
line3");
                fs::writeFileSync(testFile, content);
                auto wf = std::make_shared<WindowedFile>(object{
                    object::pair{std::string("path"), testFile}, 
                    object::pair{std::string("exitOnException"), false}
                });
                auto info = wf->insert(std::string("inserted"), 1);
                expect(info["firstInsertedLine"])->toBe(1);
                expect(info["nLinesAdded"])->toBe(1);
                auto newContent = fs::readFileSync(testFile, std::string("utf-8"));
                expect(newContent)->toBe(std::string("line1\
inserted\
line2\
line3"));
            }
            );
            it(std::string("should undo edits"), [=]() mutable
            {
                auto originalContent = std::string("original");
                fs::writeFileSync(testFile, originalContent);
                auto wf = std::make_shared<WindowedFile>(object{
                    object::pair{std::string("path"), testFile}, 
                    object::pair{std::string("exitOnException"), false}
                });
                wf->replace(std::string("original"), std::string("modified"));
                expect(fs::readFileSync(testFile, std::string("utf-8")))->toBe(std::string("modified"));
                wf->undoEdit();
                expect(fs::readFileSync(testFile, std::string("utf-8")))->toBe(originalContent);
            }
            );
        }
        );
    }
    );
}

MAIN
