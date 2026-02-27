#include "/home/runner/work/elizaos-cpp/elizaos-cpp/plugin-specification/core-plugin-v2/src/actions.h"

string formatActionNames(array<std::shared_ptr<Action>> actions)
{
    return coreFormatActionNames(actions);
};


string formatActions(array<std::shared_ptr<Action>> actions)
{
    return coreFormatActions(actions);
};


std::function<any(array<any>, double)> composeActionExamples = [=](auto actionsData, auto count) mutable
{
    return coreComposeActionExamples(actionsData, count);
};

void Main(void)
{
}

MAIN
