#include "/home/runner/work/elizaos-cpp/elizaos-cpp/plugin-specification/core-plugin-v2/__tests__/search.test.h"

void Main(void)
{
    describe(std::string("BM25 search"), [=]() mutable
    {
        it(std::string("indexes documents and finds matches"), [=]() mutable
        {
            auto docs = array<object>{ object{
                object::pair{std::string("text"), std::string("hello world")}
            }, object{
                object::pair{std::string("text"), std::string("another document")}
            }, object{
                object::pair{std::string("text"), std::string("world of javascript")}
            } };
            auto bm = std::make_shared<BM25>(docs, object{
                object::pair{std::string("fieldBoosts"), object{
                    object::pair{std::string("text"), 1}
                }}
            });
            auto results = bm->search(std::string("world"));
            expect(const_(results)[0]->index)->toBe(0);
        }
        );
    }
    );
}

MAIN
