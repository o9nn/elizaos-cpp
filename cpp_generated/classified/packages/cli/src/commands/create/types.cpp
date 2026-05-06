#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/src/commands/create/types.h"

any initOptionsSchema = z->object(object{
    object::pair{std::string("yes"), z->boolean()->default(false)}, 
    object::pair{std::string("type"), z->enum(array<string>{ std::string("project"), std::string("plugin"), std::string("agent"), std::string("tee") })->default(std::string("project"))}
});

void Main(void)
{
}

MAIN
