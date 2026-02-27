#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/src/lib/matching/__tests__/matching-logic.string.test.h"

void Main(void)
{
    describe(std:("matchString"), [=]() mutable
    {
        shared content = std:("this is a test string");
        test(std:("should match when string is contained in content"), [=]() mutable
        {
            expect(matchString(std:("test"), content, object{}))->toBe(true);
        }
        );
        test(std:("should not match when string is not in content"), [=]() mutable
        {
            expect(matchString(std:("missing"), content, object{}))->toBe(false);
        }
        );
        test(std:("should be case insensitive by default"), [=]() mutable
        {
            expect(matchString(std:("TEST"), content, object{}))->toBe(true);
            expect(matchString(std:("STRING"), content, object{}))->toBe(true);
        }
        );
        test(std:("should respect case sensitivity option"), [=]() mutable
        {
            expect(matchString(std:("TEST"), content, object{
                object::pair{std:("caseSensitive"), true}
            }))->toBe(false);
            expect(matchString(std:("string"), content, object{
                object::pair{std:("caseSensitive"), true}
            }))->toBe(true);
        }
        );
        test(std:("should match word parts by default (wordBoundary: false)"), [=]() mutable
        {
            expect(matchString(std:("tes"), content, object{}))->toBe(true);
            expect(matchString(std:("str"), content, object{}))->toBe(true);
        }
        );
        test(std:("should respect word boundary option (positive cases)"), [=]() mutable
        {
            expect(matchString(std:("test"), content, object{
                object::pair{std:("wordBoundary"), true}
            }))->toBe(true);
            expect(matchString(std:("string"), content, object{
                object::pair{std:("wordBoundary"), true}
            }))->toBe(true);
            expect(matchString(std:("a"), content, object{
                object::pair{std:("wordBoundary"), true}
            }))->toBe(true);
            expect(matchString(std:("is"), content, object{
                object::pair{std:("wordBoundary"), true}
            }))->toBe(true);
        }
        );
        test(std:("should respect word boundary option (negative cases)"), [=]() mutable
        {
            expect(matchString(std:("tes"), content, object{
                object::pair{std:("wordBoundary"), true}
            }))->toBe(false);
            expect(matchString(std:("str"), content, object{
                object::pair{std:("wordBoundary"), true}
            }))->toBe(false);
            expect(matchString(std:("testing"), content, object{
                object::pair{std:("wordBoundary"), true}
            }))->toBe(false);
        }
        );
        test(std:("should handle word boundary with case insensitivity"), [=]() mutable
        {
            expect(matchString(std:("TEST"), content, object{
                object::pair{std:("wordBoundary"), true}, 
                object::pair{std:("caseSensitive"), false}
            }))->toBe(true);
            expect(matchString(std:("TES"), content, object{
                object::pair{std:("wordBoundary"), true}, 
                object::pair{std:("caseSensitive"), false}
            }))->toBe(false);
        }
        );
        test(std:("should handle word boundary with case sensitivity"), [=]() mutable
        {
            expect(matchString(std:("test"), content, object{
                object::pair{std:("wordBoundary"), true}, 
                object::pair{std:("caseSensitive"), true}
            }))->toBe(true);
            expect(matchString(std:("TEST"), content, object{
                object::pair{std:("wordBoundary"), true}, 
                object::pair{std:("caseSensitive"), true}
            }))->toBe(false);
        }
        );
    }
    );
}

MAIN
