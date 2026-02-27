#include "matching-logic.glob.test.h"

void Main(void)
{
    describe(std:("matchGlob"), [=]() mutable
    {
        test(std:("should match simple glob patterns against full paths"), [=]() mutable
        {
            expect(matchGlob(std:("*.ts"), std:("file.ts"), object{}))->toBe(true);
            expect(matchGlob(std:("file.ts"), std:("file.ts"), object{}))->toBe(true);
            expect(matchGlob(std:("*.ts"), std:("path/to/file.ts"), object{}))->toBe(false);
            expect(matchGlob(std:("file.js"), std:("file.ts"), object{}))->toBe(false);
        }
        );
        test(std:("should handle wildcard ** matching"), [=]() mutable
        {
            expect(matchGlob(std:("src/**/*.ts"), std:("src/lib/file.ts"), object{}))->toBe(true);
            expect(matchGlob(std:("src/**/*.ts"), std:("src/components/ui/button.ts"), object{}))->toBe(true);
            expect(matchGlob(std:("src/**/*.ts"), std:("src/file.js"), object{}))->toBe(false);
            expect(matchGlob(std:("src/**/*.ts"), std:("test/file.ts"), object{}))->toBe(false);
            expect(matchGlob(std:("**/*.ts"), std:("path/to/file.ts"), object{}))->toBe(true);
        }
        );
        test(std:("should handle wildcard * matching within paths"), [=]() mutable
        {
            expect(matchGlob(std:("src/*/file.ts"), std:("src/lib/file.ts"), object{}))->toBe(true);
            expect(matchGlob(std:("src/*/file.ts"), std:("src/lib/nested/file.ts"), object{}))->toBe(false);
            expect(matchGlob(std:("src/lib/*.ts"), std:("src/lib/file.ts"), object{}))->toBe(true);
        }
        );
        test(std:("should handle wildcard ? matching"), [=]() mutable
        {
            expect(matchGlob(std:("file.js"), std:("file1.js"), object{}))->toBe(true);
            expect(matchGlob(std:("file.js"), std:("fileA.js"), object{}))->toBe(true);
            expect(matchGlob(std:("file.js"), std:("file.js"), object{}))->toBe(false);
            expect(matchGlob(std:("file.js"), std:("file12.js"), object{}))->toBe(false);
        }
        );
        test(std:("should match dotfiles"), [=]() mutable
        {
            expect(matchGlob(std:(".*"), std:(".gitignore"), object{}))->toBe(true);
            expect(matchGlob(std:(".github/*"), std:(".github/workflows"), object{}))->toBe(true);
            expect(matchGlob(std:("*.yml"), std:(".github/workflows/ci.yml"), object{}))->toBe(false);
            expect(matchGlob(std:("**/*.yml"), std:(".github/workflows/ci.yml"), object{}))->toBe(true);
        }
        );
        test(std:("should be case insensitive by default"), [=]() mutable
        {
            expect(matchGlob(std:("*.TS"), std:("file.ts"), object{}))->toBe(true);
            expect(matchGlob(std:("SRC/**/*.ts"), std:("src/lib/file.ts"), object{}))->toBe(true);
        }
        );
        test(std:("should respect case sensitivity option"), [=]() mutable
        {
            expect(matchGlob(std:("*.TS"), std:("file.ts"), object{
                object::pair{std:("caseSensitive"), true}
            }))->toBe(false);
            expect(matchGlob(std:("*.TS"), std:("file.TS"), object{
                object::pair{std:("caseSensitive"), true}
            }))->toBe(true);
            expect(matchGlob(std:("SRC/**/*.ts"), std:("src/lib/file.ts"), object{
                object::pair{std:("caseSensitive"), true}
            }))->toBe(false);
            expect(matchGlob(std:("src/**/*.ts"), std:("src/lib/file.ts"), object{
                object::pair{std:("caseSensitive"), true}
            }))->toBe(true);
        }
        );
        test(std:("should ignore word boundary option"), [=]() mutable
        {
            auto originalWarn = console->warn;
            console->warn = [=]() mutable
            {
            };
            expect(matchGlob(std:("*.ts"), std:("file.ts"), object{
                object::pair{std:("wordBoundary"), true}
            }))->toBe(true);
            expect(matchGlob(std:("file.ts"), std:("file.ts"), object{
                object::pair{std:("wordBoundary"), true}, 
                object::pair{std:("caseSensitive"), true}
            }))->toBe(true);
            console->warn = originalWarn;
        }
        );
    }
    );
}

MAIN
