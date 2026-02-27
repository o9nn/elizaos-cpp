#include "test-windowed-file.test.h"

void Main(void)
{
    describe(std:("WindowedFile"), [=]() mutable
    {
        shared<string> tmpDir;
        shared<string> testFile;
        shared<string> envFile;
        shared<any> windowedFile;
        beforeEach([=]() mutable
        {
            tmpDir = fs::mkdtempSync(path->join(os::tmpdir(), std:("test-windowed-")));
            testFile = path->join(tmpDir, std:("test.py"));
            envFile = path->join(tmpDir, std:(".swe-agent-env"));
            process->env->SWE_AGENT_ENV_FILE = envFile;
            auto content = Array->from(object{
                object::pair{std:("length"), 100}
            }, [=](auto _, auto i) mutable
            {
                return String(i);
            }
            )->join(std:("\
"));
            fs::writeFileSync(testFile, content);
            auto registryData = object{
                object::pair{std:("CURRENT_FILE"), testFile}, 
                object::pair{std:("FIRST_LINE"), std:("0")}, 
                object::pair{std:("WINDOW"), std:("10")}
            };
            fs::writeFileSync(envFile, JSON->stringify(registryData));
            windowedFile = std::make_shared<WindowedFile>(object{
                object::pair{std:("path"), testFile}, 
                object::pair{std:("exitOnException"), false}
            });
            windowedFile["offsetMultiplier"] = 0.25;
        }
        );
        afterEach([=]() mutable
        {
            if (fs::existsSync(tmpDir)) {
                fs::rmSync(tmpDir, object{
                    object::pair{std:("recursive"), true}
                });
            }
            process->env.Delete("SWE_AGENT_ENV_FILE");
        }
        );
        describe(std:("Basic operations"), [=]() mutable
        {
            it(std:("should initialize with correct properties"), [=]() mutable
            {
                expect(windowedFile["firstLine"])->toBe(0);
                expect(windowedFile["window"])->toBe(10);
                expect(windowedFile["nLines"])->toBe(100);
            }
            );
            it(std:("should calculate line range correctly"), [=]() mutable
            {
                windowedFile["firstLine"] = 10;
                auto [start, end] = windowedFile["lineRange"];
                expect(end - start)->toBe(windowedFile["window"] - 1);
                expect(windowedFile["lineRange"])->toEqual(array<double>{ 10, 19 });
            }
            );
            it(std:("should get window text"), [=]() mutable
            {
                windowedFile["firstLine"] = 10;
                auto windowText = windowedFile["getWindowText"]();
                expect(windowText)->toContain(std:("10"));
                expect(windowText)->toContain(std:("19"));
            }
            );
        }
        );
        describe(std:("Text replacement"), [=]() mutable
        {
            it(std:("should replace text in window"), [=]() mutable
            {
                windowedFile["firstLine"] = 10;
                windowedFile["replaceInWindow"](std:("10"), std:("Hello, world!"));
                expect(windowedFile["nLines"])->toBe(100);
                expect(windowedFile["lineRange"])->toEqual(array<double>{ 7, 16 });
                auto windowText = windowedFile["getWindowText"]();
                expect(windowText)->toContain(std:("Hello, world!"));
            }
            );
            it(std:("should handle multiple replacements"), [=]() mutable
            {
                windowedFile["firstLine"] = 50;
                windowedFile["replaceInWindow"](std:("50"), std:("Hello, world!"));
                expect(windowedFile["lineRange"])->toEqual(array<double>{ 47, 56 });
                auto windowText = windowedFile["getWindowText"]();
                expect(windowText)->toContain(std:("Hello, world!"));
            }
            );
            it(std:("should throw error when text not found"), [=]() mutable
            {
                expect([=]() mutable
                {
                    windowedFile["replaceInWindow"](std:("asdf"), std:("Hello, world!"));
                }
                )->toThrow(std:("Text not found"));
            }
            );
        }
        );
        describe(std:("Navigation"), [=]() mutable
        {
            it(std:("should goto line correctly"), [=]() mutable
            {
                windowedFile["goto"](0, std:("top"));
                expect(const_(windowedFile["lineRange"])[0])->toBe(0);
                windowedFile["goto"](50, std:("top"));
                expect(const_(windowedFile["lineRange"])[0])->toBe(48);
                windowedFile["goto"](100, std:("top"));
                expect(const_(windowedFile["lineRange"])[1])->toBe(99);
            }
            );
            it(std:("should scroll correctly"), [=]() mutable
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
        describe(std:("Window output"), [=]() mutable
        {
            it(std:("should print window with correct format"), [=]() mutable
            {
                windowedFile["firstLine"] = 10;
                auto output = windowedFile["getWindowText"](true, true, true);
                expect(output)->toContain(std:("[File: ") + testFile + std:(" (100 lines total)]"));
                expect(output)->toContain(std:("(10 more lines above)"));
                expect(output)->toContain(std:("11:10"));
                expect(output)->toContain(std:("20:19"));
                expect(output)->toContain(std:("(80 more lines below)"));
            }
            );
            it(std:("should handle new file with single line"), [=]() mutable
            {
                auto newFile = path->join(tmpDir, std:("new.py"));
                fs::writeFileSync(newFile, std:("\
"));
                auto registryData = object{
                    object::pair{std:("CURRENT_FILE"), newFile}, 
                    object::pair{std:("FIRST_LINE"), std:("0")}, 
                    object::pair{std:("WINDOW"), std:("10")}
                };
                fs::writeFileSync(envFile, JSON->stringify(registryData));
                auto newWindowedFile = std::make_shared<WindowedFile>(object{
                    object::pair{std:("path"), newFile}, 
                    object::pair{std:("exitOnException"), false}
                });
                expect(newWindowedFile->get_nLines())->toBe(1);
                expect(newWindowedFile->get_lineRange())->toEqual(array<double>{ 0, 0 });
                auto output = newWindowedFile->getWindowText(true, true, true);
                expect(output)->toContain(std:("[File: ") + newFile + std:(" (1 lines total)]"));
                expect(output)->toContain(std:("1:"));
            }
            );
        }
        );
        describe(std:("Text operations"), [=]() mutable
        {
            it(std:("should find all occurrences"), [=]() mutable
            {
                auto content = std:("test\
test\
other\
test");
                fs::writeFileSync(testFile, content);
                auto wf = std::make_shared<WindowedFile>(object{
                    object::pair{std:("path"), testFile}, 
                    object::pair{std:("exitOnException"), false}
                });
                auto occurrences = wf->findAllOccurrences(std:("test"), false);
                expect(occurrences)->toEqual(array<double>{ 1, 2, 4 });
                auto zeroBasedOccurrences = wf->findAllOccurrences(std:("test"), true);
                expect(zeroBasedOccurrences)->toEqual(array<double>{ 0, 1, 3 });
            }
            );
            it(std:("should handle global replacement"), [=]() mutable
            {
                auto content = std:("old\
old\
other\
old");
                fs::writeFileSync(testFile, content);
                auto wf = std::make_shared<WindowedFile>(object{
                    object::pair{std:("path"), testFile}, 
                    object::pair{std:("exitOnException"), false}
                });
                auto info = wf->replace(std:("old"), std:("new"));
                expect(info["nReplacements"])->toBe(3);
                expect(info["firstReplacedLine"])->toBe(1);
                auto newContent = fs::readFileSync(testFile, std:("utf-8"));
                expect(newContent)->toBe(std:("new\
new\
other\
new"));
            }
            );
            it(std:("should handle insertion"), [=]() mutable
            {
                auto content = std:("line1\
line2\
line3");
                fs::writeFileSync(testFile, content);
                auto wf = std::make_shared<WindowedFile>(object{
                    object::pair{std:("path"), testFile}, 
                    object::pair{std:("exitOnException"), false}
                });
                auto info = wf->insert(std:("inserted"), 1);
                expect(info["firstInsertedLine"])->toBe(1);
                expect(info["nLinesAdded"])->toBe(1);
                auto newContent = fs::readFileSync(testFile, std:("utf-8"));
                expect(newContent)->toBe(std:("line1\
inserted\
line2\
line3"));
            }
            );
            it(std:("should undo edits"), [=]() mutable
            {
                auto originalContent = std:("original");
                fs::writeFileSync(testFile, originalContent);
                auto wf = std::make_shared<WindowedFile>(object{
                    object::pair{std:("path"), testFile}, 
                    object::pair{std:("exitOnException"), false}
                });
                wf->replace(std:("original"), std:("modified"));
                expect(fs::readFileSync(testFile, std:("utf-8")))->toBe(std:("modified"));
                wf->undoEdit();
                expect(fs::readFileSync(testFile, std:("utf-8")))->toBe(originalContent);
            }
            );
        }
        );
    }
    );
}

MAIN
