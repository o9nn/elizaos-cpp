#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/src/lib/matching/__tests__/matching-logic.regex.test.h"

void Main(void)
{
    describe(std::string("matchRegex"), [=]() mutable
    {
        shared content = std::string("this is a test string 123");
        test(std::string("should match when regex pattern matches content"), [=]() mutable
        {
            expect(matchRegex(std::string("t[aeiou]st"), content, object{}))->toBe(true);
            expect(matchRegex(std::string("\d+"), content, object{}))->toBe(true);
        }
        );
        test(std::string("should not match when regex pattern doesn't match content"), [=]() mutable
        {
            expect(matchRegex(std::string("t[0-9]st"), content, object{}))->toBe(false);
            expect(matchRegex(std::string("^test"), content, object{}))->toBe(false);
        }
        );
        test(std::string("should be case insensitive by default"), [=]() mutable
        {
            expect(matchRegex(std::string("TEST"), content, object{}))->toBe(true);
            expect(matchRegex(std::string("STRING"), content, object{}))->toBe(true);
        }
        );
        test(std::string("should respect case sensitivity option"), [=]() mutable
        {
            expect(matchRegex(std::string("TEST"), content, object{
                object::pair{std::string("caseSensitive"), true}
            }))->toBe(false);
            expect(matchRegex(std::string("string"), content, object{
                object::pair{std::string("caseSensitive"), true}
            }))->toBe(true);
            expect(matchRegex(std::string("this is a test"), content, object{
                object::pair{std::string("caseSensitive"), true}
            }))->toBe(true);
        }
        );
        test(std::string("should handle word boundary option when pattern is simple word"), [=]() mutable
        {
            expect(matchRegex(std::string("test"), content, object{
                object::pair{std::string("wordBoundary"), true}
            }))->toBe(true);
            expect(matchRegex(std::string("tes"), content, object{
                object::pair{std::string("wordBoundary"), true}
            }))->toBe(false);
            expect(matchRegex(std::string("string"), content, object{
                object::pair{std::string("wordBoundary"), true}
            }))->toBe(true);
            expect(matchRegex(std::string("str"), content, object{
                object::pair{std::string("wordBoundary"), true}
            }))->toBe(false);
        }
        );
        test(std::string("should not add word boundary if pattern already has anchors or boundaries"), [=]() mutable
        {
            expect(matchRegex(std::string("\btest\b"), content, object{
                object::pair{std::string("wordBoundary"), true}
            }))->toBe(true);
            expect(matchRegex(std::string("\btest"), content, object{
                object::pair{std::string("wordBoundary"), true}
            }))->toBe(true);
            expect(matchRegex(std::string("test\b"), content, object{
                object::pair{std::string("wordBoundary"), true}
            }))->toBe(true);
            expect(matchRegex(std::string("^this"), content, object{
                object::pair{std::string("wordBoundary"), true}
            }))->toBe(true);
            expect(matchRegex(std::string("123$"), content, object{
                object::pair{std::string("wordBoundary"), true}
            }))->toBe(true);
        }
        );
        test(std::string("should handle word boundary with case insensitivity"), [=]() mutable
        {
            expect(matchRegex(std::string("TEST"), content, object{
                object::pair{std::string("wordBoundary"), true}, 
                object::pair{std::string("caseSensitive"), false}
            }))->toBe(true);
            expect(matchRegex(std::string("TES"), content, object{
                object::pair{std::string("wordBoundary"), true}, 
                object::pair{std::string("caseSensitive"), false}
            }))->toBe(false);
        }
        );
        test(std::string("should handle word boundary with case sensitivity"), [=]() mutable
        {
            expect(matchRegex(std::string("test"), content, object{
                object::pair{std::string("wordBoundary"), true}, 
                object::pair{std::string("caseSensitive"), true}
            }))->toBe(true);
            expect(matchRegex(std::string("TEST"), content, object{
                object::pair{std::string("wordBoundary"), true}, 
                object::pair{std::string("caseSensitive"), true}
            }))->toBe(false);
        }
        );
        test(std::string("should handle complex regex patterns"), [=]() mutable
        {
            expect(matchRegex(std::string("^this.*\d+$"), content, object{}))->toBe(true);
            expect(matchRegex(std::string("is\s+a"), content, object{}))->toBe(true);
        }
        );
        test(std::string("should return false for invalid regex patterns"), [=]() mutable
        {
            auto originalError = console->error;
            console->error = [=]() mutable
            {
            };
            expect(matchRegex(std::string("["), content, object{}))->toBe(false);
            expect(matchRegex(std::string("("), content, object{}))->toBe(false);
            console->error = originalError;
        }
        );
    }
    );
}

MAIN
