#include "drizzle.config.h"
#include <string>

void Main(void)
{
    config(object{
        object::pair{std::string("path"), std::string("../../.env")}
    });
}

MAIN
