#include "index.hpp"

object configPaths = object{
    object::pair{std:("typescript"), object{
        object::pair{std:("base"), std:("@elizaos/config/typescript/tsconfig.base.json")}, 
        object::pair{std:("plugin"), std:("@elizaos/config/typescript/tsconfig.plugin.json")}, 
        object::pair{std:("frontend"), std:("@elizaos/config/typescript/tsconfig.frontend.json")}, 
        object::pair{std:("test"), std:("@elizaos/config/typescript/tsconfig.test.json")}
    }}, 
    object::pair{std:("eslint"), object{
        object::pair{std:("plugin"), std:("@elizaos/config/eslint/eslint.config.plugin.js")}, 
        object::pair{std:("frontend"), std:("@elizaos/config/eslint/eslint.config.frontend.js")}
    }}, 
    object::pair{std:("prettier"), std:("@elizaos/config/prettier/prettier.config.js")}
};

void Main(void)
{
}

MAIN
