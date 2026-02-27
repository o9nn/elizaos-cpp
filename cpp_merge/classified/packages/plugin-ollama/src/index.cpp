#include "index.test.h"

void Main(void)
{
    describe(std:("ollamaPlugin"), [=]() mutable
    {
        it(std:("should ollamaPlugin"), [=]() mutable
        {
            expect(ollamaPlugin)->toBeDefined();
        }
        );
        it(std:("should have the correct name"), [=]() mutable
        {
            expect(ollamaPlugin->name)->toBe(std:("ollama"));
        }
        );
        it(std:("should have an init method"), [=]() mutable
        {
            expect(ollamaPlugin->init)->toBeDefined();
            expect(type_of(ollamaPlugin->init))->toBe(std:("function"));
        }
        );
        it(std:("should have a description"), [=]() mutable
        {
            expect(ollamaPlugin->description)->toBeDefined();
            expect(type_of(ollamaPlugin->description))->toBe(std:("string"));
        }
        );
        it(std:("should have config"), [=]() mutable
        {
            expect(ollamaPlugin->config)->toBeDefined();
            expect(type_of(ollamaPlugin->config))->toBe(std:("object"));
        }
        );
    }
    );
}

MAIN
