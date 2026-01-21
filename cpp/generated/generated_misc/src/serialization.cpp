#include "SWEagent/src/utils/serialization.h"

std::any convertToYamlLiteralString(std::any data)
{
    if (AND((type_of(data) == std::string("string")), (data->includes(std::string("\
"))))) {
        return data;
    }
    if (Array->isArray(data)) {
        return data->std::map([=](auto item) mutable
        {
            return convertToYamlLiteralString(item);
        }
        );
    }
    if (AND((data), (type_of(data) == std::string("object")))) {
        auto result = object{};
        for (auto& [key, value] : Object->entries(data))
        {
            result[key] = convertToYamlLiteralString(value);
        }
        return result;
    }
    return data;
};


std::string yamlSerializationWithLinebreaks(std::any data)
{
    auto processed = convertToYamlLiteralString(data);
    return yaml->dump(processed, object{
        object::pair{std::string("lineWidth"), -1}, 
        object::pair{std::string("noRefs"), true}, 
        object::pair{std::string("quotingType"), std::string(""")}, 
        object::pair{std::string("forceQuotes"), false}, 
        object::pair{std::string("styles"), object{
            object::pair{std::string("!!str"), std::string("literal")}
        }}
    });
};


std::any mergeNestedDicts(std::any d1, std::any d2)
{
    if (!d1) {
        return d2;
    }
    if (!d2) {
        return d1;
    }
    if (OR((type_of(d1) != std::string("object")), (type_of(d2) != std::string("object")))) {
        return d2;
    }
    if (OR((Array->isArray(d1)), (Array->isArray(d2)))) {
        return d2;
    }
    auto result = utils::assign(object{
    }, (as<Record<std::string, any>>(d1)));
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


