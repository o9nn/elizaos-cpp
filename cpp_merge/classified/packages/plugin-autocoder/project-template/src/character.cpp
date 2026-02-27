#include "character.hpp"

std::shared_ptr<Character> character = object{
    object::pair{std:("name"), std:("Eliza")}, 
    object::pair{std:("plugins"), array<any>()}, 
    object::pair{std:("settings"), object{
        object::pair{std:("secrets"), object{}}
    }}, 
    object::pair{std:("bio"), array<any>()}, 
    object::pair{std:("topics"), array<any>()}, 
    object::pair{std:("messageExamples"), array<any>()}, 
    object::pair{std:("style"), object{
        object::pair{std:("all"), array<any>()}, 
        object::pair{std:("chat"), array<any>()}, 
        object::pair{std:("post"), array<any>()}
    }}
};

void Main(void)
{
}

MAIN
