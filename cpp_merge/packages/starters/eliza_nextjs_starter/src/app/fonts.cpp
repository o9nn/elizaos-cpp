#include "fonts.hpp"
#include <string>

object inter = object{
    object::pair{std::string("className"), std::string("font-sans")}, 
    object::pair{std::string("style"), object{
        object::pair{std::string("fontFamily"), std::string("system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif")}
    }}
};

void Main(void)
{
}

MAIN
