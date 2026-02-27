#include "drizzle.config.h"

void Main(void)
{
    dotenv->config(object{
        object::pair{std:("path"), std:("../../.env")}
    });
}

MAIN
