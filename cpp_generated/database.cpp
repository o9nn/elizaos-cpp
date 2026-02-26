#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/core/src/types/database.h"

object VECTOR_DIMS = as<std::shared_ptr<const>>(object{
    object::pair{std::string("SMALL"), 384}, 
    object::pair{std::string("MEDIUM"), 512}, 
    object::pair{std::string("LARGE"), 768}, 
    object::pair{std::string("XL"), 1024}, 
    object::pair{std::string("XXL"), 1536}, 
    object::pair{std::string("XXXL"), 3072}
});

void Main(void)
{
}

MAIN
