#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/src/lib/typeHelpers.h"

std::function<any(std::shared_ptr<T>)> isNotNullOrUndefined = template <typename T>
[=](auto value) mutable
{
    return AND((value != nullptr), (value != undefined));
};
std::function<any(array<std::shared_ptr<T>>)> isNotNullOrUndefinedArray = template <typename T>
[=](auto value) mutable
{
    return value->every([=](auto v) mutable
    {
        return isNotNullOrUndefined(v);
    }
    );
};
std::function<std::function<any(std::shared_ptr<T>)>(std::shared_ptr<K>)> isNotNullOrUndefinedField = template <typename T, typename K>
[=](auto field) mutable
{
    return [=](auto value) mutable
    {
        return isNotNullOrUndefined(const_(value)[field]);
    };
};

void Main(void)
{
}

MAIN
