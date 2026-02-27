#include "next.config.h"
#include <string>

std::shared_ptr<NextConfig> nextConfig = object{
    object::pair{std::string("experimental"), object{}}
};

void Main(void)
{
}

MAIN
