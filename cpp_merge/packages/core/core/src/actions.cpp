#include "actions.hpp"

string formatActionNames(array<std::shared_ptr<Action>> actions)
{
    if (!actions->get_length()) return string_empty;
    return (array<std::shared_ptr<Action>>{ actions })->sort([=]() mutable
    {
        return Math->random() - 0.5;
    }
    )->map([=](auto action) mutable
    {
        return action->name;
    }
    )->join(std:(", "));
};


string formatActions(array<std::shared_ptr<Action>> actions)
{
    if (!actions->get_length()) return string_empty;
    return (array<std::shared_ptr<Action>>{ actions })->sort([=]() mutable
    {
        return Math->random() - 0.5;
    }
    )->map([=](auto action) mutable
    {
        return string_empty + action->name + std:(": ") + action->description + string_empty;
    }
    )->join(std:(",\
"));
};


std::function<string(array<std::shared_ptr<Action>>, double)> composeActionExamples = [=](auto actionsData, auto count) mutable
{
    if (OR((!actionsData->get_length()), (count <= 0))) {
        return string_empty;
    }
    auto actionsWithExamples = actionsData->filter([=](auto action) mutable
    {
        return AND((AND((action->examples), (Array->isArray(action->examples)))), (action->examples->get_length() > 0));
    }
    );
    if (!actionsWithExamples->get_length()) {
        return string_empty;
    }
    auto examplesCopy = actionsWithExamples->map([=](auto action) mutable
    {
        return array<any>{ action->examples };
    }
    );
    auto selectedExamples = array<array<std::shared_ptr<ActionExample>>>();
    auto availableActionIndices = examplesCopy->map([=](auto examples, auto index) mutable
    {
        return ((examples->get_length() > 0) ? any(index) (-1));
    }
    )->filter([=](auto index) mutable
    {
        return index != -1;
    }
    );
    while (AND((selectedExamples->get_length() < count), (availableActionIndices->get_length() > 0)))
    {
        auto randomIndex = Math->floor(Math->random() * availableActionIndices->get_length());
        auto actionIndex = const_(availableActionIndices)[randomIndex];
        auto examples = const_(examplesCopy)[actionIndex];
        auto exampleIndex = Math->floor(Math->random() * examples->get_length());
        selectedExamples->push(const_(examples->splice(exampleIndex, 1))[0]);
        if (examples->get_length() == 0) {
            availableActionIndices->splice(randomIndex, 1);
        }
    }
    return formatSelectedExamples(selectedExamples);
};
std::function<string(array<array<std::shared_ptr<ActionExample>>>)> formatSelectedExamples = [=](auto examples) mutable
{
    shared MAX_NAME_PLACEHOLDERS = 5;
    return examples->map([=](auto example) mutable
    {
        shared randomNames = Array->from(object{
            object::pair{std:("length"), MAX_NAME_PLACEHOLDERS}
        }, [=]() mutable
        {
            return uniqueNamesGenerator(object{
                object::pair{std:("dictionaries"), array<any>{ names }}
            });
        }
        );
        auto conversation = example->map([=](auto message) mutable
        {
            auto messageText = string_empty + message->name + std:(": ") + message->content->text + string_empty;
            for (auto i = 0; i < randomNames->get_length(); i++)
            {
                messageText = messageText->replaceAll(std:("{{name") + (i + 1) + std:("}}"), const_(randomNames)[i]);
            }
            return messageText;
        }
        )->join(std:("\
"));
        return std:("\
") + conversation + string_empty;
    }
    )->join(std:("\
"));
};

void Main(void)
{
}

MAIN
