#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/core/src/__tests__/search.test.h"

void Main(void)
{
    describe(std:("BM25 search"), [=]() mutable
    {
        it(std:("indexes documents and finds matches"), [=]() mutable
        {
            auto docs = array<object>{ object{
                object::pair{std:("text"), std:("hello world")}
            }, object{
                object::pair{std:("text"), std:("another document")}
            }, object{
                object::pair{std:("text"), std:("world of javascript")}
            } };
            auto bm = std::make_shared<BM25>(docs, object{
                object::pair{std:("fieldBoosts"), object{
                    object::pair{std:("text"), 1}
                }}
            });
            auto results = bm->search(std:("world"));
            expect(const_(results)[0]->index)->toBe(0);
        }
        );
    }
    );
}

MAIN
