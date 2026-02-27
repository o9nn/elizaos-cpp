#include "createRaydiumPool.hpp"

void main()
{
    auto provider = anchor->AnchorProvider->env();
    anchor->setProvider(provider);
};



void Main(void)
{
    main()->then([=]() mutable
    {
        return process->exit(0);
    }
    )->_catch([=](auto error) mutable
    {
        console->error(error);
        process->exit(1);
    }
    );
}

MAIN
