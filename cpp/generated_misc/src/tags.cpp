#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/src/lib/data/tags.h"

any TagScoringSchema = z->object(object{
    object::pair{std::string("points"), z->number()}, 
    object::pair{std::string("multiplier"), z->number()->optional()}, 
    object::pair{std::string("decay"), z->number()->min(0)->max(1)->optional()}, 
    object::pair{std::string("maxDaily"), z->number()->optional()}
});
any TagPatternSchema = z->object(object{
    object::pair{std::string("target"), z->nativeEnum(TagPatternType)}, 
    object::pair{std::string("pattern"), z->string()}, 
    object::pair{std::string("caseSensitive"), z->boolean()->optional()->default(false)}, 
    object::pair{std::string("scoring"), TagScoringSchema}, 
    object::pair{std::string("description"), z->string()->optional()}, 
    object::pair{std::string("enabled"), z->boolean()->optional()->default(true)}
});
any TagRuleSchema = z->object(object{
    object::pair{std::string("name"), z->string()}, 
    object::pair{std::string("category"), z->nativeEnum(TagCategory)}, 
    object::pair{std::string("description"), z->string()}, 
    object::pair{std::string("patterns"), z->array(TagPatternSchema)}, 
    object::pair{std::string("weight"), z->number()->optional()->default(1)}, 
    object::pair{std::string("dependencies"), z->array(z->string())->optional()}, 
    object::pair{std::string("createdAt"), z->string()->optional()}, 
    object::pair{std::string("updatedAt"), z->string()->optional()}
});

void Main(void)
{
}

MAIN
