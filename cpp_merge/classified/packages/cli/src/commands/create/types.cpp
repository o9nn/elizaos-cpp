#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/src/commands/create/types.h"

any initOptionsSchema = z->object(object{
    object::pair{std:("yes"), z->boolean()->default(false)}, 
    object::pair{std:("type"), z->enum(array<string>{ std:("project"), std:("plugin"), std:("agent"), std:("tee") })->default(std:("project"))}
});

void Main(void)
{
}

MAIN
