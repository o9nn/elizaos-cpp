#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/src/lib/scoring/types.h"

any UserScoreMetricsSchema = z->object(object{
    object::pair{std::string("pullRequests"), z->object(object{
        object::pair{std::string("total"), z->number()}, 
        object::pair{std::string("merged"), z->number()}, 
        object::pair{std::string("open"), z->number()}, 
        object::pair{std::string("closed"), z->number()}
    })}, 
    object::pair{std::string("issues"), z->object(object{
        object::pair{std::string("total"), z->number()}, 
        object::pair{std::string("open"), z->number()}, 
        object::pair{std::string("closed"), z->number()}
    })}, 
    object::pair{std::string("reviews"), z->object(object{
        object::pair{std::string("total"), z->number()}, 
        object::pair{std::string("approved"), z->number()}, 
        object::pair{std::string("changesRequested"), z->number()}, 
        object::pair{std::string("commented"), z->number()}
    })}, 
    object::pair{std::string("comments"), z->object(object{
        object::pair{std::string("pullRequests"), z->number()}, 
        object::pair{std::string("issues"), z->number()}
    })}, 
    object::pair{std::string("codeChanges"), z->object(object{
        object::pair{std::string("additions"), z->number()}, 
        object::pair{std::string("deletions"), z->number()}, 
        object::pair{std::string("files"), z->number()}
    })}
});

void Main(void)
{
}

MAIN
