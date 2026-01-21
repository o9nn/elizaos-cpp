#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/src/lib/data/tags.h"

std::any TagScoringSchema = z->object(object{
    object::pair{std::string("points"), z->number()}, 
    object::pair{std::string("multiplier"), z->number()->std::optional()}, 
    object::pair{std::string("decay"), z->number()->min(0)->max(1)->std::optional()}, 
    object::pair{std::string("maxDaily"), z->number()->std::optional()}
});
std::any TagPatternSchema = z->object(object{
    object::pair{std::string("target"), z->nativeEnum(TagPatternType)}, 
    object::pair{std::string("pattern"), z->std::string()}, 
    object::pair{std::string("caseSensitive"), z->boolean()->std::optional()->default(false)}, 
    object::pair{std::string("scoring"), TagScoringSchema}, 
    object::pair{std::string("description"), z->std::string()->std::optional()}, 
    object::pair{std::string("enabled"), z->boolean()->std::optional()->default(true)}
});
std::any TagRuleSchema = z->object(object{
    object::pair{std::string("name"), z->std::string()}, 
    object::pair{std::string("category"), z->nativeEnum(TagCategory)}, 
    object::pair{std::string("description"), z->std::string()}, 
    object::pair{std::string("patterns"), z->array(TagPatternSchema)}, 
    object::pair{std::string("weight"), z->number()->std::optional()->default(1)}, 
    object::pair{std::string("dependencies"), z->array(z->std::string())->std::optional()}, 
    object::pair{std::string("createdAt"), z->std::string()->std::optional()}, 
    object::pair{std::string("updatedAt"), z->std::string()->std::optional()}
});

void Main(void)
{
}

MAIN
