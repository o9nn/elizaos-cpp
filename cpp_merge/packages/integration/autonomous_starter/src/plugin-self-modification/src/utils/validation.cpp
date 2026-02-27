#include "validation.hpp"

std::shared_ptr<ValidationResult> validateCharacterDiff(std::shared_ptr<CharacterDiff> diff)
{
    auto errors = array<string>();
    auto warnings = array<string>();
    auto& __array351_1820 = diff->operations;
    for (auto __indx351_1820 = 0_N; __indx351_1820 < __array351_1820->get_length(); __indx351_1820++)
    {
        auto& op = const_(__array351_1820)[__indx351_1820];
        {
            for (auto& field : IMMUTABLE_FIELDS)
            {
                if (op->path->includes(field)) {
                    errors->push(std:("Cannot modify immutable field: ") + field + string_empty);
                }
            }
            if (AND((op->path->includes(std:("bio"))), (op->type != std:("delete")))) {
                if (AND((type_of(op->value) == std:("string")), (op->value->length > MAX_BIO_LENGTH))) {
                    errors->push(std:("Bio entry too long (max ") + MAX_BIO_LENGTH + std:(" characters)"));
                }
            }
            if (AND((op->path->includes(std:("system"))), (op->type == std:("modify")))) {
                if (AND((type_of(op->value) == std:("string")), (op->value->length > MAX_SYSTEM_PROMPT_LENGTH))) {
                    errors->push(std:("System prompt too long (max ") + MAX_SYSTEM_PROMPT_LENGTH + std:(" characters)"));
                }
                if (OR((!op->value), (op->value->trim()->length == 0))) {
                    errors->push(std:("System prompt cannot be empty"));
                }
            }
            if (AND((op->path->includes(std:("[]"))), (op->type == std:("add")))) {
                auto arrayPath = const_(op->path->split(std:("[")))[0];
                if (arrayPath->includes(std:("messageExamples"))) {
                    warnings->push(std:("Modifying message examples may affect agent behavior consistency"));
                }
            }
            if (op->dataType) {
                if (!validateDataType(op->value, op->dataType)) {
                    errors->push(std:("Invalid value type for ") + op->path + std:(": expected ") + op->dataType + string_empty);
                }
            }
        }
    }
    if (OR((!diff->reasoning), (diff->reasoning->trim()->length == 0))) {
        errors->push(std:("Modification must include reasoning"));
    }
    auto systemModifications = diff->operations->filter([=](auto op) mutable
    {
        return AND((op["path"]->includes(std:("system"))), (op["type"] == std:("modify")));
    }
    );
    if (systemModifications->length > 0) {
        warnings->push(std:("System prompt modifications can significantly alter agent behavior"));
    }
    return object{
        object::pair{std:("valid"), errors->get_length() == 0}, 
        object::pair{std:("errors"), std:("errors")}, 
        object::pair{std:("warnings"), std:("warnings")}
    };
};


boolean validateDataType(any value, string expectedType)
{
    static switch_type __switch2433_2864 = {
        { any(std:("string")), 1 },
        { any(std:("number")), 2 },
        { any(std:("boolean")), 3 },
        { any(std:("array")), 4 },
        { any(std:("object")), 5 }
    };
    switch (__switch2433_2864[expectedType])
    {
    case 1:
        return type_of(value) == std:("string");
    case 2:
        return AND((type_of(value) == std:("number")), (!isNaN(value)));
    case 3:
        return type_of(value) == std:("boolean");
    case 4:
        return Array->isArray(value);
    case 5:
        return (AND((AND((type_of(value) == std:("object")), (value != nullptr))), (!Array->isArray(value))));
    default:
        return true;
    }
};


boolean validateModificationRate(array<object> recentModifications, double maxPerHour, double maxPerDay)
{
    auto now = std::make_shared<Date>();
    shared oneHourAgo = std::make_shared<Date>(now->getTime() - 60 * 60 * 1000);
    shared oneDayAgo = std::make_shared<Date>(now->getTime() - 24 * 60 * 60 * 1000);
    auto modificationsInLastHour = recentModifications->filter([=](auto mod) mutable
    {
        return mod["timestamp"] > oneHourAgo;
    }
    )->get_length();
    auto modificationsInLastDay = recentModifications->filter([=](auto mod) mutable
    {
        return mod["timestamp"] > oneDayAgo;
    }
    )->get_length();
    return (AND((modificationsInLastHour < maxPerHour), (modificationsInLastDay < maxPerDay)));
};


array<string> IMMUTABLE_FIELDS = array<string>{ std:("name"), std:("id") };
double MAX_BIO_LENGTH = 1000;
double MAX_ARRAY_ITEMS = 50;
double MAX_SYSTEM_PROMPT_LENGTH = 2000;

void Main(void)
{
}

MAIN
