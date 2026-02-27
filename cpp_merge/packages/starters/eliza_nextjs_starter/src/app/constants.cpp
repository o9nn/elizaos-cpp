#include "constants.hpp"
#include <string>

object siteConfig = object{
    object::pair{std::string("name"), std::string("AI Eliza")}, 
    object::pair{std::string("url"), OR((OR((process->env->NEXT_PUBLIC_APP_URL), (std::string("https://ai.eliza.how/")))), (std::string("http://localhost:4000")))}, 
    object::pair{std::string("description"), std::string("Eliza is a powerful multi-agent simulation framework designed to create, deploy, and manage autonomous AI agents.")}, 
    object::pair{std::string("ogImage"), std::string("/og.png")}, 
    object::pair{std::string("creator"), std::string("Eliza Labs")}, 
    object::pair{std::string("icons"), array<object>{ object{
        object::pair{std::string("rel"), std::string("icon")}, 
        object::pair{std::string("type"), std::string("image/png")}, 
        object::pair{std::string("url"), std::string("/eliza-black.png")}, 
        object::pair{std::string("media"), std::string("(prefers-color-scheme: light)")}
    }, object{
        object::pair{std::string("rel"), std::string("icon")}, 
        object::pair{std::string("type"), std::string("image/png")}, 
        object::pair{std::string("url"), std::string("/favicon.ico")}, 
        object::pair{std::string("media"), std::string("(prefers-color-scheme: dark)")}
    } }}
};

void Main(void)
{
}

MAIN
