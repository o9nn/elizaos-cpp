#include "database.hpp"

object VECTOR_DIMS = as<std::shared_ptr<const>>(object{
    object::pair{std:("SMALL"), 384}, 
    object::pair{std:("MEDIUM"), 512}, 
    object::pair{std:("LARGE"), 768}, 
    object::pair{std:("XL"), 1024}, 
    object::pair{std:("XXL"), 1536}, 
    object::pair{std:("XXXL"), 3072}
});

void Main(void)
{
}

MAIN
