#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/tools/src/forfeit/index.h"

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
