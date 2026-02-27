#include "constants.hpp"

object siteConfig = object{
    object::pair{std:("name"), std:("AI Eliza")}, 
    object::pair{std:("url"), OR((OR((process->env->NEXT_PUBLIC_APP_URL), (std:("https://ai.eliza.how/")))), (std:("http://localhost:4000")))}, 
    object::pair{std:("description"), std:("Eliza is a powerful multi-agent simulation framework designed to create, deploy, and manage autonomous AI agents.")}, 
    object::pair{std:("ogImage"), std:("/og.png")}, 
    object::pair{std:("creator"), std:("Eliza Labs")}, 
    object::pair{std:("icons"), array<object>{ object{
        object::pair{std:("rel"), std:("icon")}, 
        object::pair{std:("type"), std:("image/png")}, 
        object::pair{std:("url"), std:("/eliza-black.png")}, 
        object::pair{std:("media"), std:("(prefers-color-scheme: light)")}
    }, object{
        object::pair{std:("rel"), std:("icon")}, 
        object::pair{std:("type"), std:("image/png")}, 
        object::pair{std:("url"), std:("/favicon.ico")}, 
        object::pair{std:("media"), std:("(prefers-color-scheme: dark)")}
    } }}
};

void Main(void)
{
}

MAIN
