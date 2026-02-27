#include "vite.config.h"

void Main(void)
{
    config(object{
        object::pair{std:("path"), std:("../../.env")}
    });
}

MAIN
