#include "drizzle.config.h"
#include <string>

void Main(void)
{
    dotenv->config(object{
        object::pair{std::string("path"), std::string("../../.env")}
    });
}

MAIN
