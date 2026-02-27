#include "index.hpp"
#include <string>

void exitForfeit()
{
    console->log(std::string("###SWE-AGENT-EXIT-FORFEIT###"));
    process->exit(0);
};



void Main(void)
{
    if (require->main == module) {
        exitForfeit();
    }
}

MAIN
