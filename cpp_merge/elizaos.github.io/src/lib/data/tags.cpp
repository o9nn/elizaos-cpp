#include "tags.hpp"

any TagScoringSchema = z->object(object{
    object::pair{std:("points"), z->number()}, 
    object::pair{std:("multiplier"), z->number()->optional()}, 
    object::pair{std:("decay"), z->number()->min(0)->max(1)->optional()}, 
    object::pair{std:("maxDaily"), z->number()->optional()}
});
any TagPatternSchema = z->object(object{
    object::pair{std:("target"), z->nativeEnum(TagPatternType)}, 
    object::pair{std:("pattern"), z->string()}, 
    object::pair{std:("caseSensitive"), z->boolean()->optional()->default(false)}, 
    object::pair{std:("scoring"), TagScoringSchema}, 
    object::pair{std:("description"), z->string()->optional()}, 
    object::pair{std:("enabled"), z->boolean()->optional()->default(true)}
});
any TagRuleSchema = z->object(object{
    object::pair{std:("name"), z->string()}, 
    object::pair{std:("category"), z->nativeEnum(TagCategory)}, 
    object::pair{std:("description"), z->string()}, 
    object::pair{std:("patterns"), z->array(TagPatternSchema)}, 
    object::pair{std:("weight"), z->number()->optional()->default(1)}, 
    object::pair{std:("dependencies"), z->array(z->string())->optional()}, 
    object::pair{std:("createdAt"), z->string()->optional()}, 
    object::pair{std:("updatedAt"), z->string()->optional()}
});

void Main(void)
{
}

MAIN
