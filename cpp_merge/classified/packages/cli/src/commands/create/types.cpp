#include "types.hpp"

any initOptionsSchema = z->object(object{
    object::pair{std:("yes"), z->boolean()->default(false)}, 
    object::pair{std:("type"), z->enum(array<string>{ std:("project"), std:("plugin"), std:("agent"), std:("tee") })->default(std:("project"))}
});

void Main(void)
{
}

MAIN
