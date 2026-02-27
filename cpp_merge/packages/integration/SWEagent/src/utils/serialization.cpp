#include "serialization.hpp"

any convertToYamlLiteralString(any data)
{
    if (AND((type_of(data) == std:("string")), (data->includes(std:("\
"))))) {
        return data;
    }
    if (Array->isArray(data)) {
        return data->map([=](auto item) mutable
        {
            return convertToYamlLiteralString(item);
        }
        );
    }
    if (AND((data), (type_of(data) == std:("object")))) {
        auto result = object{};
        for (auto& [key, value] : Object->entries(data))
        {
            result[key] = convertToYamlLiteralString(value);
        }
        return result;
    }
    return data;
};


string yamlSerializationWithLinebreaks(any data)
{
    auto processed = convertToYamlLiteralString(data);
    return yaml->dump(processed, object{
        object::pair{std:("lineWidth"), -1}, 
        object::pair{std:("noRefs"), true}, 
        object::pair{std:("quotingType"), std:(""")}, 
        object::pair{std:("forceQuotes"), false}, 
        object::pair{std:("styles"), object{
            object::pair{std:("!!str"), std:("literal")}
        }}
    });
};


any mergeNestedDicts(any d1, any d2)
{
    if (!d1) {
        return d2;
    }
    if (!d2) {
        return d1;
    }
    if (OR((type_of(d1) != std:("object")), (type_of(d2) != std:("object")))) {
        return d2;
    }
    if (OR((Array->isArray(d1)), (Array->isArray(d2)))) {
        return d2;
    }
    auto result = utils::assign(object{
    }, (as<Record<string, any>>(d1)));
    for (auto& [key, value] : Object->entries(d2))
    {
        if (in(key, result)) {
            result[key] = mergeNestedDicts(const_(result)[key], value);
        } else {
            result[key] = value;
        }
    }
    return result;
};


