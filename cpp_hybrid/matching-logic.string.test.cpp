#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/src/lib/matching/__tests__/matching-logic.string.test.h"

void Main(void)
{
    describe(std::string("matchString"), [=]() mutable
    {
        shared content = std::string("this is a test string");
        test(std::string("should match when string is contained in content"), [=]() mutable
        {
            expect(matchString(std::string("test"), content, object{}))->toBe(true);
        }
        );
        test(std::string("should not match when string is not in content"), [=]() mutable
        {
            expect(matchString(std::string("missing"), content, object{}))->toBe(false);
        }
        );
        test(std::string("should be case insensitive by default"), [=]() mutable
        {
            expect(matchString(std::string("TEST"), content, object{}))->toBe(true);
            expect(matchString(std::string("STRING"), content, object{}))->toBe(true);
        }
        );
        test(std::string("should respect case sensitivity option"), [=]() mutable
        {
            expect(matchString(std::string("TEST"), content, object{
                object::pair{std::string("caseSensitive"), true}
            }))->toBe(false);
            expect(matchString(std::string("string"), content, object{
                object::pair{std::string("caseSensitive"), true}
            }))->toBe(true);
        }
        );
        test(std::string("should match word parts by default (wordBoundary: false)"), [=]() mutable
        {
            expect(matchString(std::string("tes"), content, object{}))->toBe(true);
            expect(matchString(std::string("str"), content, object{}))->toBe(true);
        }
        );
        test(std::string("should respect word boundary option (positive cases)"), [=]() mutable
        {
            expect(matchString(std::string("test"), content, object{
                object::pair{std::string("wordBoundary"), true}
            }))->toBe(true);
            expect(matchString(std::string("string"), content, object{
                object::pair{std::string("wordBoundary"), true}
            }))->toBe(true);
            expect(matchString(std::string("a"), content, object{
                object::pair{std::string("wordBoundary"), true}
            }))->toBe(true);
            expect(matchString(std::string("is"), content, object{
                object::pair{std::string("wordBoundary"), true}
            }))->toBe(true);
        }
        );
        test(std::string("should respect word boundary option (negative cases)"), [=]() mutable
        {
            expect(matchString(std::string("tes"), content, object{
                object::pair{std::string("wordBoundary"), true}
            }))->toBe(false);
            expect(matchString(std::string("str"), content, object{
                object::pair{std::string("wordBoundary"), true}
            }))->toBe(false);
            expect(matchString(std::string("testing"), content, object{
                object::pair{std::string("wordBoundary"), true}
            }))->toBe(false);
        }
        );
        test(std::string("should handle word boundary with case insensitivity"), [=]() mutable
        {
            expect(matchString(std::string("TEST"), content, object{
                object::pair{std::string("wordBoundary"), true}, 
                object::pair{std::string("caseSensitive"), false}
            }))->toBe(true);
            expect(matchString(std::string("TES"), content, object{
                object::pair{std::string("wordBoundary"), true}, 
                object::pair{std::string("caseSensitive"), false}
            }))->toBe(false);
        }
        );
        test(std::string("should handle word boundary with case sensitivity"), [=]() mutable
        {
            expect(matchString(std::string("test"), content, object{
                object::pair{std::string("wordBoundary"), true}, 
                object::pair{std::string("caseSensitive"), true}
            }))->toBe(true);
            expect(matchString(std::string("TEST"), content, object{
                object::pair{std::string("wordBoundary"), true}, 
                object::pair{std::string("caseSensitive"), true}
            }))->toBe(false);
        }
        );
    }
    );
}

MAIN
