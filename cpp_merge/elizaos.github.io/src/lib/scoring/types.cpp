#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/src/lib/scoring/types.h"

any UserScoreMetricsSchema = z->object(object{
    object::pair{std:("pullRequests"), z->object(object{
        object::pair{std:("total"), z->number()}, 
        object::pair{std:("merged"), z->number()}, 
        object::pair{std:("open"), z->number()}, 
        object::pair{std:("closed"), z->number()}
    })}, 
    object::pair{std:("issues"), z->object(object{
        object::pair{std:("total"), z->number()}, 
        object::pair{std:("open"), z->number()}, 
        object::pair{std:("closed"), z->number()}
    })}, 
    object::pair{std:("reviews"), z->object(object{
        object::pair{std:("total"), z->number()}, 
        object::pair{std:("approved"), z->number()}, 
        object::pair{std:("changesRequested"), z->number()}, 
        object::pair{std:("commented"), z->number()}
    })}, 
    object::pair{std:("comments"), z->object(object{
        object::pair{std:("pullRequests"), z->number()}, 
        object::pair{std:("issues"), z->number()}
    })}, 
    object::pair{std:("codeChanges"), z->object(object{
        object::pair{std:("additions"), z->number()}, 
        object::pair{std:("deletions"), z->number()}, 
        object::pair{std:("files"), z->number()}
    })}
});

void Main(void)
{
}

MAIN
