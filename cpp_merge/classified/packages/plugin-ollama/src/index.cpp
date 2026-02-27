#include "index.test.h"
#include <string>

void Main(void)
{
    describe(std::string("ollamaPlugin"), [=]() mutable
    {
        it(std::string("should ollamaPlugin"), [=]() mutable
        {
            expect(ollamaPlugin)->toBeDefined();
        }
        );
        it(std::string("should have the correct name"), [=]() mutable
        {
            expect(ollamaPlugin->name)->toBe(std::string("ollama"));
        }
        );
        it(std::string("should have an init method"), [=]() mutable
        {
            expect(ollamaPlugin->init)->toBeDefined();
            expect(type_of(ollamaPlugin->init))->toBe(std::string("function"));
        }
        );
        it(std::string("should have a description"), [=]() mutable
        {
            expect(ollamaPlugin->description)->toBeDefined();
            expect(type_of(ollamaPlugin->description))->toBe(std::string("string"));
        }
        );
        it(std::string("should have config"), [=]() mutable
        {
            expect(ollamaPlugin->config)->toBeDefined();
            expect(type_of(ollamaPlugin->config))->toBe(std::string("object"));
        }
        );
    }
    );
}

MAIN
