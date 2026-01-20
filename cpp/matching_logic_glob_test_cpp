#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/src/lib/matching/__tests__/matching-logic.glob.test.h"

void Main(void)
{
    describe(std::string("matchGlob"), [=]() mutable
    {
        test(std::string("should match simple glob patterns against full paths"), [=]() mutable
        {
            expect(matchGlob(std::string("*.ts"), std::string("file.ts"), object{}))->toBe(true);
            expect(matchGlob(std::string("file.ts"), std::string("file.ts"), object{}))->toBe(true);
            expect(matchGlob(std::string("*.ts"), std::string("path/to/file.ts"), object{}))->toBe(false);
            expect(matchGlob(std::string("file.js"), std::string("file.ts"), object{}))->toBe(false);
        }
        );
        test(std::string("should handle wildcard ** matching"), [=]() mutable
        {
            expect(matchGlob(std::string("src/**/*.ts"), std::string("src/lib/file.ts"), object{}))->toBe(true);
            expect(matchGlob(std::string("src/**/*.ts"), std::string("src/components/ui/button.ts"), object{}))->toBe(true);
            expect(matchGlob(std::string("src/**/*.ts"), std::string("src/file.js"), object{}))->toBe(false);
            expect(matchGlob(std::string("src/**/*.ts"), std::string("test/file.ts"), object{}))->toBe(false);
            expect(matchGlob(std::string("**/*.ts"), std::string("path/to/file.ts"), object{}))->toBe(true);
        }
        );
        test(std::string("should handle wildcard * matching within paths"), [=]() mutable
        {
            expect(matchGlob(std::string("src/*/file.ts"), std::string("src/lib/file.ts"), object{}))->toBe(true);
            expect(matchGlob(std::string("src/*/file.ts"), std::string("src/lib/nested/file.ts"), object{}))->toBe(false);
            expect(matchGlob(std::string("src/lib/*.ts"), std::string("src/lib/file.ts"), object{}))->toBe(true);
        }
        );
        test(std::string("should handle wildcard ? matching"), [=]() mutable
        {
            expect(matchGlob(std::string("file?.js"), std::string("file1.js"), object{}))->toBe(true);
            expect(matchGlob(std::string("file?.js"), std::string("fileA.js"), object{}))->toBe(true);
            expect(matchGlob(std::string("file?.js"), std::string("file.js"), object{}))->toBe(false);
            expect(matchGlob(std::string("file?.js"), std::string("file12.js"), object{}))->toBe(false);
        }
        );
        test(std::string("should match dotfiles"), [=]() mutable
        {
            expect(matchGlob(std::string(".*"), std::string(".gitignore"), object{}))->toBe(true);
            expect(matchGlob(std::string(".github/*"), std::string(".github/workflows"), object{}))->toBe(true);
            expect(matchGlob(std::string("*.yml"), std::string(".github/workflows/ci.yml"), object{}))->toBe(false);
            expect(matchGlob(std::string("**/*.yml"), std::string(".github/workflows/ci.yml"), object{}))->toBe(true);
        }
        );
        test(std::string("should be case insensitive by default"), [=]() mutable
        {
            expect(matchGlob(std::string("*.TS"), std::string("file.ts"), object{}))->toBe(true);
            expect(matchGlob(std::string("SRC/**/*.ts"), std::string("src/lib/file.ts"), object{}))->toBe(true);
        }
        );
        test(std::string("should respect case sensitivity option"), [=]() mutable
        {
            expect(matchGlob(std::string("*.TS"), std::string("file.ts"), object{
                object::pair{std::string("caseSensitive"), true}
            }))->toBe(false);
            expect(matchGlob(std::string("*.TS"), std::string("file.TS"), object{
                object::pair{std::string("caseSensitive"), true}
            }))->toBe(true);
            expect(matchGlob(std::string("SRC/**/*.ts"), std::string("src/lib/file.ts"), object{
                object::pair{std::string("caseSensitive"), true}
            }))->toBe(false);
            expect(matchGlob(std::string("src/**/*.ts"), std::string("src/lib/file.ts"), object{
                object::pair{std::string("caseSensitive"), true}
            }))->toBe(true);
        }
        );
        test(std::string("should ignore word boundary option"), [=]() mutable
        {
            auto originalWarn = console->warn;
            console->warn = [=]() mutable
            {
            };
            expect(matchGlob(std::string("*.ts"), std::string("file.ts"), object{
                object::pair{std::string("wordBoundary"), true}
            }))->toBe(true);
            expect(matchGlob(std::string("file.ts"), std::string("file.ts"), object{
                object::pair{std::string("wordBoundary"), true}, 
                object::pair{std::string("caseSensitive"), true}
            }))->toBe(true);
            console->warn = originalWarn;
        }
        );
    }
    );
}

MAIN
