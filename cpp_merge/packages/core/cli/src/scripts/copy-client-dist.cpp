#include "copy-client-dist.h"
#include <string>

void main()
{
    std::async([=]() { copyClientDist(); });
};



void Main(void)
{
    main()->_catch([=](auto error) mutable
    {
        console->error(std::string("Error running copy-client-dist script:"), error);
        process->exit(1);
    }
    );
}

MAIN
