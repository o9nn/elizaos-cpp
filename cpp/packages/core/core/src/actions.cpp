#include "/home/runner/work/elizaos-cpp/elizaos-cpp/plugin-specification/core-plugin-v2/src/actions.h"

std::string formatActionNames(array<std::shared_ptr<Action>> actions)
{
    return coreFormatActionNames(actions);
};


std::string formatActions(array<std::shared_ptr<Action>> actions)
{
    return coreFormatActions(actions);
};


std::function<std::any(array<any>, double)> composeActionExamples = [=](auto actionsData, auto count) mutable
{
    return coreComposeActionExamples(actionsData, count);
};

void Main(void)
{
}

MAIN
