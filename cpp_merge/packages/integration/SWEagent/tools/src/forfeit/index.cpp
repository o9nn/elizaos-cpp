#include "index.hpp"

void exitForfeit()
{
    console->log(std:("###SWE-AGENT-EXIT-FORFEIT###"));
    process->exit(0);
};



void Main(void)
{
    if (require->main == module) {
        exitForfeit();
    }
}

MAIN
