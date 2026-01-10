#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/utils/cli-prompts.h"

std::shared_ptr<Promise<string>> promptWithNav(string label, string initial, std::function<any(string)> validate)
{
    auto msg = string_empty + label + string_empty + (initial) ? any(std::string(" (current: ") + initial + std::string(")")) : any(string_empty) + string_empty;
    auto input = std::async([=]() { clack->text(object{
        object::pair{std::string("message"), msg}, 
        object::pair{std::string("placeholder"), initial}, 
        object::pair{std::string("defaultValue"), initial}, 
        object::pair{std::string("validate"), (validate) ? any([=](auto val) mutable
        {
            auto result = validate(val);
            return (type_of(result) == std::string("string")) ? any(result) : any(undefined);
        }
        ) : any(undefined)}
    }); });
    if (clack->isCancel(input)) {
        clack->cancel(std::string("Operation cancelled."));
        process->exit(0);
    }
    auto trimmedInput = input->trim();
    if (trimmedInput->toLowerCase() == std::string("cancel")) return std::string("cancel");
    if (trimmedInput->toLowerCase() == std::string("back")) return NAV_BACK;
    if (OR((trimmedInput->toLowerCase() == std::string("quit")), (trimmedInput->toLowerCase() == std::string("exit")))) {
        logger->info(std::string("Exiting..."));
        process->exit(0);
    }
    if (AND((trimmedInput == string_empty), (initial))) return initial;
    if (OR((trimmedInput == string_empty), (trimmedInput->toLowerCase() == std::string("next")))) return NAV_NEXT;
    return trimmedInput;
};


std::shared_ptr<Promise<array<string>>> promptForMultipleItems(string fieldName, array<string> initial)
{
    auto items = array<string>{ initial };
    logger->info(std::string("\
") + fieldName + string_empty);
    if (initial->get_length() > 0) {
        logger->info(std::string("Current values:"));
        initial->forEach([=](auto item, auto i) mutable
        {
            return logger->info(std::string("  ") + (i + 1) + std::string(". ") + item + string_empty);
        }
        );
        logger->info(std::string("\
Press Enter to keep existing values, or start typing new ones:"));
    }
    while (true)
    {
        auto val = std::async([=]() { promptWithNav(std::string("> ") + fieldName + std::string(":")); });
        if (val == NAV_NEXT) break;
        if (val == NAV_BACK) {
            if (items->get_length() == initial->get_length()) return initial;
            break;
        }
        if (val == std::string("cancel")) return initial;
        items->push(val);
    }
    return items;
};


std::shared_ptr<Promise<boolean>> confirmAction(string message)
{
    auto response = std::async([=]() { clack->confirm(object{
        object::pair{std::string("message"), std::string("message")}, 
        object::pair{std::string("initialValue"), false}
    }); });
    if (clack->isCancel(response)) {
        clack->cancel(std::string("Operation cancelled."));
        process->exit(0);
    }
    return Boolean(response);
};


string NAV_BACK = std::string("__back__");
string NAV_NEXT = std::string("__next__");

void Main(void)
{
}

MAIN
