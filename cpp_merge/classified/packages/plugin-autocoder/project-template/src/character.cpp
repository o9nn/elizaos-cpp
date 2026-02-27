#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-autocoder/project-template/src/character.h"

std::shared_ptr<Character> character = object{
    object::pair{std::string("name"), std::string("Eliza")}, 
    object::pair{std::string("plugins"), array<any>()}, 
    object::pair{std::string("settings"), object{
        object::pair{std::string("secrets"), object{}}
    }}, 
    object::pair{std::string("bio"), array<any>()}, 
    object::pair{std::string("topics"), array<any>()}, 
    object::pair{std::string("messageExamples"), array<any>()}, 
    object::pair{std::string("style"), object{
        object::pair{std::string("all"), array<any>()}, 
        object::pair{std::string("chat"), array<any>()}, 
        object::pair{std::string("post"), array<any>()}
    }}
};

void Main(void)
{
}

MAIN
