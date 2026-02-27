#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/src/lib/matching/__tests__/matching-logic.regex.test.h"

void Main(void)
{
    describe(std:("matchRegex"), [=]() mutable
    {
        shared content = std:("this is a test string 123");
        test(std:("should match when regex pattern matches content"), [=]() mutable
        {
            expect(matchRegex(std:("t[aeiou]st"), content, object{}))->toBe(true);
            expect(matchRegex(std:("\d+"), content, object{}))->toBe(true);
        }
        );
        test(std:("should not match when regex pattern doesn't match content"), [=]() mutable
        {
            expect(matchRegex(std:("t[0-9]st"), content, object{}))->toBe(false);
            expect(matchRegex(std:("^test"), content, object{}))->toBe(false);
        }
        );
        test(std:("should be case insensitive by default"), [=]() mutable
        {
            expect(matchRegex(std:("TEST"), content, object{}))->toBe(true);
            expect(matchRegex(std:("STRING"), content, object{}))->toBe(true);
        }
        );
        test(std:("should respect case sensitivity option"), [=]() mutable
        {
            expect(matchRegex(std:("TEST"), content, object{
                object::pair{std:("caseSensitive"), true}
            }))->toBe(false);
            expect(matchRegex(std:("string"), content, object{
                object::pair{std:("caseSensitive"), true}
            }))->toBe(true);
            expect(matchRegex(std:("this is a test"), content, object{
                object::pair{std:("caseSensitive"), true}
            }))->toBe(true);
        }
        );
        test(std:("should handle word boundary option when pattern is simple word"), [=]() mutable
        {
            expect(matchRegex(std:("test"), content, object{
                object::pair{std:("wordBoundary"), true}
            }))->toBe(true);
            expect(matchRegex(std:("tes"), content, object{
                object::pair{std:("wordBoundary"), true}
            }))->toBe(false);
            expect(matchRegex(std:("string"), content, object{
                object::pair{std:("wordBoundary"), true}
            }))->toBe(true);
            expect(matchRegex(std:("str"), content, object{
                object::pair{std:("wordBoundary"), true}
            }))->toBe(false);
        }
        );
        test(std:("should not add word boundary if pattern already has anchors or boundaries"), [=]() mutable
        {
            expect(matchRegex(std:("\btest\b"), content, object{
                object::pair{std:("wordBoundary"), true}
            }))->toBe(true);
            expect(matchRegex(std:("\btest"), content, object{
                object::pair{std:("wordBoundary"), true}
            }))->toBe(true);
            expect(matchRegex(std:("test\b"), content, object{
                object::pair{std:("wordBoundary"), true}
            }))->toBe(true);
            expect(matchRegex(std:("^this"), content, object{
                object::pair{std:("wordBoundary"), true}
            }))->toBe(true);
            expect(matchRegex(std:("123$"), content, object{
                object::pair{std:("wordBoundary"), true}
            }))->toBe(true);
        }
        );
        test(std:("should handle word boundary with case insensitivity"), [=]() mutable
        {
            expect(matchRegex(std:("TEST"), content, object{
                object::pair{std:("wordBoundary"), true}, 
                object::pair{std:("caseSensitive"), false}
            }))->toBe(true);
            expect(matchRegex(std:("TES"), content, object{
                object::pair{std:("wordBoundary"), true}, 
                object::pair{std:("caseSensitive"), false}
            }))->toBe(false);
        }
        );
        test(std:("should handle word boundary with case sensitivity"), [=]() mutable
        {
            expect(matchRegex(std:("test"), content, object{
                object::pair{std:("wordBoundary"), true}, 
                object::pair{std:("caseSensitive"), true}
            }))->toBe(true);
            expect(matchRegex(std:("TEST"), content, object{
                object::pair{std:("wordBoundary"), true}, 
                object::pair{std:("caseSensitive"), true}
            }))->toBe(false);
        }
        );
        test(std:("should handle complex regex patterns"), [=]() mutable
        {
            expect(matchRegex(std:("^this.*\d+$"), content, object{}))->toBe(true);
            expect(matchRegex(std:("is\s+a"), content, object{}))->toBe(true);
        }
        );
        test(std:("should return false for invalid regex patterns"), [=]() mutable
        {
            auto originalError = console->error;
            console->error = [=]() mutable
            {
            };
            expect(matchRegex(std:("["), content, object{}))->toBe(false);
            expect(matchRegex(std:("("), content, object{}))->toBe(false);
            console->error = originalError;
        }
        );
    }
    );
}

MAIN
