#include "index.hpp"
#include <string>

object configPaths = object{
    object::pair{std::string("typescript"), object{
        object::pair{std::string("base"), std::string("@elizaos/config/typescript/tsconfig.base.json")}, 
        object::pair{std::string("plugin"), std::string("@elizaos/config/typescript/tsconfig.plugin.json")}, 
        object::pair{std::string("frontend"), std::string("@elizaos/config/typescript/tsconfig.frontend.json")}, 
        object::pair{std::string("test"), std::string("@elizaos/config/typescript/tsconfig.test.json")}
    }}, 
    object::pair{std::string("eslint"), object{
        object::pair{std::string("plugin"), std::string("@elizaos/config/eslint/eslint.config.plugin.js")}, 
        object::pair{std::string("frontend"), std::string("@elizaos/config/eslint/eslint.config.frontend.js")}
    }}, 
    object::pair{std::string("prettier"), std::string("@elizaos/config/prettier/prettier.config.js")}
};

void Main(void)
{
}

MAIN
