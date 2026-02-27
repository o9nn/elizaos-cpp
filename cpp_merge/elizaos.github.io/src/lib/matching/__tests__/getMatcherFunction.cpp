#include "getMatcherFunction.test.h"

double createMatcherCallCount = 0;
boolean shouldThrowError = false;
std::function<std::shared_ptr<MatcherFunction>(std::shared_ptr<TagPattern>)> originalCreateMatcher = createMatcher;
std::function<std::shared_ptr<MatcherFunction>(std::shared_ptr<TagPattern>)> mockCreateMatcher = [=](auto tagPattern) mutable
{
    createMatcherCallCount++;
    if (shouldThrowError) {
        throw any(std::make_shared<Error>(std:("Simulated matcher creation error")));
    }
    return originalCreateMatcher(tagPattern);
};

void Main(void)
{
    describe(std:("Matcher Cache"), [=]() mutable
    {
        beforeEach([=]() mutable
        {
            clearMatcherCache();
            createMatcherCallCount = 0;
            shouldThrowError = false;
        }
        );
        shared pattern1 = object{
            object::pair{std:("pattern"), std:("test")}, 
            object::pair{std:("patternType"), PatternType::STRING}, 
            object::pair{std:("target"), TagPatternType::COMMIT_MESSAGE}, 
            object::pair{std:("options"), object{
                object::pair{std:("caseSensitive"), false}
            }}
        };
        shared pattern1Again = object{
            object::pair{std:("pattern"), std:("test")}, 
            object::pair{std:("patternType"), PatternType::STRING}, 
            object::pair{std:("target"), TagPatternType::COMMIT_MESSAGE}, 
            object::pair{std:("options"), object{
                object::pair{std:("caseSensitive"), false}
            }}
        };
        shared pattern1CaseSensitive = object{
            object::pair{std:("pattern"), std:("test")}, 
            object::pair{std:("patternType"), PatternType::STRING}, 
            object::pair{std:("target"), TagPatternType::COMMIT_MESSAGE}, 
            object::pair{std:("options"), object{
                object::pair{std:("caseSensitive"), true}
            }}
        };
        shared pattern2 = object{
            object::pair{std:("pattern"), std:("*.ts")}, 
            object::pair{std:("patternType"), PatternType::GLOB}, 
            object::pair{std:("target"), TagPatternType::FILE_PATH}, 
            object::pair{std:("options"), object{}}
        };
        shared invalidGlobPattern = object{
            object::pair{std:("pattern"), std:("*.log")}, 
            object::pair{std:("patternType"), PatternType::GLOB}, 
            object::pair{std:("target"), TagPatternType::COMMIT_MESSAGE}, 
            object::pair{std:("options"), object{}}
        };
        it(std:("should cache matcher functions based on pattern configuration"), [=]() mutable
        {
            expect(getMatcherCacheSize())->toBe(0);
            auto matcherFn1 = getMatcherFunction(pattern1);
            expect(getMatcherCacheSize())->toBe(1);
            auto matcherFn1Again = getMatcherFunction(pattern1Again);
            expect(getMatcherCacheSize())->toBe(1);
            expect(matcherFn1Again)->toBe(matcherFn1);
            auto matcherFn1CaseSens = getMatcherFunction(pattern1CaseSensitive);
            expect(getMatcherCacheSize())->toBe(2);
            expect(matcherFn1CaseSens)->not->toBe(matcherFn1);
            auto matcherFn2 = getMatcherFunction(pattern2);
            expect(getMatcherCacheSize())->toBe(3);
            expect(matcherFn2)->not->toBe(matcherFn1);
        }
        );
        it(std:("should clear the cache"), [=]() mutable
        {
            getMatcherFunction(pattern1);
            getMatcherFunction(pattern2);
            expect(getMatcherCacheSize())->toBe(2);
            clearMatcherCache();
            expect(getMatcherCacheSize())->toBe(0);
            auto matcherFn1AfterClear = getMatcherFunction(pattern1);
            expect(getMatcherCacheSize())->toBe(1);
            auto matcherFn2AfterClear = getMatcherFunction(pattern2);
            expect(getMatcherCacheSize())->toBe(2);
        }
        );
        it(std:("should handle and cache errors during matcher creation"), [=]() mutable
        {
            auto originalError = console->error;
            console->error = [=]() mutable
            {
            };
            auto errorFn = getMatcherFunction(invalidGlobPattern);
            expect(getMatcherCacheSize())->toBe(1);
            auto content = object{
                object::pair{std:("content"), std:("test.log")}, 
                object::pair{std:("contentType"), TagPatternType::COMMIT_MESSAGE}
            };
            expect(errorFn(content))->toBe(false);
            auto errorFnAgain = getMatcherFunction(invalidGlobPattern);
            expect(getMatcherCacheSize())->toBe(1);
            expect(errorFnAgain)->toBe(errorFn);
            console->error = originalError;
        }
        );
        it(std:("should create different cache keys for different options"), [=]() mutable
        {
            auto p1 = object{
                object::pair{std:("pattern"), std:("a")}, 
                object::pair{std:("patternType"), PatternType::STRING}, 
                object::pair{std:("target"), TagPatternType::COMMENT}, 
                object::pair{std:("options"), object{
                    object::pair{std:("caseSensitive"), false}, 
                    object::pair{std:("wordBoundary"), true}
                }}
            };
            auto p2 = object{
                object::pair{std:("pattern"), std:("a")}, 
                object::pair{std:("patternType"), PatternType::STRING}, 
                object::pair{std:("target"), TagPatternType::COMMENT}, 
                object::pair{std:("options"), object{
                    object::pair{std:("wordBoundary"), true}, 
                    object::pair{std:("caseSensitive"), false}
                }}
            };
            auto p3 = object{
                object::pair{std:("pattern"), std:("a")}, 
                object::pair{std:("patternType"), PatternType::STRING}, 
                object::pair{std:("target"), TagPatternType::COMMENT}, 
                object::pair{std:("options"), object{
                    object::pair{std:("caseSensitive"), false}
                }}
            };
            auto p4 = object{
                object::pair{std:("pattern"), std:("a")}, 
                object::pair{std:("patternType"), PatternType::STRING}, 
                object::pair{std:("target"), TagPatternType::COMMENT}, 
                object::pair{std:("options"), object{}}
            };
            auto fn1 = getMatcherFunction(p1);
            expect(getMatcherCacheSize())->toBe(1);
            auto fn2 = getMatcherFunction(p2);
            expect(getMatcherCacheSize())->toBe(1);
            expect(fn1)->toBe(fn2);
            auto fn3 = getMatcherFunction(p3);
            expect(getMatcherCacheSize())->toBe(2);
            expect(fn3)->not->toBe(fn1);
            auto fn4 = getMatcherFunction(p4);
            expect(getMatcherCacheSize())->toBe(3);
            expect(fn4)->not->toBe(fn3);
        }
        );
    }
    );
}

MAIN
