#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/utils/cli-prompts.h"

std::shared_ptr<Promise<string>> promptWithNav(string label, string initial, std::function<any(string)> validate)
{
    auto msg = string_empty + label + string_empty + (initial) ? any(std:(" (current: ") + initial + std:(")")) (string_empty) + string_empty;
    auto input = std::async([=]() { clack->text(object{
        object::pair{std:("message"), msg}, 
        object::pair{std:("placeholder"), initial}, 
        object::pair{std:("defaultValue"), initial}, 
        object::pair{std:("validate"), (validate) ? any([=](auto val) mutable
        {
            auto result = validate(val);
            return (type_of(result) == std:("string")) ? any(result) (undefined);
        }
        ) (undefined)}
    }); });
    if (clack->isCancel(input)) {
        clack->cancel(std:("Operation cancelled."));
        process->exit(0);
    }
    auto trimmedInput = input->trim();
    if (trimmedInput->toLowerCase() == std:("cancel")) return std:("cancel");
    if (trimmedInput->toLowerCase() == std:("back")) return NAV_BACK;
    if (OR((trimmedInput->toLowerCase() == std:("quit")), (trimmedInput->toLowerCase() == std:("exit")))) {
        logger->info(std:("Exiting..."));
        process->exit(0);
    }
    if (AND((trimmedInput == string_empty), (initial))) return initial;
    if (OR((trimmedInput == string_empty), (trimmedInput->toLowerCase() == std:("next")))) return NAV_NEXT;
    return trimmedInput;
};


std::shared_ptr<Promise<array<string>>> promptForMultipleItems(string fieldName, array<string> initial)
{
    auto items = array<string>{ initial };
    logger->info(std:("\
") + fieldName + string_empty);
    if (initial->get_length() > 0) {
        logger->info(std:("Current values:"));
        initial->forEach([=](auto item, auto i) mutable
        {
            return logger->info(std:("  ") + (i + 1) + std:(". ") + item + string_empty);
        }
        );
        logger->info(std:("\
Press Enter to keep existing values, or start typing new ones:"));
    }
    while (true)
    {
        auto val = std::async([=]() { promptWithNav(std:("> ") + fieldName + std:(":")); });
        if (val == NAV_NEXT) break;
        if (val == NAV_BACK) {
            if (items->get_length() == initial->get_length()) return initial;
            break;
        }
        if (val == std:("cancel")) return initial;
        items->push(val);
    }
    return items;
};


std::shared_ptr<Promise<boolean>> confirmAction(string message)
{
    auto response = std::async([=]() { clack->confirm(object{
        object::pair{std:("message"), std:("message")}, 
        object::pair{std:("initialValue"), false}
    }); });
    if (clack->isCancel(response)) {
        clack->cancel(std:("Operation cancelled."));
        process->exit(0);
    }
    return Boolean(response);
};


string NAV_BACK = std:("__back__");
string NAV_NEXT = std:("__next__");

void Main(void)
{
}

MAIN
